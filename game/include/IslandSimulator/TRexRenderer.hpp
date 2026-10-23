#pragma once
#include "IslandSimulator/TRexMotion.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class DynamicVertexBuffer;
class GraphicsDevice;
struct VertexPositionColor;
}

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;

/** The tyrannosaur, built afresh each frame from its pose: a body lofted along its spine from
 *  the tail's tip through the hips and the deep chest to the S of the neck, the deep skull with
 *  its forward-facing eyes over the brow, the lower jaw hinged under it and the rows of teeth,
 *  the massive thighs, shins, metatarsi and three-toed clawed feet from the leg joints, the
 *  tiny two-fingered arms; a scaly hide darker along the back with faint bands, paler beneath;
 *  and its shadow laid on the ground along the sun. */
class TRexRenderer final {
public:
    TRexRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
                 const RenderedTerrainSurface& ground,float maximumDistance);
    ~TRexRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,const TRexMotion& rex);
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    [[nodiscard]] bool Visible() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const IslandTerrain& terrain_;
    const RenderedTerrainSurface& ground_;
    float maximumDistance_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_,shadowBuffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> vertices_,shadow_;
    int triangles_=0;
    bool visible_=false;
};

}
