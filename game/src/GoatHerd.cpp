#include "IslandSimulator/GoatHerd.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include <limits>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>,Tau=2*Pi;
// Submillimetre contact separation, inside the existing 0.15mm sole tolerance.
// Keep triangle-boundary rounding from consuming the entire mesh clearance margin.
constexpr float ContactSeparation=.0001f;
float Wrap(float angle) { return std::remainder(angle,Tau); }
std::uint32_t Mix(std::uint32_t x) {
    x^=x>>16;x*=0x7feb352dU;x^=x>>15;x*=0x846ca68bU;return x^(x>>16);
}
}
GoatHerd::GoatHerd(const IslandTerrain& terrain,const JungleLayout& jungle,
    std::span<const CircularObstacle> obstacles):terrain_(terrain),surface_(terrain)
{
    const auto homes=GoatHabitat::Find(terrain,surface_,jungle,WorldCollisionGrid(obstacles));
    if(!homes) throw std::runtime_error("No complete safe goat habitat");
    homes_=*homes;
    // Same exclusion envelopes as placement, indexed once rather than scanning plants per step.
    std::vector<CircularObstacle> occupied(obstacles.begin(),obstacles.end());
    for(const auto& tree:jungle.Canopy()) occupied.push_back({tree.x,tree.z,
        std::max(0.0f,GoatHabitat::TreeClearance*tree.scale-GoatHabitat::BodyClearance)});
    for(const auto& plant:jungle.Understory()) occupied.push_back({plant.x,plant.z,1.8f*plant.scale});
    for(const auto& bush:jungle.Bushes()) occupied.push_back({bush.x,bush.z,1.2f*bush.scale});
    clearance_.Reset(occupied);
    for(std::size_t i=0;i<states_.size();++i) {
        const auto home=homes_[i];
        states_[i].x=home.x;states_[i].y=home.y;states_[i].z=home.z;
        states_[i].heading=Wrap(static_cast<float>(i)*2.4f);
        states_[i].scale=i==states_.size()-1?.75f:1;
        intents_[i].remaining=1+static_cast<float>(i)*.7f;
        intents_[i].targetHeading=states_[i].heading;
        auto& s=states_[i];
        s.pose=FitGoatGroundPose(terrain_,surface_,s.x,s.z,s.heading,s.phase,0,s.scale);
        for(std::size_t leg=0;leg<4;++leg) {
            const auto& p=s.pose.legs[leg];
            feet_[i][leg].position=s.pose.ToWorld({p.hoof.x,p.hoof.y-GoatLegPose::HoofHeight,
                p.hoof.z+GoatLegPose::SoleForward});
            feet_[i][leg].position.y+=ContactSeparation;
        }
        s.pose=ContactPose(s,feet_[i]);
    }
    for(std::size_t i=0;i<states_.size();++i) FindDrinkingPoint(i);
}
// The bank one metre outside the channel on the home's side, at the closest point of the
// authored route the goat may stand on; dry upper slopes usually lie beyond reach.
void GoatHerd::FindDrinkingPoint(std::size_t index) noexcept {
    auto& intent=intents_[index];intent.hasDrink=false;
    float best=std::numeric_limits<float>::max();
    for(float along=6;along<StreamRoute::NominalLength-6;along+=1) {
        const float cx=StreamRoute::SourceX-along,cz=StreamRoute::CentreZ(cx);
        const float tangentZ=StreamRoute::CentreZ(cx-1)-cz,length=std::hypot(1.0f,tangentZ);
        const float normalX=tangentZ/length,normalZ=1.0f/length;
        const float side=(homes_[index].z-cz)*normalZ+(homes_[index].x-cx)*normalX<0?-1.0f:1.0f;
        const float offset=StreamRoute::HalfWidth(cx)+1.05f;
        const float x=cx+normalX*offset*side,z=cz+normalZ*offset*side;
        const float homeDistance=std::hypot(x-homes_[index].x,z-homes_[index].z);
        if(homeDistance>=best||!IsValidPosition(index,x,z))continue;
        best=homeDistance;intent.drinkX=x;intent.drinkZ=z;intent.faceX=cx;intent.faceZ=cz;intent.hasDrink=true;
    }
}
std::size_t GoatHerd::DrinkerCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_)count+=intent.hasDrink?1U:0U;
    return count;
}
bool GoatHerd::IsValidPosition(std::size_t index,float x,float z) const {
    if(index>=states_.size()) return false;
    return std::hypot(x-homes_[index].x,z-homes_[index].z)<=HomeRadius
        &&std::hypot(x-homes_[0].x,z-homes_[0].z)<=GoatHabitat::HerdRadius
        &&!clearance_.IntersectsCircle(x,z,GoatHabitat::BodyClearance)
        &&GoatHabitat::AcceptsGround(terrain_,surface_,x,z);
}
bool GoatHerd::TryMove(std::size_t index,float heading,float distance,bool& waitingForStep) {
    waitingForStep=false;
    auto& state=states_[index];
    const auto normal=surface_.Sample(state.x,state.z).normal;
    const float gradient=-(normal.x*std::cos(heading)+normal.z*std::sin(heading))/normal.y;
    const float horizontal=distance/std::hypot(1.0f,gradient);
    const float x=state.x+std::cos(heading)*horizontal,z=state.z+std::sin(heading)*horizontal;
    const float midX=(x+state.x)*.5f,midZ=(z+state.z)*.5f;
    if(!IsValidPosition(index,x,z)||!IsValidPosition(index,midX,midZ)
        ||clearance_.IntersectsCircle(midX,midZ,GoatHabitat::BodyClearance+horizontal*.5f)) return false;
    for(std::size_t other=0;other<states_.size();++other) if(other!=index) {
        // Sparring partners (and ex-partners still drawing apart) close to nose-to-nose
        // contact; any other goat keeps two body clearances.
        const bool partner=intents_[index].partner==static_cast<int>(other)
            ||intents_[other].partner==static_cast<int>(index);
        if(std::hypot(midX-states_[other].x,midZ-states_[other].z)
            <(partner?SparContact:2*GoatHabitat::BodyClearance)+horizontal*.5f) return false;
    }
    const float y=surface_.Sample(x,z).height;
    const float travelled=std::hypot(x-state.x,y-state.y,z-state.z);
    const float phase=std::fmod(state.phase+GoatPhaseAdvance(travelled,state.speed,state.scale),Tau);
    auto candidate=state;
    candidate.x=x;candidate.y=y;candidate.z=z;candidate.heading=heading;candidate.phase=phase;
    const auto pose=ContactPose(candidate,feet_[index]);
    for(const auto& leg:pose.legs) if(leg.reachLimited) {waitingForStep=true;return false;}
    state.x=x;state.y=y;state.z=z;state.heading=heading;state.phase=phase;
    return true;
}
GoatGroundPose GoatHerd::ContactPose(const GoatState& state,const Feet& feet) const {
    auto pose=MakeGoatBodyFrame(terrain_,surface_,state.x,state.z,state.heading,state.phase,state.speed,state.scale);
    for(std::size_t leg=0;leg<4;++leg) {
        auto local=pose.ToLocal(feet[leg].position);
        local.y+=GoatLegPose::HoofHeight;local.z-=GoatLegPose::SoleForward;
        auto reference=EvaluateGoatLeg(0,0,pose.bodyBob,leg);
        reference.stance=!feet[leg].swinging;
        pose.legs[leg]=RetargetGoatLeg(reference,local,leg);
        const auto normal=surface_.Sample(feet[leg].position.x,feet[leg].position.z).normal;
        const auto project=[&](GoatPoint axis){return normal.x*axis.x+normal.y*axis.y+normal.z*axis.z;};
        pose.legs[leg].soleNormal={project(pose.right),project(pose.up),project(pose.forward)};
    }
    // Lying: the body settles along its up axis and the legs fold under it, the fore
    // cannons lying back along the ground from knees set forward, the hind legs tucked with
    // the hooves brought forward, each hoof on the ground under the lowered body.
    const float lie=std::clamp(state.lieAmount,0.0f,1.0f);
    if(lie>0) {
        const float drop=lie*.30f*state.scale;
        pose.origin={pose.origin.x-pose.up.x*drop,pose.origin.y-pose.up.y*drop,pose.origin.z-pose.up.z*drop};
        pose.bodyBob*=1-lie;
        for(std::size_t leg=0;leg<4;++leg) {
            const bool front=leg%2==1;
            auto& chain=pose.legs[leg];
            const auto root=chain.root;
            GoatPoint hoof{root.x*1.12f,0,front?root.z-.20f:root.z+.40f};
            auto world=pose.ToWorld(hoof);
            world.y=surface_.Sample(world.x,world.z).height;
            const float ground=pose.ToLocal(world).y;
            hoof.y=ground+GoatLegPose::HoofHeight;
            const GoatPoint knee{root.x*1.06f,ground+.12f,root.z+(front?.16f:.12f)};
            const auto mix=[lie](GoatPoint a,GoatPoint b){return GoatPoint{std::lerp(a.x,b.x,lie),std::lerp(a.y,b.y,lie),std::lerp(a.z,b.z,lie)};};
            chain.knee=mix(chain.knee,knee);
            // The blended hoof is re-grounded exactly at its sole, as the standing solver
            // grounds the planted feet, so the contact stays on the rendered terrain
            // through the fold.
            const auto blended=mix(chain.hoof,hoof);
            auto sole=pose.ToWorld({blended.x,blended.y-GoatLegPose::HoofHeight,blended.z+GoatLegPose::SoleForward});
            sole.y=surface_.Sample(sole.x,sole.z).height;
            const auto grounded=pose.ToLocal(sole);
            chain.hoof={grounded.x,grounded.y+GoatLegPose::HoofHeight,grounded.z-GoatLegPose::SoleForward};
            // The sole takes the ground's normal where it now rests, as a planted foot does.
            const auto normal=surface_.Sample(sole.x,sole.z).normal;
            const auto project=[&](GoatPoint axis){return normal.x*axis.x+normal.y*axis.y+normal.z*axis.z;};
            chain.soleNormal={project(pose.right),project(pose.up),project(pose.forward)};
            chain.stance=true;chain.reachLimited=false;
        }
    }
    return pose;
}
void GoatHerd::AdvanceFeet(std::size_t index,float dt) {
    auto& state=states_[index];auto& feet=feet_[index];
    const auto valid=[](const GoatGroundPose& pose) {
        return std::none_of(pose.legs.begin(),pose.legs.end(),[](const auto& leg){return leg.reachLimited;});
    };
    for(std::size_t leg=0;leg<4;++leg)if(feet[leg].swinging) {
        auto trial=feet;auto& foot=trial[leg];
        foot.progress=std::min(1.0f,foot.progress+dt/.18f);
        const float t=foot.progress,u=t*t*(3-2*t),arch=std::sin(Pi*t);
        foot.position={foot.start.x+(foot.target.x-foot.start.x)*u,
            foot.start.y+(foot.target.y-foot.start.y)*u,
            foot.start.z+(foot.target.z-foot.start.z)*u};
        foot.position.y=std::max(foot.position.y,surface_.Sample(foot.position.x,foot.position.z).height+ContactSeparation)
            +.09f*state.scale*arch*arch;
        foot.swinging=t<1;
        if(valid(ContactPose(state,trial)))feet=trial;
    }
    const auto neutral=FitGoatGroundPose(terrain_,surface_,state.x,state.z,state.heading,0,0,state.scale);
    int active=static_cast<int>(std::count_if(feet.begin(),feet.end(),[](const auto& f){return f.swinging;}));
    // Start at most one foot per substep and retain at least two grounded supports.
    std::size_t selected=4;float priority=0;
    for(std::size_t leg=0;leg<4&&active<2;++leg)if(!feet[leg].swinging) {
        const auto& p=neutral.legs[leg];
        const auto sole=neutral.ToWorld({p.hoof.x,p.hoof.y-GoatLegPose::HoofHeight,p.hoof.z+GoatLegPose::SoleForward});
        const float error=std::hypot(sole.x-feet[leg].position.x,sole.z-feet[leg].position.z)/state.scale;
        const bool scheduled=state.speed>.05f&&!EvaluateGoatLeg(state.phase,state.speed,0,leg).stance;
        const float score=error+(scheduled?.10f:0);
        const float reposition=state.speed<.05f?.025f:.08f;
        if((error>reposition||scheduled)&&score>priority) {selected=leg;priority=score;}
    }
    if(selected<4) {
        const auto& p=neutral.legs[selected];
        auto trial=feet;auto& foot=trial[selected];
        foot.start=foot.position;foot.progress=0;foot.swinging=true;
        auto local=GoatPoint{p.hoof.x,p.hoof.y-GoatLegPose::HoofHeight,
            p.hoof.z+GoatLegPose::SoleForward+.18f*state.speed/MaximumSpeed};
        foot.target=neutral.ToWorld(local);
        foot.target.y=surface_.Sample(foot.target.x,foot.target.z).height+ContactSeparation;
        auto landing=trial;landing[selected].position=foot.target;
        if(!valid(ContactPose(state,landing))) {
            local.z=p.hoof.z+GoatLegPose::SoleForward;foot.target=neutral.ToWorld(local);
            foot.target.y=surface_.Sample(foot.target.x,foot.target.z).height+ContactSeparation;
            landing[selected].position=foot.target;
        }
        if(valid(ContactPose(state,landing)))feet=trial;
    }
    state.pose=ContactPose(state,feet);
}
void GoatHerd::Step(std::size_t index,float dt,GoatPoint observer,bool observerValid) {
    auto& state=states_[index];auto& intent=intents_[index];
    intent.remaining-=dt;
    intent.avoidance=std::max(0.0f,intent.avoidance-dt);
    // A sparring partner stays remembered after the bout until the two have drawn apart to
    // the ordinary two body clearances, so the close stance can be left at a walk.
    if(intent.partner>=0&&state.behavior!=GoatBehavior::Spar) {
        const auto& old=states_[static_cast<std::size_t>(intent.partner)];
        if(std::hypot(old.x-state.x,old.z-state.z)>=2*GoatHabitat::BodyClearance) intent.partner=-1;
    }
    const float observerRange=observerValid
        ?std::hypot(state.x-observer.x,state.y+.8f-observer.y,state.z-observer.z)
        :std::numeric_limits<float>::infinity();
    const bool retreat=observerRange<7;
    const bool startled=disturbanceSeconds_>0
        &&std::hypot(state.x-disturbanceX_,state.z-disturbanceZ_)<StartleDistance;
    if(retreat) {
        state.behavior=GoatBehavior::Retreat;intent.remaining=.75f;
        const float dx=state.x-observer.x,dz=state.z-observer.z;
        // Sidestep about seventy degrees off the direct line of escape.  The goat keeps the
        // player in view over its shoulder instead of turning its rump squarely to them.
        if(intent.avoidance==0&&std::hypot(dx,dz)>.01f)
            intent.targetHeading=Wrap(std::atan2(dz,dx)+(index%2==0?1.22f:-1.22f));
    } else if(observerRange<(index==0?SentinelAlertDistance
        :sentinelAlertSeconds_>=HerdReactionSeconds?SentinelAlertDistance:AlertDistance)) {
        // Stand and watch the approaching player; ordinary wandering resumes once they leave.
        // The sentinel lifts its head first; the others follow after the reaction delay.
        state.behavior=GoatBehavior::Rest;intent.remaining=std::max(intent.remaining,.5f);
        if(intent.avoidance==0)
            intent.targetHeading=std::atan2(observer.z-state.z,observer.x-state.x);
    } else if(startled) {
        // A thud nearby: the goat stops and stands facing where it came from until the
        // startle fades, then its wandering goes on.
        state.behavior=GoatBehavior::Rest;intent.remaining=std::max(intent.remaining,.5f);
        if(intent.avoidance==0)
            intent.targetHeading=std::atan2(disturbanceZ_-state.z,disturbanceX_-state.x);
    } else if(intent.remaining<=0) {
        const auto bits=Mix(GoatHabitat::Seed+static_cast<std::uint32_t>(index)*7919U+(++intent.decisions)*104729U);
        state.behavior=bits%5<3?GoatBehavior::Walk:(bits%5==3?GoatBehavior::Graze:GoatBehavior::Rest);
        intent.remaining=3+static_cast<float>((bits>>8)%50)/10;
        intent.targetHeading=static_cast<float>((bits>>16)%4096)/4096*Tau-Pi;
        if(std::hypot(state.x-homes_[index].x,state.z-homes_[index].z)>HomeRadius-3)
            intent.targetHeading=std::atan2(homes_[index].z-state.z,homes_[index].x-state.x);
        // Every third rest of a goat with a reachable bank becomes a walk to the water.
        if(intent.hasDrink&&bits%5==4&&intent.decisions%3==0) {
            state.behavior=GoatBehavior::Drink;intent.remaining=20;
        }
        // Every so often a goat squares up to its nearest free neighbour for a bout of
        // head-to-head shoving: the two walk together, stand nose to nose and take turns
        // lowering their heads. The sentinel keeps its watch and never spars.
        if(index!=0&&bits%5==2&&intent.decisions%3==1) {
            std::size_t best=GoatHabitat::Count;float bestDistance=SparRange;
            for(std::size_t other=1;other<GoatHabitat::Count;++other) {
                if(other==index) continue;
                const auto& candidate=states_[other];
                if(candidate.behavior==GoatBehavior::Drink||candidate.behavior==GoatBehavior::Retreat
                    ||candidate.behavior==GoatBehavior::Spar) continue;
                const float distance=std::hypot(candidate.x-state.x,candidate.z-state.z);
                if(distance<bestDistance) {bestDistance=distance;best=other;}
            }
            if(best<GoatHabitat::Count) {
                state.behavior=GoatBehavior::Spar;intent.remaining=SparSeconds;
                intent.partner=static_cast<int>(best);intent.sparClock=0;
                auto& partnerIntent=intents_[best];
                states_[best].behavior=GoatBehavior::Spar;partnerIntent.remaining=SparSeconds;
                partnerIntent.partner=static_cast<int>(index);partnerIntent.sparClock=0;
                ++spars_;
            }
        }
    }
    bool sparring=false,butting=false;
    if(state.behavior==GoatBehavior::Spar) {
        const bool partnerValid=intent.partner>=0&&intent.partner<static_cast<int>(GoatHabitat::Count)
            &&states_[static_cast<std::size_t>(intent.partner)].behavior==GoatBehavior::Spar
            &&intents_[static_cast<std::size_t>(intent.partner)].partner==static_cast<int>(index);
        if(!partnerValid) {
            // The partner was called away (alert, startle, its own bout over): stand a moment.
            state.behavior=GoatBehavior::Rest;intent.remaining=std::min(intent.remaining,1.0f);
        } else {
            intent.sparClock+=dt;
            const auto& partner=states_[static_cast<std::size_t>(intent.partner)];
            const float distance=std::hypot(partner.x-state.x,partner.z-state.z);
            sparring=distance<SparReach;
            if(intent.avoidance==0) intent.targetHeading=std::atan2(partner.z-state.z,partner.x-state.x);
            // Turns alternate on a shared clock: the lower index shoves in the first half.
            const bool firstHalf=std::fmod(intent.sparClock,2*SparTurnSeconds)<SparTurnSeconds;
            butting=sparring&&(firstHalf==(static_cast<int>(index)<intent.partner));
        }
    }
    bool drinking=false;
    if(state.behavior==GoatBehavior::Drink) {
        drinking=std::hypot(state.x-intent.drinkX,state.z-intent.drinkZ)<.9f;
        if(intent.avoidance==0)
            intent.targetHeading=drinking?std::atan2(intent.faceZ-state.z,intent.faceX-state.x)
                :std::atan2(intent.drinkZ-state.z,intent.drinkX-state.x);
    }
    // An alert goat shuffles slowly through its ordinary stepping until it faces the
    // player, rather than pivoting on planted hooves, then stands and watches.
    const bool alertTurning=(observerRange<AlertDistance||startled)&&!retreat
        &&std::abs(Wrap(intent.targetHeading-state.heading))>.18f;
    const float desired=state.behavior==GoatBehavior::Retreat?MaximumSpeed:
        (state.behavior==GoatBehavior::Walk||(state.behavior==GoatBehavior::Drink&&!drinking)
        ||(state.behavior==GoatBehavior::Spar&&!sparring)?.65f
        :alertTurning?.25f:0);
    state.speed+=std::clamp(desired-state.speed,-1.5f*dt,.8f*dt);
    // Drinking is the grazing head-down pose held at the water's edge; a sparring goat's
    // shove is the same lowering, taken in turns with its partner.
    const float graze=(state.behavior==GoatBehavior::Graze||drinking||butting)&&state.speed<.05f?1.0f:0.0f;
    state.grazingPose+=std::clamp(graze-state.grazingPose,-2*dt,dt);
    // A rest two seconds in, undisturbed, settles the goat onto folded legs for a longer
    // lie; the walker within the alert range, a startle, a retreat or the next walk has it
    // on its feet again within half a second.
    const bool undisturbed=state.behavior==GoatBehavior::Rest&&observerRange>=SentinelAlertDistance
        &&!startled&&!retreat&&state.speed<.05f;
    intent.restSeconds=undisturbed?intent.restSeconds+dt:0;
    const bool shouldLie=undisturbed&&intent.restSeconds>LieDelaySeconds;
    if(shouldLie&&!intent.lying) {++lies_;intent.remaining+=LieBonusSeconds;}
    intent.lying=shouldLie;
    state.lieAmount+=std::clamp((intent.lying?1.0f:0.0f)-state.lieAmount,-dt*RiseRate,dt*LieDownRate);
    // The tail: every few seconds (a little per goat) the short tail flicks side to side,
    // two beats in well under a second, and hangs still between flicks.
    const float wagPeriod=WagPeriodSeconds+.9f*static_cast<float>(index%4U);
    if(!intent.wagSeeded) { intent.wagSeeded=true; intent.wagClock=WagSeconds+1.3f*static_cast<float>(index); intent.earClock=.9f*static_cast<float>(index); }
    intent.wagClock+=dt;
    if(intent.wagClock>=wagPeriod) intent.wagClock-=wagPeriod;
    const bool wagging=intent.wagClock<WagSeconds;
    if(wagging&&!intent.wagging) ++wags_;
    intent.wagging=wagging;
    state.tailSwing=wagging?std::sin(intent.wagClock/WagSeconds*Pi)*std::sin(intent.wagClock*Tau*WagBeats/WagSeconds):0.0f;
    // The ears: every few seconds (a little per goat) one ear flicks back at a fly and
    // settles again, the ears taking turns.
    intent.earClock+=dt;
    const float earPeriod=EarFlickPeriodSeconds+.7f*static_cast<float>(index%3U);
    if(intent.earClock>=earPeriod) { intent.earClock=0; intent.earSide=-intent.earSide; }
    const bool earFlicking=intent.earClock>=1.1f&&intent.earClock<1.1f+EarFlickSeconds;
    if(earFlicking&&!intent.earFlicking) ++earFlicks_;
    intent.earFlicking=earFlicking;
    state.earFlick=(earFlicking?std::sin((intent.earClock-1.1f)/EarFlickSeconds*Pi):0.0f)*intent.earSide;
    if(state.speed<=.0001f) { state.speed=0;return; }
    const float heading=Wrap(state.heading+std::clamp(Wrap(intent.targetHeading-state.heading),-1.8f*dt,1.8f*dt));
    bool waitingForStep=false;
    if(!TryMove(index,heading,state.speed*dt,waitingForStep)) {
        state.speed=0;
        // Waiting for an anchored foot to step is not a new obstacle. Keep the
        // route intent; AdvanceFeet continues independently of root translation.
        if(waitingForStep)return;
        auto candidate=state;candidate.heading=heading;
        const auto turn=ContactPose(candidate,feet_[index]);
        if(std::none_of(turn.legs.begin(),turn.legs.end(),[](const auto& leg){return leg.reachLimited;}))
            state.heading=heading;
        // A bounded turn request for later steps, not a teleport or forced obstacle escape.
        if(intent.avoidance==0) {
            intent.targetHeading=Wrap(state.heading+(index%2==0?1.0f:-1.0f)*Pi*.65f);
            intent.avoidance=1.5f;
        }
        if(!retreat) intent.remaining=std::min(intent.remaining,.7f);
    }
}
std::size_t GoatHerd::SparringCount() const noexcept {
    std::size_t count=0;
    for(std::size_t index=0;index<GoatHabitat::Count;++index) {
        const auto& intent=intents_[index];
        if(states_[index].behavior!=GoatBehavior::Spar||intent.partner<0
            ||intent.partner>=static_cast<int>(GoatHabitat::Count)) continue;
        const auto& partner=states_[static_cast<std::size_t>(intent.partner)];
        if(std::hypot(partner.x-states_[index].x,partner.z-states_[index].z)<SparReach) ++count;
    }
    return count;
}
void GoatHerd::NoteDisturbance(float x,float z) noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)) return;
    disturbanceX_=x;disturbanceZ_=z;disturbanceSeconds_=StartleSeconds;
}
std::size_t GoatHerd::EarFlickingCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_) count+=intent.earFlicking?1U:0U;
    return count;
}

