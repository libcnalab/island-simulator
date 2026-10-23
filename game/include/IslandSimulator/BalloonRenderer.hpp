#pragma once
#include "IslandSimulator/BalloonFlight.hpp"
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

/** The hot-air balloon: the envelope of twenty-four gores in bands of red, yellow, blue and
 *  white, the load tapes down its seams, its mouth and skirt; the cables down to the burner
 *  frame and the wicker basket with its padded rim and two people in it; and the burner's
 *  flame (additive) while it fires. Built once in its own frame, turned, rocked, moved and lit
 *  by the sun every frame. */
class BalloonRenderer final {
public:
    explicit BalloonRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
    ~BalloonRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,const BalloonFlight::State& state);
    [[nodiscard]] bool Visible() const noexcept {return visible_;}
    [[nodiscard]] std::size_t TriangleCount() const noexcept {return parts_.size()/3U;}
    /** One template vertex: where it is and its normal in the balloon's frame (the mouth at the
     *  origin, y up), its colour, and whether light passes through it (the envelope's fabric). */
    struct Part {Microsoft::Xna::Framework::Vector3 position,normal;std::uint8_t r,g,b;bool fabric;};
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> flameBuffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_,flame_;
    std::vector<Part> parts_;
    bool visible_=false;
};

}
