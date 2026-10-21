#pragma once
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <array>
#include <vector>
#include "IslandSimulator/HutLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
namespace Microsoft::Xna::Framework::Graphics { class GraphicsDevice; class BasicEffect; class DynamicVertexBuffer; class ShaderEffect; class VertexBuffer; class Texture2D; }
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
    /** The map the hut's own surfaces receive (its eaves on its walls, the trees on it). */
    void SetSunShadowMap(const SunShadowMap* map) noexcept { sunShadow_=map; }
    [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
    /** The lamp lit inside at dusk: the two front windows glowing warm through their mullions
     *  and the hurricane lantern hung on the veranda, drawn after the night grade (additive);
     *  `nightness` 0 by day .. 1 in the night. The lantern's light on the ground is a local
     *  light the game draws at LanternPosition(). */
    void DrawNightLights(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                         const Microsoft::Xna::Framework::Vector3& camera,float nightness,float seconds);
    [[nodiscard]] Microsoft::Xna::Framework::Vector3 LanternPosition() const noexcept { return lantern_; }
    /** The surfaces the lantern lights beside it, as triangles a little in front of them with
     *  their normal: the front wall with its gable, and the veranda's floor. */
    struct LitPlane { std::vector<Microsoft::Xna::Framework::Vector3> triangles; Microsoft::Xna::Framework::Vector3 normal; };
    [[nodiscard]] const std::array<LitPlane,2>& LanternPlanes() const noexcept { return lanternPlanes_; }
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> material_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    bool sunShadowUnavailable_=false;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> thatch_;
    Microsoft::Xna::Framework::Vector3 thatchMean_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    float fishBarY_=0,netRailY_=0;
    Microsoft::Xna::Framework::Vector3 lantern_;
    std::array<LitPlane,2> lanternPlanes_;
    std::vector<Microsoft::Xna::Framework::Vector3> windowCorners_;   // four per front window
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> glowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> glow_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> glowStaging_;
    int triangles_=0,drawCalls_=0;
};
}
