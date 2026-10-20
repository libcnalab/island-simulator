#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /**
     * Falling rain, anchored in the world: drops sit on a fixed world grid of cells around the
     * camera and fall through fixed world heights, wrapping only at the edges of the volume
     * where they fade out, so walking or flying moves through the rain instead of dragging a
     * screen-locked sheet along. Each drop is a thin streak slanted with the wind, drawn before
     * the night grade so the night darkens it with everything else.
     */
    class RainRenderer final
    {
    public:
        static constexpr float CellSize = 1.2f, Radius = 26.0f, Height = 22.0f;
        static constexpr int DropsPerCell = 3;
        static constexpr float FallSpeed = 8.5f;          // a raindrop's terminal speed, m/s
        static constexpr float StreakLength = 0.6f;       // its motion blur over a frame or two
        static constexpr int MaximumDrops =
            static_cast<int>((2.0f * Radius / CellSize + 2.0f) * (2.0f * Radius / CellSize + 2.0f)) * DropsPerCell;

        explicit RainRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~RainRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float rain, float wind, float seconds);
        [[nodiscard]] int VisibleDrops() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
