#pragma once

#include "IslandSimulator/WalkerShadow.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include <functional>
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics
{
    class BlendState;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class ShaderEffect;
    struct VertexPositionColor;
}

namespace island
{
    class SunShadowMap;

    /**
     * Draws the walker's sun shadow (walker_shadow::Shape) on the ground: a grid draped on the
     * drawn terrain under where the shadow can fall, whose fragments carry themselves up the light
     * onto the feet's plane, so the figure lands shorter on a bank that faces the sun and longer
     * down one that falls away. The grid multiplies the ground by the very shade tint the
     * terrain takes under a tree, and only where the sun shadow map and the clouds still leave
     * it sunlit: inside a crown's shadow the walker's adds nothing.
     */
    class WalkerShadowRenderer final
    {
    public:
        explicit WalkerShadowRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~WalkerShadowRenderer();

        WalkerShadowRenderer(const WalkerShadowRenderer&) = delete;
        WalkerShadowRenderer& operator=(const WalkerShadowRenderer&) = delete;

        void SetSunShadowMap(const SunShadowMap* map) noexcept { sunShadow_ = map; }

        /** `strength`: the hour's shadow strength (none at night, faint under an overcast). */
        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const walker_shadow::Shape& shape, const Microsoft::Xna::Framework::Vector3& feet,
                  const Microsoft::Xna::Framework::Vector3& toSun, float strength,
                  const std::function<float(float, float)>& groundHeight);

        [[nodiscard]] int DrawCallCount() const noexcept { return draws_; }
        [[nodiscard]] int TriangleCount() const noexcept { return triangles_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BlendState> multiply_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        const SunShadowMap* sunShadow_ = nullptr;
        int draws_ = 0;
        int triangles_ = 0;
    };
}
