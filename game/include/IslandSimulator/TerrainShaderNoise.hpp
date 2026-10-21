#pragma once

#include <algorithm>
#include <cmath>

namespace island::terrain_noise
{
    /** CPU mirrors of the terrain shader's `hash12`, `valueNoise` and `smoothstep`, so what the
     *  game places or senses lines up with what the shader paints (the seagrass meadows, the
     *  rain puddles). */
    [[nodiscard]] inline float Fract(const float v) noexcept { return v - std::floor(v); }
    [[nodiscard]] inline float Hash12(const float x, const float y) noexcept
    {
        float a = Fract(x * 0.1031f), b = Fract(y * 0.1031f), c = Fract(x * 0.1031f);
        const float d = a * (b + 33.33f) + b * (c + 33.33f) + c * (a + 33.33f);
        a += d; b += d; c += d;
        return Fract((a + b) * c);
    }
    [[nodiscard]] inline float ValueNoise(const float x, const float y) noexcept
    {
        const float cx = std::floor(x), cy = std::floor(y);
        float fx = x - cx, fy = y - cy;
        fx = fx * fx * (3.0f - 2.0f * fx);
        fy = fy * fy * (3.0f - 2.0f * fy);
        const float lower = Hash12(cx, cy) + (Hash12(cx + 1.0f, cy) - Hash12(cx, cy)) * fx;
        const float upper = Hash12(cx, cy + 1.0f) + (Hash12(cx + 1.0f, cy + 1.0f) - Hash12(cx, cy + 1.0f)) * fx;
        return lower + (upper - lower) * fy - 0.5f;
    }
    [[nodiscard]] inline float Smooth(const float a, const float b, const float v) noexcept
    {
        const float t = std::clamp((v - a) / (b - a), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
    /** How much rain puddle stands at (x, z), 0..1, as the terrain shader paints it: in a soaking
     *  (`wetness`) the flat hollows of the paths, clearings and forest floor hold water, not the
     *  beach's sand. */
    [[nodiscard]] inline float PuddleAt(const float x, const float z, const float height, const float slopeRadians,
                                        const float signedShoreDistance, const bool sand, const float wetness) noexcept
    {
        const float soak = Smooth(0.45f, 0.90f, wetness);
        if (!(soak > 0.0f) || sand) return 0.0f;
        const float hollow = ValueNoise(x * 0.31f - 17.3f, z * 0.31f + 41.9f) + ValueNoise(x * 0.83f + 9.1f, z * 0.83f - 3.7f) * 0.35f;
        return Smooth(0.40f - soak * 0.18f, 0.46f - soak * 0.18f, hollow) * (1.0f - Smooth(0.10f, 0.17f, slopeRadians))
            * Smooth(8.0f, 25.0f, signedShoreDistance) * Smooth(0.6f, 1.2f, height) * soak;
    }
}
