#pragma once

#include "IslandSimulator/JungleMaterial.hpp"

namespace island::tide_pool_material
{
    // Terrain valueNoise is centred on zero; the pool's coverage/palette thresholds use 0..1.
    inline constexpr float NoiseBias=0.5f;
    inline constexpr std::array<float,3> BenchTerms{0.86f,1.10f,0.12f};
    inline constexpr float OuterReach=2.2f;
    // Sparse, subdued solution pits and hairline fractures; broad quiet rock stays readable.
    inline constexpr float PitStart=0.78f,PitEnd=0.92f,PitStrength=0.12f,JointStrength=0.10f;

    [[nodiscard]] constexpr float UnitNoise(float centred) noexcept {return centred+NoiseBias;}
    [[nodiscard]] constexpr float BenchExtent(float lobe,float edge) noexcept
    {
        return BenchTerms[0]+BenchTerms[1]*lobe+BenchTerms[2]*edge;
    }
    [[nodiscard]] inline float DetailWeight(float pixelWidth,float cellsPerMetre)
    {
        return JungleMaterial::DetailWeight(pixelWidth*cellsPerMetre);
    }
    inline std::string Shader()
    {
        std::ostringstream text;
        text.imbue(std::locale::classic());
        text<<std::scientific<<std::setprecision(std::numeric_limits<float>::max_digits10)
            <<"const float poolNoiseBias="<<NoiseBias<<";\n"
            <<"const float poolOuterReach="<<OuterReach<<";\n"
            <<"const vec3 poolBenchTerms=vec3("<<BenchTerms[0]<<','<<BenchTerms[1]<<','<<BenchTerms[2]<<");\n"
            <<"const vec4 poolWeatheringTerms=vec4("<<PitStart<<','<<PitEnd<<','<<PitStrength<<','<<JointStrength<<");\n"
            <<"const vec2 poolFilterRange=vec2("<<JungleMaterial::FilterStart<<','<<JungleMaterial::FilterEnd<<");\n";
        text<<R"(
float valueNoise(vec2 p);
float tidePoolNoise(vec2 p) { return valueNoise(p)+poolNoiseBias; }
float tidePoolDetail(float pixelWidth,float cellsPerMetre) {
    return 1.0-smoothstep(poolFilterRange.x,poolFilterRange.y,pixelWidth*cellsPerMetre);
}
)";
        return text.str();
    }
}
