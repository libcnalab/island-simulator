#pragma once
#include "IslandSimulator/CascadeMistRenderer.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/VolcanoSite.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <array>
#include <memory>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
class SunShadowMap;

// The small volcano: its cone of ash and black lava (dark basalt low down, grey ash high up,
// rain gullies, an old flow down one flank, sulphur on the rim) lit by the hour's sun and
// casting and taking the sun shadows; the crater's sheer wall; the lava lake at its foot, its
// crust plates drifting between glowing seams (a shader of its own); the wall lit orange by
// it; and a white plume of steam rising out of the crater and leaning away downwind. The lava
// and its light are drawn over the night's grade, so the crater glows in the dark.
class VolcanoRenderer final {
public:
    VolcanoRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
                    const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    /** The steam plume (with the scene's other mists, before the grade). */
    void DrawPlume(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                   const Microsoft::Xna::Framework::Vector3& camera,float seconds);
    /** The lava lake and its light on the crater's wall, over the hour's grade; at night
     *  (`night` 0..1) the glow it throws up into the steam over the crater as well. */
    void DrawGlow(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera,float seconds,float night);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return coneTriangles_;}
    static constexpr int PlumePuffs=22,VentPuffs=4;
private:
    [[nodiscard]] bool Visible(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                               const Microsoft::Xna::Framework::Vector3& camera) const;
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_,lavaEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> cone_,glow_,lava_;
    std::unique_ptr<CascadeMistRenderer> plume_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> halo_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> haloStaging_;
    std::array<MistPuff,PlumePuffs+4*VentPuffs> puffs_{};
    Microsoft::Xna::Framework::BoundingBox bounds_;
    int coneTriangles_=0,glowTriangles_=0,lavaTriangles_=0,drawCalls_=0;
    bool sunShadowUnavailable_=false;
};
}
