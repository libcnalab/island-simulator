#pragma once
#include <algorithm>
#include <cmath>

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    enum class FruitBatBehavior : std::uint8_t
    {
        Roost,
        DayFlight
    };

    struct FruitBatRoost
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        /** @brief The crown the roost hangs in: a roosting bat lets its fruit drop. */
        CanopyPlacement tree{};
    };

    struct FruitBatState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float speed = 0.0f;
        float wingPose = 0.0f;
        float hangAmount = 1.0f;
        /** @brief 0..1 fanning: a hanging bat half-opens its wings and beats them slowly for
         *  a few seconds every quarter minute to cool, the wing pose carrying the beat. */
        float fan = 0.0f;
        /** @brief 0..1 the stretch: now and then a hanging bat stretches one wing out and
         *  down for a couple of seconds and folds it again; stretchSide (+1 right, -1 left)
         *  is the wing, the wings taking turns. */
        float stretch = 0.0f;
        float stretchSide = 1.0f;
        float scale = 1.0f;
        FruitBatBehavior behavior = FruitBatBehavior::Roost;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    /** @brief Six bounded daytime fruit bats using real mature jungle crowns as roosts. */
    class FruitBatColony final
    {
    public:
        static constexpr std::size_t Count = 6U;
        static constexpr std::size_t MaximumDayFlights = 2U;
        static constexpr float FullSimulationDistance = 120.0f;
        static constexpr float ReducedSimulationDistance = 280.0f;
        /** @brief Fanning bouts begun by the hanging bats, and the bats mid-bout. */
        [[nodiscard]] std::uint32_t FanCount() const noexcept { return fans_; }
        [[nodiscard]] std::size_t FanningCount() const noexcept;
        static constexpr float FanSeconds = 3.0f, FanPeriodSeconds = 16.0f, FanBeatRate = 9.0f;
        /** @brief Wing stretches begun by the hanging bats, and the bats mid-stretch. */
        [[nodiscard]] std::uint32_t StretchCount() const noexcept { return stretches_; }
        [[nodiscard]] std::size_t StretchingCount() const noexcept;
        static constexpr float StretchSeconds = 2.2f, StretchPeriodSeconds = 23.0f;

        FruitBatColony(const IslandTerrain& terrain,
                       std::span<const CanopyPlacement> canopy);

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        /** @brief How dark it is (0 by day .. 1 at night): from dusk the whole colony leaves the
         *  roost to forage on wider rounds, taking off and settling back over a few seconds. */
        void SetNightness(float nightness) noexcept { nightness_ = std::isfinite(nightness) ? std::clamp(nightness, 0.0f, 1.0f) : 0.0f; }
        [[nodiscard]] std::size_t FlyingCount() const noexcept;

        [[nodiscard]] std::span<const FruitBatState> Bats() const noexcept { return states_; }
        [[nodiscard]] std::span<const FruitBatRoost> Roosts() const noexcept { return roosts_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullSimulationCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedSimulationCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantSimulationCount_; }

    private:
        struct Agent
        {
            FruitBatState state;
            FruitBatRoost roost;
            float orbitRadius = 0.0f;
            float orbitPhase = 0.0f;
            float orbitSpeed = 0.0f;
            float flapPhase = 0.0f;
            float accumulatedSeconds = 0.0f;
            bool fanning = false;
            bool stretching = false;
            float airborne = 0.0f;   // 0 hanging at the roost .. 1 on its round
        };
        float nightness_ = 0.0f;

        std::vector<Agent> agents_;
        std::vector<FruitBatState> states_;
        std::vector<FruitBatRoost> roosts_;
        std::uint64_t frameIndex_ = 0U;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
        std::uint32_t fans_ = 0U;
        std::uint32_t stretches_ = 0U;
    };
}
