#pragma once
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/WhaleSkeletonLayout.hpp"
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

// A stranded whale's bleached skeleton half sunk in the beach sand, in one opaque
// vertex-coloured draw: the skull with its broad rostrum and domed braincase, the two bowed
// jaws, the spine of forty-four vertebrae tapering to the tail (the last few scattered), the
// ribs splayed into the sand (some fallen flat, two gone), the shoulder blades and a flipper's
// bones. Weathered ivory bone, sand-stained where it meets the beach; lit by the hour's sun and
// receiving and casting the sun shadows like the jetty.
class WhaleSkeletonRenderer final {
public:
    static constexpr int VertebraCount=44,RibPairs=13;
    WhaleSkeletonRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                          const WhaleSkeletonPlacement& placement,const IslandTerrain& terrain,
                          const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    [[nodiscard]] int RibCount() const noexcept {return ribs_;}
    [[nodiscard]] const Microsoft::Xna::Framework::Vector3& Centre() const noexcept {return centre_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;   // null: the BasicEffect fallback
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,drawCalls_=0,ribs_=0;
    bool sunShadowUnavailable_=false;
};
}
