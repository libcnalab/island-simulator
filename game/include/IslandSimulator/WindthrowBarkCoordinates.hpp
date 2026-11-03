#pragma once

#include "IslandSimulator/WindthrowLayout.hpp"
#include <array>

namespace island
{
    /** Unit-circle coordinates around the existing bent trunk, then positive axial metres.
     * Interpolating the circle instead of atan's angle preserves filtering across its seam.
     * Other parts of the mesh use axial -1 and retain their original vertex colours. */
    template<class Ground>
    [[nodiscard]] std::array<float, 3> WindthrowBarkCoordinates(
        const WindthrowPlacement& wood, const std::array<float, 3>& point, const Ground& ground)
    {
        const float along = (point[0] - wood.x) * wood.dirX + (point[2] - wood.z) * wood.dirZ;
        const auto axis = wood.Axis(along, ground);
        const float radius = WindthrowPlacement::RadiusAt(along);
        const float across = (point[0] - axis[0]) * wood.AcrossX()
                           + (point[2] - axis[2]) * wood.AcrossZ();
        return {across / radius, (point[1] - axis[1]) / radius, along + 1.0f};
    }
}
