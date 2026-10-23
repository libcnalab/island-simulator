#include "IslandSimulator/Mudskippers.hpp"
#include "IslandSimulator/CanopyProfile.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

MudskipperColony::MudskipperColony(const IslandTerrain& terrain,const std::span<const CanopyPlacement> canopy) {
    std::uint32_t index=0;
    for(const CanopyPlacement& tree:canopy) {
        if(tree.variant!=MangroveCanopyVariant) continue;
        // only where the ground under the tree is low, near the water
        if(terrain.Height(tree.x,tree.z)>1.2f) continue;
        patches_.push_back({tree.x,tree.z,MudRadius*(0.85f+0.3f*Hash(index*7U+1U))});
        // the way down to the water: along the fall of the ground
        const float gx=terrain.Height(tree.x+1.0f,tree.z)-terrain.Height(tree.x-1.0f,tree.z);
        const float gz=terrain.Height(tree.x,tree.z+1.0f)-terrain.Height(tree.x,tree.z-1.0f);
        const float g=std::hypot(gx,gz);
        const float wx=g>1e-4f?-gx/g:-tree.x/std::max(1.0f,std::hypot(tree.x,tree.z));
        const float wz=g>1e-4f?-gz/g:-tree.z/std::max(1.0f,std::hypot(tree.x,tree.z));
        for(int k=0;k<PerTree;++k) {
            const auto seed=index*131U+static_cast<std::uint32_t>(k)*17U+5U;
            // out of the roots' spread, mostly on the water's side
            const float angle=std::atan2(wz,wx)+(Hash(seed)-0.5f)*2.4f;
            // (beyond the stilt roots' spread)
            const float r=3.6f+2.4f*Hash(seed+1U);
            const float x=tree.x+std::cos(angle)*r,z=tree.z+std::sin(angle)*r;
            const float h=terrain.Height(x,z);
            if(h<-0.05f||h>1.0f) continue;
            homes_.push_back({x,z,wx,wz,seed});
        }
        ++index;
    }
}

void MudskipperColony::At(const float seconds,const float walkerX,const float walkerZ,
                          const std::function<float(float,float)>& ground,std::vector<Mudskipper>& out) const {
    out.clear();
    if(!ground) return;
    const float t=std::isfinite(seconds)?seconds:0.0f;
    for(const Home& home:homes_) {
        const auto seed=home.seed;
        // a hop every few seconds between two spots a body-length or two apart, out and back
        const float period=5.0f+4.0f*Hash(seed+3U);
        const float phase=std::fmod(t+Hash(seed+5U)*period,period);
        const float hopLength=0.45f;
        const float spread=0.28f+0.2f*Hash(seed+7U);
        const float sideAngle=Hash(seed+9U)*2.0f*Pi;
        const float ax=home.x,az=home.z;
        const float bx=home.x+std::cos(sideAngle)*spread,bz=home.z+std::sin(sideAngle)*spread;
        const float cycle=std::fmod(std::floor((t+Hash(seed+5U)*period)/period),2.0f);
        const bool outward=cycle<0.5f;
        float x=outward?ax:bx,z=outward?az:bz;
        float hop=0.0f;
        float heading=std::atan2(outward?bz-az:az-bz,outward?bx-ax:ax-bx);
        if(phase<hopLength) {
            hop=phase/hopLength;
            const float k=hop;
            x=outward?ax+(bx-ax)*k:bx+(ax-bx)*k;
            z=outward?az+(bz-az)*k:bz+(az-bz)*k;
        } else {
            // resting: facing its last hop's way, turned a little now and then
            heading+=0.4f*std::sin(t*0.13f+Hash(seed+11U)*6.0f);
        }
        // the walker near: away toward the water in a run of quick skips
        const float dx=x-walkerX,dz=z-walkerZ,d=std::hypot(dx,dz);
        if(d<FleeRadius) {
            const float push=(FleeRadius-d);
            const float awayX=d>1e-3f?dx/d:home.towardWaterX,awayZ=d>1e-3f?dz/d:home.towardWaterZ;
            const float fx=awayX*0.6f+home.towardWaterX*0.4f,fz=awayZ*0.6f+home.towardWaterZ*0.4f;
            const float f=std::max(1e-3f,std::hypot(fx,fz));
            x+=fx/f*push*1.4f;z+=fz/f*push*1.4f;
            heading=std::atan2(fz,fx);
            hop=std::fmod(t*2.6f+Hash(seed+13U),1.0f);
        }
        Mudskipper skipper;
        skipper.x=x;skipper.z=z;
        skipper.hop=hop;
        skipper.y=ground(x,z)+std::sin(hop*Pi)*0.10f;
        skipper.heading=heading;
        // the dorsal fin raised now and then, at a neighbour
        const float show=std::fmod(t*0.11f+Hash(seed+17U),1.0f);
        skipper.display=show<0.12f?std::sin(show/0.12f*Pi):0.0f;
        skipper.scale=0.85f+0.3f*Hash(seed+19U);
        skipper.tone=static_cast<std::uint8_t>(Hash(seed+23U)*2.999f);
        out.push_back(skipper);
    }
}

}
