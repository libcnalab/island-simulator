#pragma once
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
// Full-screen pass drawn over the scene while the camera is below the sea surface: sun-tilted
// light shafts that fade with depth, and the diver's own rising bubbles while they swim. The
// seabed caustics live in the terrain material; this pass only adds what hangs in the water.
class UnderwaterEffectRenderer final {
public:
    explicit UnderwaterEffectRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    // cameraDepth: metres below the surface (0 disables). swimIntensity: the water-view veil
    // level, which gates bubbles to the diver's own breathing. bubbleBurst: 0..1 recent descent.
    void Draw(float cameraDepth,float swimIntensity,float bubbleBurst,float seconds,
              const Microsoft::Xna::Framework::Matrix& view,int viewportWidth,int viewportHeight);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] bool Ready() const noexcept {return effect_!=nullptr;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    int drawCalls_=0;
};
}
