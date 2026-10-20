#pragma once

#include "IslandSimulator/WhalePod.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** A surfacing humpback far offshore: its long dark back rolling up through the swell and
     *  under again with the small dorsal fin near the tail, and (one roll in three) the broad
     *  flukes raised as it sounds; shaded by the hour's light, drawn before the sea so the
     *  water covers what lies under it, and fogged with the distance. */
    class WhaleRenderer final
    {
    public:
        explicit WhaleRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~WhaleRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, const WhaleSurfacing& whale);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int drawCalls_ = 0;
    };
}
