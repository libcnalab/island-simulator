#include "IslandSimulator/LandCrabMigration.hpp"
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

LandCrabMigration::LandCrabMigration(const IslandTerrain& terrain) {
    lanes_.reserve(LaneCount);
    for(int k=0;k<LaneCount;++k) {
        // a little irregular round the coast
        const float bearing=2.0f*Pi*(static_cast<float>(k)+.8f*(Hash(static_cast<std::uint32_t>(k)*7U+1U)-.5f))/LaneCount;
        const float shore=terrain.ShoreRadius(bearing);
        const float c=std::cos(bearing),s=std::sin(bearing);
        const float startR=shore-LaneLength,endR=shore-.3f;
        if(!(startR>10.0f)) continue;
        // not down the headlands' rock steps and cliffs, nor where the lane climbs out of a hollow
        bool walkable=true;
        float previous=terrain.Height(c*startR,s*startR);
        for(float r=startR+2.0f;r<=endR&&walkable;r+=2.0f) {
            const auto sample=terrain.Sample(c*r,s*r);
            walkable=sample.surface!=TerrainSurface::RockyGround&&sample.slopeRadians<.55f&&sample.height<previous+1.2f;
            previous=sample.height;
        }
        if(!walkable) continue;
        lanes_.push_back({c*startR,s*startR,c*endR,s*endR});
    }
}

float LandCrabMigration::Strength(const float darkness,const float wetness) noexcept {
    if(!std::isfinite(darkness)||!std::isfinite(wetness)) return 0.0f;
    const float wet=std::clamp((wetness-.30f)/.40f,0.0f,1.0f);
    return std::clamp(darkness,0.0f,1.0f)*wet*wet*(3.0f-2.0f*wet);
}

void LandCrabMigration::Gather(const float seconds,const float strength,const float x,const float z,const float radius,
                               const float walkerX,const float walkerZ,const std::function<float(float,float)>& ground,
                               std::vector<MigratingCrab>& out,const std::size_t cap) const {
    if(!(strength>0.01f)||!std::isfinite(seconds)||!std::isfinite(x)||!std::isfinite(z)||!ground) return;
    const std::size_t first=out.size();
    // the lanes within reach, nearest first (the cap then drops the farthest crabs)
    struct Near {std::size_t lane;float distance;};
    std::vector<Near> near;
    for(std::size_t i=0;i<lanes_.size();++i) {
        const Lane& lane=lanes_[i];
        const float dx=lane.endX-lane.startX,dz=lane.endZ-lane.startZ;
        const float t=std::clamp(((x-lane.startX)*dx+(z-lane.startZ)*dz)/(dx*dx+dz*dz),0.0f,1.0f);
        const float d=std::hypot(lane.startX+dx*t-x,lane.startZ+dz*t-z);
        if(d<radius+2.0f) near.push_back({i,d});
    }
    std::sort(near.begin(),near.end(),[](const Near& a,const Near& b){return a.distance<b.distance;});
    for(const Near& n:near) {
        const Lane& lane=lanes_[n.lane];
        const float dx=lane.endX-lane.startX,dz=lane.endZ-lane.startZ;
        const float length=std::hypot(dx,dz);
        const float ux=dx/length,uz=dz/length;   // toward the sea
        const float travel=std::atan2(uz,ux);
        for(int k=0;k<CrabsPerLane;++k) {
            const auto seed=static_cast<std::uint32_t>(n.lane)*131U+static_cast<std::uint32_t>(k)*17U;
            // the fewer the crabs out, the fewer on each lane
            if(Hash(seed+3U)>strength) continue;
            const float pace=Speed*(.7f+.6f*Hash(seed+5U));
            const float along=std::fmod((static_cast<float>(k)+Hash(seed+7U))*length/CrabsPerLane+seconds*pace,length);
            // spread across the lane and wandering a little from side to side
            float across=(Hash(seed+11U)-.5f)*1.9f+.25f*std::sin(seconds*.7f*(.6f+Hash(seed+13U))+Hash(seed+17U)*6.0f);
            float cx=lane.startX+ux*along-uz*across,cz=lane.startZ+uz*along+ux*across;
            if(std::hypot(cx-x,cz-z)>radius) continue;
            // stepping aside from the walker's feet
            const float wx=cx-walkerX,wz=cz-walkerZ,w=std::hypot(wx,wz);
            if(w<ScatterRadius) {
                // (right under the foot: aside across the lane)
                const float ax=w>1e-3f?wx/w:-uz,az=w>1e-3f?wz/w:ux;
                cx=walkerX+ax*ScatterRadius;cz=walkerZ+az*ScatterRadius;
            }
            MigratingCrab crab;
            crab.x=cx;crab.z=cz;crab.y=ground(cx,cz);
            // sideways to the way it goes, the side it leads with its own
            crab.heading=travel+(Hash(seed+19U)<.5f?.5f:-.5f)*Pi+.25f*(Hash(seed+23U)-.5f);
            crab.stride=seconds*pace*48.0f+Hash(seed+29U)*6.2831853f;
            // up out of the litter over its first metre, under the surf over its last
            crab.scale=std::clamp(along/1.0f,0.0f,1.0f)*std::clamp((length-along)/1.0f,0.0f,1.0f)*(.85f+.3f*Hash(seed+31U));
            crab.tone=static_cast<std::uint8_t>(Hash(seed+37U)*2.999f);
            out.push_back(crab);
            if(out.size()-first>=cap) return;
        }
    }
}

}
