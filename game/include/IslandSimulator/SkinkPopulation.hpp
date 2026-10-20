#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/GrassWalkers.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    enum class SkinkBehavior : std::uint8_t { Bask, Forage, Flee };

    struct SkinkState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float gaitPhase = 0.0f, breathPhase = 0.0f, scale = 1.0f;
        /** @brief 0..1 the push-up: the front body lifted on straightened forelegs, pulsed
         *  three times in a display bout while basking in the sun. */
        float pushUp = 0.0f;
        SkinkBehavior behavior = SkinkBehavior::Bask;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct SkinkHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Eight diurnal skinks that bask and forage on open inland ground. */
    class SkinkPopulation final
    {
    public:
        static constexpr std::size_t Count = 8U;
        static constexpr float FullSimulationDistance = 48.0f;
        static constexpr float ReducedSimulationDistance = 112.0f;

        explicit SkinkPopulation(const IslandTerrain& terrain);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        /** @brief Other bodies on the ground this frame (the herds, the bear, the resident):
         *  the nearest of them frightens an animal as the walker does. */
        void SetWalkers(std::span<const GrassWalker> walkers) noexcept { walkers_ = walkers; }
        [[nodiscard]] std::uint32_t WalkerScatterCount() const noexcept { return walkerScatters_; }
        /** @brief The cloud-shadow clock (the shared sky drift): a basking skink that a cloud's
         *  shadow covers (cover over ShadeCover) gets up and forages, and a foraging skink lies
         *  down to bask only where the cover is under SunCover. */
        void SetCloudClock(float seconds) noexcept { cloudClock_ = seconds; }
        [[nodiscard]] std::uint32_t ShadeRouseCount() const noexcept { return shadeRouses_; }
        [[nodiscard]] std::size_t SunBaskingCount() const noexcept;
        static constexpr float ShadeCover = 0.5f;
        static constexpr float SunCover = 0.25f;
        /** @brief Push-up displays begun (a basking skink, DisplayDelaySeconds into its bask
         *  and every DisplayPeriodSeconds or so after, a bout of three push-ups lasting
         *  DisplaySeconds) and the skinks mid-bout. */
        [[nodiscard]] std::uint32_t DisplayCount() const noexcept { return displays_; }
        [[nodiscard]] std::size_t DisplayingCount() const noexcept;
        static constexpr float DisplaySeconds = 1.5f, DisplayPeriodSeconds = 8.0f, DisplayDelaySeconds = 1.5f;

        [[nodiscard]] std::span<const SkinkState> Skinks() const noexcept { return states_; }
        [[nodiscard]] std::span<const SkinkHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantCount_; }

    private:
        struct Agent
        {
            SkinkState state;
            SkinkHome home;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            float displayClock = 0.0f;
            bool displaying = false;
        };

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;
        [[nodiscard]] bool IsValid(float x, float z, const SkinkHome& home) const noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<SkinkState, Count> states_{};
        std::array<SkinkHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        int fullCount_ = 0, reducedCount_ = 0, dormantCount_ = 0;
        std::span<const GrassWalker> walkers_;
        std::uint32_t walkerScatters_ = 0U;
        float cloudClock_ = 0.0f;
        std::uint32_t shadeRouses_ = 0U;
        std::uint32_t displays_ = 0U;
    };
}
