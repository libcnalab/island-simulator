#pragma once

#include <cstdint>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    /** @brief Renderer-independent pose and presence state for the offshore shark. */
    struct SharkState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float pitchRadians = 0.0f;
        float bankRadians = 0.0f;
        float bodyWave = 0.0f;
        /** @brief 0..1 the jaw dropped: on patrol the shark gapes for a moment every several
         *  seconds (the ram-ventilation yawn), oftener while it investigates a breach. */
        float jawOpen = 0.0f;
        float scale = 2.8f;
        float activity = 1.0f;
        float targetX = 0.0f;
        float targetZ = 0.0f;
        bool present = true;
        bool visible = true;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
    };

    /** @brief Rare non-hostile offshore patrol with deterministic dive/appearance timing. */
    class SharkPatrol final
    {
    public:
        static constexpr float FullSimulationDistance = 180.0f;
        static constexpr float ReducedSimulationDistance = 560.0f;

        explicit SharkPatrol(const IslandTerrain& terrain);

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] const SharkState& State() const noexcept { return state_; }
        [[nodiscard]] std::uint32_t PresenceCycleCount() const noexcept {
            return presenceCycleCount_;
        }

        /** @brief A fish breaking the surface this frame: within InterestDistance of the
         *  present shark it draws the shark that way for InterestSeconds (or until it is
         *  within InterestReach), after which the patrol goes on. */
        void NoteBreach(float x, float z) noexcept;
        [[nodiscard]] std::uint32_t InvestigationCount() const noexcept { return investigations_; }
        [[nodiscard]] bool Investigating() const noexcept { return interestSeconds_ > 0.0f; }
        static constexpr float InterestDistance = 60.0f;
        static constexpr float InterestSeconds = 12.0f;
        static constexpr float InterestReach = 10.0f;
        /** @brief Gapes begun (every GapePeriodSeconds of presence, every
         *  InvestigatingGapePeriodSeconds while investigating, each for GapeSeconds) and
         *  whether the jaw is dropped this moment. */
        [[nodiscard]] std::uint32_t GapeCount() const noexcept { return gapes_; }
        [[nodiscard]] bool Gaping() const noexcept { return gaping_; }
        static constexpr float GapeSeconds = 0.8f, GapePeriodSeconds = 9.0f, InvestigatingGapePeriodSeconds = 3.5f;
    private:
        void ResolveRouteTarget() noexcept;
        void UpdateMotion(float deltaSeconds) noexcept;
        [[nodiscard]] float NextPresentDuration() const noexcept;
        [[nodiscard]] float NextHiddenDuration() const noexcept;

        const IslandTerrain& terrain_;
        SharkState state_;
        float routeAngle_ = 0.0f;
        float elapsedSeconds_ = 0.0f;
        float presenceSecondsRemaining_ = 0.0f;
        float accumulatedSeconds_ = 0.0f;
        float bodyPhase_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        std::uint32_t presenceCycleCount_ = 0U;
        int direction_ = 1;
        float interestX_ = 0.0f, interestZ_ = 0.0f, interestSeconds_ = 0.0f;
        float gapeClock_ = 0.0f;
        bool gaping_ = false;
        std::uint32_t gapes_ = 0U;
        std::uint32_t investigations_ = 0U;
    };
}
