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
        /** @param drawDistance how far the banks show; @param flatten their height to their width;
         *  @param opacity a bank's alpha at its core (of 255) at full density;
         *  @param diagnostic the line the renderer reports itself with. */
        MistBankRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, std::size_t capacity,
                         float drawDistance = DrawDistance, float flatten = 0.45f, float opacity = 125.0f,
                         const char* diagnostic = "island-simulator: morning mist lies in banks over the stream's valley, the wetland and the coastal flats at first light and burns off as the sun climbs");
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
        float drawDistance_ = DrawDistance, flatten_ = 0.45f, opacity_ = 125.0f;
        int visible_ = 0;
    };
}
