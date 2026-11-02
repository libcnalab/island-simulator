#pragma once

#include "IslandSimulator/BunkerLayout.hpp"
#include "IslandSimulator/QuadBike.hpp"
#include <cmath>
#include <optional>

namespace island
{
    // The complete footprint, not the walker's round capsule, must fit the rooms
    // AND their actual corridor openings. Keep this identical in gameplay and QA.
    [[nodiscard]] inline bool BunkerQuadFootprintBlocked(
        const BunkerLayout& bunker,const QuadBike& quad,float x,float z) noexcept
    {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(quad.Position().y))return true;
        const auto centre=bunker.Local({x,0,z});
        const float floor=bunker.EntranceGroundY()+bunker.FloorRelative(centre.z);
        if(!(centre.z>BunkerLayout::CorridorFrontDepth-1.5f&&centre.z<37.5f
            &&std::abs(centre.x)<12&&std::abs(quad.Position().y-.6f-floor)<1.15f)) return false;
        const auto forward=quad.Forward();const float length=std::hypot(forward.x,forward.z);
        const float fx=length>1e-4f?forward.x/length:0,fz=length>1e-4f?forward.z/length:1;
        const auto at=[&](float along,float across) {
            return BunkerPoint{x+fx*along+fz*across,quad.Position().y,z+fz*along-fx*across};
        };
        for(float along:{-1.0f,0.0f,1.0f}) for(float across:{-.62f,0.0f,.62f}) {
            if(along==0&&across==0) continue;
            const auto p=at(along,across);
            if(bunker.Local(p).z<BunkerLayout::CorridorFrontDepth) continue;
            if(!bunker.ContainsWalkable(p.x,p.z)) return true;
        }
        return bunker.FootprintHitsInteriorWall({at(-1,-.62f),at(1,-.62f),at(1,.62f),at(-1,.62f)});
    }

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
