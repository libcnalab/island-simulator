#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class BasicEffect; class DynamicVertexBuffer; class GraphicsDevice; }

namespace island
{
    class IslandTerrain;

    /**
     * Seagrass meadows on the lagoon floor: where the terrain paints its dark meadow beds (the
     * same noise fields, mirrored here), clumps of ribbon blades stand up
     * from the bed and lean back and forth with the swell's surge, dark olive and paler at the
     * tips, darker the deeper they stand. Placed once on a fixed grid (thinned by how well a
     * meadow suits the spot) and bucketed, so each frame only the buckets near the eye are
     * built; drawn before the sea so the water covers them.
     */
    class SeagrassRenderer final
    {
    public:
        static constexpr float Spacing = 0.6f, Radius = 24.0f, BucketSize = 8.0f;
        SeagrassRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain);
        ~SeagrassRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float seconds, float seaState);
        [[nodiscard]] std::size_t ClumpCount() const noexcept { return clumps_.size(); }
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }

    private:
        struct Clump { float x, y, z, size, phase; std::uint8_t blades; };
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::vector<Clump> clumps_;
        std::unordered_map<std::int64_t, std::vector<std::uint32_t>> buckets_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
        int visible_ = 0;
    };
}
