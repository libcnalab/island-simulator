#pragma once
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <cmath>

namespace island {
/** @brief Where a swimming animal's shadow falls on the seabed: its position carried along
 *  the fixed sun's rays down to the sandy bottom, refined twice for the bottom's own height
 *  (refraction left aside). Only an animal clear of the bottom and not too far above it,
 *  over ground that lies under the sea, casts one; land takes none here (the birds' own
 *  projection serves the land). */
struct SeabedShadowPoint { float x=0,z=0,height=0; bool valid=false; };
struct SeabedShadowProjection {
    static constexpr float MinimumHeight=0.15f,MaximumHeight=9.0f;
    [[nodiscard]] static SeabedShadowPoint Project(const IslandTerrain& terrain,float x,float y,float z) noexcept {
        SeabedShadowPoint point;
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
        point.valid=height>=MinimumHeight&&height<=MaximumHeight&&ground<0.0f&&y<=0.3f;
        return point;
    }
};
}
