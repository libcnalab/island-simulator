#pragma once

#include "IslandSimulator/QuadBike.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColorTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include <functional>
#include <memory>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics
{
    class BlendState;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class ShaderEffect;
}

namespace island
{
    /**
     * Local lights at night (the quad's headlights, the campfire), lit through the night grade
     * rather than before it: lit within the 8-bit frame before the grade, a surface can never
     * come out brighter than the grade's dark blue. After TimeOfDayRenderer::ResetAlpha(),
     * each light draws a slab over the ground it lights, depth-tested; each of its pixels
     * finds where its view ray comes down to the ground and how strongly the light reaches
     * that ground; it tints the frame's colour toward the light's (a multiply, so nothing
     * clips) and multiplies the frame's alpha by one minus that strength, so the lights add up. TimeOfDayRenderer::DrawGrade(light, true) then grades each pixel by the
     * hour's colour plus that light, and lit ground keeps its own colours.
     *
     * The slab lies along the eye's rays to the ground, a lift below the eye: what stands
     * lower than it on the lit ground is lit, what stands in front of it is not.
     */
    class LocalLightRenderer final
    {
    public:
        using GroundSampler = std::function<BikeGround(float, float)>;

        explicit LocalLightRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~LocalLightRenderer();

        /** @brief A spot (the quad's headlights): from `lamp` along `axis`, a 20 degree core and
         *  a softer spill to 40 m; the slab starts just past the lamps (`ahead` horizontal), so
         *  what stands behind them (the quad, its rider) stays unlit. */
        void DrawSpot(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                      const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& lamp,
                      const Microsoft::Xna::Framework::Vector3& axis, const GroundSampler& ground, float strength,
                      const Microsoft::Xna::Framework::Vector3& tint);
        /** @brief A point (a fire): from `light`, falling off over `radius` metres. */
        void DrawPoint(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                       const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& light,
                       float radius, const GroundSampler& ground, float strength,
                       const Microsoft::Xna::Framework::Vector3& tint);
        /** @brief A point lighting a flat surface it stands by (the hut's lantern on the boards
         *  of its wall and veranda): `triangles` lie a little in front of that surface, facing
         *  along `normal`, and each of their pixels is lit where it lies. */
        void DrawPlane(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                       const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& light,
                       float radius, std::span<const Microsoft::Xna::Framework::Vector3> triangles,
                       const Microsoft::Xna::Framework::Vector3& normal, float strength,
                       const Microsoft::Xna::Framework::Vector3& tint);
        /** @brief A point lighting a figure by it (the resident at the campfire): the figure's own
         *  triangles, just as drawn, lit through the grade on the faces turned to the light,
         *  their backs left nearly dark. */
        void DrawMesh(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                      const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& light,
                      float radius, std::span<const Microsoft::Xna::Framework::Graphics::VertexPositionColorTexture> triangles,
                      float strength, const Microsoft::Xna::Framework::Vector3& tint);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
        void ResetCounts() noexcept { drawCalls_ = 0; }

    private:
        using Vertex = Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;
        void Submit(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                    const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& light,
                    const Microsoft::Xna::Framework::Vector3& axis, float kind, float radius, float strength,
                    const Microsoft::Xna::Framework::Vector3& tint, int rings, int spokes);
        void Flush(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                   const Microsoft::Xna::Framework::Vector3& camera, const Microsoft::Xna::Framework::Vector3& light,
                   const Microsoft::Xna::Framework::Vector3& axis, float kind, float radius, float strength,
                   const Microsoft::Xna::Framework::Vector3& tint);
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BlendState> blend_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> mesh_, figure_;
        std::vector<Vertex> figureStaging_;
        std::unordered_map<std::uint64_t, Microsoft::Xna::Framework::Vector3> figureNormals_;
        std::vector<Vertex> grid_, staging_;
        int drawCalls_ = 0;
    };
}
