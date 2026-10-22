#pragma once

#include <memory>
#include <span>
#include <vector>

#include "IslandSimulator/FruitBatColony.hpp"
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
    /** @brief One-draw portable procedural renderer for the bounded fruit-bat colony. */
    class FruitBatRenderer final
    {
    public:
        /** @brief Room for `capacity` bats; `announce` prints the renderer's diagnostics (the
         *  fruit bats' renderer does, a second one for the mine's bats does not). */
        explicit FruitBatRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                                  float maximumDistance = 300.0f,
                                  std::size_t capacity = FruitBatColony::Count, bool announce = true);
        ~FruitBatRenderer();

        FruitBatRenderer(const FruitBatRenderer&) = delete;
        FruitBatRenderer& operator=(const FruitBatRenderer&) = delete;

        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& cameraPosition,
                  std::span<const FruitBatState> bats);

        [[nodiscard]] int VisibleCount() const noexcept { return visibleCount_; }
        [[nodiscard]] int NearCount() const noexcept { return nearCount_; }
        [[nodiscard]] int FarCount() const noexcept { return farCount_; }
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCallCount_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        float maximumDistance_ = 300.0f;
        std::size_t capacity_ = FruitBatColony::Count;
        int visibleCount_ = 0;
        int nearCount_ = 0;
        int farCount_ = 0;
        int drawCallCount_ = 0;
    };
}
