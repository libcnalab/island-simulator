#pragma once
#include "IslandSimulator/PandanusLayout.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
class SunShadowMap;

// The screw pines of the coastal fringe: grey ringed trunks on cones of orange-tipped stilt roots,
// forking into branches that end in spiral tufts of long, arching, sword-like leaves, the old ones
// yellowed and the dead ones hanging brown in a skirt below; now and then a pineapple-like fruit.
// One buffer in sectors round the island, each drawn when in view and near; lit by the hour's sun,
// receiving and casting the sun shadows.
class PandanusRenderer final {
public:
    PandanusRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,std::span<const PandanusPlacement> trees,
                     const IslandTerrain& terrain,const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleSectors() const noexcept {return visibleSectors_;}
    static constexpr float DrawDistance=260.0f;
private:
    struct Sector {int first=0,triangles=0;Microsoft::Xna::Framework::BoundingBox bounds;Microsoft::Xna::Framework::Vector3 centre;};
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::vector<Sector> sectors_;
    int drawCalls_=0,visibleSectors_=0;
    bool sunShadowUnavailable_=false;
};
}
