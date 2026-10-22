#pragma once
#include "IslandSimulator/FreighterLayout.hpp"
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

// The grounded coaster in one opaque vertex-coloured draw: its riveted steel hull (red below the
// white boot-top, black above) streaked and blistered with rust and green with weed at the
// waterline, the bulwarks, the deck and its hatch with the after covers still on, the hold under
// it dim but for the light through the open hatch, the stair down into it, the accommodation
// ladder to the sand, the deckhouse with its open door, mess table and portholes, the wheelhouse
// and the funnel above it, the masts with their derricks and stays, the windlass with its chains
// and anchors, bollards and ventilators. Lit by the hour's sun; receives and casts the sun shadows.
class FreighterRenderer final {
public:
    FreighterRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                      const FreighterPlacement& placement,const IslandTerrain& terrain,
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
