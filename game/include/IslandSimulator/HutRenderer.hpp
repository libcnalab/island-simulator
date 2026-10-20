#pragma once
#include "IslandSimulator/HutLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
namespace Microsoft::Xna::Framework::Graphics { class GraphicsDevice; class BasicEffect; class ShaderEffect; class VertexBuffer; class Texture2D; }
namespace island {
class SunShadowMap;
class HutRenderer final {
public:
    HutRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice&,const HutPlacement&);
    ~HutRenderer();
    // seconds: the render clock the gust fronts run on; the yard's hung fish and net swing.
    void Draw(const Microsoft::Xna::Framework::Matrix&,const Microsoft::Xna::Framework::Matrix&,
              const Microsoft::Xna::Framework::Vector3&,float seconds=0.0f);
    /** Renders the hut into the bound fixed-sun shadow map. */
    void DrawSunShadow(SunShadowMap& map);
    [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> material_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    bool sunShadowUnavailable_=false;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> thatch_;
    Microsoft::Xna::Framework::Vector3 thatchMean_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    float fishBarY_=0,netRailY_=0;
    int triangles_=0,drawCalls_=0;
};
}
