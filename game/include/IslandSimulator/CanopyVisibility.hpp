#pragma once

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace island
{
    // Above the crowns, restore the existing full-profile canopy reach. Ground views
    // retain their quality budget; understory and near-detail distances never change.
    [[nodiscard]] inline float CanopyViewRange(float groundRange, float fullRange,
                                               float clearance)
    {
        if (!std::isfinite(groundRange) || !std::isfinite(fullRange)
            || !std::isfinite(clearance) || groundRange <= 0 || fullRange < groundRange)
            throw std::invalid_argument("Invalid canopy view range");
        const float t = std::clamp((clearance - 12.0f) / 68.0f, 0.0f, 1.0f);
        return groundRange + (fullRange - groundRange) * t * t * (3.0f - 2.0f * t);
    }

    // Extend only silhouettes still resolving at distance. A 0.02 NDC vertical span
    // is 1% of the viewport height (about seven pixels at 720p). Small juveniles keep
    // their original range rather than consuming the aerial budget as subpixel crowns.
    [[nodiscard]] inline float CanopyProjectedRange(float groundRange, float viewRange,
                                                    float height, float verticalProjection)
    {
        if (!std::isfinite(height) || !std::isfinite(verticalProjection)
            || height <= 0 || verticalProjection <= 0 || !std::isfinite(groundRange)
            || !std::isfinite(viewRange) || groundRange <= 0 || viewRange < groundRange)
            throw std::invalid_argument("Invalid canopy projected range");
        return std::clamp(height * verticalProjection / 0.02f, groundRange, viewRange);
    }

    // The local envelope is the union of measured near/far vertices, already expanded for
    // wind. Placements have positive uniform scale and yaw only; a horizontal cylinder is
    // invariant under yaw, so this AABB also covers every intermediate crown orientation.
    [[nodiscard]] inline Microsoft::Xna::Framework::BoundingBox CanopyInstanceBounds(
        float localRadius, float localMinimumY, float localMaximumY,
        const Microsoft::Xna::Framework::Vector3& anchor, float scale)
    {
        using Microsoft::Xna::Framework::Vector3;
        if (!std::isfinite(localRadius) || !std::isfinite(localMinimumY)
            || !std::isfinite(localMaximumY) || !std::isfinite(scale)
            || !std::isfinite(anchor.X) || !std::isfinite(anchor.Y) || !std::isfinite(anchor.Z)
            || localRadius < 0 || localMinimumY > localMaximumY || scale <= 0) {
            throw std::invalid_argument("Invalid canopy instance envelope");
        }
        // A small world-space cushion absorbs float rounding in the shader's matrix multiply.
        const float radius = localRadius * scale + 0.01f;
        const Vector3 minimum(anchor.X - radius, anchor.Y + localMinimumY * scale - 0.01f,
                              anchor.Z - radius);
        const Vector3 maximum(anchor.X + radius, anchor.Y + localMaximumY * scale + 0.01f,
                              anchor.Z + radius);
        for (float value : {minimum.X, minimum.Y, minimum.Z, maximum.X, maximum.Y, maximum.Z}) {
            if (!std::isfinite(value)) throw std::invalid_argument("Canopy instance bounds overflow");
        }
        return {minimum, maximum};
    }
}
