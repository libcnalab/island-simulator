#pragma once
#include "IslandSimulator/PrevailingWind.hpp"
#include <cmath>

namespace island::WindGust
{
    // Gust fronts: bands of stronger, coherent wind that sweep downwind with the trade wind.
    // A front passes a fixed point every FrontSpacing / FrontSpeed seconds. The shader form and
    // the CPU form below must agree; the CPU form feeds the core test. Fronts travel toward
    // -x with the trade wind: a front sits where -x - speed*t is a multiple of the spacing.
    inline constexpr float FrontSpeed = 6.5f;      // metres per second
    inline constexpr float FrontSpacing = 140.0f;  // metres between fronts
    // Sway amplitudes are never raised by a gust: the front only makes the existing sway lean
    // together downwind, so every collision and clearance envelope stays as authored.
    inline constexpr const char* Shader = R"(
float windGust(vec2 world, float seconds) {
    float along = -world.x;
    float cycle = fract((along - seconds * 6.5) / 140.0);
    float band = smoothstep(0.0, 0.08, cycle) * (1.0 - smoothstep(0.14, 0.30, cycle));
    float across = 0.5 + 0.5 * sin(world.y * 0.021 + seconds * 0.13);
    return band * mix(0.45, 1.0, across);
}
)";
    [[nodiscard]] inline float Strength(const float x, const float z, const float seconds) noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(seconds)) return 0.0f;
        const auto smooth = [](float a, float b, float v) {
            const float t = std::fmin(1.0f, std::fmax(0.0f, (v - a) / (b - a)));
            return t * t * (3.0f - 2.0f * t);
        };
        const float along = -x;
        const float raw = (along - seconds * FrontSpeed) / FrontSpacing;
        const float cycle = raw - std::floor(raw);
        const float band = smooth(0.0f, 0.08f, cycle) * (1.0f - smooth(0.14f, 0.30f, cycle));
        const float across = 0.5f + 0.5f * std::sin(z * 0.021f + seconds * 0.13f);
        return band * (0.45f + 0.55f * across);
    }
}
