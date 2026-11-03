#pragma once

#include "IslandSimulator/JungleMaterial.hpp"
#include <cstdint>

namespace island::terrain_incision
{
    // Custom terrain keeps its original RGB light. Alpha's complement stores up to one metre
    // of actual incision; the renderer retains ordinary 255 alpha in the stock fallback.
    inline constexpr float MaximumDepth=1.0f;
    inline constexpr std::array<float,4> BankTerms{.03f,.18f,.16f,.38f};
    [[nodiscard]] inline std::uint8_t Pack(float original,float drawn)
    {
        if(!std::isfinite(original)||!std::isfinite(drawn))
            throw std::invalid_argument("Incision heights must be finite");
        return static_cast<std::uint8_t>(255-std::lround(std::clamp((original-drawn)/MaximumDepth,0.0f,1.0f)*255));
    }
    [[nodiscard]] constexpr float Depth(std::uint8_t alpha) noexcept
    {
        return (255-alpha)*(MaximumDepth/255.0f);
    }
    [[nodiscard]] inline float BankCoverage(float depth,float slope)
    {
        return JungleMaterial::Smooth(BankTerms[0],BankTerms[1],depth)
             * JungleMaterial::Smooth(BankTerms[2],BankTerms[3],slope);
    }
    inline std::string Shader()
    {
        std::ostringstream out;
        out.imbue(std::locale::classic());
        out<<std::scientific<<std::setprecision(std::numeric_limits<float>::max_digits10)
            <<"const float terrainIncisionMaximum="<<MaximumDepth<<";\n"
            <<"const vec4 terrainIncisionBankTerms=vec4("<<BankTerms[0]<<','<<BankTerms[1]<<','<<BankTerms[2]<<','<<BankTerms[3]<<");\n";
        out<<R"(
float terrainIncisionBank(float encoded,float slope) {
    return smoothstep(terrainIncisionBankTerms.x,terrainIncisionBankTerms.y,encoded*terrainIncisionMaximum)
        *smoothstep(terrainIncisionBankTerms.z,terrainIncisionBankTerms.w,slope);
}
)";
        return out.str();
    }
}
