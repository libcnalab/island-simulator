#pragma once
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/WindthrowLayout.hpp"
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

// The windthrown jungle giant: its furrowed trunk lying along the floor, mossed along its top,
// shelf fungi up its flanks and ferns rooting on it, its snapped end splintered pale and the
// crown's broken limbs strewn beyond; at its foot the torn-up root plate standing on edge, earth,
// clay and stones held in the broken roots, its old forest-floor face toward the trunk, and the
// wet pit behind it. One buffer, drawn when in view and near; lit by the hour's sun, receiving and
// casting the sun shadows.
class WindthrowRenderer final {
public:
    WindthrowRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const WindthrowPlacement& placement,
                      const IslandTerrain& terrain,const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    static constexpr float DrawDistance=180.0f;
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,castingTriangles_=0,drawCalls_=0;
    bool sunShadowUnavailable_=false;
};
}
