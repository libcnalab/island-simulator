#pragma once

#include "IslandSimulator/CanopyProfile.hpp"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

namespace island::jungle_root_grounding
{
    inline constexpr float FullHeight = 0.15f;
    // Ordinary buttresses merge into the trunk below attachments; mangrove stilts
    // retain their longer existing blend into the bank.
    inline constexpr float TrunkFadeHeight = 0.90f;
    inline constexpr float MangroveFadeHeight = 1.40f;
    inline constexpr float FootDepth = 0.016f;

    // Preserve every original trunk ring/contact surface. One extra lower ring closes
    // the downhill gap; uphill portions stay buried rather than lifting the shared trunk.
    [[nodiscard]] inline std::vector<float> RingHeightsWithFoot(int variant, bool farMesh)
    {
        auto rings = CanopyTrunkRingHeights(variant, farMesh);
        rings.insert(rings.begin(), -FootDepth);
        return rings;
    }

    [[nodiscard]] inline std::array<float,3> TrunkVertexWithFoot(int variant, float height,
        float angle, bool farMesh, std::optional<float> windTime = std::nullopt)
    {
        auto p = CanopyTrunkVertex(variant, std::max(0.0f, height), angle, farMesh, windTime);
        if (height < 0) p[1] = height;
        return p;
    }

    [[nodiscard]] inline float Weight(float localHeight, bool mangrove) noexcept
    {
        const float end = mangrove ? MangroveFadeHeight : TrunkFadeHeight;
        const float t = std::clamp((localHeight - FullHeight) / (end - FullHeight), 0.0f, 1.0f);
        return 1.0f - t*t*(3.0f - 2.0f*t);
    }

    // Shared by visible and sun-shadow vertices. Only explicitly tagged low wood
    // deforms; crowns, branches, climbing surfaces above the blend and plants keep their frame.
    [[nodiscard]] inline std::string Shader()
    {
        std::ostringstream out;
        out.imbue(std::locale::classic());
        out << std::scientific << std::setprecision(std::numeric_limits<float>::max_digits10)
            << "const vec3 jungleRootHeights=vec3(" << FullHeight << ',' << TrunkFadeHeight
            << ',' << MangroveFadeHeight << ");\n";
        out << R"(
vec4 groundJungleRoot(vec4 world,float localHeight,inout float groundHeight,float materialTag) {
    if (materialTag < -1.5) {
        float endHeight=materialTag < -2.5 ? jungleRootHeights.y : jungleRootHeights.z;
        float weight=1.0-smoothstep(jungleRootHeights.x,endHeight,localHeight);
        if (weight>0.0) {
            float relief=grassTerrainHeight(world.xz)-groundHeight;
            if (materialTag < -2.5) relief=min(relief,0.0);
            float shift=relief*weight;
            world.y+=shift;
            groundHeight+=shift;
        }
    }
    return world;
}
)";
        return out.str();
    }
}
