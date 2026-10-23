#pragma once
#include "IslandSimulator/FootprintTrail.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
// The walker's prints on the sand: a sole and a toe pad each, terrain-conforming dark fans
// fading with the print's age, alpha-blended with depth read after the contact shadows.
class FootprintTrailRenderer final {
public:
    static constexpr int Segments=8,VerticesPerPrint=Segments*3*2;
    // room for a tyrannosaur's whole trail (40 prints of four fans), more than the walker's
    static constexpr int BufferVertices=VerticesPerPrint*2*static_cast<int>(FootprintTrail::Capacity);
    FootprintTrailRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
        const RenderedTerrainSurface& ground);   // laid on the drawn ground where the mesh reaches
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const Footprint> prints);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const IslandTerrain& terrain_;
    const RenderedTerrainSurface& ground_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
};
}
