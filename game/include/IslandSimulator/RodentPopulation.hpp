#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/GrassWalkers.hpp"
#include "IslandSimulator/FallingFruitField.hpp"
#include "IslandSimulator/SnakePopulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    enum class RodentSpecies : std::uint8_t { Mouse, Rat };
    enum class RodentBehavior : std::uint8_t { Rest, Forage, Scurry, Hidden };

    struct RodentState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float gaitPhase = 0.0f, sniffPhase = 0.0f, visibility = 1.0f;
        RodentSpecies species = RodentSpecies::Mouse;
        RodentBehavior behavior = RodentBehavior::Rest;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct RodentHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Twelve field mice and eight rats in bounded inland edge habitats. */
    class RodentPopulation final
    {
    public:
        static constexpr std::size_t MouseCount = 12U;
        static constexpr std::size_t RatCount = 8U;
        static constexpr std::size_t Count = MouseCount + RatCount;
        static constexpr float FullSimulationDistance = 34.0f;
        static constexpr float ReducedSimulationDistance = 85.0f;

        explicit RodentPopulation(const IslandTerrain& terrain);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        /** @brief Other bodies on the ground this frame (the herds, the bear, the resident):
         *  the nearest of them frightens an animal as the walker does. */
        void SetWalkers(std::span<const GrassWalker> walkers) noexcept { walkers_ = walkers; }
        [[nodiscard]] std::uint32_t WalkerScatterCount() const noexcept { return walkerScatters_; }
        /** @brief The snakes this frame: a snake's head within SnakeFleeDistance is the worst
         *  of threats; the rodent bolts from it and goes to ground for six seconds or more. */
        void SetSnakes(std::span<const SnakeState> snakes) noexcept { snakes_ = snakes; }
        [[nodiscard]] std::uint32_t SnakeScatterCount() const noexcept { return snakeScatters_; }
        static constexpr float SnakeFleeDistance = 3.5f;
        static constexpr float SnakeHideSeconds = 6.0f;
        /** @brief Fruit lying on the ground this frame: a rodent setting out to forage with
         *  a fruit lying within FruitRange inside its range goes to it and feeds there. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        [[nodiscard]] std::uint32_t FruitVisitCount() const noexcept { return fruitVisits_; }
        [[nodiscard]] std::size_t FeedingCount() const noexcept;
        static constexpr float FruitRange = 6.0f;
        static constexpr float FruitReach = 0.32f;
        static constexpr float FeedSeconds = 6.0f;

        [[nodiscard]] std::span<const RodentState> Rodents() const noexcept { return states_; }
        [[nodiscard]] std::span<const RodentHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] bool IsValidPosition(std::size_t index, float x, float z) const noexcept;

    private:
        struct Agent
        {
            RodentState state;
            bool snakeBolt = false;   // this scurry is from a snake: the hide after it is longer
            RodentHome home;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            int fruitTarget = -1;          // index into the lying fruit; -1 for none
            float feedSeconds = 0.0f;      // time spent feeding at it
        };

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, std::size_t, float, float, float, bool) noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<RodentState, Count> states_{};
        std::array<RodentHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        std::span<const GrassWalker> walkers_;
        std::uint32_t walkerScatters_ = 0U;
        std::span<const SnakeState> snakes_;
        std::uint32_t snakeScatters_ = 0U;
        std::span<const FallingFruit> lyingFruit_;
        std::uint32_t fruitVisits_ = 0U;
        [[nodiscard]] int FindFruit(const Agent& agent, std::size_t index) const noexcept;
    };
}
