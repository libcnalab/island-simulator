#pragma once
#include "IslandSimulator/WeaverColony.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <array>
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class DynamicVertexBuffer;
class GraphicsDevice;
struct VertexPositionColor;
}

namespace island {

/** The weavers' nests in one batch, each built once in its own frame and hung from its frond
 *  every frame (swayed with the frond, rocking on its strips): the torn leaflet strips it is
 *  tied on by, the woven chamber (a kidney of grass in criss-cross strands, fresh green or
 *  dried to straw), the short entrance spout under it with the dark of the inside, the loose
 *  ends sticking out; a nest still being built is a bare hoop with a start at the roof. */
class WeaverNestRenderer final {
public:
    static constexpr float DrawDistance=110.0f;
    WeaverNestRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const WeaverColony& colony);
    ~WeaverNestRenderer();
    /** `offsets`: each site's frond point moved from rest this moment (one per site). */
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,float seconds,
              std::span<const std::array<float,3>> offsets);
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
    [[nodiscard]] std::size_t TriangleCount() const noexcept;
    struct TemplateVertex {float x,y,z;std::uint8_t r,g,b;};
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const WeaverColony& colony_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    std::vector<std::vector<TemplateVertex>> nests_;   // one mesh a nest, in its own frame
    int visible_=0;
};

}
