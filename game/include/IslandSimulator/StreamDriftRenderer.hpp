#pragma once
#include "IslandSimulator/CanopyLeafFallField.hpp"
#include "IslandSimulator/StreamDriftField.hpp"
#include <algorithm>
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace island {
// Small flat leaves on the stream surface, rebuilt each frame from the drift field: one
// dynamic vertex-coloured draw, drawn after the stream water.
class StreamDriftRenderer final {
public:
    static constexpr int VerticesPerLeaf=12;   // two lobes, two triangles each, both faces by CullNone
    // The same renderer draws the stream's drifting leaves and the canopy's falling ones, so
    // its buffer holds the larger field; a frame never uploads more than this.
    static constexpr std::size_t MaximumLeaves=std::max(StreamDriftField::Capacity,CanopyLeafFallField::Capacity);
    explicit StreamDriftRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const DriftingLeaf> leaves);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
};
}
