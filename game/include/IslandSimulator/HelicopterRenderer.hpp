#pragma once

#include "IslandSimulator/HelicopterLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include <memory>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class GraphicsDevice;
class Texture2D;
class VertexBuffer;
}

namespace island {
/** Original procedural grounded utility helicopter with opaque and glazed batches. */
class SunShadowMap;
class HelicopterRenderer final {
public:
    HelicopterRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice&,
                       const HelicopterPlacement&);
    ~HelicopterRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    /** Renders the opaque airframe into the bound fixed-sun shadow map. */
    void DrawSunShadow(SunShadowMap& map);
    [[nodiscard]] int DrawCallCount() const noexcept{return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept{return triangles_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
    bool sunShadowUnavailable_=false;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> materialTexture_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_,glassVertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,glassTriangles_=0,drawCalls_=0;
};
}
