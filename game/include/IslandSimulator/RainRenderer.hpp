#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class BlendState; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** A lamp the rain falls through at night: a point (cosCone -1) or a spot along `axis`
     *  within the cone whose half-angle's cosine is `cosCone`; drops within `reach` glitter. */
    struct RainLight
    {
        Microsoft::Xna::Framework::Vector3 position, axis;
        float cosCone = -1.0f, reach = 6.0f, strength = 1.0f;
        Microsoft::Xna::Framework::Vector3 tint{1.0f, 1.0f, 1.0f};
    };

    /**
     * Falling rain, anchored in the world: drops sit on a fixed world grid of cells around the
     * camera and fall through fixed world heights, wrapping only at the edges of the volume
     * where they fade out, so walking or flying moves through the rain instead of dragging a
     * screen-locked sheet along. Each drop is a thin streak slanted with the wind, drawn before
     * the night grade so the night darkens it with everything else.
     */
    class RainRenderer final
    {
    public:
        static constexpr float CellSize = 1.2f, Radius = 26.0f, Height = 22.0f;
        static constexpr int DropsPerCell = 3;
        static constexpr float FallSpeed = 8.5f;          // a raindrop's terminal speed, m/s
        static constexpr float StreakLength = 0.6f;       // its motion blur over a frame or two
        static constexpr int MaximumDrops =
            static_cast<int>((2.0f * Radius / CellSize + 2.0f) * (2.0f * Radius / CellSize + 2.0f)) * DropsPerCell;

        explicit RainRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~RainRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float rain, float wind, float seconds);
        [[nodiscard]] int VisibleDrops() const noexcept { return visible_; }
        /** @brief The same drops where the night's lamps light them (the quad's headlights, the
         *  lantern, the fire), after the night grade (additive): they glitter in the beams. */
        void DrawLit(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                     const Microsoft::Xna::Framework::Vector3& camera, float rain, float wind, float seconds,
                     std::span<const RainLight> lights);
        [[nodiscard]] int LitDrops() const noexcept { return litVisible_; }
        /** @brief A gabled roof the rain does not fall through: centred at (@p x, @p z), turned
         *  by @p yawRadians, its ridge along the local z axis, @p halfSpan out to each eave and
         *  @p halfLength along the ridge; no drop is drawn below its slopes (from @p ridgeY at
         *  the ridge down to @p eaveY at the eaves). */
        void SetShelter(float x, float z, float yawRadians, float halfSpan, float halfLength, float eaveY,
                        float ridgeY) noexcept;

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0, litVisible_ = 0;
        template <class Emit>
        void ForEachDrop(const Microsoft::Xna::Framework::Vector3& camera, float rain, float wind, float seconds,
                         Emit&& emit) const;
        void Flush(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                   const Microsoft::Xna::Framework::Graphics::BlendState& blend);
        struct Shelter { float x, z, cosYaw, sinYaw, halfSpan, halfLength, eaveY, ridgeY; };
        bool sheltered_ = false;
        Shelter shelter_{};
    };
}
