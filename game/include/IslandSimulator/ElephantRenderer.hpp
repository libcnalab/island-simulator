#pragma once
#include "IslandSimulator/ElephantMotion.hpp"
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
/** The elephant, built afresh each frame from its pose: the deep barrel of the body lofted from
 *  the sloping rump over the arched back to the short neck; the head with the twin domes of its
 *  kind over the brow, small eyes, the lower lip and the small ears that fan out from its sides,
 *  freckled pink at the edges; the trunk along the motion's curve, ringed with wrinkles and
 *  ending in its one finger; the tusks; the pillar legs from the motion's joints, wrinkled at
 *  the joints, on round soles with their toenails; the thin tail with its tuft; and a grey-brown
 *  hide dusted paler over the back by the sand it throws. (Its sun shadow is its figure in
 *  walker_shadow::ElephantLimbs.) */
class ElephantRenderer final {
public:
    ElephantRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,float maximumDistance);
    ~ElephantRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,const ElephantMotion& elephant);
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    [[nodiscard]] bool Visible() const noexcept {return visible_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    float maximumDistance_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> vertices_;
    int triangles_=0;
    bool visible_=false;
};

}
