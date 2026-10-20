#include "IslandSimulator/GoatHerd.hpp"
#include "IslandSimulator/GoatMesh.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/ShipwreckLayout.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <vector>

int main() {
    using namespace island;
    const auto check=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;
    const RenderedTerrainSurface surface(terrain);
    const VegetationLayout palms(terrain);
    const JungleLayout jungle(terrain);
    const NaturalPropLayout props(terrain,palms,jungle);
    const ShipwreckLayout wreck(terrain);
    std::vector<CircularObstacle> occupied;
    for(const auto obstacles:std::array<std::span<const CircularObstacle>,4>{
        palms.Obstacles(),jungle.Obstacles(),props.Obstacles(),wreck.Obstacles()})
        occupied.insert(occupied.end(),obstacles.begin(),obstacles.end());
    const auto hut=HutLayout::FindInWorld(terrain,jungle,occupied);
    check(hut.has_value(),"reference hut missing");
    occupied.push_back({hut->x,hut->z,HutLayout::CollisionRadius});
    const WorldCollisionGrid grid(occupied);
    GoatHerd herd(terrain,jungle,occupied),repeat(terrain,jungle,occupied);
    {
        // A coconut's thud twelve metres from a goat, no observer about: it stands facing the
        // fall and turns toward it; after the startle fades it wanders on.
        GoatHerd startled(terrain,jungle,occupied);
        const GoatPoint nobody{std::numeric_limits<float>::quiet_NaN(),0,0};
        for(int frame=0;frame<200;++frame) startled.Update(.05f,nobody);
        const auto grazer=startled.Goats()[1];
        const float thudX=grazer.x+12,thudZ=grazer.z;
        startled.NoteDisturbance(thudX,thudZ);
        bool stoodFacing=false;
        for(int frame=0;frame<80;++frame) {
            startled.Update(.05f,nobody);
            const auto& goat=startled.Goats()[1];
            const float toward=std::remainder(std::atan2(thudZ-goat.z,thudX-goat.x)-goat.heading,6.2831853f);
            if(goat.behavior==GoatBehavior::Rest&&std::abs(toward)<.35f) stoodFacing=true;
        }
        check(startled.Startled()&&stoodFacing,"a thud twelve metres off stops the goat and turns it toward the fall");
        for(int frame=0;frame<80;++frame) startled.Update(.05f,nobody);
        check(!startled.Startled(),"the goat's startle fades after six seconds");
    }
    const auto copy=[](const GoatHerd& h){return std::vector<GoatState>(h.Goats().begin(),h.Goats().end());};
    const auto equal=[&](const GoatHerd& a,const GoatHerd& b){return copy(a)==copy(b);};
    const GoatPoint distant{1000,1000,1000};
    std::array<float,GoatHabitat::Count> travelled{};
    std::array<bool,6> behaviors{};
    float minimumHoofClearance=1;
    for(int frame=0;frame<900;++frame) {
        const auto before=copy(herd);
        herd.Update(.1f,distant);repeat.Update(.1f,distant);
        check(equal(herd,repeat),"identical goat inputs diverged");
        for(std::size_t i=0;i<herd.Goats().size();++i) {
            const auto s=herd.Goats()[i],p=before[i];
            const float d=std::hypot(s.x-p.x,s.y-p.y,s.z-p.z);
            check(d<=.13f,"goat teleported or exceeded movement bound");
            travelled[i]+=d;behaviors[static_cast<std::size_t>(s.behavior)]=true;
            check(herd.IsValidPosition(i,s.x,s.z),"goat left allowed habitat/home");
            check(GoatHabitat::Accepts(terrain,surface,jungle,grid,s.x,s.z),
                "indexed movement clearance differs from original placement checks");
            check(s.y==surface.Sample(s.x,s.z).height,"goat lost rendered ground height");
            check(std::abs(std::remainder(s.heading-p.heading,2*std::numbers::pi_v<float>))<=.1801f,
                "goat heading snapped beyond bounded turn rate");
            check(std::isfinite(s.phase)&&s.speed>=0&&s.speed<=1.20001f
                &&s.grazingPose>=0&&s.grazingPose<=1,"invalid goat motion state");
            if(d==0) check(s.phase==p.phase,"blocked/stationary goat advanced stride");
            int supports=0;
            for(std::size_t leg=0;leg<4;++leg) {
                const auto& current=s.pose.legs[leg];const auto& previous=p.pose.legs[leg];
                supports+=current.stance?1:0;
                check(!current.reachLimited,"authoritative goat contact pose is unreachable");
                // A goat folding its legs to lie down, or rising, moves its hooves under it;
                // the planted-foot invariant holds for a goat on its feet.
                if(current.stance&&previous.stance&&s.lieAmount==0&&p.lieAmount==0) {
                    const auto a=p.pose.ToWorld({previous.hoof.x,previous.hoof.y-GoatLegPose::HoofHeight,
                        previous.hoof.z+GoatLegPose::SoleForward});
                    const auto b=s.pose.ToWorld({current.hoof.x,current.hoof.y-GoatLegPose::HoofHeight,
                        current.hoof.z+GoatLegPose::SoleForward});
                    check(std::hypot(b.x-a.x,b.y-a.y,b.z-a.z)<.0001f,"planted goat hoof moved with the body");
                    const auto local=s.pose.ToLocal(b);
                    check(std::abs(local.x-current.hoof.x)<.0001f,"goat body inverse transform drifted");
                }
            }
            check(supports>=2,"goat lost two-foot support during stepping");
            if(frame%10==0) {
                const auto& pose=s.pose;
                const auto mesh=BuildGoatMesh(pose,s.grazingPose,true);
                for(const auto& vertex:mesh.vertices) {
                    const auto point=pose.ToWorld(vertex.position);
                    check(std::hypot(point.x-s.x,point.z-s.z)<=GoatHabitat::BodyClearance,
                        "anchored limb mesh exceeds collision clearance envelope");
                    if(vertex.part==GoatPart::Hoof)minimumHoofClearance=std::min(minimumHoofClearance,
                        point.y-surface.Sample(point.x,point.z).height);
                }
                for(const auto& leg:pose.legs) {
                    check(!leg.reachLimited,"moving goat lost reachable hoof support");
                    const auto sole=pose.ToWorld({leg.hoof.x,leg.hoof.y-GoatLegPose::HoofHeight,
                        leg.hoof.z+GoatLegPose::SoleForward});
                    const float error=sole.y-surface.Sample(sole.x,sole.z).height;
                    check(error>=-.00015f&&(!leg.stance||std::abs(error)<.00015f),
                        "moving goat sole contact differs from rendered terrain");
                }
            }
            for(std::size_t j=0;j<i;++j) {
                // Sparring partners stand nose to nose; everyone else keeps two body clearances.
                const bool partners=herd.SparPartner(i)==static_cast<int>(j)||herd.SparPartner(j)==static_cast<int>(i);
                check(std::hypot(s.x-herd.Goats()[j].x,s.z-herd.Goats()[j].z)
                    >=(partners?GoatHerd::SparContact-.01f:2*GoatHabitat::BodyClearance),"goats overlap");
            }
        }
    }
    std::cout<<"goat minimum actual hoof vertex clearance="<<minimumHoofClearance<<std::endl;
    check(minimumHoofClearance>=-.001f,"goat hoof mesh penetrates rendered terrain");
    for(float distance:travelled) {
        std::cout<<"goat 90s travelled="<<distance<<'\n';
        check(distance>1,"goat stayed stuck throughout 90s");
    }
    check(behaviors[0]&&behaviors[1]&&behaviors[2],"grazing/walking/rest cycle missing");
    const auto unchanged=copy(herd);
    herd.Update(0,distant);herd.Update(-1,distant);
    herd.Update(std::numeric_limits<float>::quiet_NaN(),distant);
    check(copy(herd)==unchanged,"invalid delta mutated goats");
    herd.Update(100,distant);repeat.Update(.1f,distant);
    check(equal(herd,repeat),"large delta not bounded to .1s");
    GoatHerd close(terrain,jungle,occupied),high(terrain,jungle,occupied),far(terrain,jungle,occupied);
    const auto initial=close.Goats().front();
    const GoatPoint observer{initial.x-2.5f,initial.y+.8f,initial.z};
    bool retreated=false;
    for(int frame=0;frame<200;++frame) {
        close.Update(.1f,observer);
        high.Update(.1f,{initial.x,initial.y+100,initial.z});far.Update(.1f,distant);
        check(equal(high,far),"high fly-mode observer triggers ground retreat");
        retreated|=close.Goats().front().behavior==GoatBehavior::Retreat;
    }
    const auto escaped=close.Goats().front();
    const float clearance=std::hypot(escaped.x-observer.x,escaped.z-observer.z);
    std::cout<<"goat observer clearance="<<clearance<<'\n';
    check(retreated&&clearance>3.5f,"near observer did not cause a bounded retreat");
    high.Update(.1f,{std::numeric_limits<float>::quiet_NaN(),0,0});far.Update(.1f,distant);
    check(equal(high,far),"invalid observer affects goat decisions");
    auto obstacleRoute=occupied;
    const CircularObstacle headOn{initial.x+2.7f,initial.z,.6f};
    obstacleRoute.push_back(headOn);
    GoatHerd deflected(terrain,jungle,obstacleRoute);
    check(deflected.Goats().front().x==initial.x&&deflected.Goats().front().z==initial.z,
        "head-on regression unexpectedly changed the initial home");
    for(int frame=0;frame<200;++frame) {
        deflected.Update(.1f,observer);
        for(const auto& s:deflected.Goats())
            check(std::hypot(s.x-headOn.x,s.z-headOn.z)>=GoatHabitat::BodyClearance+headOn.radius,
                "retreat tunneled through head-on obstacle");
    }
    const auto around=deflected.Goats().front();
    const float aroundClearance=std::hypot(around.x-observer.x,around.z-observer.z);
    std::cout<<"goat head-on obstacle observer clearance="<<aroundClearance<<'\n';
    check(aroundClearance>3.5f,"head-on obstacle permanently trapped retreating goat");
    {
        // Sparring: within eight minutes two goats stand nose to nose, and over the next
        // seconds each lowers its head in turn while the other holds its head up; the
        // sentinel never joins a bout.
        GoatHerd sparrers(terrain,jungle,occupied);
        bool squaredUp=false;
        for(int step=0;step<4800&&!squaredUp;++step) {
            sparrers.Update(.1f,distant);
            squaredUp=sparrers.SparringCount()>=2;
        }
        check(squaredUp&&sparrers.SparCount()>=1,"no two goats squared up for a sparring bout within eight minutes");
        std::size_t a=GoatHabitat::Count,b=GoatHabitat::Count;
        for(std::size_t i=1;i<GoatHabitat::Count&&a==GoatHabitat::Count;++i)
            if(sparrers.Goats()[i].behavior==GoatBehavior::Spar&&sparrers.SparPartner(i)>=0) {a=i;b=static_cast<std::size_t>(sparrers.SparPartner(i));}
        check(a<GoatHabitat::Count&&b<GoatHabitat::Count&&a!=0&&b!=0,"the sentinel joined a sparring bout");
        const auto& ga=sparrers.Goats()[a];const auto& gb=sparrers.Goats()[b];
        const float facing=std::cos(ga.heading-std::atan2(gb.z-ga.z,gb.x-ga.x));
        check(std::hypot(ga.x-gb.x,ga.z-gb.z)<GoatHerd::SparReach&&facing>.5f,"sparring goats do not stand nose to nose");
        bool aLowered=false,bLowered=false;
        for(int step=0;step<80&&!(aLowered&&bLowered);++step) {
            sparrers.Update(.1f,distant);
            const auto& sa=sparrers.Goats()[a];const auto& sb=sparrers.Goats()[b];
            if(sa.behavior!=GoatBehavior::Spar||sb.behavior!=GoatBehavior::Spar) break;
            if(sa.grazingPose>.9f&&sb.grazingPose<.3f) aLowered=true;
            if(sb.grazingPose>.9f&&sa.grazingPose<.3f) bLowered=true;
        }
        check(aLowered&&bLowered,"sparring goats do not take turns lowering their heads");
    }
    {
        // A resting goat lies down: a rest two seconds in, undisturbed, settles the goat onto
        // folded legs (the rest lengthened for it); the lying pose sits lower with every hoof
        // on the ground; the walker at ten metres has it up within half a second.
        GoatHerd liers(terrain,jungle,occupied);
        const GoatPoint nobody{std::numeric_limits<float>::quiet_NaN(),0,0};
        bool lay=false;
        for(int frame=0;frame<3600&&!lay;++frame) { liers.Update(.05f,nobody); lay=liers.Goats()[1].lieAmount>.9f; }
        check(lay&&liers.LieCount()>=1&&liers.Lying(1)&&liers.Goats()[1].behavior==GoatBehavior::Rest,
            "a resting goat does not lie down within three minutes");
        const auto lying=liers.Goats()[1];
        const auto standing=FitGoatGroundPose(terrain,surface,lying.x,lying.z,lying.heading,0,0,lying.scale);
        check(standing.origin.y-lying.pose.origin.y>.2f&&standing.origin.y-lying.pose.origin.y<.4f,
            "the lying goat's body does not settle lower");
        for(const auto& leg:lying.pose.legs) {
            const auto sole=lying.pose.ToWorld({leg.hoof.x,leg.hoof.y-GoatLegPose::HoofHeight,leg.hoof.z});
            check(std::abs(sole.y-surface.Sample(sole.x,sole.z).height)<.12f,"a lying goat's hoof leaves the ground");
            check(!leg.reachLimited,"a lying goat's folded leg is flagged unreachable");
        }
        bool up=false;
        for(int frame=0;frame<12&&!up;++frame) { liers.Update(.05f,{lying.x+10,lying.y+1.6f,lying.z}); up=liers.Goats()[1].lieAmount<.2f&&!liers.Lying(1); }
        check(up,"the walker at ten metres does not get the lying goat up within half a second");
    }
    {
        // The tail flick: within twenty seconds every goat has flicked its tail, the swing
        // reaching both sides, and hangs still between flicks; the mesh carries the swing to
        // the tail's tip and nowhere else.
        GoatHerd flickers(terrain,jungle,occupied);
        std::array<float,GoatHabitat::Count> left{},right{}; std::array<bool,GoatHabitat::Count> still{};
        for(int frame=0;frame<400;++frame) {
            flickers.Update(.05f,{1000,0,1000});
            for(std::size_t i=0;i<GoatHabitat::Count;++i) {
                const float swing=flickers.Goats()[i].tailSwing;
                left[i]=std::min(left[i],swing);right[i]=std::max(right[i],swing);
                if(swing==0&&flickers.WagCount()>0) still[i]=true;
            }
        }
        bool all=true;
        for(std::size_t i=0;i<GoatHabitat::Count;++i) all=all&&left[i]<-.6f&&right[i]>.6f&&still[i];
        check(flickers.WagCount()>=GoatHabitat::Count&&all,"the goats do not flick their tails side to side and hang them still between");
        const auto& subject=flickers.Goats()[0];
        const auto stillMesh=BuildGoatMesh(subject.pose,subject.grazingPose,true,0.0f);
        const auto swungMesh=BuildGoatMesh(subject.pose,subject.grazingPose,true,1.0f);
        std::size_t moved=0; float furthest=0; bool onlyTail=true;
        for(std::size_t v=0;v<stillMesh.vertices.size()&&v<swungMesh.vertices.size();++v) {
            const float dx=swungMesh.vertices[v].position.x-stillMesh.vertices[v].position.x;
            const float dy=swungMesh.vertices[v].position.y-stillMesh.vertices[v].position.y;
            const float dz=swungMesh.vertices[v].position.z-stillMesh.vertices[v].position.z;
            if(std::abs(dx)+std::abs(dy)+std::abs(dz)>1e-4f) {
                ++moved; furthest=std::max(furthest,std::abs(dx));
                if(stillMesh.vertices[v].part!=GoatPart::Tail) onlyTail=false;
            }
        }
        check(stillMesh.vertices.size()==swungMesh.vertices.size()&&moved>0&&onlyTail&&furthest>.1f,"the tail swing does not move the tail alone");
    }
    const std::array<CircularObstacle,1> blocked{{{0,0,400}}};
    bool rejected=false;
    try { GoatHerd impossible(terrain,jungle,blocked); } catch(const std::runtime_error&) { rejected=true; }
    check(rejected,"impossible goat habitat silently accepted");
    std::cout<<"Goat herd CPU tests passed; this is not a live rendering audit\n";
}
