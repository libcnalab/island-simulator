#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** An animal's eye, where its renderer drew it this frame, and the colour of the shine the
     *  reflecting layer behind its retina gives back. */
    struct EyeGlint
    {
        Microsoft::Xna::Framework::Vector3 at;
        Microsoft::Xna::Framework::Vector3 tint;
    };

    /**
     * Eyeshine: at night the eyes of the deer, goats and boars caught in the quad's headlight
     * beam shine back. The reflecting layer returns the light towards its source, so the shine
     * is brightest when the eye looks back along the beam to a viewer beside the lamp (the
     * rider) and fades as the viewer moves off that line; an eye turned away is hidden by its
     * own head (depth-tested). Small glowing points drawn additively after the night grade.
     */
    class EyeshineRenderer final
    {
    public:
        explicit EyeshineRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~EyeshineRenderer();
        /** @param lamp,axis the beam's origin and direction; @param cosCone its half-angle's
         *  cosine; @param reach its range; @param strength 0..1 how dark it is for it to show. */
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, std::span<const EyeGlint> eyes,
                  const Microsoft::Xna::Framework::Vector3& lamp, const Microsoft::Xna::Framework::Vector3& axis,
                  float cosCone, float reach, float strength);
        [[nodiscard]] int ShiningCount() const noexcept { return shining_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int shining_ = 0;
    };
}
