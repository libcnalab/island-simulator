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
    // With the weather (the game's live clock): in a stronger wind the fronts come faster and
    // closer together and fill more of their width, so a storm sweeps the island every few
    // seconds; the clock accumulates the distance travelled, so a change of wind never makes
    // the fronts jump. Unset (the default, and every test), the fixed trade-wind form above.
    // uGustClock: x the distance the fronts have travelled, y their spacing, z the floor of
    // their across-wind strength, w 1 when the live clock drives them.
    inline constexpr const char* Shader = R"(
uniform vec4 uGustClock;
float windGust(vec2 world, float seconds) {
    float along = -world.x;
    bool live = uGustClock.w > 0.5;
    float cycle = live ? fract((along - uGustClock.x) / uGustClock.y) : fract((along - seconds * 6.5) / 140.0);
    float band = smoothstep(0.0, 0.08, cycle) * (1.0 - smoothstep(0.14, 0.30, cycle));
    float across = 0.5 + 0.5 * sin(world.y * 0.021 + seconds * 0.13);
    return band * mix(live ? uGustClock.z : 0.45, 1.0, across);
}
)";
    struct Clock
    {
        float travelled = 0.0f, spacing = FrontSpacing, floor = 0.45f;
        bool live = false;
    };
    /** @brief The game's live gust clock (inactive until the game advances it). */
    [[nodiscard]] inline Clock& Live() noexcept
    {
        static Clock clock;
        return clock;
    }
    /** @brief Advances the live clock by real seconds in a wind of `wind` (1 the trade wind). */
    inline void Advance(const float seconds, const float wind) noexcept
    {
        Clock& clock = Live();
        const float w = std::isfinite(wind) ? std::fmax(0.5f, std::fmin(wind, 3.0f)) : 1.0f;
        const float dt = std::isfinite(seconds) ? std::fmax(0.0f, std::fmin(seconds, 0.25f)) : 0.0f;
        clock.travelled = std::fmod(clock.travelled + dt * FrontSpeed * std::pow(w, 0.6f), 1.0e5f);
        clock.spacing = FrontSpacing / std::pow(w, 0.8f);
        clock.floor = 0.45f + 0.25f * std::fmin(1.0f, std::fmax(0.0f, w - 1.0f));
        clock.live = true;
    }
    [[nodiscard]] inline float Strength(const float x, const float z, const float seconds) noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(seconds)) return 0.0f;
        const auto smooth = [](float a, float b, float v) {
            const float t = std::fmin(1.0f, std::fmax(0.0f, (v - a) / (b - a)));
            return t * t * (3.0f - 2.0f * t);
        };
        const float along = -x;
        const Clock& clock = Live();
        const float raw = clock.live ? (along - clock.travelled) / clock.spacing : (along - seconds * FrontSpeed) / FrontSpacing;
        const float cycle = raw - std::floor(raw);
        const float band = smooth(0.0f, 0.08f, cycle) * (1.0f - smooth(0.14f, 0.30f, cycle));
        const float across = 0.5f + 0.5f * std::sin(z * 0.021f + seconds * 0.13f);
        const float floor = clock.live ? clock.floor : 0.45f;
        return band * (floor + (1.0f - floor) * across);
    }
}
