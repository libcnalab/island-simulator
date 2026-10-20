#pragma once

#include <algorithm>
#include <cmath>

namespace island
{
    struct GrassLodSplit
    {
        int nearCount = 0;
        int middleCount = 0;
        int farCount = 0;
    };

    /** @brief One patch's share of each mesh tier; the three sum to 1. */
    struct GrassLodWeights
    {
        float nearWeight = 1.0f;
        float middleWeight = 0.0f;
        float farWeight = 0.0f;
    };

    /** @brief The instance ranges [begin, end) of a cell that each tier's draw must cover. */
    struct GrassLodRanges
    {
        int nearBegin = 0, nearEnd = 0;
        int middleBegin = 0, middleEnd = 0;
        int farBegin = 0, farEnd = 0;
    };

    /** @brief Deterministic population split used to stagger grass mesh LOD changes. */
    class GrassLod final
    {
    public:
        static constexpr float NearOnlyDistance = 62.0f;
        static constexpr float MiddleOnlyStartDistance = 78.0f;
        static constexpr float MiddleOnlyEndDistance = 86.0f;
        static constexpr float FarOnlyDistance = 102.0f;
        /** Half the width over which one patch morphs from one tier's mesh to the next. */
        static constexpr float MorphHalfWidth = 2.5f;

        /** The fallback path's split by the cell centre's distance (no per-patch shader). */
        [[nodiscard]] static GrassLodSplit Split(int patchCount, float distance) noexcept;

        // The shader path decides per patch, by the patch's own distance and its rank in its
        // cell's hashed instance order, (index + 0.5) / count. A patch of rank r leaves the near
        // mesh at MiddleOnlyStart - r * (MiddleOnlyStart - NearOnly) and reaches the far mesh at
        // FarOnly - r * (FarOnly - MiddleOnlyEnd), morphing (both meshes scaled) over
        // +-MorphHalfWidth. Choosing by the cell centre switched whole 37.5 m cells at once,
        // patches 36 m away among them, so in flight the grass changed in square blocks.
        [[nodiscard]] static float NearThreshold(float rank) noexcept;
        [[nodiscard]] static float FarThreshold(float rank) noexcept;
        [[nodiscard]] static GrassLodWeights PatchWeights(float rank, float distance) noexcept;
        /** Conservative instance ranges for a cell whose patches all lie between the nearest
         *  and farthest horizontal distances; instances outside a tier's range have zero
         *  weight in that tier everywhere in the cell. */
        [[nodiscard]] static GrassLodRanges Ranges(int patchCount, float nearestDistance,
                                                   float farthestDistance) noexcept;
    };
}
