#pragma once
#include <cstdint>
#include "IslandSimulator/ShootingStars.hpp"

#include "IslandSimulator/DayNightCycle.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class BlendState;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class VertexBuffer;
}

namespace island
{
    /**
     * The hour on screen: one multiply of the drawn scene by the hour's grade (warm at the
     * golden hours, dark blue at night; nothing at full day), then the stars and the moon over
     * the graded sky, depth-tested so the land and the trees stand in front of them. Emissive
     * things drawn after the grade (the campfire's flames) keep their own brightness.
     */
    class TimeOfDayRenderer final
    {
    public:
        static constexpr int StarCount = 1100;
        static constexpr float StarDistance = 850.0f, MoonDistance = 820.0f;
        static constexpr float MoonRadiusDegrees = 1.2f;

        explicit TimeOfDayRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~TimeOfDayRenderer();

        /** @brief True when DrawGrade will darken the frame (anything short of full day). */
        [[nodiscard]] static bool GradeActive(const DayLighting& light) noexcept;
        /** @brief Sets the frame's alpha to one, for a local light to write into before the grade. */
        void ResetAlpha();
        /** @param localLight the frame's alpha carries a local light (one minus its strength,
         *  written after ResetAlpha): each pixel is graded by the hour's colour plus that light,
         *  so a lamp's pool keeps the surfaces' own colours instead of the night's blue. */
        void DrawGrade(const DayLighting& light, bool localLight = false);
        /** @brief A lightning flash over everything already drawn (0..1). */
        void DrawFlash(float flash);
        /** @brief Distant rain curtains: grey shafts of rain hanging from far showers over the
         *  sea round the horizon, streaked, slanting with the wind and drifting slowly, before
         *  the grade (the night darkens them), depth-read so the island stands in front;
         *  `rain` 0..1 the showers about. */
        void DrawRainCurtains(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                              const Microsoft::Xna::Framework::Vector3& camera, const DayLighting& light, float rain,
                              double seconds);
        /** @brief The lightning bolt of a strike while its flash lasts: a jagged channel with a
         *  branch or two from the cloud base down to the horizon at the strike's bearing,
         *  sized for its distance, additive after the grade, depth-read so the island stands
         *  in front. `strike` numbers the strike (its shape and bearing follow from it). */
        void DrawBolt(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                      const Microsoft::Xna::Framework::Vector3& camera, std::uint32_t strike, float distance, float flash);
        /** @brief A rainbow: the primary bow at 42 degrees round the point opposite the sun
         *  (red outside, violet inside) and a fainter, reversed secondary at 51, only above the
         *  horizon, depth-read so the land stands in front; `strength` 0..1. */
        void DrawRainbow(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                         const Microsoft::Xna::Framework::Vector3& camera, const DayLighting& light, float strength);
        /** @brief How strong a rainbow is: rain still falling (lightly, a shower easing off),
         *  the sun out and low enough (5-40 degrees) for the bow to stand above the horizon. */
        [[nodiscard]] static float RainbowStrength(const DayLighting& light, float rain) noexcept;
        void DrawNightSky(const Microsoft::Xna::Framework::Matrix& view,
                          const Microsoft::Xna::Framework::Matrix& projection,
                          const Microsoft::Xna::Framework::Vector3& camera, const DayLighting& light,
                          double seconds = 0.0);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
        [[nodiscard]] bool GradeApplied() const noexcept { return gradeApplied_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> gradeEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> skyEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BlendState> gradeBlend_, litGradeBlend_, alphaResetBlend_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> gradeTriangle_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> stars_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> moon_, meteor_, rainbow_, bolt_, starsInSky_, curtains_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> curtainStaging_;
        // the stars as built, and those low enough for the cumulus to pass in front of
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> starBase_, starStaging_;
        std::vector<std::uint32_t> lowStars_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> boltStaging_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> rainbowStaging_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> moonStaging_;
        int starVertexCount_ = 0;
        int drawCalls_ = 0;
        bool gradeApplied_ = false;
    };
}
