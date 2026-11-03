#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/DeerHoofSupport.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WalkerShadow.hpp"
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;const RenderedTerrainSurface ground(terrain);
    std::size_t contacts=0,turnContacts=0,accelerationContacts=0,swings=0,settled=0;
    float worstSlip=0,worstReach=0;
    for(float dt:{1.0f/120,.05f,.1f}) {
        DeerHerd herd(terrain,{});herd.SetGround(&ground);herd.PreviewBed(0);
        for(int frame=0;frame<static_cast<int>(30/dt);++frame) {
            const auto a=herd.Deer().front();
            const float time=static_cast<float>(frame)*dt;
            const float direction=time<7.5f?1.5707963f:time<15?-1.5707963f:time<22.5f?3.1415926f:0;
            const auto home=herd.Homes().front();
            const float distance=time<22.5f?16.0f:6.0f;
            herd.Update(dt,home.x+std::cos(direction)*distance,a.y+1.7f,home.z+std::sin(direction)*distance);
            const auto b=herd.Deer().front();
            if(!b.contactsValid)continue;
            int supporting=0;
            const auto shadows=walker_shadow::DeerLimbs(b,&ground);
            for(int foot=0;foot<4;++foot) {
                const auto& q=b.feet[static_cast<std::size_t>(foot)];
                const auto pose=MakeDeerLegPose(b,foot,&ground);
                const auto shadow=shadows[3+3*foot];
                require(std::hypot(pose.hoof.x-shadow.bx,pose.hoof.y-shadow.by,pose.hoof.z-shadow.bz)<.000001f,
                    "Rendered leg and contact shadow disagree");
                const float reach=std::hypot(pose.hip.x-pose.hoof.x,pose.hip.y-pose.hoof.y,pose.hip.z-pose.hoof.z)/b.scale;
                worstReach=std::max(worstReach,reach);
                if(!(std::isfinite(reach)&&reach<1.4f))
                    std::cerr<<"escape dt="<<dt<<" frame="<<frame<<" foot="<<foot<<" speed="<<b.speed
                        <<" heading="<<b.headingRadians<<" hip="<<pose.hip.x<<','<<pose.hip.y<<','<<pose.hip.z
                        <<" hoof="<<pose.hoof.x<<','<<pose.hoof.y<<','<<pose.hoof.z
                        <<" planted="<<q.planted<<" lift="<<q.lift<<" reach="<<reach<<'\n';
                require(std::isfinite(reach)&&reach<1.4f,"Anchored hoof has escaped the existing leg envelope");
                const bool planted=q.planted&&!(foot==0&&b.stamp>.0001f);
                supporting+=planted;swings+=!planted;
                if(!planted)continue;
                require(std::abs(pose.hoof.y-pose.support.centre.y)<.000001f,"Planted hoof is above its fitted support");
                if(!a.contactsValid||!a.feet[static_cast<std::size_t>(foot)].planted||(foot==0&&a.stamp>.0001f))continue;
                const auto before=MakeDeerLegPose(a,foot,&ground);
                const float slip=std::hypot(pose.hoof.x-before.hoof.x,pose.hoof.y-before.hoof.y,pose.hoof.z-before.hoof.z);
                worstSlip=std::max(worstSlip,slip);
                require(slip<.00003f,"Live deer stance hoof slides during acceleration or turning");
                require(q.heading==a.feet[static_cast<std::size_t>(foot)].heading,"Planted toes rotate against the ground");
                ++contacts;
                turnContacts+=std::abs(a.headingRadians-b.headingRadians)>.000001f;
                accelerationContacts+=std::abs(a.speed-b.speed)>.000001f;
            }
            require(supporting>=2,"Deer lifts too many supporting hooves together");
            settled+=b.speed==0&&supporting==4;
        }
    }
    require(contacts>2000&&turnContacts>100&&accelerationContacts>100&&swings>100&&settled>100,
        "Insufficient live turn, acceleration, swing or settled support coverage");
    std::cout<<"deer planted pairs="<<contacts<<" turns="<<turnContacts<<" acceleration="<<accelerationContacts
        <<" swings="<<swings<<" settled="<<settled<<" worst slip="<<worstSlip<<" maximum hip/hoof reach="<<worstReach<<'\n';
    return 0;
} catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
