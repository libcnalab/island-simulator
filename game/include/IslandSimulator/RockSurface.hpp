#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace island
{
    inline constexpr float RockSurfaceMaximumHeight = 1.23f;
    // Buried basal rim: closes downhill gaps without moving placements, crowns or perches.
    // Includes relief at accepted coastal/clearing sites; shared by both LODs and culling.
    inline constexpr float RockSurfaceMinimumHeight = -1.25f;
    inline constexpr int RockSurfaceNearSlices = 24;
    inline constexpr int RockSurfaceFarSlices = 7;
    struct RockSurfaceVertex
    {
        std::array<float, 3> position;
        std::array<float, 3> normal;
    };

    struct RockSurface
    {
        std::vector<RockSurfaceVertex> vertices;
        std::vector<std::uint16_t> indices;
    };

    // Four shared authored profiles, with higher angular resolution only at close distance.
    // Normals are area weighted over shared topology; the basal rim remains open and buried.
    [[nodiscard]] RockSurface BuildRockSurface(int variant, bool farMesh);
}
