#include "IslandSimulator/UfoWreckLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::array<CircularObstacle,3> UfoWreckPlacement::Obstacles() const noexcept {
    const auto hull=At(0.0f,0.0f),rim=At(2.2f,0.4f),strut=At(-3.9f,-3.6f);
    return {{{hull[0],hull[1],3.7f},{rim[0],rim[1],3.2f},{strut[0],strut[1],0.7f}}};
}

std::array<float,2> UfoWreckPlacement::DebrisSpot(const std::size_t index) noexcept {
    static constexpr std::array<std::array<float,2>,DebrisCount> Spots{{
        {{-9.5f,2.2f}},{{-6.5f,-5.8f}},{{-12.5f,-1.4f}},{{1.5f,7.4f}},{{-3.0f,6.9f}},{{-15.0f,3.4f}}}};
    return Spots[index%DebrisCount];
}

std::optional<UfoWreckPlacement> UfoWreckLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<UfoWreckPlacement> best;
    float bestClearance=MinimumClearance;
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
        for(const float dx:{-8.0f,0.0f,8.0f}) for(const float dz:{-8.0f,0.0f,8.0f}) {
            const float h=terrain.Height(x+dx,z+dz);
            low=std::min(low,h);high=std::max(high,h);
        }
        if(high-low>MaximumRise*2.0f||low<1.0f) continue;
        const float heading=2.4f+std::fmod(std::abs(x*0.011f+z*0.017f),1.0f);
        bestClearance=clearance;
        best=UfoWreckPlacement{x,z,std::cos(heading),std::sin(heading),terrain.Height(x,z)};
    }
    return best;
}
}