std::size_t GoatHerd::WaggingCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_) count+=intent.wagging?1U:0U;
    return count;
}

std::size_t GoatHerd::LyingCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_) count+=intent.lying?1U:0U;
    return count;
}
void GoatHerd::PreviewLie(std::size_t index) {
    if(index>=states_.size()) return;
    auto& state=states_[index];auto& intent=intents_[index];
    state.behavior=GoatBehavior::Rest;state.speed=0;state.grazingPose=0;state.lieAmount=1;
    intent.remaining=30;intent.restSeconds=LieDelaySeconds+1;intent.lying=true;
    state.pose=ContactPose(state,feet_[index]);
}
void GoatHerd::Update(float dt,GoatPoint observer) {
    if(!std::isfinite(dt)||dt<=0) return;
    dt=std::min(dt,.1f);
    const int steps=std::min(5,static_cast<int>(std::ceil(dt/.02f)));
    const bool observerValid=std::isfinite(observer.x)&&std::isfinite(observer.y)&&std::isfinite(observer.z);
    // Goat ranges are authored eight metres apart inside the forty-metre herd radius, so the
    // herd's cohesion is its habitat; only the sentinel's alarm is shared here.
    const auto& sentinel=states_[0];
    const bool sentinelSees=observerValid&&std::hypot(sentinel.x-observer.x,
        sentinel.y+.8f-observer.y,sentinel.z-observer.z)<SentinelAlertDistance;
    sentinelAlertSeconds_=sentinelSees?sentinelAlertSeconds_+dt:0;
    disturbanceSeconds_=std::max(0.0f,disturbanceSeconds_-dt);
    for(int step=0;step<steps;++step) for(std::size_t i=0;i<states_.size();++i) {
        Step(i,dt/static_cast<float>(steps),observer,observerValid);
        AdvanceFeet(i,dt/static_cast<float>(steps));
    }
}
}
