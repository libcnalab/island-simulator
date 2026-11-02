#pragma once

#include "IslandSimulator/BunkerLayout.hpp"
#include <algorithm>

namespace island {
// Resolve the whole sightline AFTER smoothing. An endpoint may fit a room while
// its ray or the interpolation crosses the divider at a doorway's corner.
[[nodiscard]] inline BunkerPoint FitBunkerChaseCamera(
    const BunkerLayout& bunker,BunkerPoint focus,BunkerPoint candidate)noexcept
{
    constexpr float radius=.18f;
    const auto local=bunker.Local(candidate);
    if(local.z>=-.35f) {
        const float floor=bunker.EntranceGroundY()+bunker.FloorRelative(local.z);
        // Lamps/conduits hang below the slab. Keep the eye AND its near plane
        // beneath them, including when the quad climbs above a deeper camera.
        candidate.y=std::clamp(candidate.y,floor+.6f,floor+(local.z<10?2.75f:3.05f)-.65f);
    }
    auto from=focus,to=candidate;from.y-=radius;to.y-=radius;
    const float fraction=bunker.SafeTravelFraction(from,to,radius,2*radius);
    return {std::lerp(focus.x,candidate.x,fraction),std::lerp(focus.y,candidate.y,fraction),
            std::lerp(focus.z,candidate.z,fraction)};
}
}
