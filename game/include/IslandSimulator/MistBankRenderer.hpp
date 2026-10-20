#pragma once

#include "IslandSimulator/CascadeMistField.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** Banks of mist seen across the island: broad, very faint discs facing the eye with a
     *  long soft falloff (so where one dips into the ground the cut is all but invisible),
     *  pale grey-white, alpha-blended before the night grade (the dawn colours them), to
     *  220 m. */
    class MistBankRenderer final
    {
    public:
        static constexpr float DrawDistance = 220.0f;
        MistBankRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, std::size_t capacity);
        ~MistBankRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, std::span<const MistPuff> puffs);
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        std::size_t capacity_ = 0;
        int visible_ = 0;
    };
}
