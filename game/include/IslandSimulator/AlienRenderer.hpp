#pragma once
#include "IslandSimulator/AlienMotion.hpp"
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
/** The alien, built afresh each frame from its pose: the long thin grey body lofted from the
 *  narrow hips over the slim waist to the chest, the slender neck, the great egg-shaped head
 *  widest above the eyes and tapering to a small chin, two big slanted black eyes with a glint,
 *  nostril dots and a thin mouth, thin legs and long arms from the motion's joints on small
 *  splayed feet and long three-fingered hands with a thumb. (Its sun shadow is its figure in
 *  walker_shadow::AlienLimbs.) */
class AlienRenderer final {
public:
    AlienRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,float maximumDistance);
    ~AlienRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,const AlienMotion& alien);
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
