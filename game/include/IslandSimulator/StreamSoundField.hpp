#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include "IslandSimulator/StreamRoute.hpp"

namespace island
{
    /** @brief Nearest audible point on the shared authored stream route. */
    struct StreamSoundSample
    {
        float x = 0.0f;
        float z = 0.0f;
        float distance = 0.0f;
        float presence = 0.0f;
        /** @brief Share of the audible stream that is the cascade's white water, 0..1. */
        float turbulence = 0.0f;
        bool valid = false;
    };

    /** @brief The cascade lip, which carries further and louder than the quiet reaches. */
    struct StreamCascadePoint
    {
        float x = 0.0f;
        float z = 0.0f;
        bool valid = false;
    };

    /** @brief Cheap listener-relative field used by the stream sound and wet audio biome. */
    class StreamSoundField final
    {
    public:
        static constexpr int SegmentCount = 32;
        static constexpr float FullPresenceDistance = 4.0f;
        static constexpr float SilentDistance = 34.0f;
        static constexpr float CascadeFullDistance = 5.0f;
        static constexpr float CascadeSilentDistance = 58.0f;

        [[nodiscard]] static StreamSoundSample Sample(float listenerX, float listenerZ,
                                                       const StreamCascadePoint& cascade = {}) noexcept
        {
            if (!std::isfinite(listenerX) || !std::isfinite(listenerZ)) {
                return {};
            }

            float bestDistanceSquared = std::numeric_limits<float>::max();
            float bestX = StreamRoute::SourceX;
            float bestZ = StreamRoute::CentreZ(bestX);
            for (int segment = 0; segment < SegmentCount; ++segment) {
                const float along0 = StreamRoute::NominalLength
                    * static_cast<float>(segment) / static_cast<float>(SegmentCount);
                const float along1 = StreamRoute::NominalLength
                    * static_cast<float>(segment + 1) / static_cast<float>(SegmentCount);
                const float x0 = StreamRoute::SourceX - along0;
                const float x1 = StreamRoute::SourceX - along1;
                const float z0 = StreamRoute::CentreZ(x0);
                const float z1 = StreamRoute::CentreZ(x1);
                const float dx = x1 - x0;
                const float dz = z1 - z0;
                const float lengthSquared = dx * dx + dz * dz;
                const float projection = lengthSquared > 0.000001f
                    ? std::clamp(((listenerX - x0) * dx + (listenerZ - z0) * dz)
                                     / lengthSquared,
                                 0.0f, 1.0f)
                    : 0.0f;
                const float x = x0 + dx * projection;
                const float z = z0 + dz * projection;
                const float distanceX = listenerX - x;
                const float distanceZ = listenerZ - z;
                const float distanceSquared = distanceX * distanceX + distanceZ * distanceZ;
                if (distanceSquared < bestDistanceSquared) {
                    bestDistanceSquared = distanceSquared;
                    bestX = x;
                    bestZ = z;
                }
            }

            const float distance = std::sqrt(bestDistanceSquared);
            const float unit = std::clamp(
                (distance - FullPresenceDistance)
                    / (SilentDistance - FullPresenceDistance),
                0.0f, 1.0f);
            const float smooth = unit * unit * (3.0f - 2.0f * unit);
            StreamSoundSample sample{bestX, bestZ, distance, 1.0f - smooth, 0.0f, true};
            if (cascade.valid && std::isfinite(cascade.x) && std::isfinite(cascade.z)) {
                const float cascadeDistance = std::hypot(listenerX - cascade.x, listenerZ - cascade.z);
                const float cascadeUnit = std::clamp(
                    (cascadeDistance - CascadeFullDistance)
                        / (CascadeSilentDistance - CascadeFullDistance),
                    0.0f, 1.0f);
                const float cascadePresence = 1.0f - cascadeUnit * cascadeUnit * (3.0f - 2.0f * cascadeUnit);
                sample.turbulence = cascadePresence;
                if (cascadePresence > sample.presence) {
                    sample.presence = cascadePresence;
                    sample.x = cascade.x;
                    sample.z = cascade.z;
                    sample.distance = cascadeDistance;
                }
            }
            return sample;
        }

        /** @brief Hysteretic arbitration for the stream/player-water shared voice. */
        [[nodiscard]] static bool ShouldUseStreamPlayback(bool swimming,
                                                          float presence,
                                                          bool currentlyActive) noexcept
        {
            if (swimming || !std::isfinite(presence)) {
                return false;
            }
            return presence > (currentlyActive ? 0.005f : 0.015f);
        }
    };
}
