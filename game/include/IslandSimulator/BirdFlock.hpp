#pragma once
#include <algorithm>
#include <cmath>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/SnakePopulation.hpp"
#include "IslandSimulator/GrassWalkers.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    struct FallingFruit;
    class IslandTerrain;

    /** @brief Broad route family used to compose varied island bird traffic. */
    enum class BirdPathFamily : std::uint8_t
    {
        Shore,
        Canopy,
        High
    };

    enum class BirdBehavior : std::uint8_t
    {
        Fly,
        Approach,
        Perched,
        Depart,
        GroundApproach,
        GroundForage,
        GroundDepart
    };

    enum class BirdSpecies : std::uint8_t
    {
        Coastal,
        Parrot,
        FruitDove,
        Seabird,
        /** Great frigatebirds: the last two of the high birds, soaring far above the island on
         *  long crooked wings with deeply forked tails, hardly ever flapping; silent, and never
         *  diving or sitting on the sea (their plumage is not waterproof). */
        Frigatebird,
        /** Brown boobies: the seabird islet's colony (BoobyColony), not the flock's. */
        Booby
    };

    /** @brief Renderer-independent pose for one procedural bird. */
    struct BirdState
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float headingRadians = 0.0f;
        float speed = 0.0f;
        float wingPose = 0.0f;
        float bankRadians = 0.0f;
        float glideAmount = 0.0f;
        float perchAmount = 0.0f;
        /** @brief Pigeon-walk head thrust, 0..1, nonzero only while foraging on the ground. */
        float headBob = 0.0f;
        float scale = 1.0f;
        /** @brief 0..1 while a seabird has left its high orbit to circle low over a fish school. */
        float huntAmount = 0.0f;
        /** @brief 0..1 while a seabird sits on the sea after a hunt, wings folded, drifting downwind. */
        float raftAmount = 0.0f;
        /** @brief 0..1 during a bathing bout: a dove at the pool bank flicking its wings and dipping. */
        float bathAmount = 0.0f;
        /** @brief 0..1 during a feeding bout: a parrot on a fruiting lobe reaching to the fruit. */
        float feedAmount = 0.0f;
        /** @brief 0..1 during a preening bout: a perched bird's head turned back over one
         *  shoulder and down into the wing. */
        float preenAmount = 0.0f;
        /** @brief The wing the head turns to while preening: +1 right, -1 left. */
        float preenSide = 1.0f;
        /** @brief 0..1 during a wing stretch: a perched bird opens one wing out and down
         *  from its fold for a second or two, between its preens. */
        float stretchAmount = 0.0f;
        /** @brief The wing stretched: +1 right, -1 left, alternating stretch by stretch. */
        float stretchSide = 1.0f;
        /** @brief 0..1 the ruffle: a perched bird shakes its feathers out as each preen ends,
         *  the body puffed for a moment and smoothed again. */
        float ruffleAmount = 0.0f;
        /** @brief 0..1 the head scratch: a perched bird brings one foot up past the wing to
         *  its head for a second or so now and then; scratchSide (+1 right, -1 left) is the
         *  foot, the feet taking turns. */
        float scratchAmount = 0.0f;
        float scratchSide = 1.0f;
        BirdPathFamily family = BirdPathFamily::Shore;
        BirdSpecies species = BirdSpecies::Coastal;
        BirdBehavior behavior = BirdBehavior::Fly;
        WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant = 0U;
        std::uint8_t perchSiteIndex = 0U;
    };

    /** @brief Immutable authored orbit and spawn data for one procedural bird. */
    struct BirdRouteState
    {
        float spawnX = 0.0f;
        float spawnY = 0.0f;
        float spawnZ = 0.0f;
        float spawnHeadingRadians = 0.0f;
        float preferredRadius = 0.0f;
        float baseAltitude = 0.0f;
        float minimumClearance = 0.0f;
        float verticalAmplitude = 0.0f;
        int direction = 1;
        BirdPathFamily family = BirdPathFamily::Shore;
    };

    /** @brief Nominal radial and altitude envelope shared by one bird route family. */
    struct BirdRouteRegion
    {
        float minimumRadius = 0.0f;
        float maximumRadius = 0.0f;
        float minimumAltitude = 0.0f;
        float maximumAltitude = 0.0f;
        std::size_t population = 0U;
        BirdPathFamily family = BirdPathFamily::Shore;
    };

    /** @brief The most recent plunge-dive strike point and its age, for the water's foam ring. */
    struct BirdDiveSplash
    {
        float x = 0.0f;
        float z = 0.0f;
        float ageSeconds = 1.0e9f;
        bool valid = false;
    };

    /** @brief The last branch perch a bird settled on; the crown shakes a leaf loose. */
    struct BirdPerchLanding
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        bool valid = false;
    };

    /** @brief Deterministic, bounded and distance-throttled flying-bird simulation. */
    class BirdFlock final
    {
    public:
        static constexpr std::size_t BirdCount = 24U;
        static constexpr float FullSimulationDistance = 190.0f;
        static constexpr float ReducedSimulationDistance = 560.0f;

        explicit BirdFlock(const IslandTerrain& terrain);

        /** @brief Advances autonomous flight and updates observer-relative simulation tiers. */
        void Update(float deltaSeconds, float observerX, float observerY,
                    float observerZ) noexcept;

        /** @brief Installs static anchors after world layouts have been constructed. */
        void SetPerchSites(std::span<const BirdPerchSite> sites);
        /** @brief Seats one bird on a perch as the island loads (after SetPerchSites). */
        void SettlePerched(std::size_t agentIndex, std::size_t siteIndex) noexcept;
        /** @brief The fish school the seabirds may work this moment; invalid clears it. */
        void SetSeaForage(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::size_t HuntingCount() const noexcept;
        [[nodiscard]] const BirdDiveSplash& DiveSplash() const noexcept { return diveSplash_; }
        [[nodiscard]] const BirdPerchLanding& LastLanding() const noexcept { return lastLanding_; }
        [[nodiscard]] std::uint32_t LandingCount() const noexcept { return landings_; }
        [[nodiscard]] std::uint32_t DiveCount() const noexcept { return diveCount_; }
        /** @brief Seabirds sitting on the water after a hunt. */
        [[nodiscard]] std::size_t RaftingCount() const noexcept;
        [[nodiscard]] std::uint32_t RaftCount() const noexcept { return raftCount_; }
        /** @brief A seabird's strike within JoinDistance of another on its high route brings
         *  that one in to hunt at once instead of at its own time: the birds gather over a
         *  school one has found. */
        [[nodiscard]] std::uint32_t HuntJoinCount() const noexcept { return huntJoins_; }
        static constexpr float JoinDistance = 120.0f;
        /** @brief A dolphin's leap: the pod is working a school, and seabirds on their routes
         *  within DolphinJoinDistance come in to hunt the school of the moment at once. */
        void NoteDolphinLeap(float x, float z) noexcept;
        [[nodiscard]] std::uint32_t DolphinJoinCount() const noexcept { return dolphinJoins_; }
        static constexpr float DolphinJoinDistance = 160.0f;
        /** @brief A sudden sound at a point; perched birds near it flush within the second. */
        void NoteDisturbance(float x, float z) noexcept;
        /** @brief How dark it is (0 by day .. 1 at night): from dusk every bird that perches
         *  goes to a perch of its own and stays there until dawn (the doves leave the ground);
         *  a disturbance still flushes them. */
        void SetNightness(float nightness) noexcept { nightness_ = std::isfinite(nightness) ? std::clamp(nightness, 0.0f, 1.0f) : 0.0f; }
        /** @brief The snakes this frame: a snake's head within SnakeFlushDistance of a perched
         *  bird, at its height, flushes it as the walker does. */
        void SetSnakes(std::span<const SnakeState> snakes) noexcept { snakes_ = snakes; }
        [[nodiscard]] std::uint32_t SnakeFlushCount() const noexcept { return snakeFlushes_; }
        /** @brief Perched flushes from the resident or an animal walking up under the perch. */
        [[nodiscard]] std::uint32_t WalkerFlushCount() const noexcept { return walkerFlushes_; }
        /** @brief Mobbing: a bird flushed by a snake does not simply leave; for MobSeconds it
         *  circles low over the snake's head at MobRadius, chattering the alarm every
         *  MobChatterSeconds (LastMob counts the chatters, at the snake), before it takes
         *  its route. */
        [[nodiscard]] std::uint32_t MobCount() const noexcept { return mobs_; }
        [[nodiscard]] std::size_t MobbingCount() const noexcept;
        struct BirdMob { float x = 0.0f, y = 0.0f, z = 0.0f; std::uint32_t count = 0U; bool valid = false; };
        [[nodiscard]] const BirdMob& LastMob() const noexcept { return lastMob_; }
        static constexpr float MobSeconds = 8.0f, MobRadius = 2.4f, MobHeight = 2.2f, MobChatterSeconds = 2.0f;
        /** @brief Preening bouts begun: a perched bird, between feeding bouts and after its
         *  first second on the perch, turns its head back into one wing for PreenBoutSeconds
         *  about every PreenPeriodSeconds, the wings alternating. A departure or flush drops it. */
        [[nodiscard]] std::uint32_t PreenCount() const noexcept { return preens_; }
        [[nodiscard]] std::size_t PreeningCount() const noexcept;
        /** @brief QA: holds one bird mid-preen for a still capture. */
        void PreviewPreen(std::size_t index) noexcept;
        static constexpr float PreenBoutSeconds = 2.6f;
        static constexpr float PreenPeriodSeconds = 11.0f;
        /** @brief Wing stretches begun: a perched bird, not feeding or preening and after
         *  its first second and a half on the perch, opens one wing out and down for
         *  StretchSeconds about every StretchPeriodSeconds, the wings alternating. A
         *  departure or flush drops it. */
        [[nodiscard]] std::uint32_t StretchCount() const noexcept { return stretches_; }
        [[nodiscard]] std::size_t StretchingCount() const noexcept;
        static constexpr float StretchSeconds = 1.8f;
        static constexpr float StretchPeriodSeconds = 17.0f;
        /** @brief Ruffles begun (one as each preen bout ends on the perch, for RuffleSeconds)
         *  and the perched birds mid-ruffle. */
        [[nodiscard]] std::uint32_t RuffleCount() const noexcept { return ruffles_; }
        [[nodiscard]] std::size_t RufflingCount() const noexcept;
        static constexpr float RuffleSeconds = 0.7f;
        /** @brief Water shakes begun: a seabird lifting off its raft shakes the water from
         *  its feathers, a longer ruffle of WaterShakeSeconds as it climbs. */
        [[nodiscard]] std::uint32_t WaterShakeCount() const noexcept { return waterShakes_; }
        static constexpr float WaterShakeSeconds = 1.4f;
        /** @brief Head scratches begun (a perched bird, not feeding, preening or stretching,
         *  about every HeadScratchPeriodSeconds for HeadScratchSeconds) and the birds at it. */
        [[nodiscard]] std::uint32_t HeadScratchCount() const noexcept { return headScratches_; }
        [[nodiscard]] std::size_t HeadScratchingCount() const noexcept;
        static constexpr float HeadScratchSeconds = 1.2f;
        static constexpr float HeadScratchPeriodSeconds = 29.0f;
        static constexpr float SnakeFlushDistance = 3.0f;
        /** @brief Fruit lying on the ground this frame; a fruit dove's ground visit lands
         *  beside the nearest one within FruitLandingRange when there is any. The doves'
         *  routes ring the island, so the range is the island: a dove sees the fruit from
         *  the air and crosses to it. */
        void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept { lyingFruit_ = fruit; }
        [[nodiscard]] std::uint32_t FruitLandingCount() const noexcept { return fruitLandings_; }
        /** @brief The plunge pool's bank: every fourth ground visit of a fruit dove goes there
         *  to bathe, flicking its wings and dipping in bouts, instead of a dry-ground walk. */
        void SetBathSite(float x, float z, bool valid) noexcept;
        [[nodiscard]] std::uint32_t BathCount() const noexcept { return baths_; }
        [[nodiscard]] std::size_t BathingCount() const noexcept;
        static constexpr float BathBoutSeconds = 1.1f;
        static constexpr float BathRestSeconds = 2.2f;
        /** @brief A parrot perched on a fruiting lobe feeds in bouts and stays twice as long. */
        static constexpr float FeedBoutSeconds = 1.6f;
        static constexpr float FeedRestSeconds = 2.6f;
        [[nodiscard]] std::uint32_t FeedCount() const noexcept { return feeds_; }
        [[nodiscard]] std::size_t FeedingCount() const noexcept;
        /** @brief Other bodies on the ground this frame (the herds, the bear, the resident): a
         *  dove foraging on the ground flushes from one within GroundFlushDistance. */
        void SetWalkers(std::span<const GrassWalker> walkers) noexcept { walkers_ = walkers; }
        [[nodiscard]] std::uint32_t GroundFlushCount() const noexcept { return groundFlushes_; }
        static constexpr float GroundFlushDistance = 4.0f;
        static constexpr float FruitLandingRange = 240.0f;
        [[nodiscard]] std::uint32_t FlushCount() const noexcept { return flushCount_; }
        /** @brief The last bird to flush, perched or on the ground, where it was: the wing
         *  clap sounds there. */
        struct BirdFlush { float x = 0.0f, y = 0.0f, z = 0.0f; bool valid = false; };
        [[nodiscard]] const BirdFlush& LastFlush() const noexcept { return lastFlush_; }
        [[nodiscard]] std::uint32_t AnyFlushCount() const noexcept { return flushCount_ + groundFlushes_; }
        // A perched bird leaves when the observer comes this close, or a thud lands this near.
        static constexpr float FlushDistance = 9.0f;
        static constexpr float DisturbanceDistance = 30.0f;
        static constexpr float DisturbanceSeconds = 1.0f;
        static constexpr float DiveIntervalSeconds = 9.0f;
        static constexpr float DiveSeconds = 1.6f;
        static constexpr float SplashSeconds = 2.6f;
        static constexpr float HuntOrbitRadius = 11.0f;
        static constexpr float HuntAltitude = 7.5f;
        static constexpr float HuntSeconds = 42.0f;
        static constexpr float HuntIntervalSeconds = 75.0f;
        static constexpr float RaftSeconds = 24.0f;
        static constexpr float RaftHeight = 0.18f;
        static constexpr float RaftDriftSpeed = 0.3f;

        [[nodiscard]] std::span<const BirdState> Birds() const noexcept;
        [[nodiscard]] std::span<const BirdRouteState> Routes() const noexcept;
        [[nodiscard]] std::span<const BirdRouteRegion> RouteRegions() const noexcept;
        [[nodiscard]] std::span<const BirdPerchSite> PerchSites() const noexcept
        {
            return perchSites_;
        }
        [[nodiscard]] int ApproachingCount() const noexcept { return approachingCount_; }
        [[nodiscard]] int PerchedCount() const noexcept { return perchedCount_; }
        [[nodiscard]] int DepartingCount() const noexcept { return departingCount_; }
        [[nodiscard]] int FullSimulationCount() const noexcept { return fullSimulationCount_; }
        [[nodiscard]] int ReducedSimulationCount() const noexcept {
            return reducedSimulationCount_;
        }
        [[nodiscard]] int DormantSimulationCount() const noexcept {
            return dormantSimulationCount_;
        }

    private:
        float nightness_ = 0.0f;
        struct Agent
        {
            BirdState state;
            BirdRouteState route;
            float verticalPhase = 0.0f;
            float verticalFrequency = 0.0f;
            float wanderPhaseA = 0.0f;
            float wanderPhaseB = 0.0f;
            float wanderFrequencyA = 0.0f;
            float wanderFrequencyB = 0.0f;
            float radialDriftPhase = 0.0f;
            float radialDriftFrequency = 0.0f;
            float radialDriftAmplitude = 0.0f;
            float glidePhase = 0.0f;
            float glideFrequency = 0.0f;
            float flapPhase = 0.0f;
            float flapSpeed = 0.0f;
            float accumulatedSeconds = 0.0f;
            float behaviorSeconds = 0.0f;
            float nextPerchSeconds = 0.0f;
            std::uint32_t perchCycle = 0U;
            float groundTargetX = 0.0f;
            float groundTargetZ = 0.0f;
            float huntSeconds = 0.0f;       // remaining low circling; 0 when on the high route
            float nextHuntSeconds = 0.0f;
            float nextDiveSeconds = 0.0f;   // countdown to the next plunge while circling low
            float diveSeconds = 0.0f;       // remaining plunge; 0 when not diving
            bool struck = false;            // this plunge has already touched the water
            float raftSeconds = 0.0f;       // remaining sit on the sea after a hunt; 0 when flying
            bool bathing = false;           // this ground visit is a bathe at the pool bank
            bool feeding = false;           // in a feeding bout on a fruiting perch
            bool preening = false;          // in a preening bout on the perch
            bool stretching = false;        // in a wing stretch on the perch
            float ruffleSeconds = 0.0f;     // remaining feather ruffle after a preen; 0 when smooth
            bool headScratching = false;    // in a head scratch on the perch
            float mobSeconds = 0.0f;        // remaining circling over a snake; 0 when not mobbing
            float mobX = 0.0f, mobY = 0.0f, mobZ = 0.0f;
            float mobChatter = 0.0f;
            std::size_t index = 0U;
        };

        void InitializeAgent(std::size_t index);
        void BuildRouteRegions() noexcept;
        void UpdateAgent(Agent& agent, float deltaSeconds, bool allowNewPerch) noexcept;
        void UpdateFlight(Agent& agent, float deltaSeconds) noexcept;
        void UpdatePerching(Agent& agent, float deltaSeconds) noexcept;
        void UpdateGroundDove(Agent& agent, float deltaSeconds) noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<BirdState> states_;
        std::vector<BirdRouteState> routes_;
        std::vector<BirdPerchSite> perchSites_;
        std::array<BirdRouteRegion, 3> routeRegions_{};
        float elapsedSeconds_ = 0.0f;
        std::uint64_t frameIndex_ = 0U;
        float forageX_ = 0.0f, forageZ_ = 0.0f;
        bool forageValid_ = false;
        BirdDiveSplash diveSplash_;
        float dolphinLeapX_ = 0.0f, dolphinLeapZ_ = 0.0f, dolphinLeapAge_ = 1.0e9f;
        bool dolphinLeapValid_ = false;
        std::uint32_t dolphinJoins_ = 0U;
        BirdPerchLanding lastLanding_;
        std::uint32_t landings_ = 0U;
        std::uint32_t diveCount_ = 0U;
        std::uint32_t raftCount_ = 0U;
        std::uint32_t huntJoins_ = 0U;
        float bathX_ = 0.0f, bathZ_ = 0.0f;
        bool bathValid_ = false;
        std::uint32_t baths_ = 0U;
        std::uint32_t feeds_ = 0U;
        float disturbanceX_ = 0.0f, disturbanceZ_ = 0.0f, disturbanceSeconds_ = 0.0f;
        std::uint32_t flushCount_ = 0U;
        BirdFlush lastFlush_;
        std::span<const SnakeState> snakes_;
        std::uint32_t snakeFlushes_ = 0U;
        std::uint32_t walkerFlushes_ = 0U;
        std::uint32_t preens_ = 0U;
        std::uint32_t stretches_ = 0U;
        std::uint32_t ruffles_ = 0U;
        std::uint32_t waterShakes_ = 0U;
        std::uint32_t headScratches_ = 0U;
        std::uint32_t mobs_ = 0U;
        BirdMob lastMob_;
        std::span<const FallingFruit> lyingFruit_;
        std::uint32_t fruitLandings_ = 0U;
        std::span<const GrassWalker> walkers_;
        std::uint32_t groundFlushes_ = 0U;
        [[nodiscard]] bool FindFruitLanding(const Agent& agent, float& x, float& z) const noexcept;
        int fullSimulationCount_ = 0;
        int reducedSimulationCount_ = 0;
        int dormantSimulationCount_ = 0;
        int approachingCount_ = 0;
        int perchedCount_ = 0;
        int departingCount_ = 0;
    };
}
