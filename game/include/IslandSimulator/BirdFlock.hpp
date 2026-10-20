#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    struct FallingFruit;
    class IslandTerrain;

    /** @brief Broad route family used to compose varied island bird traffic. */
    enum class BirdPathFamily : std::uint8_t
    {
        Shore,
        Canopy,
        High
    };

    enum class BirdBehavior : std::uint8_t
    {
        Fly,
        Approach,
        Perched,
        Depart,
        GroundApproach,
        GroundForage,
        GroundDepart
    };

    enum class BirdSpecies : std::uint8_t
    {
        Coastal,
        Parrot,
        FruitDove,
        Seabird
    };

    /** @brief Renderer-independent pose for one procedural bird. */
    struct BirdState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float speed = 0.0f;
        float wingPose = 0.0f;
        float bankRadians = 0.0f;
        float glideAmount = 0.0f;
        float perchAmount = 0.0f;
        /** @brief Pigeon-walk head thrust, 0..1, nonzero only while foraging on the ground. */
        float headBob = 0.0f;
        float scale = 1.0f;
        /** @brief 0..1 while a seabird has left its high orbit to circle low over a fish school. */
        float huntAmount = 0.0f;
        BirdPathFamily family = BirdPathFamily::Shore;
        BirdSpecies species = BirdSpecies::Coastal;
        BirdBehavior behavior = BirdBehavior::Fly;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
        std::uint8_t perchSiteIndex = 0U;
    };

    /** @brief Immutable authored orbit and spawn data for one procedural bird. */
    struct BirdRouteState
    {
        float spawnX = 0.0f;
        float spawnY = 0.0f;
        float spawnZ = 0.0f;
        float spawnHeadingRadians = 0.0f;
        float preferredRadius = 0.0f;
        float baseAltitude = 0.0f;
        float minimumClearance = 0.0f;
        float verticalAmplitude = 0.0f;
        int direction = 1;
        BirdPathFamily family = BirdPathFamily::Shore;
    };

    /** @brief Nominal radial and altitude envelope shared by one bird route family. */
    struct BirdRouteRegion
    {
        float minimumRadius = 0.0f;
        float maximumRadius = 0.0f;
        float minimumAltitude = 0.0f;
        float maximumAltitude = 0.0f;
        std::size_t population = 0U;
        BirdPathFamily family = BirdPathFamily::Shore;
    };

    /** @brief The most recent plunge-dive strike point and its age, for the water's foam ring. */
    struct BirdDiveSplash
    {
        float x = 0.0f;
        float z = 0.0f;
        float ageSeconds = 1.0e9f;
        bool valid = false;
    };

    /** @brief Deterministic, bounded and distance-throttled flying-bird simulation. */
    class BirdFlock final
    {
    public:
        static constexpr std::size_t BirdCount = 24U;
        static constexpr float FullSimulationDistance = 190.0f;
        static constexpr float ReducedSimulationDistance = 560.0f;

        explicit BirdFlock(const IslandTerrain& terrain);

        /** @brief Advances autonomous flight and updates observer-relative simulation tiers. */
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        /** @brief Installs static anchors after world layouts have been constructed. */
        void SetPerchSites(std::span<const BirdPerchSite> sites);
        /** @brief The fish school the seabirds may work this moment; invalid clears it. */
        void SetSeaForage(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::size_t HuntingCount() const noexcept;
        [[nodiscard]] const BirdDiveSplash& DiveSplash() const noexcept { return diveSplash_; }
        [[nodiscard]] std::uint32_t DiveCount() const noexcept { return diveCount_; }
        /** @brief A sudden sound at a point; perched birds near it flush within the second. */
        void NoteDisturbance(float x, float z) noexcept;
        /** @brief Fruit lying on the ground this frame; a fruit dove's ground visit lands
         *  beside the nearest one within FruitLandingRange when there is any. The doves'
         *  routes ring the island, so the range is the island: a dove sees the fruit from
         *  the air and crosses to it. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        [[nodiscard]] std::uint32_t FruitLandingCount() const noexcept { return fruitLandings_; }
        static constexpr float FruitLandingRange = 240.0f;
        [[nodiscard]] std::uint32_t FlushCount() const noexcept { return flushCount_; }
        // A perched bird leaves when the observer comes this close, or a thud lands this near.
        static constexpr float FlushDistance = 9.0f;
        static constexpr float DisturbanceDistance = 30.0f;
        static constexpr float DisturbanceSeconds = 1.0f;
        static constexpr float DiveIntervalSeconds = 9.0f;
        static constexpr float DiveSeconds = 1.6f;
        static constexpr float SplashSeconds = 2.6f;
        static constexpr float HuntOrbitRadius = 11.0f;
        static constexpr float HuntAltitude = 7.5f;
        static constexpr float HuntSeconds = 42.0f;
        static constexpr float HuntIntervalSeconds = 75.0f;

        [[nodiscard]] std::span<const BirdState> Birds() const noexcept;
        [[nodiscard]] std::span<const BirdRouteState> Routes() const noexcept;
        [[nodiscard]] std::span<const BirdRouteRegion> RouteRegions() const noexcept;
        [[nodiscard]] std::span<const BirdPerchSite> PerchSites() const noexcept
        {
            return perchSites_;
        }
        [[nodiscard]] int ApproachingCount() const noexcept { return approachingCount_; }
        [[nodiscard]] int PerchedCount() const noexcept { return perchedCount_; }
        [[nodiscard]] int DepartingCount() const noexcept { return departingCount_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullSimulationCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept {
            return reducedSimulationCount_;
        }
        [[nodiscard]] int DormantSimulationCount() const noexcept {
            return dormantSimulationCount_;
        }

    private:
        struct Agent
        {
            BirdState state;
            BirdRouteState route;
            float verticalPhase = 0.0f;
            float verticalFrequency = 0.0f;
            float wanderPhaseA = 0.0f;
            float wanderPhaseB = 0.0f;
            float wanderFrequencyA = 0.0f;
            float wanderFrequencyB = 0.0f;
            float radialDriftPhase = 0.0f;
            float radialDriftFrequency = 0.0f;
            float radialDriftAmplitude = 0.0f;
            float glidePhase = 0.0f;
            float glideFrequency = 0.0f;
            float flapPhase = 0.0f;
            float flapSpeed = 0.0f;
            float accumulatedSeconds = 0.0f;
            float behaviorSeconds = 0.0f;
            float nextPerchSeconds = 0.0f;
            std::uint32_t perchCycle = 0U;
            float groundTargetX = 0.0f;
            float groundTargetZ = 0.0f;
            float huntSeconds = 0.0f;       // remaining low circling; 0 when on the high route
            float nextHuntSeconds = 0.0f;
            float nextDiveSeconds = 0.0f;   // countdown to the next plunge while circling low
            float diveSeconds = 0.0f;       // remaining plunge; 0 when not diving
            bool struck = false;            // this plunge has already touched the water
            std::size_t index = 0U;
        };

        void InitializeAgent(std::size_t index);
        void BuildRouteRegions() noexcept;
        void UpdateAgent(Agent& agent, float deltaSeconds, bool allowNewPerch) noexcept;
        void UpdateFlight(Agent& agent, float deltaSeconds) noexcept;
        void UpdatePerching(Agent& agent, float deltaSeconds) noexcept;
        void UpdateGroundDove(Agent& agent, float deltaSeconds) noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<BirdState> states_;
        std::vector<BirdRouteState> routes_;
        std::vector<BirdPerchSite> perchSites_;
        std::array<BirdRouteRegion, 3> routeRegions_{};
        float elapsedSeconds_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        float forageX_ = 0.0f, forageZ_ = 0.0f;
        bool forageValid_ = false;
        BirdDiveSplash diveSplash_;
        std::uint32_t diveCount_ = 0U;
        float disturbanceX_ = 0.0f, disturbanceZ_ = 0.0f, disturbanceSeconds_ = 0.0f;
        std::uint32_t flushCount_ = 0U;
        std::span<const FallingFruit> lyingFruit_;
        std::uint32_t fruitLandings_ = 0U;
        [[nodiscard]] bool FindFruitLanding(const Agent& agent, float& x, float& z) const noexcept;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
        int approachingCount_ = 0;
        int perchedCount_ = 0;
        int departingCount_ = 0;
    };
}
