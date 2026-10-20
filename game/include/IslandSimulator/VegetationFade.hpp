#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

// How the jungle floor's plants leave the view, kept free of graphics types for the core
// tests. Before this, every understory plant, bush and ground plant swapped meshes on one
// perfect circle and vanished on another, six hard rings between 30 and 70 m that swept over
// the forest floor in flight. Each plant now swaps meshes at its own staggered distance and
// shrinks into the ground over the last part of its range instead of vanishing.
namespace island::vegetation_fade
{
    inline constexpr float NearStagger = 0.12f;      // +-12 % of the base near/far swap distance
    inline constexpr float ShrinkFraction = 0.22f;   // the last 22 % of the range shrinks
    inline constexpr float TreeShrinkFraction = 0.12f;   // trees and palms: a narrower band

    /** @brief A plant's own 0..1 seed from its ground position. */
    [[nodiscard]] inline float Seed(const float x, const float z) noexcept
    {
        const float value = std::sin(x * 12.9898f + z * 78.233f) * 43758.5453f;
        return value - std::floor(value);
    }

    /** @brief The plant's own near/far mesh swap distance around the base distance. */
    [[nodiscard]] inline float StaggeredNearDistance(const float baseDistance, const float x, const float z) noexcept
    {
        return baseDistance * (1.0f - NearStagger + 2.0f * NearStagger * Seed(x, z));
    }

    /** @brief 0..1, the plant's scale at a horizontal distance: 1 until the last fraction
     *  of the range, then a smooth shrink to 0 at the maximum distance. */
    [[nodiscard]] inline float FarShrink(const float distance, const float maximumDistance,
                                         const float fraction = ShrinkFraction) noexcept
    {
        if (!(maximumDistance > 0.0f) || std::isnan(distance)) return 0.0f;
        const float start = maximumDistance * (1.0f - std::clamp(fraction, 0.01f, 1.0f));
        const float t = std::clamp((distance - start) / (maximumDistance - start), 0.0f, 1.0f);
        return 1.0f - t * t * (3.0f - 2.0f * t);
    }

    /** @brief Scales the world-space vertices from `first` on about the foot point (an animal
     *  built per frame shrinks into the ground over the end of its range like the plants). */
    template <class Vertex>
    void ShrinkAbout(std::vector<Vertex>& vertices, const std::size_t first, const float footX,
                     const float footY, const float footZ, const float scale)
    {
        if (scale >= 1.0f) return;
        for (std::size_t i = first; i < vertices.size(); ++i) {
            auto& p = vertices[i].Position;
            p.X = footX + (p.X - footX) * scale;
            p.Y = footY + (p.Y - footY) * scale;
            p.Z = footZ + (p.Z - footZ) * scale;
        }
    }
}
