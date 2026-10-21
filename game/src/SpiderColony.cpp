#include "IslandSimulator/SpiderColony.hpp"
#include "IslandSimulator/CaveSite.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>,Contact=.0001f;
SpiderPoint Add(SpiderPoint a,SpiderPoint b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
SpiderPoint Sub(SpiderPoint a,SpiderPoint b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
SpiderPoint Mul(SpiderPoint a,float s){return {a.x*s,a.y*s,a.z*s};}
float Dot(SpiderPoint a,SpiderPoint b){return a.x*b.x+a.y*b.y+a.z*b.z;}
SpiderPoint Cross(SpiderPoint a,SpiderPoint b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
SpiderPoint Unit(SpiderPoint p){return Mul(p,1/std::hypot(p.x,p.y,p.z));}
float Wrap(float a){return std::remainder(a,2*Pi);}
std::uint32_t Mix(std::uint32_t v){v^=v>>16;v*=0x7feb352dU;v^=v>>15;v*=0x846ca68bU;return v^(v>>16);}
}
SpiderPoint SpiderGroundPose::ToWorld(SpiderPoint p) const noexcept {
    return Add(origin,Add(Add(Mul(right,p.x),Mul(up,p.y)),Mul(forward,p.z)));
}
SpiderPoint SpiderGroundPose::ToLocal(SpiderPoint p) const noexcept {
    p=Sub(p,origin);return {Dot(p,right),Dot(p,up),Dot(p,forward)};
}
SpiderGroundPose SpiderColony::Frame(float x,float z,float heading) const {
    SpiderGroundPose pose{};
    const auto sample=surface_.Sample(x,z);
    pose.origin={x,sample.height,z};pose.up={sample.normal.x,sample.normal.y,sample.normal.z};
    pose.right=Unit(Cross(pose.up,{std::cos(heading),0,std::sin(heading)}));
    pose.forward=Unit(Cross(pose.right,pose.up));
    return pose;
}
SpiderGroundPose SpiderColony::Pose(const SpiderState& state,const Feet& feet) const {
    auto pose=Frame(state.x,state.z,state.heading);
    for(std::size_t leg=0;leg<8;++leg) {
        pose.feet[leg]=feet[leg].position;pose.stance[leg]=!feet[leg].swinging;
        pose.legs[leg]=SpiderPose::Solve(leg,pose.ToLocal(feet[leg].position));
    }
    // Rearing, the front pair of legs is raised: once the display is past half, each front
    // foot leaves its anchor for a point lifted and drawn in by the display amount, well
    // within the leg's reach; the other six stay planted.
    // Grooming, the one front leg is drawn in to the mouthparts: once the groom is past
    // half its foot leaves its anchor for a point in under the body's front and up,
    // worked back and forth a little with the groom.
    if(state.groom>.5f&&state.display<=.5f) {
        const std::size_t leg=state.groomLeg==4U?4U:0U;
        const auto neutral=SpiderPose::NeutralFoot(leg);
        const float k=std::clamp(state.groom,0.0f,1.0f);
        const SpiderPoint drawn{neutral.x*.55f,.040f,neutral.z*.85f+.012f*std::sin(k*31.0f)};
        const auto target=Add(Mul(neutral,1-k),Mul(drawn,k));
        // Only a target within the leg's reach is taken; otherwise the foot keeps its anchor.
        const auto solved=SpiderPose::Solve(leg,target);
        if(!solved.reachLimited) {pose.feet[leg]=pose.ToWorld(target);pose.stance[leg]=false;pose.legs[leg]=solved;}
    }
    if(state.display>.5f)for(const std::size_t leg:{std::size_t{0},std::size_t{4}}) {
        const auto neutral=SpiderPose::NeutralFoot(leg);
        const float k=std::clamp(state.display,0.0f,1.0f);
        const SpiderPoint raised{neutral.x*.72f,.060f,neutral.z*.80f};
        const auto target=Add(Mul(neutral,1-k),Mul(raised,k));
        pose.feet[leg]=pose.ToWorld(target);pose.stance[leg]=false;
        pose.legs[leg]=SpiderPose::Solve(leg,target);
    }
    return pose;
}
std::size_t SpiderColony::GroomingCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_)count+=intent.grooming?1U:0U;
    return count;
}
std::size_t SpiderColony::DisplayingCount() const noexcept {
    std::size_t count=0;
    for(const auto& intent:intents_)count+=intent.displaying?1U:0U;
    return count;
}
bool SpiderColony::Valid(const SpiderGroundPose& pose) const {
    if(pose.up.y<.95f||CaveSite::Reserves(pose.origin.x,pose.origin.z,SpiderHabitat::BodyRadius))return false;
    for(const auto& leg:pose.legs) {
        if(leg.reachLimited)return false;
        const std::array<SpiderPoint,4> joints{leg.root,leg.hip,leg.knee,leg.foot};
        constexpr std::array<float,4> radii{.00494f,.00416f,.00299f,0};
        for(std::size_t segment=0;segment<3;++segment)for(int step=0;step<=4;++step) {
            const float t=static_cast<float>(step)/4;
            const auto p=pose.ToWorld(Add(Mul(joints[segment],1-t),Mul(joints[segment+1],t)));
            const float r=radii[segment]*(1-t)+radii[segment+1]*t;
            if(std::hypot(p.x-pose.origin.x,p.z-pose.origin.z)>SpiderHabitat::BodyRadius
                ||p.y-surface_.Sample(p.x,p.z).height<r-.00002f)return false;
        }
    }
    return true;
}
SpiderColony::SpiderColony(const IslandTerrain& terrain,const JungleLayout& jungle,
    std::span<const CircularObstacle> obstacles):surface_(terrain) {
    const auto homes=SpiderHabitat::FindGround(terrain,surface_,jungle,WorldCollisionGrid(obstacles));
    if(!homes)throw std::runtime_error("No complete ground spider habitat");
    homes_=*homes;
    for(std::size_t i=0;i<states_.size();++i) {
        auto& s=states_[i];const auto h=homes_[i];
        s.x=h.x;s.y=h.y;s.z=h.z;s.heading=Wrap(static_cast<float>(i)*2.4f);
        const auto frame=Frame(s.x,s.z,s.heading);
        for(std::size_t leg=0;leg<8;++leg) {
            auto p=frame.ToWorld(SpiderPose::NeutralFoot(leg));
            p.y=surface_.Sample(p.x,p.z).height+Contact;
            feet_[i][leg].position=p;
        }
        s.pose=Pose(s,feet_[i]);
        if(!Valid(s.pose))throw std::runtime_error("Initial spider pose does not fit ground");
        intents_[i]={.5f+.13f*static_cast<float>(i),s.heading,0};
    }
}
void SpiderColony::AdvanceFeet(std::size_t index,float dt) {
    auto& state=states_[index];auto& feet=feet_[index];
    for(std::size_t leg=0;leg<8;++leg)if(feet[leg].swinging) {
        auto trial=feet;auto& foot=trial[leg];
        foot.progress=std::min(1.0f,foot.progress+dt/.08f);
        const float t=foot.progress,u=t*t*(3-2*t),arch=std::sin(Pi*t);
        foot.position=Add(Mul(foot.start,1-u),Mul(foot.target,u));
        foot.position.y=std::max(foot.position.y,surface_.Sample(foot.position.x,foot.position.z).height+Contact)
            +.0104f*arch*arch;
        foot.swinging=t<1;
        if(Valid(Pose(state,trial)))feet=trial;
    }
    const auto frame=Frame(state.x,state.z,state.heading);
    const auto active=std::count_if(feet.begin(),feet.end(),[](const auto& f){return f.swinging;});
    std::size_t selected=8;float largest=.00455f;
    // A rearing spider plants no new steps: its feet stay where they are under the display.
    if(active<2&&state.display<.1f&&state.groom<.1f)for(std::size_t leg=0;leg<8;++leg)if(!feet[leg].swinging) {
        const auto neutral=SpiderPose::NeutralFoot(leg);
        const auto local=frame.ToLocal(feet[leg].position);
        // A freshly planted foot ahead of its neutral point is useful support,
        // not an error to immediately lift again. Sideways error still allows turns.
        const float lag=state.behavior==SpiderBehavior::Rest?neutral.z-local.z:
            std::max(0.0f,neutral.z-local.z);
        const float error=std::hypot(neutral.x-local.x,lag);
        if(error>largest){largest=error;selected=leg;}
    }
    if(selected<8) {
        auto trial=feet;auto& foot=trial[selected];
        auto local=SpiderPose::NeutralFoot(selected);
        if(state.behavior!=SpiderBehavior::Rest)local.z+=.0156f;
        foot.target=frame.ToWorld(local);
        foot.target.y=surface_.Sample(foot.target.x,foot.target.z).height+Contact;
        auto landing=trial;landing[selected].position=foot.target;
        if(Valid(Pose(state,landing))) {
            foot.start=foot.position;foot.progress=0;foot.swinging=true;feet=trial;
        }
    }
    state.pose=Pose(state,feet);
}
void SpiderColony::Step(std::size_t index,float dt,SpiderPoint observer,bool observerValid) {
    auto& state=states_[index];auto& intent=intents_[index];
    AdvanceFeet(index,dt);
    intent.remaining-=dt;
    const float observerDistance=observerValid?std::hypot(state.x-observer.x,state.y+.04f-observer.y,state.z-observer.z)
        :std::numeric_limits<float>::infinity();
    const bool retreat=observerDistance<RetreatDistance;
    // The threat display: the walker standing at ground level short of the retreat range
    // has the spider stop where it is and rear; closer it retreats, gone it lowers the legs.
    const bool threatened=observerValid&&!retreat&&observerDistance<DisplayDistance
        &&std::abs(observer.y-state.y)<DisplayHeightReach;
    if(threatened&&!intent.displaying)++displays_;
    intent.displaying=threatened;
    state.display+=std::clamp((threatened?1.0f:0.0f)-state.display,-DisplayLowerRate*dt,DisplayRaiseRate*dt);
    if(threatened){state.behavior=SpiderBehavior::Rest;intent.remaining=std::max(intent.remaining,.3f);}
    // The groom: resting and unthreatened, a second in and every dozen seconds after (a
    // little per spider), one front leg is drawn in and worked through the mouthparts for
    // a few seconds, the legs taking turns; a threat or the crawl on drops it.
    const bool resting=state.behavior==SpiderBehavior::Rest&&!threatened&&!retreat;
    intent.groomClock=resting?intent.groomClock+dt:0.0f;
    const float groomPeriod=GroomPeriodSeconds+1.3f*static_cast<float>(index%4U);
    const float groomCycle=std::fmod(intent.groomClock,groomPeriod);
    const bool grooming=resting&&groomCycle>=GroomDelaySeconds&&groomCycle<GroomDelaySeconds+GroomSeconds;
    if(grooming&&!intent.grooming){++grooms_;state.groomLeg=state.groomLeg==0U?4U:0U;}
    intent.grooming=grooming;
    state.groom+=std::clamp((grooming?1.0f:0.0f)-state.groom,-3.0f*dt,2.0f*dt);
    if(retreat) {
        state.behavior=SpiderBehavior::Retreat;intent.remaining=.5f;
        if(std::hypot(state.x-observer.x,state.z-observer.z)>.001f)
            intent.heading=std::atan2(state.z-observer.z,state.x-observer.x);
    } else if(intent.remaining<=0) {
        const auto bits=Mix(SpiderHabitat::Seed+static_cast<std::uint32_t>(index)*7919U+(++intent.decisions)*104729U);
        state.behavior=bits%4==0?SpiderBehavior::Rest:SpiderBehavior::Crawl;
        intent.remaining=1.5f+static_cast<float>((bits>>8)%30)/10;
        intent.heading=static_cast<float>((bits>>16)%4096)/4096*2*Pi-Pi;
        if(std::hypot(state.x-homes_[index].x,state.z-homes_[index].z)>.40f)
            intent.heading=std::atan2(homes_[index].z-state.z,homes_[index].x-state.x);
    }
    if(state.behavior==SpiderBehavior::Rest){state.speed=0;state.pose=Pose(state,feet_[index]);return;}
    auto trial=state;
    trial.heading=Wrap(state.heading+std::clamp(Wrap(intent.heading-state.heading),-.6f*dt,.6f*dt));
    const float speed=state.behavior==SpiderBehavior::Retreat?RetreatSpeed:CrawlSpeed;
    // Turn before retreating rather than walking toward the observer while the
    // body catches up with the desired heading. Turning still replants feet.
    const float distance=retreat&&std::abs(Wrap(intent.heading-trial.heading))>Pi/3?0:speed*dt;
    trial.x+=std::cos(trial.heading)*distance;trial.z+=std::sin(trial.heading)*distance;
    if(std::hypot(trial.x-homes_[index].x,trial.z-homes_[index].z)>SpiderHabitat::PatrolRadius) {
        // Retain the turn even when translation reaches the patrol boundary.
        // Otherwise an outward-facing animal could never turn back into its disk.
        trial.x=state.x;trial.z=state.z;if(!retreat)intent.remaining=0;
    }
    trial.y=surface_.Sample(trial.x,trial.z).height;trial.pose=Pose(trial,feet_[index]);
    if(!Valid(trial.pose)){state.speed=0;return;}
    trial.speed=std::hypot(trial.x-state.x,trial.y-state.y,trial.z-state.z)/dt;
    state=trial;
}
void SpiderColony::Update(float dt,SpiderPoint observer) {
    if(!std::isfinite(dt)||dt<=0)return;
    dt=std::min(dt,.1f);
    const bool valid=std::isfinite(observer.x)&&std::isfinite(observer.y)&&std::isfinite(observer.z);
    ++frameIndex_;
    for(std::size_t i=0;i<states_.size();++i) {
        auto& state=states_[i];
        const float distance=valid?std::hypot(state.x-observer.x,state.y+.04f-observer.y,
            state.z-observer.z):std::numeric_limits<float>::infinity();
        const auto previousTier=state.tier;
        // Drawn within 14 m (SpiderRenderer::Reach), so simulated in full that far and asleep
        // past 22 m: the reduced tier replays the same 20 ms gait steps, only batched, and
        // out to 48 m it stepped every spider in a wide ring nobody could see.
        state.tier=WildlifeActivationPolicy::Classify(distance,14.0f,22.0f);
        // Outside the visible habitat keep the exact planted pose, without accruing
        // catch-up work. Replaying up to fifteen IK steps for every dormant spider
        // made aerial views pay for every ground animal even when none was drawn.
        if(state.tier==WildlifeSimulationTier::Dormant) {
            accumulatedSeconds_[i]=0;
            state.speed=0;
            continue;
        }
        if(state.tier==WildlifeSimulationTier::Full && previousTier!=state.tier)
            accumulatedSeconds_[i]=0; // Do not reveal a deferred reduced-tier jump on approach.
        accumulatedSeconds_[i]=std::min(accumulatedSeconds_[i]+dt,.30f);
        const auto divisor=WildlifeActivationPolicy::UpdateDivisor(state.tier);
        if((frameIndex_+i)%divisor!=0U)continue;
        const float elapsed=accumulatedSeconds_[i];accumulatedSeconds_[i]=0;
        const int steps=static_cast<int>(std::ceil(elapsed/.02f));
        for(int step=0;step<steps;++step)
            Step(i,elapsed/static_cast<float>(steps),observer,valid);
    }
}
}
