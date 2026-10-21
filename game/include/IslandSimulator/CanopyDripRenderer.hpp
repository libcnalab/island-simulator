#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    class CanopyLightField;
    class IslandTerrain;

    /**
     * The forest goes on dripping after the rain: under the crowns (where the canopy light field
     * is shaded) big drops let go from the leaves now and then, fall a few metres and splash on
     * the floor, on a fixed world grid round the walker, the more the wetter the leaves are; in
     * the rain itself the crowns drip too, under the rain. World-anchored, before the night
     * grade; none in the open.
     */
    class CanopyDripRenderer final
    {
    public:
        static constexpr float CellSize = 0.9f, Radius = 14.0f;
        CanopyDripRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain,
                           const CanopyLightField& light);
        ~CanopyDripRenderer();
        /** @param dripping how wet the leaves are to shed, 0 dry .. 1 streaming. */
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float dripping, float seconds);
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const IslandTerrain& terrain_;
        const CanopyLightField& light_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
