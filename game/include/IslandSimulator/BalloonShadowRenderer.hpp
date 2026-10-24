#pragma once

#include "IslandSimulator/BalloonFlight.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class DynamicVertexBuffer;
class GraphicsDevice;
struct VertexPositionColor;
}

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;

/** A single soft projected envelope shadow, laid on either the island or the sea after the ocean
 * pass. It is independent of the camera-centred static sun-shadow map because the balloon is both
 * high and moving. */
class BalloonShadowRenderer final {
public:
    static constexpr int Segments=24,VerticesPerShadow=Segments*9;
    static constexpr float MaximumCameraDistance=280.0f;
    BalloonShadowRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
        const IslandTerrain& terrain,const RenderedTerrainSurface& ground);
    ~BalloonShadowRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
        const Microsoft::Xna::Framework::Matrix& projection,
        const Microsoft::Xna::Framework::Vector3& camera,const BalloonFlight::State& balloon);
    [[nodiscard]] int VisibleCount() const noexcept{return visible_;}
    [[nodiscard]] int DrawCallCount() const noexcept{return drawCalls_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const IslandTerrain& terrain_;
    const RenderedTerrainSurface& ground_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int visible_=0,drawCalls_=0;
};
}
