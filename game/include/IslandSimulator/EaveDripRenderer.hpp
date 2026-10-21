#pragma once

#include "IslandSimulator/HutLayout.hpp"
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
     * The rain running off the hut's thatch: in rain every leaf tip along both eaves drips,
     * drops falling (with gravity, so they streak longer as they fall) to the ground below and
     * splashing there in a short ring, more drops from each tip the harder it rains, until in
     * a downpour the eaves pour in near-curtains. World-anchored, before the night grade.
     */
    class EaveDripRenderer final
    {
    public:
        static constexpr int DropsPerTip = 3;
        EaveDripRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const HutPlacement& hut,
                         const IslandTerrain& terrain);
        ~EaveDripRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float rain, float seconds);
        [[nodiscard]] int TipCount() const noexcept { return static_cast<int>(tips_.size()); }
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        struct Tip
        {
            Microsoft::Xna::Framework::Vector3 at;
            float ground, fallSeconds;
        };
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::vector<Tip> tips_;
        Microsoft::Xna::Framework::Vector3 centre_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
