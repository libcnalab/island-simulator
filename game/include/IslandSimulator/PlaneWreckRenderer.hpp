#pragma once
#include "IslandSimulator/PlaneWreckLayout.hpp"
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

// The wreck of a Second World War navy fighter in one opaque vertex-coloured draw: its fuselage
// nose-down in the ground and broken behind the cockpit, the tail section thrown aside and
// twisted; the radial engine behind its cowling and the three-bladed propeller bent back; the
// framed canopy, its rear panes gone; the right wing crumpled at its root, the left torn off and
// lying upside down beside it; a landing leg sticking up. Faded dark green over light grey,
// the paint worn through to aluminium in patches and white with corrosion, the red discs
// faded; lit by the hour's sun and receiving and casting the sun shadows.
class PlaneWreckRenderer final {
public:
    PlaneWreckRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                       const PlaneWreckPlacement& placement,const IslandTerrain& terrain,
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
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_,glass_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,glassTriangles_=0,drawCalls_=0;
    bool sunShadowUnavailable_=false;
};
}
