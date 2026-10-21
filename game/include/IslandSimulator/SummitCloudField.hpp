#pragma once

#include "IslandSimulator/CascadeMistField.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    /**
     * The cloud cap on the island's summit: moist trade-wind air forced up over the hill
     * condenses just upwind of the top, hugs it and dissolves again in its lee. Puffs are born
     * upwind of the summit, ride the wind over it (thickening as they reach the top, thinning
     * away beyond it) at a little above its height, each on its own cycle; how much cap there is
     * follows the air's moisture (none in clear weather, wisps on a fair day, a heavy hood under
     * overcast and in rain, more at dawn). Renderer-independent and deterministic in its clock.
     */
    class SummitCloudField final
    {
    public:
        static constexpr std::size_t Count = 18;
        static constexpr float Upwind = 70.0f, Downwind = 90.0f;   // the stretch a puff rides, metres

        void SetSummit(const float x, const float y, const float z) noexcept { x_ = x; y_ = y; z_ = z; }
        /** @brief How much cap the weather and the hour make: 0 none .. 1 a heavy hood. */
        [[nodiscard]] static float Cover(const float cloudCover, const float rain, const float hour) noexcept
        {
            const float dawn = std::clamp(1.0f - std::abs(hour - 6.5f) / 2.5f, 0.0f, 1.0f);
            return std::clamp(0.35f + 0.65f * cloudCover + 0.35f * rain + 0.25f * dawn, 0.0f, 1.0f);
        }
        /** @param windX,windZ the direction the wind blows toward; @param wind its strength (1 the trade wind). */
        void Update(const float seconds, const float cover, const float windX, const float windZ, const float wind) noexcept
        {
            if (std::isfinite(seconds) && seconds > 0.0f) clock_ = std::fmod(clock_ + seconds, 1.0e6f);
            const float along = std::hypot(windX, windZ);
            const float dx = along > 1e-4f ? windX / along : -1.0f, dz = along > 1e-4f ? windZ / along : 0.0f;
            const float speed = 2.2f * std::clamp(std::isfinite(wind) ? wind : 1.0f, 0.4f, 2.5f);
            const float span = Upwind + Downwind;
            const float amount = std::clamp(std::isfinite(cover) ? cover : 0.0f, 0.0f, 1.0f);
            for (std::size_t i = 0; i < Count; ++i) {
                MistPuff& puff = puffs_[i];
                const auto k = static_cast<std::uint32_t>(i);
                // each puff rides the stretch from upwind of the top to its lee, then starts again
                const float travelled = std::fmod(clock_ * speed + span * Unit(k, 1U), span);
                const float fromTop = travelled - Upwind;   // < 0 upwind, > 0 in the lee
                const float lateral = (Unit(k, 2U) - 0.5f) * 70.0f;
                puff.x = x_ + dx * fromTop - dz * lateral;
                puff.z = z_ + dz * fromTop + dx * lateral;
                puff.y = y_ - 3.0f + 9.0f * Unit(k, 3U) - 0.04f * std::max(0.0f, fromTop);
                puff.size = 18.0f + 16.0f * Unit(k, 4U);
                // condensing on the way up to the top, dissolving in the lee
                const float forming = std::clamp((fromTop + Upwind) / (Upwind * 0.7f), 0.0f, 1.0f);
                const float clearing = 1.0f - std::clamp(fromTop / Downwind, 0.0f, 1.0f);
                // only some puffs on a fair day, all of them in a heavy hood
                const float share = std::clamp((amount - Unit(k, 5U) * 0.85f) / 0.15f, 0.0f, 1.0f);
                puff.density = forming * clearing * clearing * share * (0.45f + 0.55f * amount);
                puff.active = puff.density > 0.01f;
                puff.age = 0.0f;
            }
        }
        [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept { return puffs_; }

    private:
        [[nodiscard]] static float Unit(const std::uint32_t i, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = i * 0x9e3779b1U ^ salt * 0x85ebca77U ^ 0x7c3a51f9U;
            h ^= h >> 15; h *= 0x2c1b3c6dU; h ^= h >> 12;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        std::array<MistPuff, Count> puffs_{};
        float clock_ = 0.0f, x_ = 0.0f, y_ = 0.0f, z_ = 0.0f;
    };
}
