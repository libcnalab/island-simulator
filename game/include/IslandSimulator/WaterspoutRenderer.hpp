#pragma once

#include "IslandSimulator/Waterspout.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** A waterspout far offshore: the grey condensation funnel from the cloud base down to the
     *  sea, a sinuous tapering tube (drawn as a ribbon facing the eye, denser at its edges as a
     *  hollow funnel looks, with slow turning bands), reaching down as it forms, thinning to a
     *  rope at the end of its life, and a bush of spray where it meets the sea; hazed with the
     *  distance, alpha-blended before the night grade. */
    class WaterspoutRenderer final
    {
    public:
        explicit WaterspoutRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~WaterspoutRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, const WaterspoutState& spout, float seconds);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int drawCalls_ = 0;
    };
}
