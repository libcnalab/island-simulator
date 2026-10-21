#pragma once

#include <memory>
#include <span>
#include <vector>

#include "IslandSimulator/ReefHeronGroup.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    struct VertexPositionColor;
}

namespace island
{
    /** @brief One-draw renderer for the reef herons: a slim body tilted up at the breast, an
     *  S-curved neck, a dagger bill and long legs; in flight the neck drawn in, broad slow
     *  wings and the legs trailing. Dark slate or white. */
    class ReefHeronRenderer final
    {
    public:
        static constexpr int MaximumVerticesPerHeron = 2400;
        explicit ReefHeronRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                                   float maximumDistance = 170.0f);
        ~ReefHeronRenderer();

        ReefHeronRenderer(const ReefHeronRenderer&) = delete;
        ReefHeronRenderer& operator=(const ReefHeronRenderer&) = delete;

        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& cameraPosition,
                  std::span<const ReefHeronState> herons);

        [[nodiscard]] int VisibleCount() const noexcept { return visibleCount_; }
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCallCount_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        float maximumDistance_ = 170.0f;
        int visibleCount_ = 0;
        int drawCallCount_ = 0;
    };
}
