#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island
{
    class IslandTerrain;

    enum class CoconutCrabBehavior : std::uint8_t { Rest, Forage, Retreat, Climb };

    struct CoconutCrabState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float legPhase = 0.0f, clawPose = 0.0f, scale = 1.0f;
        bool working = false;   // claws on a lying coconut
        /** @brief 0..1 on a palm trunk: the body turned up the trunk, its underside to the
         *  bark; trunkX/Z is the trunk's axis the renderer faces the body away from. */
        float climbAmount = 0.0f;
        float trunkX = 0.0f, trunkZ = 0.0f;
        CoconutCrabBehavior behavior = CoconutCrabBehavior::Rest;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
    };

    struct FallingFruit;
    struct PalmPlacement;

    struct CoconutCrabHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Six large terrestrial coconut crabs in coastal palm-zone habitats. */
    class CoconutCrabGroup final
    {
    public:
        static constexpr std::size_t Count = 6U;
        static constexpr float MinimumShoreDistance = 22.0f;
        static constexpr float MaximumShoreDistance = 58.0f;
        static constexpr float FullSimulationDistance = 78.0f;
        static constexpr float ReducedSimulationDistance = 185.0f;

        explicit CoconutCrabGroup(const IslandTerrain& terrain);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        [[nodiscard]] std::span<const CoconutCrabState> Crabs() const noexcept {
            return states_;
        }
        [[nodiscard]] std::span<const CoconutCrabHome> Homes() const noexcept {
            return homes_;
        }
        /** @brief Coconuts lying on the ground this frame; a crab's forage may go to one and
         *  work it with its claws until it is gone. */
        void SetLyingCoconuts(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        /** @brief Indices into that span the crabs finished during the last update. */
        [[nodiscard]] std::span<const std::size_t> EatenCoconuts() const noexcept { return eaten_; }
        [[nodiscard]] std::uint32_t WorkCount() const noexcept { return works_; }
        static constexpr float WorkSeconds = 18.0f;
        /** @brief The palms: every fourth forage of a crab with a living trunk in its range is
         *  a climb: the walk to the trunk, up it at ClimbSpeed to ClimbHeight, a pause, and
         *  down again. A climbing crab is on the trunk, not the ground. */
        void SetPalms(std::span<const PalmPlacement> palms) noexcept { palms_ = palms; }
        [[nodiscard]] std::uint32_t ClimbCount() const noexcept { return climbs_; }
        [[nodiscard]] std::size_t ClimbingCount() const noexcept;
        static constexpr float ClimbHeight = 2.8f;
        static constexpr float ClimbSpeed = 0.3f;
        static constexpr float DescendSpeed = 0.35f;
        static constexpr float ClimbPauseSeconds = 4.0f;
        static constexpr float TrunkStand = 0.42f;
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept { return reducedCount_; }
        [[nodiscard]] int DormantSimulationCount() const noexcept { return dormantCount_; }

    private:
        struct Agent
        {
            CoconutCrabState state;
            CoconutCrabHome home;
            float behaviorSeconds = 0.0f;
            float steeringPhase = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            int nutTarget = -1;
            float workSeconds = 0.0f;
            int trunkTarget = -1;        // index into the palms while walking to one
            float climbY = 0.0f;         // height up the trunk
            float climbPause = 0.0f;     // remaining pause at the top
            bool descending = false;
        };
        bool ChooseNut(Agent& agent) noexcept;
        bool ChooseTrunk(Agent& agent) noexcept;
        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;
        [[nodiscard]] bool IsValid(float x, float z,
                                   const CoconutCrabHome& home) const noexcept;

        const IslandTerrain& terrain_;
        std::array<Agent, Count> agents_{};
        std::array<CoconutCrabState, Count> states_{};
        std::array<CoconutCrabHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        std::span<const FallingFruit> lyingFruit_;
        std::vector<std::size_t> eaten_;
        std::uint32_t works_ = 0U;
        std::span<const PalmPlacement> palms_;
        std::uint32_t climbs_ = 0U;
        int fullCount_ = 0, reducedCount_ = 0, dormantCount_ = 0;
    };
}
