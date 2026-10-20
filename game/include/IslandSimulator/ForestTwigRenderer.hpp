#pragma once
#include "IslandSimulator/ForestTwigLayout.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>

namespace island {
class IslandTerrain;
// The fallen twigs: each a bent three-segment five-sided stick of dark bark with a paler
// broken end, lit by the fixed sun per facet, in one static vertex-coloured draw.
class ForestTwigRenderer final {
public:
    static constexpr int Segments=3,Sides=5,VerticesPerTwig=Segments*Sides*6+Sides*3;
    ForestTwigRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
                       std::span<const ForestTwigPlacement> twigs);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TwigCount() const noexcept {return twigs_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    int twigs_=0,primitives_=0,drawCalls_=0;
};
}
