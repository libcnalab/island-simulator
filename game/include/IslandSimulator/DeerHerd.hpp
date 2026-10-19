#pragma once

#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    enum class DeerBehavior : std::uint8_t { Graze, Walk, Rest, Retreat, Drink };

    struct DeerState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float gaitPhase = 0.0f, grazeAmount = 0.0f, scale = 1.0f;
        DeerBehavior behavior = DeerBehavior::Graze;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t coatVariant = 0U;
        bool antlered = false;
    };

    struct DeerHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Four peaceful feral deer using open inland clearings and bounded retreat. */
    class DeerHerd final
    {
    public:
        static constexpr std::size_t Count = 4U;
        // Within this range a deer stands alert facing the player; closer than 9 m it bounds off obliquely.
        static constexpr float AlertDistance = 18.0f;
        static constexpr float FullSimulationDistance = 90.0f;
        static constexpr float ReducedSimulationDistance = 190.0f;

        DeerHerd(const IslandTerrain& terrain, std::span<const CircularObstacle> obstacles);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const DeerState> Deer() const noexcept { return states_; }
        [[nodiscard]] std::span<const DeerHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] bool IsValidPosition(std::size_t index, float x, float z) const noexcept;
        /** @brief Deer whose home reaches a stream bank; they walk there to drink. */
        [[nodiscard]] std::size_t DrinkerCount() const noexcept;
        [[nodiscard]] bool DrinkPoint(std::size_t index, float& x, float& z) const noexcept;

    private:
        struct Agent
        {
            DeerState state;
            DeerHome home;
            float targetHeadingRadians = 0.0f;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            // Nearest reachable stream bank and the water it faces; valid when hasDrink.
            float drinkX = 0.0f, drinkZ = 0.0f, faceX = 0.0f, faceZ = 0.0f;
            bool hasDrink = false;
        };
        void FindDrinkingPoint(std::size_t index) noexcept;

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;

        const IslandTerrain& terrain_;
        WorldCollisionGrid obstacles_;
        std::array<Agent, Count> agents_{};
        std::array<DeerState, Count> states_{};
        std::array<DeerHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
    };
}
