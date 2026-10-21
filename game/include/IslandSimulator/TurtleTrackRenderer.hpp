#pragma once
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class RenderedTerrainSurface;
// The nesting turtles' crawls up the beach, one continuous ribbon through each nest's placed
// trace pieces: a soft-edged trough pressed by the plastron, a faint wavering tail drag down its
// middle and the paired flippers' oblique gouges either side, irregular from stroke to stroke,
// their outer ends swept back toward the sea. Premultiplied black, so it only darkens the sand
// (it never glows under the night grade); alpha-blended with depth read beside the footprints.
class TurtleTrackRenderer final {
public:
    static constexpr float DrawDistance=75.0f,FadeStart=58.0f;
    // laid on the drawn terrain (the analytic ground or a plane sank into the mesh in places)
    TurtleTrackRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                        std::span<const BeachDetailPlacement> details,
                        const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
    [[nodiscard]] int TrackCount() const noexcept {return static_cast<int>(tracks_.size());}
private:
    struct Track {
        Microsoft::Xna::Framework::Vector3 centre;
        float radius=0.0f;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> vertices;   // full alpha
    };
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Track> tracks_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
};
}
