#include "IslandSimulator/PlaneWreckLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::array<CircularObstacle,4> PlaneWreckPlacement::Obstacles() const noexcept {
    const auto nose=At(2.4f,0.0f),root=At(0.0f,0.0f),tail=At(-3.4f,1.2f),wing=At(-0.8f,-6.8f);
    return {{{nose[0],nose[1],1.0f},{root[0],root[1],1.1f},{tail[0],tail[1],1.0f},{wing[0],wing[1],1.6f}}};
}

std::optional<PlaneWreckPlacement> PlaneWreckLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<PlaneWreckPlacement> best;
    float bestClearance=7.0f;
    constexpr float Step=6.0f,Extent=250.0f;
    for(float z=-Extent;z<=Extent;z+=Step) for(float x=-Extent;x<=Extent;x+=Step) {
        if(terrain.SignedShoreDistance(x,z)<MinimumShoreDistance) continue;
        bool far=true;
        for(const auto& point:avoid) far=far&&std::hypot(point[0]-x,point[1]-z)>keepAway;
        if(!far) continue;
        float clearance=1e9f;
        for(const CircularObstacle& obstacle:obstacles) {
            clearance=std::min(clearance,std::hypot(obstacle.x-x,obstacle.z-z)-obstacle.radius);
            if(clearance<=bestClearance) break;
        }
        if(clearance<=bestClearance) continue;
        float low=1e9f,high=-1e9f;
        for(const float dx:{-6.0f,0.0f,6.0f}) for(const float dz:{-6.0f,0.0f,6.0f}) {
            const float h=terrain.Height(x+dx,z+dz);
            low=std::min(low,h);high=std::max(high,h);
        }
        if(high-low>MaximumRise*2.0f||low<1.0f) continue;
        // it came down heading roughly along the slope's fall, here simply from the north-east
        const float heading=0.9f+std::fmod(std::abs(x*0.013f+z*0.007f),1.0f);
        bestClearance=clearance;
        best=PlaneWreckPlacement{x,z,std::cos(heading),std::sin(heading),terrain.Height(x,z)};
    }
    return best;
}
}
