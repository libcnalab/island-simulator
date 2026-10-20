#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    /** @brief Renderer-independent pose for one shallow-water fish. */
    struct FishState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float pitchRadians = 0.0f;
        float bankRadians = 0.0f;
        float speed = 0.0f;
        float bodyWave = 0.0f;
        /** @brief 0..1 the flank's silver flash: a fish rolling through a turn catches the
         *  light, brightest in a hard turn (parting from the shark), gone within a third of
         *  a second after it straightens. */
        float flash = 0.0f;
        /** @brief 0..1 the pectoral fan: a fish of a school holding in the shade fans its
         *  pectoral fins to hover, finPhase carrying the fan; folded back on the move. */
        float finFan = 0.0f, finPhase = 0.0f;
        float scale = 1.0f;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t schoolIndex = 0U;
        std::uint8_t colorVariant = 0U;
    };

    /** @brief Authoritative centre and motion tier for one shallow-water school. */
    struct FishSchoolState
    {
        float centreX = 0.0f;
        float centreY = 0.0f;
        float centreZ = 0.0f;
        int direction = 1;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
    };

    /** @brief Deterministic near-shore fish schools with bounded water constraints. */
    class FishSchools final
    {
    public:
        static constexpr std::size_t SchoolCount = 8U;
        static constexpr std::size_t FishPerSchool = 6U;
        static constexpr std::size_t FishCount = SchoolCount * FishPerSchool;
        static constexpr float FullSimulationDistance = 80.0f;
        static constexpr float ReducedSimulationDistance = 225.0f;
        // Fish closer than this to the swimmer are pushed out of their school slot and away.
        static constexpr float PartingDistance = 3.0f;

        explicit FishSchools(const IslandTerrain& terrain);

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const FishState> Fish() const noexcept;
        [[nodiscard]] std::span<const FishSchoolState> Schools() const noexcept;
        /** @brief The last fish to break the surface over a fully simulated school: the
         *  ring's point and age, counted so a breach is noted once. */
        struct FishBreach { float x = 0.0f, z = 0.0f, ageSeconds = 0.0f; std::uint32_t count = 0; bool valid = false; };
        [[nodiscard]] const FishBreach& LastBreach() const noexcept { return breach_; }
        static constexpr float BreachSeconds = 2.2f;
        static constexpr float BreachIntervalSeconds = 14.0f;
        /** @brief A shaded hold (the water under the jetty's deck): a school passing within
         *  ShelterRange turns in, holds there ShelterSeconds, and goes on; not again for
         *  ShelterIntervalSeconds. */
        void SetShelter(float x, float z, bool valid) noexcept;
        /** @brief A large body in the water this frame (the shark, when present): a school
         *  parts around it at ThreatPartingDistance as around the swimmer at the parting
         *  distance, the fish hurrying out of its way. */
        void SetThreat(float x, float y, float z, bool valid) noexcept;
        [[nodiscard]] std::size_t ThreatPartingCount() const noexcept { return threatParting_; }
        static constexpr float ThreatPartingDistance = 7.0f;
        [[nodiscard]] std::size_t ShelteringCount() const noexcept;
        [[nodiscard]] std::uint32_t ShelterCount() const noexcept { return shelters_; }
        static constexpr float ShelterRange = 12.0f;
        static constexpr float ShelterSeconds = 30.0f;
        static constexpr float ShelterIntervalSeconds = 90.0f;
        static constexpr float ShelterApproachSpeed = 1.5f;
        /** @brief Water this deep is where a shelter point is placed (the schools' seabed minimum and a little). */
        static constexpr float ShelterDepth = 0.95f;
        /** @brief Fish fanning their pectoral fins this moment (the fan past half). */
        [[nodiscard]] std::size_t FanningCount() const noexcept;
        static constexpr float FinFanRate = 4.0f;
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
            FishState state;
            float alongOffset = 0.0f;
            float radialOffset = 0.0f;
            float wanderPhase = 0.0f;
            float wanderFrequency = 0.0f;
            float depthPhase = 0.0f;
            float bodyPhase = 0.0f;
            float bodySpeed = 0.0f;
        };

        struct School
        {
            float routeAngle = 0.0f;
            float offshoreDistance = 0.0f;
            float routeSpeed = 0.0f;
            float centreX = 0.0f;
            float centreY = 0.0f;
            float centreZ = 0.0f;
            float accumulatedSeconds = 0.0f;
            float nextBreachSeconds = 6.0f;
            float holdSeconds = 0.0f;         // remaining hold in the shelter
            float nextShelterSeconds = 0.0f;  // cooldown before the next hold
            bool sheltering = false;
            int direction = 1;
            WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        };

        void InitializeSchool(std::size_t schoolIndex);
        void InitializeAgent(std::size_t fishIndex);
        void UpdateSchool(std::size_t schoolIndex, float deltaSeconds) noexcept;
        void ResolveSchoolCentre(School& school) const noexcept;
        void SyncSchoolStates() noexcept;

        const IslandTerrain& terrain_;
        std::vector<School> schools_;
        FishBreach breach_;
        float shelterX_ = 0.0f, shelterZ_ = 0.0f;
        bool shelterValid_ = false;
        std::uint32_t shelters_ = 0U;
        std::vector<Agent> agents_;
        std::vector<FishState> states_;
        std::vector<FishSchoolState> schoolStates_;
        float elapsedSeconds_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        float observerX_ = 0.0f, observerY_ = 0.0f, observerZ_ = 0.0f;
        bool observerValid_ = false;
        float threatX_ = 0.0f, threatY_ = 0.0f, threatZ_ = 0.0f;
        bool threatValid_ = false;
        std::size_t threatParting_ = 0U;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
    };
}
