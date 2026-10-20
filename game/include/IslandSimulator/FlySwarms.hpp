#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    struct FallingFruit;
    enum class FlyBehavior : std::uint8_t { Hover, Orbit, Dart };

    struct FlyState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, wingPhase = 0.0f, scale = 1.0f;
        /** @brief 0..1 landed: a fly at the fruit sets down on it every other hover, its
         *  wings still and its forelegs rubbing (rubPhase carries the rub); 0 in the air. */
        float landed = 0.0f, rubPhase = 0.0f;
        FlyBehavior behavior = FlyBehavior::Hover;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct FlyHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Four close-range swarms of six harmless flies over inland organic ground. */
    class FlySwarms final
    {
    public:
        static constexpr std::size_t SwarmCount = 4U;
        static constexpr std::size_t PerSwarm = 6U;
        static constexpr std::size_t Count = SwarmCount * PerSwarm;
        static constexpr float FullSimulationDistance = 22.0f;
        static constexpr float ReducedSimulationDistance = 48.0f;

        explicit FlySwarms(const IslandTerrain& terrain);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const FlyState> Flies() const noexcept { return states_; }
        [[nodiscard]] std::span<const FlyHome> Homes() const noexcept { return homes_; }
        /** @brief Fruit lying on the ground this frame: a swarm whose ground has fermenting
         *  fruit within FruitRange shifts over it and hangs low there until it is gone. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        [[nodiscard]] std::uint32_t FruitVisitCount() const noexcept { return fruitVisits_; }
        [[nodiscard]] std::size_t AtFruitCount() const noexcept;
        /** @brief A gecko's snap at a point this frame: flies within LungeDartRange dart. */
        void NoteLunge(float x, float z) noexcept;
        [[nodiscard]] std::uint32_t LungeDartCount() const noexcept { return lungeDarts_; }
        static constexpr float LungeDartRange = 1.0f;
        static constexpr float FruitRange = 10.0f;
        static constexpr float FermentSeconds = 20.0f;
        static constexpr float ShiftSpeed = 1.0f;
        static constexpr float FruitAltitude = 0.26f;
        /** @brief Landings begun (a hover at the fruit, every other one) and the flies
         *  landed this moment. */
        [[nodiscard]] std::uint32_t LandingCount() const noexcept { return landings_; }
        [[nodiscard]] std::size_t LandedCount() const noexcept;
        static constexpr float LandAltitude = 0.07f, RubRate = 9.0f;

    private:
        struct Agent
        {
            FlyState state;
            FlyHome home;            // the swarm's centre this moment (shifted over fruit)
            float baseX = 0.0f, baseZ = 0.0f;   // the swarm's own ground
            float baseAltitude = 0.0f;
            bool atFruit = false;
            float angle = 0.0f, radius = 0.0f, altitude = 0.0f;
            float behaviorSeconds = 0.0f, accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            bool landing = false;      // this hover at the fruit is a landing on it
        };

        [[nodiscard]] bool FindFermentingFruit(const Agent& agent, float& x, float& z) const noexcept;
        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float deltaSeconds, float observerX,
                         float observerZ, bool observerValid) noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<FlyState, Count> states_{};
        std::array<FlyHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        std::span<const FallingFruit> lyingFruit_;
        std::uint32_t fruitVisits_ = 0U;
        float lungeX_ = 0.0f, lungeZ_ = 0.0f;
        std::uint64_t lungeFrame_ = 0U;
        bool lungeValid_ = false;
        std::uint32_t lungeDarts_ = 0U;
        std::uint32_t landings_ = 0U;
    };
}
