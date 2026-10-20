#pragma once
#include <cstddef>
#include "IslandSimulator/RootingDustField.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace island {
// Small camera-facing puffs of soil at the rooting boars' snouts, brown fading to clear at
// the rim, premultiplied and alpha-blended with depth read.
class RootingDustRenderer final {
public:
    static constexpr int Segments=8,VerticesPerPuff=Segments*3;
    // message: the startup diagnostic this instance prints (the boars' by default); red, green,
    // blue: the puff colour (earth brown by default; the stingrays' sand shares the renderer).
    // capacity: the most puffs one Draw may upload (the field's own capacity); any beyond it
    // are left out of the frame rather than overflowing the vertex buffer.
    explicit RootingDustRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const char* message=nullptr,
                                 int red=118,int green=92,int blue=62,std::size_t capacity=18);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const DustPuff> puffs);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
    int red_=118,green_=92,blue_=62;
    std::size_t capacity_=18;
};
}
