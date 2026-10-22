#pragma once
#include "IslandSimulator/BeachLitterLayout.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
class SunShadowMap;

// The washed-up PET bottles on the beaches, in groups along the shore drawn only near the eye:
// their caps and torn, sun-faded labels opaque; the plastic itself see-through (its far wall,
// then its near wall, blended over the sand, a glint along the faces turned to the sun); and a
// soft shade on the sand under each. Lit by the hour's sun and receiving the sun shadows.
class BeachLitterRenderer final {
public:
    BeachLitterRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                        std::span<const BeachBottle> bottles,const IslandTerrain& terrain,
                        const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    static constexpr float DrawDistance=140.0f;   // past it a bottle is under a pixel thick
private:
    struct Group {
        Microsoft::Xna::Framework::BoundingBox bounds;
        Microsoft::Xna::Framework::Vector3 centre;
        int solidStart=0,solidTriangles=0,shadeStart=0,shadeTriangles=0,clearStart=0,clearTriangles=0;
    };
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::vector<Group> groups_;
    int triangles_=0,drawCalls_=0;
};
}
