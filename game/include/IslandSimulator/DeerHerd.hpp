#pragma once

#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class CanopyLightField;
    enum class DeerBehavior : std::uint8_t { Graze, Walk, Rest, Retreat, Drink };

    struct DeerState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float headingRadians = 0.0f, speed = 0.0f;
        float gaitPhase = 0.0f, grazeAmount = 0.0f, scale = 1.0f;
        /** @brief 0..1 the tail raised: flagged up at a bolt, half up in the flick a grazing
         *  or resting deer gives every few seconds, hanging otherwise. */
        float tailRaise = 0.0f;
        /** @brief 0..1 the ear twitch: every few seconds a deer not bolting swings one ear
         *  back and out at a fly and lets it settle, the other ear still; earSide (-1 or 1)
         *  is the ear, alternating twitch by twitch. */
        float earTwitch = 0.0f;
        float earSide = 1.0f;
        /** @brief 0..1 the stamp: a deer standing alert at the walker lifts its left forefoot
         *  and stamps it down now and then, a warning; nothing while grazing or bedded. */
        float stamp = 0.0f;
        /** @brief -1..1 the chew: a deer at rest, undisturbed (standing or bedded), chews its
         *  cud, the muzzle working side to side; 0 on the move or alert. */
        float chew = 0.0f;
        /** @brief 0..1 the head toss: coming up from a graze the deer throws its head up and
         *  forward for a moment and settles it. */
        float headToss = 0.0f;
        /** @brief 0..1 bedded down: the body lowered onto folded legs for a rest taken in
         *  the shade, rising over a second and a half; back on its feet within half a
         *  second of any other behaviour (an alarm, a bolt, the next graze). */
        float bedAmount = 0.0f;
        DeerBehavior behavior = DeerBehavior::Graze;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t coatVariant = 0U;
        bool antlered = false;
    };

    struct DeerHome { float x = 0.0f, z = 0.0f, radius = 0.0f; };

    /** Four peaceful feral deer using open inland clearings and bounded retreat. */
    class DeerHerd final
    {
    public:
        static constexpr std::size_t Count = 4U;
        // Within this range a deer stands alert facing the player; closer than 9 m it bounds off obliquely.
        static constexpr float AlertDistance = 18.0f;
        // The antlered sentinel watches further out; its clearing partner lifts its head within
        // the wider range once the sentinel has stood alert for the reaction delay.
        static constexpr float SentinelAlertDistance = 24.0f;
        static constexpr float HerdReactionSeconds = .7f;
        // A sudden sound (a coconut's thud) within this range makes a deer stand and face it.
        static constexpr float StartleDistance = 30.0f;
        // No deer comes closer than this to the campfire, grazing or fleeing.
        static constexpr float FireClearance = 7.0f;
        // The tail flick's bout and the period between flicks (plus a little per deer).
        static constexpr float FlickSeconds = 0.45f;
        static constexpr float FlickPeriodSeconds = 4.0f;
        // The ear twitch's bout and the period between twitches (plus a little per deer).
        static constexpr float EarTwitchSeconds = 0.35f;
        static constexpr float EarTwitchPeriodSeconds = 3.1f;
        // The alert stamp's bout and the period between stamps while the deer stands alert.
        static constexpr float StampSeconds = 0.4f;
        static constexpr float StampPeriodSeconds = 3.5f;
        /** @brief Deer chewing the cud this moment (at rest, undisturbed, the chew past half). */
        [[nodiscard]] std::size_t ChewingCount() const noexcept;
        static constexpr float ChewRate = 6.0f;
        /** @brief Head tosses begun (one as each graze ends) and the deer mid-toss. */
        [[nodiscard]] std::uint32_t TossCount() const noexcept { return tosses_; }
        [[nodiscard]] std::size_t TossingCount() const noexcept;
        static constexpr float TossSeconds = 0.4f;
        /** @brief Stamps begun, and the deer mid-stamp. */
        [[nodiscard]] std::uint32_t StampCount() const noexcept { return stamps_; }
        [[nodiscard]] std::size_t StampingCount() const noexcept;
        static constexpr float StartleSeconds = 6.0f;
        // A clearing pair drifting further apart than this walks back together.
        static constexpr float CohesionDistance = 9.0f;
        static constexpr float FullSimulationDistance = 90.0f;
        static constexpr float ReducedSimulationDistance = 190.0f;

        DeerHerd(const IslandTerrain& terrain, std::span<const CircularObstacle> obstacles);
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        [[nodiscard]] std::span<const DeerState> Deer() const noexcept { return states_; }
        [[nodiscard]] std::span<const DeerHome> Homes() const noexcept { return homes_; }
        [[nodiscard]] bool IsValidPosition(std::size_t index, float x, float z) const noexcept;
        /** @brief Deer whose home reaches a stream bank; they walk there to drink. */
        [[nodiscard]] std::size_t DrinkerCount() const noexcept;
        [[nodiscard]] bool DrinkPoint(std::size_t index, float& x, float& z) const noexcept;
        [[nodiscard]] bool SentinelAlert() const noexcept { return sentinelAlertSeconds_ > 0.0f; }
        /** @brief A sudden sound at a point; nearby deer stand alert facing it for a while. */
        void NoteDisturbance(float x, float z) noexcept;
        [[nodiscard]] bool Startled() const noexcept { return disturbanceSeconds_ > 0.0f; }
        [[nodiscard]] float SentinelAlertSeconds() const noexcept { return sentinelAlertSeconds_; }
        /** @brief The light over the ground (crowns, palm heads and the solid casters' shade):
         *  a deer whose rest comes on sunlit ground first walks to the shadiest valid spot
         *  within ShadeSeekRadius and lies up there. Null keeps the deer resting where it is. */
        void SetLight(const CanopyLightField* light) noexcept { light_ = light; }
        [[nodiscard]] std::uint32_t ShadeRestCount() const noexcept { return shadeRests_; }
        /** @brief Bolts begun from the walker, and the deer that last began one: its
         *  hoofbeats drum where it goes. */
        [[nodiscard]] std::uint32_t BoltCount() const noexcept { return bolts_; }
        [[nodiscard]] std::size_t LastBoltIndex() const noexcept { return lastBolt_; }
        /** @brief Ear twitches begun, and the deer mid-twitch. */
        [[nodiscard]] std::uint32_t EarTwitchCount() const noexcept { return earTwitches_; }
        [[nodiscard]] std::size_t TwitchingCount() const noexcept;
        /** @brief The deer is lying up (a shade rest in progress). */
        [[nodiscard]] bool Bedded(std::size_t index) const noexcept
        {
            return index < Count && agents_[index].bedded;
        }
        static constexpr float BedDownRate = 0.6f, RiseRate = 2.5f;
        /** @brief QA: sets one deer fully bedded down at rest for a still capture. */
        void PreviewBed(std::size_t index) noexcept;
        [[nodiscard]] bool SeekingShade(std::size_t index) const noexcept
        {
            return index < Count && agents_[index].seekingShade;
        }
        static constexpr float ShadeSeekRadius = 10.0f;
        static constexpr float SunlitLevel = 0.72f;
        static constexpr float ShadedLevel = 0.55f;

    private:
        struct Agent
        {
            DeerState state;
            DeerHome home;
            float targetHeadingRadians = 0.0f;
            float behaviorSeconds = 0.0f;
            float accumulatedSeconds = 0.0f;
            std::uint32_t transitions = 0U;
            // Nearest reachable stream bank and the water it faces; valid when hasDrink.
            float drinkX = 0.0f, drinkZ = 0.0f, faceX = 0.0f, faceZ = 0.0f;
            bool hasDrink = false;
            float flickClock = 0.0f;
            float earClock = 1.0f;
            bool twitching = false;
            float stampClock = 0.0f;
            bool stamping = false;
            float cud = 0.0f, chewPhase = 0.0f;   // 0..1 chewing the cud, and the jaw's phase
            float tossSeconds = 0.0f;             // remaining head toss as a graze ends
            bool wasGrazing = false;
            // The shade a sunlit rest walks to; valid while seekingShade.
            float shadeX = 0.0f, shadeZ = 0.0f;
            bool seekingShade = false;
            bool bedded = false;
        };
        [[nodiscard]] float LightAt(float x, float z) const noexcept;
        [[nodiscard]] bool FindShade(const Agent& agent, std::size_t index, float& x, float& z) const noexcept;
        void FindDrinkingPoint(std::size_t index) noexcept;

        void Initialize(std::size_t index);
        void UpdateAgent(Agent&, float, float, float, bool) noexcept;

        const IslandTerrain& terrain_;
        WorldCollisionGrid obstacles_;
        std::array<Agent, Count> agents_{};
        std::array<DeerState, Count> states_{};
        std::array<DeerHome, Count> homes_{};
        std::uint64_t frameIndex_ = 0U;
        float sentinelAlertSeconds_ = 0.0f;
        float disturbanceX_ = 0.0f, disturbanceZ_ = 0.0f, disturbanceSeconds_ = 0.0f;
        const CanopyLightField* light_ = nullptr;
        std::uint32_t shadeRests_ = 0U;
        std::uint32_t bolts_ = 0U;
        std::size_t lastBolt_ = 0U;
        std::uint32_t earTwitches_ = 0U;
        std::uint32_t stamps_ = 0U;
        std::uint32_t tosses_ = 0U;
    };
}
