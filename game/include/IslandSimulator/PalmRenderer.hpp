#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "IslandSimulator/TerrainMeshLayout.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class IndexBuffer;
    class ShaderEffect;
    class VertexBuffer;
    class VertexDeclaration;
}

namespace island
{
    /** @brief Procedural instanced palm rendering with portable non-instanced fallback. */
    class SunShadowMap;
    class RenderedTerrainSurface;

    class PalmRenderer final
    {
    public:
        PalmRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                     const VegetationLayout& layout,
                     float maximumDistanceScale = 1.0f,
                     const RenderedTerrainSurface* ground = nullptr);
        ~PalmRenderer();

        PalmRenderer(const PalmRenderer&) = delete;
        PalmRenderer& operator=(const PalmRenderer&) = delete;
        PalmRenderer(PalmRenderer&&) = delete;
        PalmRenderer& operator=(PalmRenderer&&) = delete;

        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& cameraPosition,
                  float timeSeconds);

        [[nodiscard]] bool UsesInstancing() const noexcept { return instancingSupported_; }
        [[nodiscard]] bool UsesCustomShader() const noexcept { return customShaderReady_; }
        [[nodiscard]] int TotalPalmCount() const noexcept { return totalPalmCount_; }
        [[nodiscard]] int VisiblePalmCount() const noexcept { return visiblePalmCount_; }
        [[nodiscard]] int VisibleNearPalmCount() const noexcept { return visibleNearPalmCount_; }
        [[nodiscard]] int VisibleFarPalmCount() const noexcept { return visibleFarPalmCount_; }
        [[nodiscard]] int VisibleCellCount() const noexcept { return visibleCellCount_; }
        /** Receives the fixed-sun shadow map; nullptr leaves palms unshadowed. */
        void SetSunShadowMap(const SunShadowMap* map) noexcept { sunShadow_ = map; }
        /** Renders far-LOD living and broken palms into the bound map. */
        void DrawSunShadow(SunShadowMap& map);
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCallCount_; }
        [[nodiscard]] int FallenPalmCount() const noexcept { return fallenPalmCount_; }

        /** A point on a living palm's frond midrib: the palm's instance transform, the point in
         *  the palm's own frame, its wind weight and phase (as the frond's vertices carry them),
         *  the frond's outward bearing in the world, and the frond's index in the crown. */
        struct FrondAnchor
        {
            Microsoft::Xna::Framework::Matrix transform;
            Microsoft::Xna::Framework::Vector3 local;
            float windWeight = 0.0f, windPhase = 0.0f, bearing = 0.0f;
            int frond = 0;
        };
        /** The midrib point `along` (0 the crown .. 1 the tip) of each mature frond of a living
         *  palm (the near and middle meshes' crown); none for a broken or fallen palm. */
        [[nodiscard]] static std::vector<FrondAnchor> MatureFrondAnchors(const PalmPlacement& palm, float along);
        /** The anchor in the world at `timeSeconds`, swayed as the palm shader sways its frond. */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3 FrondAnchorWorld(const FrondAnchor& anchor,
                                                                                 float timeSeconds);
        [[nodiscard]] int BrokenPalmCount() const noexcept { return brokenPalmCount_; }

    private:
        struct PalmMesh
        {
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices;
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer> indices;
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> instances;
            int vertexCount = 0;
            int primitiveCount = 0;
            int instanceCapacity = 0;
        };

        struct PalmDrawInstance
        {
            Microsoft::Xna::Framework::Matrix transform;
            float x = 0.0f;
            float z = 0.0f;
            float nearLodDistanceSquared = 0.0f;
            float midLodDistanceSquared = 0.0f;
            static constexpr std::uint16_t NoFallen = 0xFFFFU;
            std::uint16_t fallen = NoFallen;   // its own collapsed-crown meshes in fallen_
            std::uint8_t variant = 0U;
            PalmCondition condition = PalmCondition::Living;
        };

        struct PalmCell
        {
            Microsoft::Xna::Framework::BoundingBox bounds;
            std::vector<PalmDrawInstance> instances;
            bool occupied = false;
        };

        [[nodiscard]] static const Microsoft::Xna::Framework::Graphics::VertexDeclaration&
            InstanceDeclaration();
        void CreateMeshes(const RenderedTerrainSurface* ground);
        void CreateCells(const VegetationLayout& layout, const RenderedTerrainSurface* ground);
        void DrawVariant(PalmMesh& mesh,
                         const std::vector<Microsoft::Xna::Framework::Matrix>& transforms);

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> customEffect_;
        std::array<PalmMesh, VegetationLayout::PalmVariantCount> nearMeshes_;
        std::array<PalmMesh, VegetationLayout::PalmVariantCount> midMeshes_;
        std::array<PalmMesh, VegetationLayout::PalmVariantCount> farMeshes_;
        std::array<PalmMesh, VegetationLayout::PalmVariantCount> brokenNearMeshes_;
        std::array<PalmMesh, VegetationLayout::PalmVariantCount> brokenFarMeshes_;
        std::array<PalmCell, TerrainMeshLayout::CellCount> cells_;
        /** A fallen palm's own meshes, its crown collapsed onto the ground under it. */
        struct FallenPalm
        {
            PalmMesh nearMesh;
            PalmMesh farMesh;
            std::uint8_t variant = 0U;
            std::vector<Microsoft::Xna::Framework::Matrix> transform;   // the one instance
            bool drawNear = false;
            bool drawFar = false;
        };
        std::vector<FallenPalm> fallen_;
        std::array<std::vector<Microsoft::Xna::Framework::Matrix>,
                   VegetationLayout::PalmVariantCount> visibleNearTransforms_;
        std::array<std::vector<Microsoft::Xna::Framework::Matrix>,
                   VegetationLayout::PalmVariantCount> visibleMidTransforms_;
        std::array<std::vector<Microsoft::Xna::Framework::Matrix>,
                   VegetationLayout::PalmVariantCount> visibleFarTransforms_;
        std::array<std::vector<Microsoft::Xna::Framework::Matrix>,
                   VegetationLayout::PalmVariantCount> visibleBrokenNearTransforms_;
        std::array<std::vector<Microsoft::Xna::Framework::Matrix>,
                   VegetationLayout::PalmVariantCount> visibleBrokenFarTransforms_;
        std::string fallbackReason_;
        bool instancingSupported_ = false;
        bool customShaderReady_ = false;
        float maximumDistance_ = 0.0f;
        int totalPalmCount_ = 0;
        int fallenPalmCount_ = 0;
        int brokenPalmCount_ = 0;
        int visiblePalmCount_ = 0;
        int visibleNearPalmCount_ = 0;
        int visibleFarPalmCount_ = 0;
        int visibleCellCount_ = 0;
        int drawCallCount_ = 0;
        const SunShadowMap* sunShadow_ = nullptr;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> sunShadowEffect_;
        bool sunShadowUnavailable_ = false;
    };
}
