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
    class CanopyLightField;
    enum class DeerBehavior : std::uint8_t { Graze, Walk, Rest, Retreat, Drink };

    struct DeerState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float gaitPhase = 0.0f, grazeAmount = 0.0f, scale = 1.0f;
        /** @brief 0..1 the tail raised: flagged up at a bolt, half up in the flick a grazing
         *  or resting deer gives every few seconds, hanging otherwise. */
        float tailRaise = 0.0f;
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
        // The antlered sentinel watches further out; its clearing partner lifts its head within
        // the wider range once the sentinel has stood alert for the reaction delay.
        static constexpr float SentinelAlertDistance = 24.0f;
        static constexpr float HerdReactionSeconds = .7f;
        // A sudden sound (a coconut's thud) within this range makes a deer stand and face it.
        static constexpr float StartleDistance = 30.0f;
        // No deer comes closer than this to the campfire, grazing or fleeing.
        static constexpr float FireClearance = 7.0f;
        // The tail flick's bout and the period between flicks (plus a little per deer).
        static constexpr float FlickSeconds = 0.45f;
        static constexpr float FlickPeriodSeconds = 4.0f;
        static constexpr float StartleSeconds = 6.0f;
        // A clearing pair drifting further apart than this walks back together.
        static constexpr float CohesionDistance = 9.0f;
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
        [[nodiscard]] bool SentinelAlert() const noexcept { return sentinelAlertSeconds_ > 0.0f; }
        /** @brief A sudden sound at a point; nearby deer stand alert facing it for a while. */
        void NoteDisturbance(float x, float z) noexcept;
        [[nodiscard]] bool Startled() const noexcept { return disturbanceSeconds_ > 0.0f; }
        [[nodiscard]] float SentinelAlertSeconds() const noexcept { return sentinelAlertSeconds_; }
        /** @brief The light over the ground (crowns, palm heads and the solid casters' shade):
         *  a deer whose rest comes on sunlit ground first walks to the shadiest valid spot
         *  within ShadeSeekRadius and lies up there. Null keeps the deer resting where it is. */
        void SetLight(const CanopyLightField* light) noexcept { light_ = light; }
        [[nodiscard]] std::uint32_t ShadeRestCount() const noexcept { return shadeRests_; }
        [[nodiscard]] bool SeekingShade(std::size_t index) const noexcept
        {
            return index < Count && agents_[index].seekingShade;
        }
        static constexpr float ShadeSeekRadius = 10.0f;
        static constexpr float SunlitLevel = 0.72f;
        static constexpr float ShadedLevel = 0.55f;

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
            float flickClock = 0.0f;
            // The shade a sunlit rest walks to; valid while seekingShade.
            float shadeX = 0.0f, shadeZ = 0.0f;
            bool seekingShade = false;
        };
        [[nodiscard]] float LightAt(float x, float z) const noexcept;
        [[nodiscard]] bool FindShade(const Agent& agent, std::size_t index, float& x, float& z) const noexcept;
        void FindDrinkingPoint(std::size_t index) noexcept;

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;

        const IslandTerrain& terrain_;
        WorldCollisionGrid obstacles_;
        std::array<Agent, Count> agents_{};
        std::array<DeerState, Count> states_{};
        std::array<DeerHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        float sentinelAlertSeconds_ = 0.0f;
        float disturbanceX_ = 0.0f, disturbanceZ_ = 0.0f, disturbanceSeconds_ = 0.0f;
        const CanopyLightField* light_ = nullptr;
        std::uint32_t shadeRests_ = 0U;
    };
}
