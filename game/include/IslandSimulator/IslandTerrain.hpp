#pragma once

#include <cstdint>

namespace island
{
    /** @brief Coarse physical surface class shared by placement, collision, and presentation. */
    enum class TerrainSurface : std::uint8_t
    {
        DeepSeabed,
        ShallowSeabed,
        WetSand,
        DrySand,
        CoastalGround,
        InteriorGround,
        SteepGround,
        RockyGround
    };

    /** @brief Renderer-independent unit surface normal. */
    struct TerrainNormal
    {
        float x = 0.0f;
        float y = 1.0f;
        float z = 0.0f;
    };

    /** @brief Complete authoritative terrain query at one horizontal world position. */
    struct TerrainSample
    {
        float height = 0.0f;
        float signedShoreDistance = 0.0f;
        TerrainNormal normal;
        float slopeRadians = 0.0f;
        TerrainSurface surface = TerrainSurface::WetSand;
        float coastalRockiness = 0.0f;
        float beachSuitability = 0.0f;

        /** @brief Reports whether mean water level covers the sampled terrain. */
        [[nodiscard]] bool IsSubmerged() const noexcept { return height < 0.0f; }
    };

    /**
     * @brief Deterministic CPU representation shared by island rendering and world queries.
     *
     * The first campaign slice intentionally keeps this representation analytic. Later authored
     * masks and landmark adjustments can layer on top without allowing collision, placement, and
     * rendering to disagree about the shoreline.
     */
    class IslandTerrain final
    {
    public:
        /** @brief Nominal island diameter in metres. */
        static constexpr float NominalDiameter = 500.0f;

        /** @brief Largest supported absolute horizontal query coordinate in metres. */
        static constexpr float MaximumQueryCoordinate = 1'000'000.0f;

        /**
         * @brief Returns the radial shoreline distance for a polar angle.
         * @param angleRadians Angle around the island in radians.
         * @return Shore radius in metres.
         */
        [[nodiscard]] float ShoreRadius(float angleRadians) const noexcept;

        /**
         * @brief Returns a stable island-relative signed shoreline distance approximation.
         * @param x World x coordinate in metres.
         * @param z World z coordinate in metres.
         * @return Positive on land, zero at the shore, and negative offshore.
         */
        [[nodiscard]] float SignedShoreDistance(float x, float z) const noexcept;

        /**
         * @brief The shore's least radius: the nominal radius less the broad swell's (12.5 m),
         * the coastal noise's (9 m) and the sheltered bay's (70 m) deepest cuts, with a margin.
         */
        static constexpr float MinimumShoreRadius = NominalDiameter * 0.46f - 12.5f - 9.0f - 70.0f - 0.5f;

        /**
         * @brief Whether SignedShoreDistance(x, z) >= distance, without evaluating the coastline
         * when the point lies so deep inside MinimumShoreRadius that the answer is certain.
         */
        [[nodiscard]] bool IsInlandBy(float x, float z, float distance) const noexcept
        {
            const float within = MinimumShoreRadius - distance;
            return (within > 0.0f && x * x + z * z <= within * within)
                || SignedShoreDistance(x, z) >= distance;
        }

        /**
         * @brief Samples terrain or seabed height.
         * @param x World x coordinate in metres.
         * @param z World z coordinate in metres.
         * @return Height in metres relative to mean water level zero.
         */
        [[nodiscard]] float Height(float x, float z) const noexcept;

        /**
         * @brief Samples height, shoreline distance, normal, slope, and coarse surface class.
         *
         * This is the preferred query when a system needs more than height alone. Keeping these
         * derived values together prevents collision, placement, and rendering from using subtly
         * different finite-difference conventions. Non-finite or out-of-domain coordinates return
         * a finite, upward-facing deep-seabed sample instead of entering procedural noise math.
         */
        [[nodiscard]] TerrainSample Sample(float x, float z) const noexcept;

        /**
         * @brief Reports whether the coordinate is on dry land.
         * @param x World x coordinate in metres.
         * @param z World z coordinate in metres.
         * @return True for positive terrain height.
         */
        [[nodiscard]] bool IsLand(float x, float z) const noexcept;

        /** @brief Finite-difference spacing used by authoritative surface queries, in metres. */
        static constexpr float SurfaceSampleDistance = 0.75f;

    private:
        /** The ground before the authored earthworks (the bunker's berm) and the tide pools. */
        [[nodiscard]] float LandHeight(float x, float z) const noexcept;
        /** Earth heaped over the bunker's descending access corridor, so its concrete roof
         *  lies under some 0.9 m of soil instead of standing out of the hillside's flank. */
        [[nodiscard]] float BunkerBerm(float x, float z, float land) const noexcept;
        [[nodiscard]] static std::uint32_t Hash(std::int32_t x, std::int32_t z,
                                                std::uint32_t seed) noexcept;
        [[nodiscard]] static float ValueNoise(float x, float z, std::uint32_t seed) noexcept;
        [[nodiscard]] static float FractalNoise(float x, float z, std::uint32_t seed) noexcept;
    };
}
