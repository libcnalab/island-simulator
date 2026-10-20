#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;
    class JungleLayout;
    struct FallingFruit;

    enum class AntBehavior : std::uint8_t { Walk, Carry, Pause };

    struct AntState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float forwardX = 1.0f, forwardY = 0.0f, forwardZ = 0.0f;
        float normalX = 0.0f, normalY = 1.0f, normalZ = 0.0f;
        float gaitPhase = 0.0f, scale = 1.0f;
        /** @brief 0..1 the antennae sweeping: an ant pausing on the trail feels the way ahead,
         *  sweepPhase carrying the sweep; walking on, the antennae settle to their set. */
        float antennaSweep = 0.0f, sweepPhase = 0.0f;
        AntBehavior behavior = AntBehavior::Walk;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
        std::uint8_t trailIndex = 0U;
    };

    struct AntTrailHome
    {
        float x = 0.0f, y = 0.0f, z = 0.0f, halfLength = 0.0f;
        bool onTrunk = false;
    };

    /** Four close-range ant trails; three cross soil and one climbs a real canopy trunk. */
    class AntTrails final
    {
    public:
        static constexpr std::size_t TrailCount = 4U;
        static constexpr std::size_t PerTrail = 16U;
        static constexpr std::size_t Count = TrailCount * PerTrail;
        static constexpr float FullSimulationDistance = 16.0f;
        static constexpr float ReducedSimulationDistance = 34.0f;

        AntTrails(const IslandTerrain& terrain, const JungleLayout& jungle);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const AntState> Ants() const noexcept { return states_; }
        [[nodiscard]] std::span<const AntTrailHome, TrailCount> Trails() const noexcept
        {
            return trails_;
        }
        /** @brief Fruit lying on the ground this frame: a ground trail whose food end lies
         *  within FoodReach of a resting fruit bends to it, and the carriers come back from
         *  the fruit; with the fruit gone the trail returns to its line. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept;
        [[nodiscard]] std::size_t FoodTrailCount() const noexcept;
        /** @brief The far (food) end of a trail as the ants walk it now. */
        [[nodiscard]] bool FoodEnd(std::size_t trail, float& x, float& z) const noexcept;
        static constexpr float FoodReach = 5.0f;
        /** @brief Antennae sweeps begun (one per pause on the trail) and the ants sweeping. */
        [[nodiscard]] std::uint32_t SweepCount() const noexcept { return sweeps_; }
        [[nodiscard]] std::size_t SweepingCount() const noexcept;
        static constexpr float SweepRate = 11.0f;
        /** @brief Meetings: two ants of a trail coming head to head within MeetReach pause
         *  MeetSeconds to touch antennae (the pause's sweep), once per passing. */
        [[nodiscard]] std::uint32_t MeetCount() const noexcept { return meets_; }
        static constexpr float MeetReach = 0.035f, MeetSeconds = 0.3f, MeetCooldownSeconds = 3.0f;

    private:
        struct Agent
        {
            AntState state;
            float progress = 0.0f, speed = 0.0f, pauseSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            bool outward = true;
            bool sweeping = false;
            float meetCooldown = 0.0f;   // keeps one passing from repeating its meeting
        };

        void Initialize(std::size_t index);
        void Place(Agent&) noexcept;
        void UpdateAgent(Agent&, float deltaSeconds) noexcept;

        const IslandTerrain& terrain_;
        std::array<AntTrailHome, TrailCount> trails_{};
        std::array<float, TrailCount> trailAngles_{};
        // The line the ants walk now: the authored one, or one bent to a fruit.
        struct Line { float x = 0.0f, z = 0.0f, angle = 0.0f, halfLength = 0.0f; bool food = false; };
        std::array<Line, TrailCount> lines_{};
        bool linesReady_ = false;
        void RefreshLines() noexcept;
        std::array<float, TrailCount> trunkScales_{};
        std::array<std::uint8_t, TrailCount> trunkVariants_{};
        std::array<Agent, Count> agents_{};
        std::array<AntState, Count> states_{};
        std::uint64_t frameIndex_ = 0U;
        std::uint32_t sweeps_ = 0U;
        std::uint32_t meets_ = 0U;
    };
}
