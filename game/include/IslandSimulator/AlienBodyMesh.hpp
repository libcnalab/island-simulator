#pragma once

#include "IslandSimulator/AlienMotion.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <vector>

namespace island
{
    // CPU-only construction shared by rendering and geometry qualification; no graphics device.
    void BuildAlienBodyMesh(const AlienState& state,
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor>& vertices);
}
