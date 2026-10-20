#pragma once
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <cmath>

namespace island {
// Where a flying bird's shadow falls: the bird's position carried along the fixed sun's rays
// down to the rendered ground, refined twice for the ground's own height. Only a bird clear
// of the ground and not too high, over land, casts one; the sea takes none.
struct BirdShadowPoint { float x=0,z=0,height=0; bool valid=false; };
struct BirdShadowProjection {
    static constexpr float MinimumHeight=0.3f,MaximumHeight=40.0f;
    [[nodiscard]] static BirdShadowPoint Project(const IslandTerrain& terrain,float x,float y,float z) noexcept {
        BirdShadowPoint point;
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)) return point;
        const float slopeX=environment::SunDirectionX/environment::SunDirectionY;
        const float slopeZ=environment::SunDirectionZ/environment::SunDirectionY;
        float sx=x,sz=z,height=y-terrain.Height(x,z);
        for(int pass=0;pass<2;++pass) {
            sx=x-slopeX*height;sz=z-slopeZ*height;
            height=y-terrain.Height(sx,sz);
        }
        const float ground=terrain.Height(sx,sz);
        point.x=sx;point.z=sz;point.height=height;
        point.valid=height>=MinimumHeight&&height<=MaximumHeight&&ground>0.0f;
        return point;
    }
};
}
