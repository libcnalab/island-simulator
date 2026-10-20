#pragma once

#include <cstddef>
#include <cstdint>
#include <array>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    /** @brief Compact autonomous behaviour used by the beach-crab population. */
    enum class CrabBehavior : std::uint8_t
    {
        Pause,
        Forage,
        Scuttle,
        Hide
    };

    /** @brief Renderer-independent state for one procedural beach crab. */
    struct CrabState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float bodyYawRadians = 0.0f;
        float travelHeadingRadians = 0.0f;
        float speed = 0.0f;
        float legPhase = 0.0f;
        float clawPose = 0.0f;
        /** @brief The claw wave: 0..1 one claw raised (waveSide +1 right, -1 left) and swept
         *  up and down on wavePhase; a crab paused in the open displays a second into each
         *  pause and every nine seconds or so after, dropping it the moment it moves. */
        float waveAmount = 0.0f;
        float waveSide = 1.0f;
        float wavePhase = 0.0f;
        /** @brief 0..1 down the colony's burrow: the body sinks below the sand at 1. */
        float hideAmount = 0.0f;
        /** @brief 0..1 the eyestalks folded flat: a crab scuttling for its burrow, or down
         *  it, lays its eyes along the carapace front and raises them again as it comes up
         *  and stands. */
        float eyeFold = 0.0f;
        /** @brief 0..1 feeding: a crab paused among the strandings works the food with its
         *  mouthparts, blowing a small bubble now and then (the bubble field draws them). */
        float feeding = 0.0f;
        float scale = 1.0f;
        CrabBehavior behavior = CrabBehavior::Pause;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    /** @brief Immutable home range used to keep a crab inside suitable beach habitat. */
    struct CrabHomeRange
    {
        float x = 0.0f;
        float z = 0.0f;
        float radius = 0.0f;
    };

    struct CrabHabitatCue { float x = 0.0f, z = 0.0f; };

    /** @brief Visible sandy retreat shared by the five members of one beach colony. */
    struct CrabBurrow
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float radius = 0.0f;
    };

    /** @brief Deterministic beach-constrained crab simulation with mild player avoidance. */
    class CrabColony final
    {
    public:
        static constexpr std::size_t CrabCount = 30U;
        static constexpr float MinimumShoreDistance = 1.4f;
        static constexpr float MaximumShoreDistance = 17.0f;
        static constexpr float FullSimulationDistance = 82.0f;
        static constexpr float ReducedSimulationDistance = 210.0f;
        static constexpr float MinimumAgentSeparation = 0.72f;

        explicit CrabColony(const IslandTerrain& terrain);
        /** @brief Transactionally reanchors six colonies beside real non-solid beach debris. */
        void SetDebrisHabitats(std::span<const CrabHabitatCue> cues);
        [[nodiscard]] static bool IsNearDebris(
            const CrabHomeRange& home, std::span<const CrabHabitatCue> cues) noexcept;

        /** @brief Advances autonomous beach movement and observer-relative activation. */
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        /** @brief A second walker on the beach (the resident); crabs scatter from whichever
         *  of the observer and the walker is nearer. Activation stays observer-relative. */
        void SetWalker(float x, float z, bool valid) noexcept;
        static constexpr float ScuttleDistance = 5.2f;
        /** @brief A frightened crab this near its colony's burrow, with the hole roughly along
         *  its line of flight, bolts for it and goes down; it comes up once the beach has been
         *  clear (no threat within 1.6 scuttle distances) for EmergeDelaySeconds. */
        static constexpr float BurrowDashDistance = 3.5f;
        static constexpr float BurrowArriveDistance = 0.55f;
        static constexpr float EmergeDelaySeconds = 4.0f;
        [[nodiscard]] std::size_t HiddenCount() const noexcept;
        [[nodiscard]] std::uint32_t HideCount() const noexcept { return hides_; }
        /** @brief Claw waves begun, and the crabs mid-wave. */
        [[nodiscard]] std::uint32_t WaveCount() const noexcept { return waves_; }
        [[nodiscard]] std::size_t WavingCount() const noexcept;
        static constexpr float WaveSeconds = 1.8f, WavePeriodSeconds = 9.0f, WaveDelaySeconds = 0.5f;
        /** @brief Crabs with their eyestalks folded this moment (the fold past half). */
        [[nodiscard]] std::size_t EyeFoldedCount() const noexcept;
        /** @brief Crabs feeding among the strandings this moment (the feed past half). */
        [[nodiscard]] std::size_t FeedingCount() const noexcept;

        [[nodiscard]] std::span<const CrabState> Crabs() const noexcept;
        [[nodiscard]] std::span<const CrabHomeRange> Homes() const noexcept;
        [[nodiscard]] std::span<const CrabBurrow> Burrows() const noexcept { return burrows_; }
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
            CrabState state;
            CrabHomeRange home;
            float behaviorSeconds = 0.0f;
            float movementPhase = 0.0f;
            float animationPhase = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitionCount = 0U;
            int preferredDirection = 1;
            bool toBurrow = false;
            float clearSeconds = 0.0f;
            float waveClock = 0.0f;
            bool waving = false;
            bool feedingPause = false;     // this pause is a feed among the strandings
        };

        void InitializeAgent(std::size_t index);
        void RebuildBurrows() noexcept;
        void UpdateAgent(Agent& agent, std::size_t index, float deltaSeconds,
                         float observerX, float observerZ,
                         bool observerValid) noexcept;
        float walkerX_ = 0.0f, walkerZ_ = 0.0f;
        bool walkerValid_ = false;
        [[nodiscard]] bool IsValidHabitat(float x, float z,
                                          const CrabHomeRange& home) const noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<CrabState> states_;
        std::vector<CrabHomeRange> homes_;
        std::array<CrabBurrow, 6> burrows_{};
        std::uint64_t frameIndex_ = 0U;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
        std::array<CrabHabitatCue, 6> debrisAnchors_{};
        bool debrisAnchored_ = false;
        std::uint32_t hides_ = 0U;
        std::uint32_t waves_ = 0U;
    };
}
