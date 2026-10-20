#pragma once
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/JettyLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
class SunShadowMap;

// One opaque vertex-coloured draw: plank deck, stringers, posts, the mooring post and the
// dugout canoe with its paddle, lit by the fixed sun per face at build time.
class JettyRenderer final {
public:
    JettyRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                  const JettyPlacement& jetty);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void DrawSunShadow(SunShadowMap& map);
    /** The map this structure's own surfaces receive (StructureShadowReceiver). */
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;   // null: the BasicEffect fallback
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,drawCalls_=0;
    bool sunShadowUnavailable_=false;
};
}
