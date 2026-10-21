#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace island::satellites
{
    /** A satellite crossing the sky at `seconds`, if one is: in the hours after dusk and before
     *  dawn, while the sun (5-20 degrees down) still lights it high above the dark ground, now and
     *  then a steady point glides across the stars from one horizon to the other in two minutes,
     *  over the zenith or off to one side, fading near the horizon; a function of the time. */
    struct Pass
    {
        bool visible = false;
        float x = 0.0f, y = 0.0f, z = 0.0f;   // its direction in the sky, a unit vector
        float brightness = 0.0f;              // 0..1
    };

    [[nodiscard]] inline Pass At(const double seconds, const float sunElevationDegrees) noexcept
    {
        constexpr double Window = 240.0, Duration = 120.0;
        Pass pass;
        if (!std::isfinite(seconds) || seconds < 0.0 || !std::isfinite(sunElevationDegrees)) return pass;
        // lit up there while the sun is a little below the horizon, in the Earth's shadow later
        const float lit = std::clamp((-sunElevationDegrees - 5.0f) / 2.0f, 0.0f, 1.0f)
            * std::clamp((20.0f + sunElevationDegrees) / 3.0f, 0.0f, 1.0f);
        if (lit <= 0.0f) return pass;
        const auto window = static_cast<std::uint32_t>(std::floor(seconds / Window));
        const auto hash = [window](const std::uint32_t salt) {
            std::uint32_t h = window * 0x9e3779b9U ^ salt * 0x85ebca6bU ^ 0x51a7e117U;
            h ^= h >> 16; h *= 0x7feb352dU; h ^= h >> 15; h *= 0x846ca68bU; h ^= h >> 16;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        };
        if (hash(1U) > 0.6f) return pass;   // two windows in five pass without one
        const double start = static_cast<double>(window) * Window + hash(2U) * (Window - Duration);
        const double since = seconds - start;
        if (since < 0.0 || since > Duration) return pass;
        // its orbit's arc: from a horizon point at `rise`, over a highest point `top` up
        const float rise = hash(3U) * 2.0f * std::numbers::pi_v<float>;
        const float top = 0.45f + 1.05f * hash(4U);   // 26-86 degrees
        const float px = std::cos(rise), pz = std::sin(rise);
        const float side = hash(5U) < 0.5f ? 1.0f : -1.0f;
        const float qx = -pz * side * std::cos(top), qy = std::sin(top), qz = px * side * std::cos(top);
        const float theta = 0.12f + (std::numbers::pi_v<float> - 0.24f) * static_cast<float>(since / Duration);
        pass.x = std::cos(theta) * px + std::sin(theta) * qx;
        pass.y = std::sin(theta) * qy;
        pass.z = std::cos(theta) * pz + std::sin(theta) * qz;
        pass.brightness = lit * (0.55f + 0.35f * hash(6U)) * std::clamp(pass.y / 0.17f, 0.0f, 1.0f);
        pass.visible = pass.brightness > 0.02f;
        return pass;
    }
}
