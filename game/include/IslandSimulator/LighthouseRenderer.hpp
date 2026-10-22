#pragma once
#include "IslandSimulator/LighthouseLayout.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
class SunShadowMap;

// The lighthouse in one opaque vertex-coloured draw: a tapering masonry tower, whitewashed with
// two red bands, on an octagonal stone plinth, with its door facing inland and small windows up
// its height; the gallery on corbels with its railing; the twelve-sided glazed lantern on its
// low wall under a red dome, ball and rod. Rust bleeding from under the gallery and the
// windows, salt grime at its foot; lit by the hour's sun and receiving and casting the sun
// shadows like the jetty.
class LighthouseRenderer final {
public:
    LighthouseRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                       const LighthousePlacement& placement,const IslandTerrain& terrain,
                       const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,drawCalls_=0;
    bool sunShadowUnavailable_=false;
};
}
