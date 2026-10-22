#pragma once
#include "IslandSimulator/FlyingFishShoal.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {

// The flying fish in flight: slender dark-blue backs over silver bellies, the great pectoral fins
// spread like a dragonfly's wings (folding as they drop back in), the smaller pelvic pair behind,
// and the forked tail with its long lower lobe. One dynamic batch, lit by the hour's sun.
class FlyingFishRenderer final {
public:
    static constexpr float DrawDistance=260.0f;
    explicit FlyingFishRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,std::span<const FlyingFishState> fish);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
};

}
