#include "IslandSimulator/GrassLod.hpp"

namespace island
{
    GrassLodSplit GrassLod::Split(const int patchCount, const float distance) noexcept
    {
        if (patchCount <= 0) {
            return {};
        }
        if (std::isnan(distance) || distance <= NearOnlyDistance) {
            return GrassLodSplit{patchCount, 0, 0};
        }
        if (distance < MiddleOnlyStartDistance) {
            const float progress = std::clamp(
                (distance - NearOnlyDistance)
                    / (MiddleOnlyStartDistance - NearOnlyDistance),
                0.0f, 1.0f);
            const float smoothProgress = progress * progress * (3.0f - 2.0f * progress);
            const int middleCount = std::clamp(
                static_cast<int>(std::lround(
                    static_cast<float>(patchCount) * smoothProgress)),
                0, patchCount);
            return GrassLodSplit{patchCount - middleCount, middleCount, 0};
        }
        if (distance <= MiddleOnlyEndDistance) {
            return GrassLodSplit{0, patchCount, 0};
        }
        if (distance < FarOnlyDistance) {
            const float progress = std::clamp(
                (distance - MiddleOnlyEndDistance)
                    / (FarOnlyDistance - MiddleOnlyEndDistance),
                0.0f, 1.0f);
            const float smoothProgress = progress * progress * (3.0f - 2.0f * progress);
            const int farCount = std::clamp(
                static_cast<int>(std::lround(
                    static_cast<float>(patchCount) * smoothProgress)),
                0, patchCount);
            return GrassLodSplit{0, patchCount - farCount, farCount};
        }
        return GrassLodSplit{0, 0, patchCount};
    }

    float GrassLod::NearThreshold(const float rank) noexcept
    {
        return MiddleOnlyStartDistance - std::clamp(rank, 0.0f, 1.0f) * (MiddleOnlyStartDistance - NearOnlyDistance);
    }

    float GrassLod::FarThreshold(const float rank) noexcept
    {
        return FarOnlyDistance - std::clamp(rank, 0.0f, 1.0f) * (FarOnlyDistance - MiddleOnlyEndDistance);
    }

    namespace
    {
        [[nodiscard]] float SmoothStep(const float edge0, const float edge1, const float x) noexcept
        {
            const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        }

        // The number of ranks (index + 0.5) / count below the fraction, at least.
        [[nodiscard]] int CountBelow(const int count, const float fraction) noexcept
        {
            if (!(fraction > 0.0f)) return 0;
            if (fraction >= 1.0f) return count;
            return std::clamp(static_cast<int>(std::ceil(fraction * static_cast<float>(count) - 0.5f)) + 1, 0, count);
        }

        // The first index whose rank may lie above the fraction, at most.
        [[nodiscard]] int FirstAbove(const int count, const float fraction) noexcept
        {
            if (!(fraction > 0.0f)) return 0;
            if (fraction >= 1.0f) return count;
            return std::clamp(static_cast<int>(std::floor(fraction * static_cast<float>(count) - 0.5f)) - 1, 0, count);
        }
    }

    GrassLodWeights GrassLod::PatchWeights(const float rank, const float distance) noexcept
    {
        if (std::isnan(distance)) return {};
        const float nearAt = NearThreshold(rank), farAt = FarThreshold(rank);
        const float nearWeight = 1.0f - SmoothStep(nearAt - MorphHalfWidth, nearAt + MorphHalfWidth, distance);
        const float farWeight = SmoothStep(farAt - MorphHalfWidth, farAt + MorphHalfWidth, distance);
        return {nearWeight, std::clamp(1.0f - nearWeight - farWeight, 0.0f, 1.0f), farWeight};
    }

    GrassLodRanges GrassLod::Ranges(const int patchCount, float nearestDistance, float farthestDistance) noexcept
    {
        if (patchCount <= 0) return {};
        if (std::isnan(nearestDistance) || std::isnan(farthestDistance))
            return {0, patchCount, 0, patchCount, 0, patchCount};
        nearestDistance = std::max(nearestDistance, 0.0f);
        farthestDistance = std::max(farthestDistance, nearestDistance);
        const float nearSpan = MiddleOnlyStartDistance - NearOnlyDistance;
        const float farSpan = FarOnlyDistance - MiddleOnlyEndDistance;
        GrassLodRanges ranges;
        // near: some patch is nearer than its rank's threshold plus the morph
        ranges.nearEnd = CountBelow(patchCount, (MiddleOnlyStartDistance + MorphHalfWidth - nearestDistance) / nearSpan);
        // far: some patch is farther than its rank's threshold minus the morph
        ranges.farBegin = FirstAbove(patchCount, (FarOnlyDistance - MorphHalfWidth - farthestDistance) / farSpan);
        ranges.farEnd = ranges.farBegin < patchCount && farthestDistance > MiddleOnlyEndDistance - MorphHalfWidth
            ? patchCount : ranges.farBegin;
        // middle: some patch lies between its near threshold minus and its far threshold plus the morph
        ranges.middleBegin = FirstAbove(patchCount, (MiddleOnlyStartDistance - MorphHalfWidth - farthestDistance) / nearSpan);
        ranges.middleEnd = CountBelow(patchCount, (FarOnlyDistance + MorphHalfWidth - nearestDistance) / farSpan);
        if (farthestDistance <= NearOnlyDistance - MorphHalfWidth) ranges.middleEnd = ranges.middleBegin;
        ranges.middleEnd = std::max(ranges.middleEnd, ranges.middleBegin);
        return ranges;
    }
}
