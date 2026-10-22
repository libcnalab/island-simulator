#pragma once

#include "IslandSimulator/SoundscapeModel.hpp"
#include "IslandSimulator/StreamRoute.hpp"

#include <cmath>

namespace island::footprint_ground
{
    inline constexpr float BankReach = 1.6f;       // metres of mud beyond the stream's water
    inline constexpr float MuddyWetness = 0.45f;   // the rain's wetness that turns bare soil to mud

    /** @brief Whether bare soil takes a footprint here: the stream's muddy banks always, and any
     *  bare soil once the rain has soaked it (grass, rock and the forest's litter take none). */
    [[nodiscard]] inline bool Muddy(const FootstepSurface surface, const float x, const float z,
                                    const float wetness) noexcept
    {
        if (surface != FootstepSurface::Soil || !std::isfinite(x) || !std::isfinite(z)) return false;
        if (wetness >= MuddyWetness) return true;
        const float along = StreamRoute::SourceX - x;
        return along >= 0.0f && along <= StreamRoute::NominalLength
            && std::abs(z - StreamRoute::CentreZ(x)) < StreamRoute::HalfWidth(x) + BankReach;
    }
}
