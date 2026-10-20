#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    enum class FrogBehavior : std::uint8_t { Rest, Hop, Flee, Dive };

    struct FrogState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, hopPhase = 0.0f, breathPhase = 0.0f;
        /** @brief 0..1 the vocal sac: swelling and pulsing under the chin through a call. */
        float throat = 0.0f;
        /** @brief 0..1 the blink: a resting frog draws its eyes down into its head for a
         *  moment every several seconds (frogs blink by retracting the eyes). */
        float blink = 0.0f;
        float scale = 1.0f;
        /** @brief 0..1 under the pool's surface: a pool frog that leapt in from the walker. */
        float submerged = 0.0f;
        FrogBehavior behavior = FrogBehavior::Rest;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct FrogHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Ten peaceful stream/wetland frogs with bounded rest, hop and player-retreat states. */
    class FrogPopulation final
    {
    public:
        // Eight along the stream, two at the wetland, and two on the banks of the cascade's
        // plunge pool (placed from the stream course at construction).
        static constexpr std::size_t Count = 12U;
        static constexpr std::size_t PoolFrogCount = 2U;
        static constexpr float PoolFrogRadius = 4.5f;
        static constexpr float FullSimulationDistance = 48.0f;
        static constexpr float ReducedSimulationDistance = 105.0f;
        /** @brief A pool frog fleeing the walker leaps for the pool, swims under for
         *  DiveSeconds, surfaces for SurfaceSeconds and makes back for its bank. */
        static constexpr float DiveSeconds = 6.0f;
        static constexpr float SurfaceSeconds = 2.0f;
        static constexpr float DiveDepth = 0.12f;
        [[nodiscard]] float PoolWaterHeight() const noexcept { return poolWater_; }
        [[nodiscard]] std::uint32_t DiveCount() const noexcept { return dives_; }
        struct FrogDive { float x = 0.0f, z = 0.0f; bool valid = false; };
        [[nodiscard]] const FrogDive& LastDive() const noexcept { return lastDive_; }
        /** @brief A call given by this frog: its throat swells and pulses for CallSeconds. */
        void NoteCall(std::size_t index) noexcept;
        [[nodiscard]] std::uint32_t CallCount() const noexcept { return calls_; }
        static constexpr float CallSeconds = 1.4f;
        /** @brief Blinks begun (a resting frog BlinkDelaySeconds into its rest and every
         *  BlinkPeriodSeconds or so after, for BlinkSeconds) and the frogs mid-blink. */
        [[nodiscard]] std::uint32_t BlinkCount() const noexcept { return blinks_; }
        [[nodiscard]] std::size_t BlinkingCount() const noexcept;
        static constexpr float BlinkDelaySeconds = 1.5f, BlinkSeconds = 0.25f, BlinkPeriodSeconds = 7.0f;

        explicit FrogPopulation(const IslandTerrain& terrain);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const FrogState> Frogs() const noexcept { return states_; }
        [[nodiscard]] std::span<const FrogHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantCount_; }

    private:
        struct Agent
        {
            FrogState state;
            FrogHome home;
            float groundY = 0.0f;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            bool poolFrog = false;        // keeps a bank of the plunge pool
            float diveSeconds = 0.0f;     // time under or on the surface in a dive
            bool surfaced = false;        // the dive's second half, floating
            float callSeconds = 0.0f;     // remaining call; the throat pulses while it runs
            float blinkClock = 0.0f;      // time into the rest's blink cycle
            bool blinking = false;
        };

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;
        [[nodiscard]] bool IsValid(float x, float z, const FrogHome& home) const noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<FrogState, Count> states_{};
        std::array<FrogHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        float poolX_ = 0.0f, poolZ_ = 0.0f, poolAcrossX_ = 0.0f, poolAcrossZ_ = 1.0f;
        float poolWater_ = 0.0f, poolHalfWidth_ = 1.0f;
        std::uint32_t dives_ = 0U;
        FrogDive lastDive_;
        std::uint32_t calls_ = 0U;
        std::uint32_t blinks_ = 0U;
        int fullCount_ = 0, reducedCount_ = 0, dormantCount_ = 0;
    };
}
