#pragma once

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    struct TideState
    {
        float level = 0.0f;          // metres relative to the authored mean sea level
        float phase = 0.0f;          // 0..1 through one complete high/low cycle
        float highWater = 0.5f;      // 0 at low water, 1 at high water
        bool rising = true;
    };

    /** A deliberately small semidiurnal tide. One island day is 24 real minutes, so a
     *  twelve-minute cycle gives two high waters per island day without making the shore race. */
    class TideCycle final
    {
    public:
        static constexpr float Amplitude = 0.18f;
        static constexpr float PeriodSeconds = 12.0f * 60.0f;

        [[nodiscard]] static TideState At(float seconds) noexcept
        {
            const float safe = std::isfinite(seconds) ? seconds : 0.0f;
            float phase = std::fmod(safe / PeriodSeconds, 1.0f);
            if (phase < 0.0f) phase += 1.0f;
            const float angle = phase * 2.0f * std::numbers::pi_v<float>;
            const float wave = std::sin(angle);
            return {Amplitude * wave, phase, .5f + .5f * wave, std::cos(angle) >= 0.0f};
        }
    };
}
