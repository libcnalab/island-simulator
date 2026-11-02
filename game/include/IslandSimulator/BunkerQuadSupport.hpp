#pragma once

#include "IslandSimulator/BunkerLayout.hpp"
#include "IslandSimulator/QuadBike.hpp"
#include <cmath>
#include <optional>

namespace island
{
    // Front wheels reach the opening before the body centre. Selecting the floor
    // only after that centre enters made them hit the hidden heightfield above it,
    // then switch between that hill and the floor, injecting a large spring impulse.
    [[nodiscard]] inline std::optional<BikeGround> SampleBunkerQuadGround(
        const BunkerLayout& bunker,float x,float z,float currentFeetY,bool inside) noexcept
    {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(currentFeetY)) return std::nullopt;
        const auto local=bunker.Local({x,bunker.EntranceGroundY(),z});
        float floor=0;
        // The same height hint as walking keeps a quad ON the ridge above the rooms
        // on that ridge. Once inside, retain the existing floor continuation under
        // footprint points reaching into the wall; horizontal collision handles walls.
        if(!bunker.SupportHeight(x,z,currentFeetY,floor)
            && !(inside&&local.z>=BunkerLayout::CorridorFrontDepth)) return std::nullopt;
        const auto floorAt=[&](float px,float pz) {
            const auto p=bunker.Local({px,bunker.EntranceGroundY(),pz});
            return bunker.EntranceGroundY()+bunker.FloorRelative(p.z);
        };
        const float slopeX=(floorAt(x+.25f,z)-floorAt(x-.25f,z))/.5f;
        const float slopeZ=(floorAt(x,z+.25f)-floorAt(x,z-.25f))/.5f;
        const float length=std::sqrt(slopeX*slopeX+1.0f+slopeZ*slopeZ);
        BikeGround result;
        result.height=floorAt(x,z);
        result.normal={-slopeX/length,1.0f/length,-slopeZ/length};
        return result;
    }
}
