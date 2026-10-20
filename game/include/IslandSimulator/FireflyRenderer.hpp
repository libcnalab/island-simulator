#pragma once

#include "IslandSimulator/FireflySwarm.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** The fireflies' lanterns: a bright yellow-green point in a soft halo, additive after
     *  the night grade (they make their own light), depth-tested against the leaves. */
    class FireflyRenderer final
    {
    public:
        static constexpr float DrawDistance = 70.0f;
        explicit FireflyRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~FireflyRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, std::span<const Firefly> flies);
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
