#pragma once

#include "IslandSimulator/SailboatPassage.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    /** The passing yacht: an 11 m white sloop, its hull, deck and cabin, the mast, the boom swung
     *  out to leeward with the mainsail and the jib drawing, heeled by the wind and riding the
     *  swell; shaded by the hour's light and hazed with the distance, drawn before the sea so the
     *  water covers the hull below it. At night its navigation lights burn after the grade: the
     *  white masthead and stern lights, red to port and green to starboard. */
    class SailboatRenderer final
    {
    public:
        explicit SailboatRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~SailboatRenderer();
        /** @param seaHeight the swell's height under the boat; @param wind the weather's (1 the trade wind). */
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, const SailboatState& boat, float seaHeight,
                  float wind, float seconds);
        /** @brief The navigation lights, after the night grade (additive); `nightness` 0 by day .. 1. */
        void DrawLights(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                        const Microsoft::Xna::Framework::Vector3& camera, float nightness);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_, glow_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_, glowStaging_;
        // the lights' places this frame (masthead, port, starboard, stern), for DrawLights
        Microsoft::Xna::Framework::Vector3 masthead_, port_, starboard_, stern_;
        bool drawn_ = false;
        int drawCalls_ = 0;
    };
}
