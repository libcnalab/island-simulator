#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    class IslandTerrain;

    /**
     * Rain striking the land round the walker: in heavier rain each cell of a fixed world grid
     * takes a drop now and then, and the drop throws a small crown of droplets that fly out and
     * fall back within a fraction of a second, so the ground near the walker seethes with pale
     * splashes in a downpour. World-anchored, faded towards the edge of its radius, drawn before
     * the night grade with the rain; the sea has its own rings.
     */
    class RainSplashRenderer final
    {
    public:
        static constexpr float CellSize = 0.4f, Radius = 9.0f, Life = 0.16f;
        static constexpr int Droplets = 4;
        RainSplashRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain);
        ~RainSplashRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float rain, float seconds);
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const IslandTerrain& terrain_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
