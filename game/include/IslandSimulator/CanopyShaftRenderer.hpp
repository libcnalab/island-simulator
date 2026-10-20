#pragma once
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
// Full-screen pass drawn under the canopy: slow sun shafts leaning toward the sun's side of
// the frame, brightest high in the view, scaled by CanopyShafts::Strength at the camera.
class CanopyShaftRenderer final {
public:
    explicit CanopyShaftRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    void Draw(float strength,float seconds,const Microsoft::Xna::Framework::Matrix& view,
              int viewportWidth,int viewportHeight);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] bool Ready() const noexcept {return effect_!=nullptr;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    int drawCalls_=0;
};
}
