#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    /** One spark of plankton light stirred up by a swimmer. */
    struct GlowSpeck
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float age = 0.0f, life = 1.0f;
        bool active = false;
        /** @brief 0..1: a quick flash, then a fading glow. */
        [[nodiscard]] float Brightness() const noexcept
        {
            if (!active || !(life > 0.0f)) return 0.0f;
            const float t = std::clamp(age / life, 0.0f, 1.0f);
            return std::min(1.0f, t * 8.0f) * (1.0f - t) * (1.0f - t);
        }
    };

    /**
     * The sea lights up round a swimmer on a dark night: plankton stirred by the strokes flash
     * blue and fade within a second or two. Sparks are born in a ring about the swimmer at the
     * surface, a few while treading water and many more swimming hard, more of them behind than
     * ahead; they stay where they were stirred (world-anchored). Deterministic in its clock.
     */
    class SwimGlowField final
    {
    public:
        static constexpr std::size_t Capacity = 200;

        /** @param strength 0..1 how dark it is for the glow to show (0 none, by day). */
        void Update(const float seconds, const float x, const float surfaceY, const float z, const float speed,
                    const float headingX, const float headingZ, const bool swimming, const float strength) noexcept
        {
            const float dt = std::isfinite(seconds) && seconds > 0.0f ? std::min(seconds, 0.25f) : 0.0f;
            clock_ += dt;
            for (GlowSpeck& speck : specks_) {
                if (!speck.active) continue;
                speck.age += dt;
                if (speck.age >= speck.life) speck.active = false;
            }
            if (!swimming || !(strength > 0.05f) || !std::isfinite(x) || !std::isfinite(z)) return;
            const float moving = std::clamp(std::isfinite(speed) ? speed : 0.0f, 0.0f, 2.0f);
            owed_ += dt * (10.0f + 55.0f * moving);
            const float h = std::hypot(headingX, headingZ);
            const float fx = h > 1e-4f ? headingX / h : 0.0f, fz = h > 1e-4f ? headingZ / h : 0.0f;
            while (owed_ >= 1.0f) {
                owed_ -= 1.0f;
                GlowSpeck& speck = specks_[next_];
                next_ = (next_ + 1) % Capacity;
                const auto k = ++born_;
                const float angle = 6.2831853f * Unit(k, 1U);
                const float radius = 0.3f + 1.0f * Unit(k, 2U);
                // more of them in the wake behind than ahead
                const float behind = moving * 0.6f * Unit(k, 3U);
                speck.x = x + std::cos(angle) * radius - fx * behind;
                speck.z = z + std::sin(angle) * radius - fz * behind;
                speck.y = surfaceY + 0.02f - 0.15f * Unit(k, 4U);
                speck.age = 0.0f;
                speck.life = 0.8f + 1.0f * Unit(k, 5U);
                speck.active = true;
            }
        }
        [[nodiscard]] std::span<const GlowSpeck> Specks() const noexcept { return specks_; }

    private:
        [[nodiscard]] static float Unit(const std::uint32_t i, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = i * 0x9e3779b1U ^ salt * 0x85ebca77U ^ 0x2b7e1516U;
            h ^= h >> 15; h *= 0x2c1b3c6dU; h ^= h >> 12;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        std::array<GlowSpeck, Capacity> specks_{};
        std::size_t next_ = 0;
        std::uint32_t born_ = 0;
        float owed_ = 0.0f, clock_ = 0.0f;
    };
}
