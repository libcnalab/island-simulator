#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    /** @brief Calm near-shore activity states for a sea turtle. */
    enum class SeaTurtleBehavior : std::uint8_t
    {
        Cruise,
        Forage,
        Surface
    };

    /** @brief Renderer-independent pose for one non-hostile sea turtle. */
    struct SeaTurtleState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float pitchRadians = 0.0f;
        float bankRadians = 0.0f;
        float speed = 0.0f;
        float flipperStroke = 0.0f;
        /** @brief 0..1 the head dipped to the bottom in a grazing bite. */
        float graze = 0.0f;
        /** @brief 0..1 the blink: the eyes squeezed shut as the head breaks the surface for
         *  a breath, and a slow blink every dozen seconds or so otherwise. */
        float blink = 0.0f;
        float scale = 1.0f;
        SeaTurtleBehavior behavior = SeaTurtleBehavior::Cruise;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    /** @brief Immutable water-space range that bounds one turtle's local route. */
    struct SeaTurtleHomeRange
    {
        float x = 0.0f;
        float z = 0.0f;
        float radius = 0.0f;
    };

    /** @brief Deterministic near-shore turtles with slow cruising and occasional surfacing. */
    class SeaTurtleGroup final
    {
    public:
        static constexpr std::size_t TurtleCount = 5U;
        static constexpr float MinimumShoreDistance = -38.0f;
        static constexpr float MaximumShoreDistance = -7.0f;
        static constexpr float FullSimulationDistance = 135.0f;
        static constexpr float ReducedSimulationDistance = 330.0f;

        explicit SeaTurtleGroup(const IslandTerrain& terrain);

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const SeaTurtleState> Turtles() const noexcept;
        /** @brief The last turtle to lift its head through the surface for a breath: where,
         *  which way it faced, its scale, counted so a breath is noted once. */
        struct Breath { float x = 0.0f, z = 0.0f, headingRadians = 0.0f, scale = 1.0f; std::uint32_t count = 0; };
        [[nodiscard]] const Breath& LastBreath() const noexcept { return breath_; }
        /** @brief The last grazing bite: a turtle foraging low over the bottom dips its head
         *  to it every BiteSeconds; the bite is noted at the beak, on the bottom, counted so
         *  the sand it stirs rises once. */
        struct Bite { float x = 0.0f, y = 0.0f, z = 0.0f, scale = 1.0f; std::uint32_t count = 0U; };
        [[nodiscard]] const Bite& LastBite() const noexcept { return bite_; }
        [[nodiscard]] std::uint32_t BiteCount() const noexcept { return bite_.count; }
        static constexpr float BiteSeconds = 2.4f, BiteDipSeconds = 0.9f, GrazeHeight = 1.2f;
        /** @brief Blinks begun (one at each breath, one every BlinkPeriodSeconds or so
         *  otherwise, each BlinkSeconds) and the turtles mid-blink. */
        [[nodiscard]] std::uint32_t BlinkCount() const noexcept { return blinks_; }
        [[nodiscard]] std::size_t BlinkingCount() const noexcept;
        static constexpr float BlinkSeconds = 0.35f, BlinkPeriodSeconds = 12.0f;
        /** @brief The shark this frame (while present): a turtle within ThreatDistance of it
         *  turns away as from the swimmer, a surfacing one dives, and both swim on faster
         *  until it is clear. */
        void SetThreat(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::uint32_t ThreatDiveCount() const noexcept { return threatDives_; }
        static constexpr float ThreatDistance = 18.0f;
        static constexpr float BreathDepth = -0.2f;
        [[nodiscard]] std::span<const SeaTurtleHomeRange> Homes() const noexcept;
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
            SeaTurtleState state;
            SeaTurtleHomeRange home;
            float behaviorSeconds = 0.0f;
            float wanderPhase = 0.0f;
            float depthPhase = 0.0f;
            float flipperPhase = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitionCount = 0U;
            float grazeClock = 0.0f;
            bool bitten = false;
            float blinkClock = 0.0f, blinkSeconds = 0.0f;
            bool blinking = false;
        };

        void InitializeAgent(std::size_t index);
        void UpdateAgent(Agent& agent, float deltaSeconds,
                         float observerX, float observerZ,
                         bool observerValid) noexcept;
        [[nodiscard]] bool IsValidHabitat(float x, float z,
                                          const SeaTurtleHomeRange& home) const noexcept;
        [[nodiscard]] float DesiredDepth(const Agent& agent,
                                         float seabedHeight) const noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<SeaTurtleState> states_;
        std::vector<SeaTurtleHomeRange> homes_;
        float elapsedSeconds_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        Breath breath_;
        float threatX_ = 0.0f, threatZ_ = 0.0f;
        bool threatValid_ = false;
        std::uint32_t threatDives_ = 0U;
        Bite bite_;
        std::uint32_t blinks_ = 0U;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
    };
}
