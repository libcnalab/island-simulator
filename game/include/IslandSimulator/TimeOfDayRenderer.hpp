#pragma once

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

        void DrawGrade(const DayLighting& light);
        /** @brief A lightning flash over everything already drawn (0..1). */
        void DrawFlash(float flash);
        void DrawNightSky(const Microsoft::Xna::Framework::Matrix& view,
                          const Microsoft::Xna::Framework::Matrix& projection,
                          const Microsoft::Xna::Framework::Vector3& camera, const DayLighting& light);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
        [[nodiscard]] bool GradeApplied() const noexcept { return gradeApplied_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> gradeEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> skyEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BlendState> gradeBlend_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> gradeTriangle_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> stars_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> moon_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> moonStaging_;
        int starVertexCount_ = 0;
        int drawCalls_ = 0;
        bool gradeApplied_ = false;
    };
}
