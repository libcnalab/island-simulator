#include "IslandSimulator/CrocodilePose.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include "IslandSimulator/WalkerShadow.hpp"
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;namespace cp=crocodile_pose;
    const auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
    for(int frame=0;frame<720;++frame) {
        const float phase=cp::Tau*frame/720;
        for(float speed:{.05f,.34f,1.05f}) {
            int support=0;
            for(int leg=0;leg<4;++leg) {
                const auto p=cp::Sample(phase,speed,leg);support+=p.stance;
                require(std::isfinite(p.along)&&p.lift>=0&&p.lift<=.08001f,
                        "Crocodile step is non-finite or has an excessive lift");
                if(p.stance&&cp::Sample(phase+.001f,speed,leg).stance) {
                    constexpr float distance=.00001f;
                    const auto q=cp::Sample(cp::Advance(phase,distance,speed,1),speed,leg);
                    if(q.stance)require(std::abs(q.along-p.along+distance)<.00001f,
                        "Crocodile stance foot slides relative to accepted straight travel");
                }
            }
            require(support>=2,"Heavy low crawl loses overlapping ground support");
        }
        for(int leg=0;leg<4;++leg) {
            const auto p=cp::Sample(phase,0,leg);
            require(p.along==0&&p.lift==0&&p.stance,"Resting feet keep marching");
        }
    }
    require(cp::Sample(.2f,.34f,0).along!=cp::Sample(.2f,.34f,2).along,
            "Same-side front and rear limbs remain synchronized");
    require(cp::Advance(1,0,.34f,1)==1&&cp::Advance(1,.5f,0,1)==1,
            "Blocked or stopped travel invents gait phase");
    CrocodileState swimmer;swimmer.submerged=1;swimmer.gaitSpeed=1;
    for(float fold:{0.0f,.5f,1.0f}) {
        swimmer.legFold=fold;
        for(int leg=0;leg<4;++leg)require(std::abs(cp::SampleLeg(swimmer,leg).wrist.y-std::lerp(.025f,.14f,fold))<1e-6f,
            "Land support correction changes folded swimming limb height");
    }
    const IslandTerrain terrain;RenderedTerrainSurface surface(terrain);
    const StreamBedPatch bed(StreamCourse(terrain),surface);surface.SetLocalPatches({&bed,nullptr});
    CrocodileGroup group(terrain);group.SetGround(&surface);const auto start=group.Crocodiles()[0];
    int planted=0,swing=0;
    for(int frame=0;frame<240;++frame) {
        group.Update(.1f,start.x+80,start.y+2,start.z);
        for(const auto& c:group.Crocodiles()) {
            if(c.submerged!=0||c.legFold!=0)continue;
            const float fx=std::cos(c.headingRadians),fz=std::sin(c.headingRadians);
            const auto world=[&](cp::Point p) {
                return std::array<float,3>{c.x+(-fz*p.x+fx*p.z)*c.scale,c.y+p.y*c.scale,
                                          c.z+(fx*p.x+fz*p.z)*c.scale};
            };
            const auto shadows=walker_shadow::CrocodileLimbs(c);
            for(int i=0;i<4;++i) {
                const auto leg=cp::SampleLeg(c,i);const auto foot=world(leg.foot);
                const float gap=foot[1]-.045f*c.scale-surface.Height(foot[0],foot[2]);
                const auto step=cp::Sample(c.gaitPhase,c.gaitSpeed,i);
                require(gap>=-.0001f&&gap<=.081f*c.scale,"Crocodile foot floats or penetrates the rendered bed");
                if(step.stance){++planted;require(std::abs(gap)<.0001f,"Stance foot is detached from the ground");}
                else{++swing;require(std::abs(gap-step.lift*c.scale)<.0001f,"Swing loses its terrain-relative clearance");}
                const auto shoulder=world(leg.shoulder),elbow=world(leg.elbow);
                const auto& shadow=shadows[8+2*i];
                require(std::hypot(shadow.ax-shoulder[0],shadow.az-shoulder[2])<.0001f
                    &&std::abs(shadow.ay-shoulder[1])<.0001f
                    &&std::hypot(shadow.bx-elbow[0],shadow.bz-elbow[2])<.0001f
                    &&std::abs(shadow.by-elbow[1])<.0001f,"Shadow limb disagrees with the drawn pose");
            }
        }
    }
    require(planted>100&&swing>10,"Real crawl did not exercise support and swing");
    std::cout<<"Four-beat ground gait, stance travel, swim control and shared terrain/shadow pose: "
             <<planted<<" support, "<<swing<<" swing samples\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
