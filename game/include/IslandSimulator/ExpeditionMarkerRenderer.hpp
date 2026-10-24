#pragma once

#include <cstdint>
#include <memory>
#include <span>

namespace Microsoft::Xna::Framework
{
    struct Matrix;
    struct Vector3;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class GraphicsDevice;
    class VertexBuffer;
}

namespace island
{
    /** A small non-solid survey stake. The repeated compass-and-wave plate is the physical clue
     *  connecting otherwise separate ruins; chapter studs distinguish the stations. */
    struct ExpeditionMarkerSite
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f;
        std::uint8_t chapter = 0U;
    };

    class ExpeditionMarkerRenderer final
    {
    public:
        ExpeditionMarkerRenderer(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            std::span<const ExpeditionMarkerSite> sites);
        ~ExpeditionMarkerRenderer();

        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera);
        [[nodiscard]] int MarkerCount() const noexcept { return markerCount_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
        int vertexCount_ = 0;
        int markerCount_ = 0;
    };
}
