#pragma once
#include "IslandSimulator/BoarHerd.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
// The boars' wallows: a dark patch of churned wet mud at each bank point they wallow at,
// elongated along the bank, terrain-conforming, alpha-blended in the contact-shadow pass.
// Built once; the sites do not move.
class WallowScarRenderer final {
public:
    static constexpr int Segments=14,VerticesPerScar=Segments*3;
    static constexpr float AlongBank=1.15f,TowardWater=0.8f;   // half-axes in metres
    WallowScarRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
                       std::span<const BoarHerd::WallowSite> sites);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int ScarCount() const noexcept {return scars_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Vector3> centres_;
    int scars_=0,drawCalls_=0;
};
}
