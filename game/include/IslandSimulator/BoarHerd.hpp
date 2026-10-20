#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/WorldCollision.hpp"

namespace island
{
    class IslandTerrain;
    struct FallingFruit;

    enum class BoarBehavior : std::uint8_t { Rest, Forage, Wander, Retreat, Wallow };

    /** @brief Ground-relative pose; yaw zero faces positive X, matching other wildlife. */
    struct BoarState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float homeX = 0.0f, homeZ = 0.0f;
        float headingRadians = 0.0f;
        float speed = 0.0f;
        float gaitPhase = 0.0f;
        float rootingPose = 0.0f;
        /** @brief 0..1 wet mud on the legs and belly from the last wallow; dries off slowly. */
        float mudAmount = 0.0f;
        /** @brief True while the boar stands on the stream's drainage ground for its wallow trip. */
        bool inChannel = false;
        float scale = 1.0f;
        BoarBehavior behavior = BoarBehavior::Rest;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0;
    };

    /** @brief Small deterministic, non-hostile inland population. No renderer dependencies. */
    class BoarHerd final
    {
    public:
        static constexpr std::size_t BoarCount = 6;
        static constexpr float HomeRadius = 27.0f;
        // Conservatively includes the adult snout and terrain-tilted body, not just its centre.
        static constexpr float CollisionRadius = 1.30f;
        static constexpr float MinimumShoreDistance = 38.0f;
        static constexpr float MaximumSlope = 0.48f;
        static constexpr float MinimumHabitatMoisture = 0.16f;
        static constexpr float MaximumHabitatDrainage = 0.74f;
        static constexpr float MaximumHabitatSaltExposure = 0.12f;
        // Within this range a boar stops and faces the player; closer than 6 m it trots off obliquely.
        static constexpr float AlertDistance = 12.0f;
        static constexpr float FullSimulationDistance = 95.0f;
        static constexpr float ReducedSimulationDistance = 220.0f;

        explicit BoarHerd(const IslandTerrain& terrain,
                          std::span<const CircularObstacle> obstacles = {});
        /** @brief Rebuilds valid homes transactionally; refuses impossible habitat. */
        void SetObstacles(std::span<const CircularObstacle> obstacles);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;
        [[nodiscard]] std::span<const BoarState> Boars() const noexcept { return states_; }
        [[nodiscard]] bool IsValidGround(float x, float z, bool allowDrainage = false) const noexcept;
        /** @brief Boars whose range reaches a stream bank; they walk there to wallow. */
        [[nodiscard]] std::size_t WallowerCount() const noexcept;
        /** @brief The bank points the boars wallow at (distinct within 1.5 m) and the
         *  direction each faces into the water: the churned mud shows there. */
        struct WallowSite { float x = 0.0f, z = 0.0f, faceX = 1.0f, faceZ = 0.0f; };
        [[nodiscard]] std::vector<WallowSite> WallowSites() const;
        /** @brief Fruit and coconuts lying on the ground this frame; a forage may go to one. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        /** @brief Indices into that span the boars ate during the last update. */
        [[nodiscard]] std::span<const std::size_t> EatenFruit() const noexcept { return eatenFruit_; }
        [[nodiscard]] std::uint32_t EatCount() const noexcept { return eatCount_; }
        static constexpr float WallowSeconds = 14.0f;
        static constexpr float MudDrySeconds = 150.0f;

    private:
        struct Agent
        {
            BoarState state;
            float remainingSeconds = 0.0f;
            float pendingSeconds = 0.0f;
            float targetHeading = 0.0f;
            std::uint32_t decisions = 0;
            // Standing point beside the trunk base the boar forages toward; valid when hasRoot.
            float rootX = 0.0f, rootZ = 0.0f, trunkX = 0.0f, trunkZ = 0.0f;
            bool hasRoot = false;
            int fruitTarget = -1;           // the lying fruit this forage goes to; -1 for a trunk
            // The nearest reachable stream bank and the water it faces; valid when hasWallow.
            float wallowX = 0.0f, wallowZ = 0.0f, faceX = 0.0f, faceZ = 0.0f;
            bool hasWallow = false;
            bool inCorridor = false;        // still on drainage ground after the wallow ended
        };
        void ChooseRootingSite(Agent& agent) noexcept;
        bool ChooseFruitTarget(Agent& agent) noexcept;
        void FindWallow(Agent& agent) noexcept;
        void Initialize(std::size_t index);
        void Step(Agent& agent, float dt, float observerX, float observerZ,
                  bool observerValid) noexcept;

        const IslandTerrain& terrain_;
        std::vector<CircularObstacle> obstacles_;
        std::array<Agent, BoarCount> agents_{};
        std::array<BoarState, BoarCount> states_{};
        std::uint64_t frame_ = 0;
        std::span<const FallingFruit> lyingFruit_;
        std::vector<std::size_t> eatenFruit_;
        std::uint32_t eatCount_ = 0;
    };
}
