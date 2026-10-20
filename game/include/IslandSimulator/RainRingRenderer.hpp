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
     * Rain on the sea: the rings each drop sends out on the water round the walker, spreading
     * and fading within two thirds of a second, on the wave surface (the shared spectrum),
     * world-anchored (rain_rings) and only over water; thin pale circles, alpha-blended before
     * the night grade.
     */
    class RainRingRenderer final
    {
    public:
        static constexpr float Radius = 13.0f;
        static constexpr int Segments = 14;
        static constexpr int MaximumRings = 1400;

        RainRingRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain);
        ~RainRingRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float rain, float seconds);
        [[nodiscard]] int VisibleRings() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const IslandTerrain& terrain_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
