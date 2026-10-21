#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics { class GraphicsDevice; class IndexBuffer; class ShaderEffect; class VertexBuffer; }

namespace island
{
    class IslandTerrain;

    /**
     * Seagrass meadows on the lagoon floor: where the terrain paints its dark meadow beds (the
     * same noise fields, mirrored here), clumps of flat ribbon blades stand up from the bed and
     * lean back and forth with the swell's surge, dark olive and paler at the tips, darker the
     * deeper they stand. The lagoon is cut into 8 m beds; a bed is grown (on a fixed grid, so it
     * comes back the same) when the eye first nears it, kept as a static mesh, and dropped again
     * once the eye has long left. The surge, and the far blades shrinking into the bed, are the
     * vertex shader's, so a frame uploads nothing. Drawn before the sea so the water covers it.
     */
    class SeagrassRenderer final
    {
    public:
        static constexpr float Spacing = 0.3f, Radius = 24.0f, BucketSize = 8.0f;
        static constexpr int MaximumBuildsPerFrame = 12;
        SeagrassRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, const IslandTerrain& terrain);
        ~SeagrassRenderer();
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, float seconds, float seaState);
        /** @brief Blades in the beds drawn last frame. */
        [[nodiscard]] int VisibleCount() const noexcept { return visible_; }
        [[nodiscard]] std::size_t ResidentBeds() const noexcept { return buckets_.size(); }

    private:
        struct Bucket
        {
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices;
            int count = 0, blades = 0;
            float low = 0.0f, high = 0.0f;   // the bed's height range, for the frustum test
            std::uint32_t seen = 0;
        };
        void Build(int bx, int bz, Bucket& bucket);
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        const IslandTerrain& terrain_;
        std::unordered_map<std::int64_t, Bucket> buckets_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> indices_;   // every bed's blades share one pattern
        std::uint32_t frame_ = 0;
        int visible_ = 0;
    };
}
