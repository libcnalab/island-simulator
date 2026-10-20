#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace island {
// Soft camera-facing puffs of spray at the cascade's foot: a fan each, white fading to clear
// at the rim, alpha-blended with depth read after the stream water.
class CascadeMistRenderer final {
public:
    static constexpr int Segments=10,VerticesPerPuff=Segments*3;
    // message: the startup diagnostic this instance prints (the cascade's by default; the surf
    // spray shares the renderer with its own line).
    // capacity: the most puffs one Draw uploads (the largest field drawn through this instance).
    explicit CascadeMistRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const char* message=nullptr,
                                 std::size_t capacity=CascadeMistField::Capacity);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const MistPuff> puffs);
    // Far puffs (a whale's blow offshore): fade out from `start` over `length` metres instead
    // of the near spray's 40 + 20, and drawn this much denser.
    void SetRange(float start,float length,float density=1.0f) noexcept {fadeStart_=start;fadeLength_=length;densityScale_=density;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    std::size_t capacity_=CascadeMistField::Capacity;
    float fadeStart_=40.0f,fadeLength_=20.0f,densityScale_=1.0f;
    int drawCalls_=0,visible_=0;
};
}
