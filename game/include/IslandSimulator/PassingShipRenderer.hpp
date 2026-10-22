#pragma once
#include "IslandSimulator/PassingShip.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace island {

// The coastal freighter passing far offshore: a dark hull, a white bridge house aft, a funnel and
// two cranes, softened by the haze of its distance like the far islets, a pale wake behind it;
// at night its navigation lights: two white masthead lights, red to port, green to starboard,
// white astern, and a few lit windows (drawn over the night's grade).
class PassingShipRenderer final {
public:
    explicit PassingShipRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,float seconds);
    void DrawLights(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                    const Microsoft::Xna::Framework::Vector3& camera,float seconds,float night);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] bool Visible() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_,lightEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> hull_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> lights_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int triangles_=0,drawCalls_=0;
    bool visible_=false;
};

}
