#pragma once
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
class IslandTerrain;
// The goats' rain-filled rock basin: a terrain-conforming disc of still dark water with a
// paler sky-lit far edge, inside a ring of wet soil fading to the dry slope; one static
// draw, premultiplied and alpha-blended with depth read, culled beyond 90 m.
class WaterholeRenderer final {
public:
    static constexpr int Segments=24;
    WaterholeRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
                      float x,float z,float radius);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int count_=0,drawCalls_=0;
};
}
