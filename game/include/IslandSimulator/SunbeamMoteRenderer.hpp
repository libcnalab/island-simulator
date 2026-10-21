#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    class CanopyLightField;
    class IslandTerrain;

    /**
     * Dust and pollen drifting in the forest's sunbeams: specks on a fixed world grid round the
     * walker, each wandering slowly in the air, that glint only where they hang in a gap of
     * sunlight with shade round about (the canopy light field: a bright spot under a darker
     * neighbourhood), so they show against the shaded forest and not in the open; additive,
     * brightest looking into the light (dust scatters forwards), faded with the distance and
     * with the direct sun.
     */
    class SunbeamMoteRenderer final
    {
    public:
        static constexpr float CellSize = 0.8f, Radius = 8.0f;
        SunbeamMoteRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain,
                            const CanopyLightField& light);
        ~SunbeamMoteRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float seconds, float directSun,
                  const Microsoft::Xna::Framework::Vector3& toSun);
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const IslandTerrain& terrain_;
        const CanopyLightField& light_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
