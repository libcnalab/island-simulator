#include "IslandSimulator/CrocodileGroup.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include "IslandSimulator/WalkerShadow.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {void Check(bool value,const char* message){if(!value)throw std::runtime_error(message);}}

int main() {
    const island::IslandTerrain terrain;
    island::CrocodileGroup first(terrain),second(terrain);
    Check(first.Crocodiles().size()==3&&first.Homes().size()==3,"wrong crocodile population");
    const float mouthX=island::StreamRoute::SourceX-island::StreamRoute::NominalLength;
    const float mouthZ=island::StreamRoute::CentreZ(mouthX);
    bool basking=false,swimming=false;
    for(std::size_t i=0;i<first.Crocodiles().size();++i) {
        const auto& state=first.Crocodiles()[i];const auto& home=first.Homes()[i];
        Check(std::hypot((home.landX+home.waterX)*.5f-mouthX,
                         (home.landZ+home.waterZ)*.5f-mouthZ)<26.0f,
              "crocodile home is not at the stream estuary");
        Check(first.IsValidHabitat(state.x,state.z,home),"initial crocodile habitat invalid");
        std::cout<<"crocodile "<<i<<" position "<<state.x<<','<<state.y<<','<<state.z
                 <<" land "<<home.landX<<','<<home.landZ<<" water "
                 <<home.waterX<<','<<home.waterZ<<'\n';
        basking|=state.behavior==island::CrocodileBehavior::Bask;
        swimming|=state.behavior==island::CrocodileBehavior::Swim;
    }
    Check(basking&&swimming,"initial population lacks basking and swimming animals");
    {
        // On the drawn ground (the stream's carved bed at the mouth among its patches) a hauled-out
        // crocodile lies on what is drawn, not on the analytic sheet the bed was carved from.
        const island::RenderedTerrainSurface plain(terrain);
        const island::StreamBedPatch bed(island::StreamCourse(terrain),plain);
        island::RenderedTerrainSurface drawn(terrain);
        drawn.SetLocalPatches({&bed,nullptr});
        island::CrocodileGroup settled(terrain);
        settled.SetGround(&drawn);
        for(int step=0;step<30;++step) settled.Update(.1f,NAN,0,0);
        bool lying=true;std::size_t onBed=0;
        for(const auto& state:settled.Crocodiles()) {
            if(state.submerged>0.0f) continue;
            const float ground=island::DrawnOrAnalyticHeight(drawn,terrain,state.x,state.z);
            lying=lying&&std::abs(state.y-(ground+.10f*state.scale))<.06f;
            if(bed.Contains(state.x,state.z)) ++onBed;
        }
        std::cout<<"crocodiles hauled out on the carved stream bed: "<<onBed<<'\n';
        Check(lying,"a hauled-out crocodile does not lie on the drawn ground");
    }
    {
        // A crocodile's sun shadow is laid from the renderer's frame: a long low figure, the
        // snout some two metres ahead, the tail's tip three and a half behind, nothing above
        // its back, and the barrel's shadow inside it.
        island::CrocodileState basking;
        basking.headingRadians=0.0f;
        const auto limbs=island::walker_shadow::CrocodileLimbs(basking);
        float front=-1e9f,back=1e9f,highest=-1e9f,lowest=1e9f;
        for(const auto& limb:limbs) {
            if(!(limb.radius>0.0f)) continue;
            front=std::max({front,limb.ax,limb.bx});back=std::min({back,limb.ax,limb.bx});
            highest=std::max({highest,limb.ay+limb.radius,limb.by+limb.radius});
            lowest=std::min({lowest,limb.ay-limb.radius,limb.by-limb.radius});
        }
        const auto figure=island::walker_shadow::Lay(limbs,0,0,0,-.45f,.82f,-.34f,.7f);
        Check(front>2.0f&&front<2.4f&&back<-3.5f&&back>-4.0f&&highest<.8f&&lowest>-.1f&&lowest<.1f
            &&island::walker_shadow::SignedDistance(figure,.45f/.82f*.34f,.34f/.82f*.34f)<-.1f,
            "a crocodile's sun shadow is not laid from its renderer's frame");
    }
    island::CrocodileGroup obstacleAware(terrain);
    const auto blocked=obstacleAware.Crocodiles()[2U];
    const std::array obstacles={island::CircularObstacle{blocked.x,blocked.z,2.0f}};
    obstacleAware.SetObstacles(obstacles);
    for(const auto& state:obstacleAware.Crocodiles())
        Check(std::hypot(state.x-blocked.x,state.z-blocked.z)>=4.65f,
              "crocodile placement overlaps a static world obstacle");
    for(int frame=0;frame<7200;++frame) {
        first.Update(1.0f/60.0f,1000,1000,1000);
        second.Update(1.0f/60.0f,1000,1000,1000);
    }
    for(std::size_t i=0;i<first.Crocodiles().size();++i) {
        const auto& a=first.Crocodiles()[i];const auto& b=second.Crocodiles()[i];
        Check(first.IsValidHabitat(a.x,a.z,first.Homes()[i]),"crocodile left estuary habitat");
        Check(a.x==b.x&&a.y==b.y&&a.z==b.z&&a.behavior==b.behavior,
              "crocodile simulation is not deterministic");
    }
    const auto before=first.Crocodiles()[0];
    for(int frame=0;frame<600;++frame)
        first.Update(1.0f/60.0f,before.x,before.y+1,before.z);
    const auto after=first.Crocodiles()[0];
    Check(after.behavior==island::CrocodileBehavior::Retreat,
          "nearby crocodile does not retain retreat state");
    Check(std::hypot(after.x-before.x,after.z-before.z)>2.0f,
          "nearby crocodile does not move away toward water");
    std::cout<<"Three deterministic estuary crocodiles bask, swim and retreat from the player\n";
}
