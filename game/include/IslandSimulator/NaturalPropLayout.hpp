#pragma once

#include "IslandSimulator/WorldCollision.hpp"

#include <cstdint>
#include <vector>

namespace island
{
    class IslandTerrain;
    class JungleLayout;
    class VegetationLayout;

    /** @brief One irregular land or emergent-reef rock with independent axis scale. */
    struct RockPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        float scaleZ = 1.0f;
        std::uint8_t variant = 0U;
        bool solid = true;
        bool offshore = false;
        bool clearingLandmark = false;
        bool tidePoolLandmark = false;
        bool streamBankLandmark = false;
        bool cliffTalus = false;
    };

    // Wood placement uses a local-X heading (cos, sin), unlike a model's Y-axis rotation.
    // CNA CreateRotationY maps local +X to (cos, -sin); convert once at rendering boundaries.
    [[nodiscard]] constexpr float WoodModelYaw(const float headingRadians) noexcept
    {
        return -headingRadians;
    }

    /** @brief One fallen natural log, normally procedural or explicitly used as a landmark. */
    struct FallenLogPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f; // Local-X heading shared by support queries and collision proxies.
        float length = 1.0f;
        float radius = 0.25f;
        std::uint8_t variant = 0U;
        bool clearingLandmark = false;
        bool wetBank = false;
    };

    /** @brief One small non-solid piece of sun-bleached driftwood on the beach. */
    struct DriftwoodPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f; // Local-X heading shared by support/placement validation.
        float length = 1.0f;
        float radius = 0.1f;
        std::uint8_t variant = 0U;
        bool coveLandmark = false;
    };

    /** @brief One non-solid shell, pebble, or dry wrack cluster scattered across sand. */
    struct BeachDetailPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scale = 1.0f;
        std::uint8_t variant = 0U;
        bool tidePoolLandmark = false;
        bool turtleTrace = false;
        bool turtleNest = false;
        // The ground's normal under a flat trace, which is laid along it (not level) so a
        // slope neither buries one side nor lifts the other.
        float normalX = 0.0f, normalY = 1.0f, normalZ = 0.0f;
    };

    /** @brief One short line of deer or boar prints pressed into stream-bank mud or wet sand. */
    struct AnimalTrackPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scale = 1.0f;
        std::uint8_t variant = 0U; // 0 deer cloven hooves, 1 boar trotters, 2 bare human feet
        bool streamBank = false;
    };

    /** @brief One non-solid fallen palm frond resting near its source grove. */
    struct FallenFrondPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scale = 1.0f;
        std::uint8_t variant = 0U;
    };

    /** @brief One fallen coconut resting on the ground near a bearing palm. */
    struct FallenCoconutPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scale = 1.0f;
        std::uint8_t variant = 0U; // 0 fresh green-yellow husk, 1 dry brown husk
    };

    /** @brief One non-solid coral head or low underwater reef rock rooted in the seabed. */
    struct ReefHabitatPlacement
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float yawRadians = 0.0f;
        float scale = 1.0f;
        std::uint8_t variant = 0U;
    };

    /** @brief Deterministic beach, shallow-reef, interior-rock, and fallen-wood composition. */
    class NaturalPropLayout final
    {
    public:
        static constexpr std::uint32_t DefaultSeed = 0xa7816c39U;
        static constexpr int RockVariantCount = 4;
        static constexpr int LogVariantCount = 3;
        static constexpr int LogCollisionSamples = 7;
        static constexpr int DriftwoodVariantCount = 3;
        static constexpr int BeachDetailVariantCount = 6;
        static constexpr int AnimalTrackVariantCount = 3;   // 2: the resident's bare feet
        static constexpr int FallenFrondVariantCount = 3;
        static constexpr int FallenCoconutVariantCount = 2;
        static constexpr int ReefHabitatVariantCount = 4;
        static constexpr int ReefHabitatSiteCount = 640;
        static constexpr int AuthoredClearingRockCount = 5;
        static constexpr int StreamBankRockCandidateCount = 18;
        static constexpr int AuthoredCliffTalusCount = 6;
        static constexpr int AuthoredClearingLogCount = 2;
        static constexpr int StreamBankLogCandidateCount = 8;
        static constexpr int AuthoredCoveDriftwoodCount = 3;
        static constexpr int WindSortedBayDriftwoodCount = 10;
        static constexpr int WindSortedBayDetailCount = 24;
        static constexpr int TurtleNestingTrackCount = 2;
        static constexpr int TurtleTrackSegmentsPerNest = 7;
        // The north arm of the broad western lee-side beach, clear of the
        // rocky south-west headland and stream mouth.
        static constexpr float AuthoredCoveAngle = 3.02f;
        static constexpr float RockMinimumScale = 0.48f;
        static constexpr float RockMaximumScale = 1.85f;
        static constexpr int ReefRockSiteCount = 56;
        static constexpr float LogMinimumLength = 3.8f;
        static constexpr float LogMaximumLength = 7.2f;
        static constexpr float DriftwoodMinimumLength = 1.8f;
        static constexpr float DriftwoodMaximumLength = 4.6f;
        static constexpr float BeachDetailMinimumScale = 0.72f;
        static constexpr float BeachDetailMaximumScale = 1.30f;
        static constexpr float FallenFrondMinimumScale = 2.45f;
        static constexpr float FallenFrondMaximumScale = 3.75f;
        static constexpr float ReefHabitatMinimumScale = 0.42f;
        static constexpr float ReefHabitatMaximumScale = 1.38f;

        NaturalPropLayout(const IslandTerrain& terrain,
                          const VegetationLayout& palms,
                          const JungleLayout& jungle,
                          std::uint32_t seed = DefaultSeed);

        [[nodiscard]] const std::vector<RockPlacement>& Rocks() const noexcept { return rocks_; }
        [[nodiscard]] std::size_t StreamBankRockCount() const noexcept
        {
            return streamBankRockCount_;
        }
        [[nodiscard]] const std::vector<FallenLogPlacement>& FallenLogs() const noexcept
        {
            return fallenLogs_;
        }
        [[nodiscard]] std::size_t StreamBankLogCount() const noexcept { return streamBankLogCount_; }
        [[nodiscard]] const std::vector<DriftwoodPlacement>& Driftwood() const noexcept
        {
            return driftwood_;
        }
        [[nodiscard]] const std::vector<BeachDetailPlacement>& BeachDetails() const noexcept
        {
            return beachDetails_;
        }
        [[nodiscard]] const std::vector<AnimalTrackPlacement>& AnimalTracks() const noexcept
        {
            return animalTracks_;
        }
        [[nodiscard]] const std::vector<FallenFrondPlacement>& FallenFronds() const noexcept
        {
            return fallenFronds_;
        }
        [[nodiscard]] const std::vector<FallenCoconutPlacement>& FallenCoconuts() const noexcept
        {
            return fallenCoconuts_;
        }
        [[nodiscard]] const std::vector<ReefHabitatPlacement>& ReefHabitat() const noexcept
        {
            return reefHabitat_;
        }
        [[nodiscard]] const std::vector<CircularObstacle>& Obstacles() const noexcept
        {
            return obstacles_;
        }

    private:
        [[nodiscard]] static std::uint32_t Hash(std::int32_t x, std::int32_t z,
                                                std::uint32_t seed) noexcept;
        [[nodiscard]] static float UnitRandom(std::uint32_t value) noexcept;
        [[nodiscard]] static float ClusterNoise(float x, float z,
                                                std::uint32_t seed) noexcept;

        std::vector<RockPlacement> rocks_;
        std::vector<FallenLogPlacement> fallenLogs_;
        std::vector<DriftwoodPlacement> driftwood_;
        std::vector<BeachDetailPlacement> beachDetails_;
        std::vector<AnimalTrackPlacement> animalTracks_;
        std::vector<FallenFrondPlacement> fallenFronds_;
        std::vector<FallenCoconutPlacement> fallenCoconuts_;
        std::vector<ReefHabitatPlacement> reefHabitat_;
        std::vector<CircularObstacle> obstacles_;
        std::size_t streamBankRockCount_ = 0;
        std::size_t streamBankLogCount_ = 0;
    };
}
