#include "IslandSimulator/IdolStatueLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::optional<IdolStatuePlacement> IdolStatueLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<IdolStatuePlacement> best;
    constexpr float Step=6.0f,Extent=260.0f;
    for(float z=-Extent;z<=Extent;z+=Step) for(float x=-Extent;x<=Extent;x+=Step) {
        if(terrain.SignedShoreDistance(x,z)<MinimumShoreDistance) continue;
        const float centreHeight=terrain.Height(x,z);
        if(centreHeight<MinimumHeight||centreHeight>MaximumHeight) continue;
        if(best&&centreHeight<=best->groundY) continue;
        bool far=true;
        for(const auto& point:avoid) far=far&&std::hypot(point[0]-x,point[1]-z)>keepAway;
        if(!far) continue;
        // level enough over the platform and a little round it
        float low=centreHeight,high=centreHeight;
        for(const float dx:{-3.5f,0.0f,3.5f}) for(const float dz:{-3.5f,0.0f,3.5f}) {
            const float h=terrain.Height(x+dx,z+dz);
            low=std::min(low,h);high=std::max(high,h);
        }
        if(high-low>MaximumRise) continue;
        bool clear=true;
        for(const CircularObstacle& obstacle:obstacles) {
            const float gap=std::hypot(obstacle.x-x,obstacle.z-z)-obstacle.radius;
            if(gap<IdolStatuePlacement::PlatformHalfWidth+ClearRadius) {clear=false;break;}
        }
        if(!clear) continue;
        const float length=std::max(std::hypot(x,z),1.0f);
        best=IdolStatuePlacement{x,z,x/length,z/length,high};
    }
    return best;
}
}
