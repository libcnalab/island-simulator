#pragma once
#include "IslandSimulator/LandCrabMigration.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
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

/** The marching red crabs in one batch: a near crab (a domed carapace, eight jointed legs
 *  stepping in their wave, two claws and stalked eyes) and a coarse one past NearDistance. */
class LandCrabRenderer final {
public:
    static constexpr float NearDistance=9.0f;
    static constexpr std::size_t MaximumCrabs=700;
    explicit LandCrabRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    ~LandCrabRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,std::span<const MigratingCrab> crabs);
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
    [[nodiscard]] int DrawCallCount() const noexcept {return draws_;}
    [[nodiscard]] std::size_t NearTemplateVertices() const noexcept {return near_.size();}

    // A template vertex: its place on a crab one unit across, facing +z, and which leg (0..7)
    // it belongs to with how far down that leg it lies (0 at the body, 1 at the tip), or -1.
    struct TemplateVertex {float x,y,z;std::uint8_t r,g,b;std::int8_t leg;float reach;};
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    std::vector<TemplateVertex> near_,far_;
    int visible_=0,draws_=0;
};

}
