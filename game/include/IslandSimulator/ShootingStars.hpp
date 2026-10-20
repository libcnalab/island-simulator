#pragma once

#include <cmath>
#include <cstdint>
#include <numbers>

namespace island::shooting_stars
{
    /** A shooting star, if one is crossing the sky at `seconds`: about one a minute, each a
     *  brief bright streak sliding down the dark sky; a function of the time. */
    struct Meteor
    {
        bool visible = false;
        float progress = 0.0f;                          // 0 it appears .. 1 it is gone
        float azimuth = 0.0f, elevation = 0.0f, slant = 0.0f;
    };

    [[nodiscard]] inline Meteor At(const double seconds) noexcept
    {
        // One chance in each 47-second window, at a time within it set by the window's hash.
        constexpr double Window = 47.0, Duration = 0.75;
        Meteor meteor;
        if (!std::isfinite(seconds) || seconds < 0.0) return meteor;
        const auto window = static_cast<std::uint32_t>(std::floor(seconds / Window));
        const auto hash = [window](const std::uint32_t salt) {
            std::uint32_t h = window * 0x9e3779b9U ^ salt * 0x85ebca6bU;
            h ^= h >> 16; h *= 0x7feb352dU; h ^= h >> 15; h *= 0x846ca68bU; h ^= h >> 16;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        };
        if (hash(1U) > 0.8f) return meteor;   // a fifth of the windows pass without one
        const double start = static_cast<double>(window) * Window + hash(2U) * (Window - Duration);
        const double since = seconds - start;
        if (since < 0.0 || since > Duration) return meteor;
        meteor.visible = true;
        meteor.progress = static_cast<float>(since / Duration);
        meteor.azimuth = hash(3U) * 2.0f * std::numbers::pi_v<float>;
        meteor.elevation = 0.45f + 0.65f * hash(4U);          // 26-63 degrees up as it begins
        meteor.slant = (hash(5U) - 0.5f) * 1.6f;               // the way it falls across the sky
        return meteor;
    }
}
