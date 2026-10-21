#pragma once
#include "IslandSimulator/EnvironmentLighting.hpp"
#include <algorithm>
#include <cmath>

namespace island::underwater_fog
{
    /** Seawater limits a swimmer's sight: a few clear metres, then the seabed, the reef and the
     *  fish fade into the water's own blue-green, mostly gone by thirty-five metres. Above the
     *  surface nothing changes. */
    inline constexpr float Density = 0.085f;                       // per metre, exponential (custom shaders)
    inline constexpr float LinearStart = 2.0f, LinearEnd = 38.0f;  // BasicEffect's linear fog

    struct State
    {
        float amount = 0.0f;   // 0 above the surface, 1 from 0.8 m down: no pop as the eye bobs
        float red = 0.17f, green = 0.50f, blue = 0.55f;
    };

    [[nodiscard]] inline State& Live() noexcept
    {
        static State state;
        return state;
    }

    /** @brief Once a frame, before the world is drawn: the eye's depth under the sea surface
     *  (0 or less above it). The water a little darker and bluer the deeper the eye. */
    inline void Update(const float cameraDepth) noexcept
    {
        State& state = Live();
        const float depth = std::isfinite(cameraDepth) ? cameraDepth : 0.0f;
        const float t = std::clamp(depth / 0.8f, 0.0f, 1.0f);
        state.amount = t * t * (3.0f - 2.0f * t);
        const float dim = std::exp(-0.05f * std::clamp(depth, 0.0f, 20.0f));
        state.red = 0.17f * dim * dim;
        state.green = 0.50f * dim;
        state.blue = 0.55f * dim;
    }

    /** The custom shaders' half: unbound (zero) it leaves the colour alone. */
    inline constexpr const char* ShaderDefinition =
        "uniform vec4 uUnderwaterFog;   // rgb the water's colour, w its density (0 above the surface)\n"
        "vec3 applyUnderwaterFog(vec3 color, float distanceToCamera) {\n"
        "    return uUnderwaterFog.w > 0.0\n"
        "        ? mix(color, uUnderwaterFog.rgb, 1.0 - exp(-distanceToCamera * uUnderwaterFog.w)) : color;\n"
        "}\n";

    /** @brief Sets a custom shader's uUnderwaterFog (a ShaderEffect). */
    template <class ShaderEffect>
    void Bind(ShaderEffect& effect)
    {
        const State& state = Live();
        effect.SetUniformVec4("uUnderwaterFog", state.red, state.green, state.blue,
                              Density * state.amount);
    }

    /** @brief A BasicEffect renderer's fog for this frame: its own land range and colour above
     *  the surface, the water's below it. */
    template <class BasicEffect>
    void Apply(BasicEffect& effect, const float landStart, const float landEnd)
    {
        const State& state = Live();
        const float a = state.amount;
        effect.setFogStartProperty(landStart + (LinearStart - landStart) * a);
        effect.setFogEndProperty(landEnd + (LinearEnd - landEnd) * a);
        effect.setFogColorProperty({
            environment::FogColorRed + (state.red - environment::FogColorRed) * a,
            environment::FogColorGreen + (state.green - environment::FogColorGreen) * a,
            environment::FogColorBlue + (state.blue - environment::FogColorBlue) * a});
    }
}
