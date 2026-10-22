#pragma once
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <array>
#include <cmath>

namespace island {
/** @brief Where a swimming animal's shadow falls on the seabed: its position carried along
 *  the light's rays under the water (slopeX/slopeZ, the horizontal run per metre of depth: the
 *  hour's sun refracted at the surface, see Refracted; the former fixed sun unrefracted by
 *  default) down to the sandy bottom, refined twice for the bottom's own height. Only an
 *  animal clear of the bottom and not too far above it,
 *  over ground that lies under the sea, casts one; land takes none here (the birds' own
 *  projection serves the land). */
struct SeabedShadowPoint { float x=0,z=0,height=0; bool valid=false; };
struct SeabedShadowProjection {
    static constexpr float MinimumHeight=0.15f,MaximumHeight=9.0f;
    /** @brief The run per metre of depth of a light toward (x, y, z) once it has crossed the
     *  surface: Snell's law bends the ray toward the vertical (water's index 1.333). */
    [[nodiscard]] static std::array<float,2> Refracted(const float x,const float y,const float z) noexcept {
        const float horizontal=std::hypot(x,z),length=std::hypot(horizontal,y);
        if(!(length>1e-6f)||!(y>0.0f)||!(horizontal>1e-6f)) return {0.0f,0.0f};
        const float sine=horizontal/length/1.333f;
        const float run=sine/std::sqrt(std::max(1.0f-sine*sine,1e-6f));
        return {x/horizontal*run,z/horizontal*run};
    }
    [[nodiscard]] static SeabedShadowPoint Project(const IslandTerrain& terrain,float x,float y,float z,
        const float slopeX=environment::SunDirectionX/environment::SunDirectionY,
        const float slopeZ=environment::SunDirectionZ/environment::SunDirectionY) noexcept {
        SeabedShadowPoint point;
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(slopeX)||!std::isfinite(slopeZ)) return point;
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
