#pragma once
#include "IslandSimulator/TRexBody.hpp"
#include "IslandSimulator/TRexMotion.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class DynamicVertexBuffer;
class GraphicsDevice;
class IndexBuffer;
class ShaderEffect;
struct VertexPositionColor;
}

namespace island {
class SunShadowMap;

/** The tyrannosaur: its skin (TRexBody) posed afresh each frame and drawn in one indexed call
 *  with its own material: lit per pixel from the smoothed normals by the hour's sun and the
 *  sky, its pebbled scales raised in relief from where they lie on the body at rest (fading out
 *  smoothly before they shrink to a few pixels), the trees' shadows and the clouds' shade laid
 *  over it, darker and glossier wet; teeth and claws of horn, the eyes with their iris, pupil and
 *  the sky's glint, the mouth wet inside. Where custom shaders are not run, the same mesh is lit
 *  per vertex with the scene's other animals. Its ground shadow is cast with the other figures'
 *  (walker_shadow::TRexLimbs). */
class TRexRenderer final {
public:
    TRexRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,float maximumDistance);
    ~TRexRenderer();
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,const TRexMotion& rex);
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    [[nodiscard]] bool Visible() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    float maximumDistance_;
    TRexBody body_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> skin_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> fallback_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> indices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> fallbackVertices_;
    const SunShadowMap* sunShadow_=nullptr;
    int triangles_=0;
    bool visible_=false;
};

}
