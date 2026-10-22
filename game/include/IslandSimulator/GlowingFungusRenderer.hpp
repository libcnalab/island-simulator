#pragma once
#include "IslandSimulator/GlowingFungusLayout.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class SunShadowMap;

// The foxfire on the dead wood: clumps of little mushrooms on thin stalks, their caps pale buff by
// day; on dark nights each cap glows a soft yellow-green in a faint halo over the wood (additive
// after the night's grade: they make their own light), depth-tested against the wood and leaves.
class GlowingFungusRenderer final {
public:
    static constexpr float DrawDistance=90.0f,GlowDistance=45.0f;
    GlowingFungusRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,std::span<const FungusCluster> clusters);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    /** The caps' glow, `dark` 0 by day to 1 on a moonless night. */
    void DrawGlow(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera,float dark);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int GlowingCount() const noexcept {return glowing_;}
private:
    struct Cap {Microsoft::Xna::Framework::Vector3 centre;float radius;std::size_t cluster;};
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_,glowEffect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> glow_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    std::vector<Cap> caps_;
    std::vector<FungusCluster> clusters_;
    std::vector<Microsoft::Xna::Framework::BoundingBox> clusterBounds_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    int triangles_=0,drawCalls_=0,glowing_=0;
};
}
