#include "IslandSimulator/WhaleSkeletonLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
float WhaleSkeletonPlacement::SpineAcross(const float along) noexcept {
    return 0.55f*std::sin(along*0.33f)-0.25f*along/SpineLength;
}

std::array<CircularObstacle,3> WhaleSkeletonPlacement::Obstacles() const noexcept {
    const auto braincase=At(0.15f,0.0f),rostrum=At(-2.1f,0.0f),jaws=At(-1.2f,0.0f);
    return {{{braincase[0],braincase[1],1.25f},{rostrum[0],rostrum[1],0.95f},{jaws[0],jaws[1],1.7f}}};
}

bool WhaleSkeletonLayout::Fits(const IslandTerrain& terrain,const WhaleSkeletonPlacement& placement,
                               const std::span<const CircularObstacle> obstacles) {
    // The bones' ground: from beyond the rostrum's tip to past the tail, the ribs' spread either side.
    float lowest=1e9f,highest=-1e9f;
    for(float along=-WhaleSkeletonPlacement::SkullLength-0.5f;along<=WhaleSkeletonPlacement::SpineLength+0.5f;along+=0.75f) {
        for(const float seaward:{-2.6f,0.0f,2.6f}) {
            const auto p=placement.At(along,seaward);
            const TerrainSample sample=terrain.Sample(p[0],p[1]);
            if(sample.surface!=TerrainSurface::DrySand||sample.slopeRadians>0.16f||sample.height<0.35f) return false;
            lowest=std::min(lowest,sample.height);highest=std::max(highest,sample.height);
        }
    }
    if(highest-lowest>1.1f) return false;
    const auto centre=placement.At(WhaleSkeletonPlacement::SpineLength*0.5f-WhaleSkeletonPlacement::SkullLength*0.5f,0.0f);
    const float reach=(WhaleSkeletonPlacement::SpineLength+WhaleSkeletonPlacement::SkullLength)*0.5f+ClearRadius;
    for(const CircularObstacle& obstacle:obstacles) {
        // only the obstacles near the bones' line count
        const float dx=obstacle.x-centre[0],dz=obstacle.z-centre[1];
        if(dx*dx+dz*dz>reach*reach) continue;
        const float along=dx*placement.dirX+dz*placement.dirZ;
        const float across=dx*placement.seaX+dz*placement.seaZ;
        const float half=(WhaleSkeletonPlacement::SpineLength+WhaleSkeletonPlacement::SkullLength)*0.5f;
        const float outside=std::max(std::abs(along)-half,0.0f);
        const float gap=std::hypot(outside,std::max(std::abs(across)-2.6f,0.0f))-obstacle.radius;
        if(gap<2.5f) return false;
    }
    return true;
}

std::optional<WhaleSkeletonPlacement> WhaleSkeletonLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    constexpr int Steps=144;
    constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    for(int step=0;step<Steps;++step) {
        const float angle=0.6f+Tau*static_cast<float>(step)/static_cast<float>(Steps);
        const float shore=terrain.ShoreRadius(angle);
        // the skull's back up the beach from the shoreline, the spine along the shore (sunwise)
        const float radius=shore-InlandFromShore;
        WhaleSkeletonPlacement placement;
        placement.x=std::cos(angle)*radius;placement.z=std::sin(angle)*radius;
        placement.seaX=std::cos(angle);placement.seaZ=std::sin(angle);
        placement.dirX=-std::sin(angle);placement.dirZ=std::cos(angle);
        bool far=true;
        for(const auto& point:avoid) {
            const auto mid=placement.At(WhaleSkeletonPlacement::SpineLength*0.4f,0.0f);
            far=far&&std::hypot(point[0]-mid[0],point[1]-mid[1])>keepAway+WhaleSkeletonPlacement::SpineLength*0.6f;
        }
        if(!far) continue;
        // the shore curves: lie along the chord, the sea still across it at the tail's end
        if(!Fits(terrain,placement,obstacles)) continue;
        return placement;
    }
    return std::nullopt;
}
}
