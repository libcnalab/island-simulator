#pragma once
#include "IslandSimulator/EyeshineRenderer.hpp"
#include <span>

#include "IslandSimulator/DeerHerd.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include <array>
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class SamplerState;
    class Texture2D;
    struct VertexPositionColorTexture;
}

namespace island
{
    class CanopyLightField;

    /** Portable procedural renderer for the small inland deer herd: the coat in one draw
     *  over the goats' short-hair strand texture, the bare parts in a second. */
    class DeerRenderer final
    {
    public:
        DeerRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                     const CanopyLightField& canopy, float maximumDistance,
                     Microsoft::Xna::Framework::Graphics::Texture2D& coat,
                     const std::array<float, 3>& coatMean);
        ~DeerRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera,
                  std::span<const DeerState> deer);
        [[nodiscard]] int VisibleCount() const noexcept { return visibleCount_; }
        /** The near deer's eyes as drawn this frame (for the eyeshine). */
        [[nodiscard]] std::span<const EyeGlint> Eyes() const noexcept { return eyes_; }
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCallCount_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const CanopyLightField& canopy_;
        float maximumDistance_ = 0.0f;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
        Microsoft::Xna::Framework::Graphics::Texture2D& coat_;
        std::array<float, 3> coatMean_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SamplerState> mirrored_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColorTexture> staging_, solid_;
        int visibleCount_ = 0;
        int drawCallCount_ = 0;
        std::vector<EyeGlint> eyes_;
    };
}
