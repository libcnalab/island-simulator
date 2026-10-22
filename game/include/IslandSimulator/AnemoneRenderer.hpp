#pragma once

#include <memory>
#include <span>
#include <vector>

#include "IslandSimulator/ReefFishField.hpp"
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
    /** @brief One-draw renderer for the reef's sea anemones: a purple column and oral disc
     *  crowned by a mop of tan, pink-tipped tentacles swaying in the surge, within 25 m. */
    class AnemoneRenderer final
    {
    public:
        static constexpr int Tentacles = 40, VerticesPerAnemone = 1548, MaximumVisible = 8;
        static constexpr float DrawDistance = 25.0f;
        explicit AnemoneRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~AnemoneRenderer();
        AnemoneRenderer(const AnemoneRenderer&) = delete;
        AnemoneRenderer& operator=(const AnemoneRenderer&) = delete;

        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& cameraPosition,
                  std::span<const ReefAnemone> anemones, float timeSeconds);

        [[nodiscard]] int VisibleCount() const noexcept { return visibleCount_; }
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCallCount_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visibleCount_ = 0;
        int drawCallCount_ = 0;
    };
}
