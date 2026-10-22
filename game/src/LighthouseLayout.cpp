#include "IslandSimulator/LighthouseLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
std::optional<LighthousePlacement> LighthouseLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<LighthousePlacement> best;
    constexpr int Steps=180;
    constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    for(int step=0;step<Steps;++step) {
        const float angle=Tau*static_cast<float>(step)/static_cast<float>(Steps);
        const float seaX=std::cos(angle),seaZ=std::sin(angle);
        const float shore=terrain.ShoreRadius(angle);
        for(float back=10.0f;back<=25.0f;back+=2.5f) {
            const float x=seaX*(shore-back),z=seaZ*(shore-back);
            const TerrainSample centre=terrain.Sample(x,z);
            if(centre.height<MinimumHeight||centre.height>MaximumHeight) continue;
            if(centre.surface==TerrainSurface::DrySand||centre.surface==TerrainSurface::WetSand) continue;
            if(best&&centre.height<=best->groundY) continue;
            bool far=true;
            for(const auto& point:avoid) far=far&&std::hypot(point[0]-x,point[1]-z)>keepAway;
            if(!far) continue;
            float low=centre.height,high=centre.height;
            for(const float dx:{-3.5f,0.0f,3.5f}) for(const float dz:{-3.5f,0.0f,3.5f}) {
                const float h=terrain.Height(x+dx,z+dz);
                low=std::min(low,h);high=std::max(high,h);
            }
            if(high-low>MaximumRise) continue;
            bool clear=true;
            for(const CircularObstacle& obstacle:obstacles)
                if(std::hypot(obstacle.x-x,obstacle.z-z)-obstacle.radius<LighthousePlacement::PlinthRadius+ClearRadius) {clear=false;break;}
            if(!clear) continue;
            best=LighthousePlacement{x,z,high,seaX,seaZ};
        }
    }
    return best;
}
}
