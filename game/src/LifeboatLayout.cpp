#include "IslandSimulator/LifeboatLayout.hpp"
#include "IslandSimulator/FreighterLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::array<CircularObstacle,3> LifeboatPlacement::Obstacles() const noexcept {
    const auto bow=At(Length*0.3f,0.0f),middle=At(0.0f,0.0f),stern=At(-Length*0.3f,0.0f);
    return {{{bow[0],bow[1],1.0f},{middle[0],middle[1],1.2f},{stern[0],stern[1],1.0f}}};
}

std::optional<LifeboatPlacement> LifeboatLayout::FindNear(const IslandTerrain& terrain,const FreighterPlacement& ship,
        const std::span<const CircularObstacle> obstacles,const std::span<const std::array<float,2>> avoid,const float keepAway) {
    using P=FreighterPlacement;
    // along the beach from the ladder's side first, then the other: across the ship's axis, at a
    // few heights up the sand
    for(const float side:{1.0f,-1.0f})
    for(float off=P::HalfBeam+14.0f;off<=P::HalfBeam+40.0f;off+=2.0f) {
        for(const float along:{22.0f,26.0f,18.0f,30.0f,14.0f,34.0f}) {
            const FreighterPoint centre=ship.World(along,0.0f,side*off);
            const TerrainSample sample=terrain.Sample(centre.x,centre.z);
            if(sample.surface!=TerrainSurface::DrySand||sample.slopeRadians>0.12f) continue;
            LifeboatPlacement boat;
            boat.x=centre.x;boat.z=centre.z;
            // lying roughly along the shore, the bow turned a little up the beach
            boat.dirX=std::cos(0.35f)*ship.AcrossX()+std::sin(0.35f)*ship.dirX;
            boat.dirZ=std::cos(0.35f)*ship.AcrossZ()+std::sin(0.35f)*ship.dirZ;
            const float norm=std::hypot(boat.dirX,boat.dirZ);
            boat.dirX/=norm;boat.dirZ/=norm;
            bool fits=true;
            for(const float a:{-LifeboatPlacement::Length*0.5f,0.0f,LifeboatPlacement::Length*0.5f})
                for(const float c:{-LifeboatPlacement::HalfBeam,LifeboatPlacement::HalfBeam}) {
                    const auto p=boat.At(a,c);
                    const TerrainSample s=terrain.Sample(p[0],p[1]);
                    fits=fits&&s.surface==TerrainSurface::DrySand&&s.slopeRadians<0.16f
                        &&!ship.Blocks(p[0],p[1],3.0f);
                }
            if(!fits) continue;
            for(const CircularObstacle& o:obstacles)
                fits=fits&&std::hypot(o.x-boat.x,o.z-boat.z)>o.radius+LifeboatPlacement::Length*0.5f+1.5f;
            for(const auto& point:avoid) fits=fits&&std::hypot(point[0]-boat.x,point[1]-boat.z)>keepAway;
            if(!fits) continue;
            boat.groundY=terrain.Height(boat.x,boat.z);
            return boat;
        }
    }
    return std::nullopt;
}
}
