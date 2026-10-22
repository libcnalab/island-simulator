#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/TermiteNestLayout.hpp"
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
class SunShadowMap;

// The arboreal termites' nests on the jungle trunks: dark lumpy cartons of chewed wood and earth
// hugging the bark a few metres up, their covered mud runways meandering down the trunk to the
// ground and up it toward the crown. One buffer in sectors round the island, each drawn when in
// view and near; lit by the hour's sun and receiving the sun shadows (the crowns' among them).
class TermiteNestRenderer final {
public:
    TermiteNestRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,std::span<const TermiteNest> nests,
                        std::span<const CanopyPlacement> canopy);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    static constexpr float DrawDistance=150.0f;
private:
    struct Sector {int first=0,triangles=0;Microsoft::Xna::Framework::BoundingBox bounds;};
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::vector<Sector> sectors_;
    int drawCalls_=0;
};
}
