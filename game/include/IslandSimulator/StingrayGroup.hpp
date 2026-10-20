#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;

    enum class StingrayBehavior : std::uint8_t
    {
        Glide,
        Forage,
        Settle
    };

    struct StingrayState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float bankRadians = 0.0f;
        float speed = 0.0f;
        float wingWave = 0.0f;
        float scale = 1.0f;
        StingrayBehavior behavior = StingrayBehavior::Glide;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct StingrayHomeRange
    {
        float x = 0.0f;
        float z = 0.0f;
        float radius = 0.0f;
    };

    /** @brief Five peaceful stingrays confined to shallow sandy water. */
    class StingrayGroup final
    {
    public:
        static constexpr std::size_t Count = 5U;
        static constexpr float MinimumShoreDistance = -34.0f;
        static constexpr float MaximumShoreDistance = -6.0f;
        static constexpr float FullSimulationDistance = 140.0f;
        static constexpr float ReducedSimulationDistance = 330.0f;

        explicit StingrayGroup(const IslandTerrain& terrain);
        /** @brief The last settled ray to burst out of the sand at a swimmer's approach: where
         *  it lay, counted so a burst is noted once. */
        struct StingrayBurst { float x = 0.0f, y = 0.0f, z = 0.0f, scale = 1.0f; std::uint32_t count = 0; };
        [[nodiscard]] const StingrayBurst& LastBurst() const noexcept { return burst_; }
        /** @brief The shark this frame (while present): a ray within ThreatDistance of it
         *  turns away as from the swimmer, and a settled one bursts from the sand. */
        void SetThreat(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::uint32_t ThreatBurstCount() const noexcept { return threatBursts_; }
        static constexpr float ThreatDistance = 9.0f;
        static constexpr float BurstDistance = 3.6f, BurstSpeed = 0.95f;

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const StingrayState> Stingrays() const noexcept {
            return states_;
        }
        [[nodiscard]] std::span<const StingrayHomeRange> Homes() const noexcept {
            return homes_;
        }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantCount_; }

    private:
        struct Agent
        {
            StingrayState state;
            StingrayHomeRange home;
            float routePhase = 0.0f;
            float flapPhase = 0.0f;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitionCount = 0U;
        };

        void Initialize(std::size_t index);
        void UpdateAgent(Agent& agent, float deltaSeconds,
                         float observerX, float observerZ,
                         bool observerValid) noexcept;
        [[nodiscard]] bool IsSandyHabitat(float x, float z,
                                          const StingrayHomeRange& home) const noexcept;
        [[nodiscard]] float DesiredHeight(const Agent& agent) const noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<StingrayState> states_;
        std::vector<StingrayHomeRange> homes_;
        float elapsedSeconds_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        StingrayBurst burst_;
        float threatX_ = 0.0f, threatZ_ = 0.0f;
        bool threatValid_ = false;
        std::uint32_t threatBursts_ = 0U;
        int fullCount_ = 0;
        int reducedCount_ = 0;
        int dormantCount_ = 0;
    };
}
