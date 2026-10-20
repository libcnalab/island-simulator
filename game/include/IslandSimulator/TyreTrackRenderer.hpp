#pragma once
#include "IslandSimulator/QuadWheelTraces.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
// The quad's tyre tracks: each piece a terrain-conforming rut, darker inside soft edges,
// with the knobs' staggered prints across it, fading as the ground takes it back;
// alpha-blended with depth read beside the footprints.
class TyreTrackRenderer final {
public:
    static constexpr int KnobsPerSegment=5,VerticesPerSegment=(3*2+KnobsPerSegment*2)*6;
    static constexpr float DrawDistance=42.0f;
    TyreTrackRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const TyreTrack> tracks);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const IslandTerrain& terrain_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
};
}
