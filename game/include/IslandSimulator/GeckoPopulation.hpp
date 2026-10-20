#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/FlySwarms.hpp"
#include "IslandSimulator/GrassWalkers.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    enum class GeckoBehavior : std::uint8_t
    {
        Rest,
        Scurry,
        Hidden,
        Stalk,
        Lunge
    };

    struct GeckoState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float speed = 0.0f;
        float gaitPhase = 0.0f;
        float visibility = 1.0f;
        /** @brief 0..1 the stalking tail wag: the tail tip sways wide and fast while the
         *  gecko creeps on a fly, and stills again after the lunge or when the hunt ends. */
        float tailWag = 0.0f;
        float tailWagPhase = 0.0f;
        /** @brief 0..1 the eye lick: a resting gecko's tongue out of the snout and drawn
         *  over one eye to clean it for half a second every ten seconds or so; lickSide
         *  (+1 right, -1 left) is the eye, alternating lick by lick. */
        float eyeLick = 0.0f;
        float lickSide = 1.0f;
        float scale = 1.0f;
        GeckoBehavior behavior = GeckoBehavior::Rest;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct GeckoHome
    {
        float x = 0.0f;
        float z = 0.0f;
        float radius = 0.0f;
    };

    /** @brief Small deterministic inland lizards that rest, flee and hide. */
    class GeckoPopulation final
    {
    public:
        static constexpr std::size_t Count = 8U;
        static constexpr float FullSimulationDistance = 44.0f;
        static constexpr float ReducedSimulationDistance = 105.0f;

        explicit GeckoPopulation(const IslandTerrain& terrain);

        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        /** @brief Other bodies on the ground this frame (the herds, the bear, the resident):
         *  the nearest of them frightens an animal as the walker does. */
        void SetWalkers(std::span<const GrassWalker> walkers) noexcept { walkers_ = walkers; }
        [[nodiscard]] std::uint32_t WalkerScatterCount() const noexcept { return walkerScatters_; }
        /** @brief The flies this frame: a resting gecko with one hanging low within HuntRange
         *  on ground it accepts stalks it at StalkSpeed and lunges from LungeReach, then
         *  rests and does not hunt again for HuntCooldownSeconds. */
        void SetFlies(std::span<const FlyState> flies) noexcept { flies_ = flies; }
        struct GeckoLunge { float x = 0.0f, z = 0.0f; bool valid = false; };
        [[nodiscard]] const GeckoLunge& LastLunge() const noexcept { return lastLunge_; }
        [[nodiscard]] std::uint32_t LungeCount() const noexcept { return lunges_; }
        static constexpr float HuntRange = 4.5f;
        static constexpr float StalkSpeed = 0.35f;
        static constexpr float LungeReach = 0.45f;
        static constexpr float LungeSpeed = 1.8f;
        static constexpr float LungeSeconds = 0.3f;
        static constexpr float StalkSeconds = 14.0f;
        static constexpr float HuntCooldownSeconds = 20.0f;
        /** @brief Geckos wagging the tail this moment (those stalking). */
        [[nodiscard]] std::size_t WaggingCount() const noexcept;
        static constexpr float TailWagRate = 9.0f;
        /** @brief Eye licks begun (a resting, simulated gecko every LickPeriodSeconds or so,
         *  for LickSeconds) and the geckos mid-lick. */
        [[nodiscard]] std::uint32_t LickCount() const noexcept { return licks_; }
        [[nodiscard]] std::size_t LickingCount() const noexcept;
        static constexpr float LickSeconds = 0.5f, LickPeriodSeconds = 11.0f;

        [[nodiscard]] std::span<const GeckoState> Geckos() const noexcept { return states_; }
        [[nodiscard]] std::span<const GeckoHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullSimulationCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedSimulationCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantSimulationCount_; }

    private:
        struct Agent
        {
            GeckoState state;
            GeckoHome home;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitionCount = 0U;
            float huntCooldown = 0.0f;
            int prey = -1;                 // index into the flies while stalking; -1 for none
            float lickClock = 0.0f;        // time into the eye-lick cycle
            bool licking = false;
        };

        void Initialize(std::size_t index);
        void UpdateAgent(Agent& agent, float deltaSeconds,
                         float observerX, float observerZ, bool observerValid) noexcept;
        [[nodiscard]] bool ValidGround(float x, float z, const GeckoHome& home) const noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<GeckoState, Count> states_{};
        std::array<GeckoHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
        std::span<const GrassWalker> walkers_;
        std::uint32_t walkerScatters_ = 0U;
        std::span<const FlyState> flies_;
        std::uint32_t lunges_ = 0U;
        std::uint32_t licks_ = 0U;
        GeckoLunge lastLunge_;
    };
}
