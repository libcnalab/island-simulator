#pragma once
#include <algorithm>
#include <cmath>

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
        /** @brief -1..1 the snout's sideways work: rooting with the snout down, the disc and
         *  nostrils swing side to side; still with the head up. */
        float snoutSway = 0.0f;
        /** @brief 0..1 wet mud on the legs and belly from the last wallow; dries off slowly. */
        float mudAmount = 0.0f;
        /** @brief 0..1 lying down: a rest a second and a half in settles the boar onto folded
         *  legs, body on the ground; it is on its feet again within half a second of an alarm. */
        float lieAmount = 0.0f;
        /** @brief 0..1 the shake: a boar stepping out of its wallow stands and shakes its
         *  body side to side for a second and a bit, the mud flying; shakePhase carries
         *  the side-to-side beat. */
        float shake = 0.0f;
        float shakePhase = 0.0f;
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
        /** @brief How alarming the observer is: 1 a walker; more on a roaring quad, so every alert
         *  and flight distance stretches by it (clamped 1..4). */
        void SetObserverThreat(float scale) noexcept { observerThreat_ = std::isfinite(scale) ? std::clamp(scale, 1.0f, 4.0f) : 1.0f; }
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
        /** @brief The bear's position this frame: a boar treats it as it treats the walker,
         *  standing off at the alert range and trotting away when it comes close. */
        void SetBear(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::uint32_t BearRetreatCount() const noexcept { return bearRetreats_; }
        /** @brief The boar the bear last sent off (valid once BearRetreatCount is nonzero). */
        [[nodiscard]] std::size_t LastBearRetreatIndex() const noexcept { return lastBearRetreat_; }
        /** @brief Trots off begun, from the walker or the bear, and the boar that last began one:
         *  its hoofbeats drum where it goes. */
        [[nodiscard]] std::uint32_t RetreatCount() const noexcept { return retreats_; }
        [[nodiscard]] std::size_t LastRetreatIndex() const noexcept { return lastRetreat_; }
        /** @brief Fruit and coconuts lying on the ground this frame; a forage may go to one. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        /** @brief Indices into that span the boars ate during the last update. */
        [[nodiscard]] std::span<const std::size_t> EatenFruit() const noexcept { return eatenFruit_; }
        [[nodiscard]] std::uint32_t EatCount() const noexcept { return eatCount_; }
        static constexpr float WallowSeconds = 14.0f;
        static constexpr float MudDrySeconds = 150.0f;
        /** @brief Lie-downs begun: a rest LieDelaySeconds in, undisturbed, settles the boar
         *  onto folded legs and lengthens the rest by LieBonusSeconds; an alert at the walker
         *  or the bear, or the next forage, has it up at RiseRate. */
        [[nodiscard]] std::uint32_t LieCount() const noexcept { return lies_; }
        [[nodiscard]] std::size_t LyingCount() const noexcept;
        [[nodiscard]] bool Lying(std::size_t index) const noexcept
        {
            return index < agents_.size() && agents_[index].lying;
        }
        /** @brief QA: holds one boar lying at rest for a still capture. */
        void PreviewLie(std::size_t index) noexcept;
        static constexpr float LieDelaySeconds = 1.5f, LieBonusSeconds = 8.0f;
        static constexpr float LieDownRate = 0.7f, RiseRate = 2.0f;
        /** @brief Shakes begun: a boar leaving the mud with its coat wet stands and shakes for
         *  ShakeSeconds; the last is noted where the body is for the mud it flings. */
        struct Shake { float x = 0.0f, y = 0.0f, z = 0.0f, headingRadians = 0.0f, scale = 1.0f; std::uint32_t count = 0U; };
        [[nodiscard]] const Shake& LastShake() const noexcept { return shake_; }
        [[nodiscard]] std::uint32_t ShakeCount() const noexcept { return shake_.count; }
        [[nodiscard]] std::size_t ShakingCount() const noexcept;
        static constexpr float ShakeSeconds = 1.3f, ShakeBeatRate = 17.0f;
        /** @brief Boars working their snouts this moment (rooting, the snout down). */
        [[nodiscard]] std::size_t SnoutingCount() const noexcept;
        static constexpr float SnoutSwayRate = 5.0f;

    private:
        float observerThreat_ = 1.0f;
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
            float restSeconds = 0.0f;       // time into the current undisturbed rest
            bool lying = false;
            bool wasWallowing = false;
            float snoutPhase = 0.0f;        // the snout's side-to-side work while rooting
            float shakeSeconds = 0.0f;      // remainder of the shake on leaving the wallow
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
        float bearX_ = 0.0f, bearZ_ = 0.0f;
        bool bearValid_ = false;
        std::uint32_t bearRetreats_ = 0;
        std::size_t lastBearRetreat_ = 0;
        std::uint32_t lies_ = 0;
        Shake shake_;
        std::uint32_t retreats_ = 0;
        std::size_t lastRetreat_ = 0;
    };
}
