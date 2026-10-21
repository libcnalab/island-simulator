#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace island
{
    /** The passing sailboat this moment: where it is, which way it sails (radians, x towards z),
     *  and whether one is out at all. */
    struct SailboatState
    {
        float x = 0.0f, z = 0.0f, headingRadians = 0.0f;
        bool active = false;
    };

    /**
     * Now and then a cruising yacht sails past the island well offshore: every PeriodSeconds
     * one crosses a chord of the sea 560-680 m from the island's centre, from a fresh bearing
     * each time, at about five knots, taking four minutes or so; between passages the sea is
     * empty. Deterministic in its own clock.
     */
    class SailboatPassage final
    {
    public:
        static constexpr float PeriodSeconds = 540.0f, Speed = 2.6f, Reach = 720.0f;
        static constexpr float FirstSeconds = 40.0f;   // the first one sets out soon after the start

        void Update(const float seconds) noexcept
        {
            if (std::isfinite(seconds) && seconds > 0.0f) clock_ = std::fmod(clock_ + seconds, 1.0e6f);
            state_.active = false;
            const float since = clock_ - FirstSeconds;
            if (since < 0.0f) return;
            const auto passage = static_cast<std::uint32_t>(since / PeriodSeconds);
            const float t = since - static_cast<float>(passage) * PeriodSeconds;
            // the chord: its nearest point to the island at `offing` on `bearing`, sailed one way
            const float bearing = 6.2831853f * Unit(passage, 1U);
            const float offing = 560.0f + 120.0f * Unit(passage, 2U);
            const float half = std::sqrt(std::max(0.0f, Reach * Reach - offing * offing));
            const float along = -half + Speed * t;
            if (along > half) return;
            const float direction = Unit(passage, 3U) < 0.5f ? 1.0f : -1.0f;
            const float cx = std::cos(bearing) * offing, cz = std::sin(bearing) * offing;
            const float tx = -std::sin(bearing) * direction, tz = std::cos(bearing) * direction;
            state_.x = cx + tx * along;
            state_.z = cz + tz * along;
            state_.headingRadians = std::atan2(tz, tx);
            state_.active = true;
        }
        [[nodiscard]] const SailboatState& State() const noexcept { return state_; }
        /** @brief QA: sets the clock (a passage half done is at FirstSeconds + 120). */
        void SetClock(const float seconds) noexcept { clock_ = seconds; Update(0.0f); }

    private:
        [[nodiscard]] static float Unit(const std::uint32_t passage, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = passage * 0x9e3779b1U ^ salt * 0x85ebca77U ^ 0x5a17b0a7U;
            h ^= h >> 15; h *= 0x2c1b3c6dU; h ^= h >> 12;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        SailboatState state_{};
        float clock_ = 0.0f;
    };
}
