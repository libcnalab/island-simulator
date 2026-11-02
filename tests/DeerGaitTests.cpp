#include "IslandSimulator/DeerHoofSupport.hpp"
#include "IslandSimulator/DeerGait.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WalkerShadow.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message) {
        if(!ok) throw std::runtime_error(message);
    };
    std::size_t contacts=0,swings=0;
    float worstSlip=0;
    // Observe the actual shared renderer/shadow joint endpoints in world space. A
    // stance hoof must stay put while differently sized bodies move over flat ground.
    for(float scale:{.7f,.84f,1.045f}) for(float speed:{.1f,.4f,.78f,1.5f,3.1f})
        for(float heading:{0.0f,.8f}) {
            DeerState deer;deer.y=3.5f;deer.speed=speed;deer.scale=scale;
            deer.headingRadians=heading;deer.gaitPhase=.17f;
            for(int frame=0;frame<720;++frame) {
                const auto previous=deer;
                const auto before=walker_shadow::DeerLimbs(previous);
                constexpr float dt=1.0f/240.0f;
                deer.x+=std::cos(heading)*speed*dt;
                deer.z+=std::sin(heading)*speed*dt;
                deer.gaitPhase=deer_gait::Advance(deer.gaitPhase,speed*dt,speed,scale);
                const auto after=walker_shadow::DeerLimbs(deer);
                for(int foot=0;foot<4;++foot) {
                    const auto a=deer_gait::Sample(previous.gaitPhase,speed,foot);
                    const auto b=deer_gait::Sample(deer.gaitPhase,speed,foot);
                    const auto& p=before[3+3*foot];const auto& q=after[3+3*foot];
                    if(!a.stance||!b.stance) {++swings;continue;}
                    const float slip=std::hypot(q.bx-p.bx,q.bz-p.bz);
                    worstSlip=std::max(worstSlip,slip);
                    require(slip<.000004f,"Steady stance hoof slides with the deer body");
                    require(std::abs(q.by-deer.y-DeerHoofSupport::RearRadius*scale)<2e-6f,
                            "Steady stance hoof lost ground contact");
                    ++contacts;
                }
            }
        }
    require(contacts>10000&&swings>10000,"Insufficient stance/swing coverage");
    // No motion from elapsed time alone, blocked travel, invalid input or a stopped body.
    const float nan=std::numeric_limits<float>::quiet_NaN();
    const float inf=std::numeric_limits<float>::infinity();
    for(float bad:{-1.0f,0.0f,nan,inf}) {
        require(deer_gait::Advance(.3f,bad,.78f,1)==.3f,"Invalid/zero travel advances gait");
        require(deer_gait::Advance(.3f,.1f,bad,1)==.3f,"Invalid/zero speed advances gait");
        require(deer_gait::Advance(.3f,.1f,.78f,bad)==.3f,"Invalid/zero scale advances gait");
    }
    for(float phase:{-.00000001f,-7.0f,0.0f,7.0f,nan,inf}) {
        const float normalized=deer_gait::Advance(phase,.1f,.78f,1);
        require(std::isfinite(normalized)&&normalized>=0&&normalized<deer_gait::Tau,
                "Gait phase is outside its wrapped finite range");
    }
    std::cout<<"flat planted contacts="<<contacts<<" swing samples="<<swings
             <<" worst world slip="<<worstSlip<<'\n';
    const IslandTerrain terrain;const RenderedTerrainSurface surface(terrain);
    DeerHerd herd(terrain,{});herd.SetGround(&surface);
    std::size_t moving=0,steady=0,stopped=0;float travel=0,worstRatio=0;
    for(int frame=0;frame<3600;++frame) {
        const auto before=herd.Deer().front();
        herd.Update(1.0f/120.0f,before.x+60,before.y+20,before.z);
        const auto after=herd.Deer().front();
        require(after.simulationTier==WildlifeSimulationTier::Full,"QA observer throttles live deer");
        const float distance=std::hypot(after.x-before.x,after.z-before.z);
        travel+=distance;
        if(distance==0) {
            require(after.gaitPhase==before.gaitPhase,"Blocked/stationary deer advances gait");
            ++stopped;continue;
        }
        ++moving;
        const auto a=MakeDeerSupportFrame(before,&surface),b=MakeDeerSupportFrame(after,&surface);
        // Only steady motion on one support plane has the planted-foot guarantee;
        // turning, acceleration and triangle-normal transitions remain procedural.
        if(before.speed!=after.speed||std::abs(before.headingRadians-after.headingRadians)>1e-5f
            ||before.bedAmount>0||after.bedAmount>0||before.stamp>0||after.stamp>0
            ||std::hypot(a.up.x-b.up.x,a.up.y-b.up.y,a.up.z-b.up.z)>1e-6f) continue;
        const auto p=walker_shadow::DeerLimbs(before,&surface),q=walker_shadow::DeerLimbs(after,&surface);
        for(int foot=0;foot<4;++foot) {
            if(!deer_gait::Sample(before.gaitPhase,before.speed,foot).stance
                ||!deer_gait::Sample(after.gaitPhase,after.speed,foot).stance) continue;
            const auto& left=p[3+3*foot];const auto& right=q[3+3*foot];
            const float slip=std::hypot(right.bx-left.bx,right.bz-left.bz);
            worstRatio=std::max(worstRatio,slip/distance);
            require(slip<distance*.05f+.00002f,"Live steady stance hoof slides over rendered ground");
            ++steady;
        }
    }
    std::cout<<"live distance="<<travel<<" moving="<<moving<<" steady contacts="<<steady
             <<" stopped="<<stopped<<" worst slip/body travel="<<worstRatio<<'\n';
    require(travel>1&&moving>100&&steady>100&&stopped>100,"Insufficient live moving/stopped deer coverage");
}
catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
