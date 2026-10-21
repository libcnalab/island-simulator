#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace island
{
    /** The waterspout this moment, if there is one: where it touches the sea, how far through its
     *  life (0 the funnel starts down from the cloud .. 1 it is gone) and how strong. */
    struct WaterspoutState
    {
        float x = 0.0f, z = 0.0f, progress = 0.0f, strength = 0.0f;
        bool active = false;
    };

    /**
     * Waterspouts, common round tropical islands in squally weather: in a storm, now and then a
     * funnel reaches down from the cloud base 620-780 m offshore (downwind of the island or off
     * to its side), touches the sea and stands for a few minutes, drifting away from the island
     * with the wind, before it ropes out and is gone. Each window of WindowSeconds holds one or
     * none (a fresh bearing each time); a spout fades away early if the weather settles.
     * Deterministic in its own clock.
     */
    class Waterspout final
    {
    public:
        static constexpr float WindowSeconds = 600.0f, LifeSeconds = 240.0f, CloudBase = 260.0f, DriftSpeed = 1.5f;

        /** @param storm how squally the weather is, 0 settled .. 1 a storm. */
        void Update(const float seconds, const float storm, const float windX, const float windZ) noexcept
        {
            const float dt = std::isfinite(seconds) && seconds > 0.0f ? seconds : 0.0f;
            clock_ = std::fmod(clock_ + dt, 1.0e6f);
            const float target = std::clamp(std::isfinite(storm) ? storm : 0.0f, 0.0f, 1.0f);
            strength_ += (target - strength_) * std::min(1.0f, dt / 20.0f);
            state_.active = false;
            const auto window = static_cast<std::uint32_t>(clock_ / WindowSeconds);
            if (Unit(window, 1U) > 0.55f) return;   // nearly half the windows pass without one
            const float start = (Unit(window, 2U) * (WindowSeconds - LifeSeconds));
            const float t = clock_ - static_cast<float>(window) * WindowSeconds - start;
            if (t < 0.0f || t > LifeSeconds || strength_ < 0.05f) return;
            const float bearing = 6.2831853f * Unit(window, 3U);
            const float offing = 620.0f + 160.0f * Unit(window, 4U);
            float bx = std::cos(bearing), bz = std::sin(bearing);
            // it stands downwind of the island or off to the side, so the wind carries it away
            // from the island, never over it
            if (bx * windX + bz * windZ < 0.0f) { bx = -bx; bz = -bz; }
            const float drift = DriftSpeed * t;
            state_.x = bx * offing + windX * drift;
            state_.z = bz * offing + windZ * drift;
            state_.progress = t / LifeSeconds;
            state_.strength = strength_;
            state_.active = true;
        }
        [[nodiscard]] const WaterspoutState& State() const noexcept { return state_; }

    private:
        [[nodiscard]] static float Unit(const std::uint32_t window, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = window * 0x9e3779b1U ^ salt * 0x85ebca77U ^ 0x3d9a5c11U;
            h ^= h >> 15; h *= 0x2c1b3c6dU; h ^= h >> 12;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        WaterspoutState state_{};
        float clock_ = 0.0f, strength_ = 0.0f;
    };
}
