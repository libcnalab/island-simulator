#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island
{
    class IslandTerrain;

    enum class ReefHeronBehavior : std::uint8_t
    {
        Stalk,    // wading slowly along the shore, the neck half drawn up
        Freeze,   // standing still over the water, the neck craned to watch
        Strike,   // the bill's lunge into the water
        Fly,      // flushed: off along the shore, the neck drawn in, the legs trailing
        Rest      // the night: hunched on one leg, the head sunk on the shoulders
    };

    struct ReefHeronState
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;   // the feet (in flight the body's foot point)
        float headingRadians = 0.0f;
        float neckStretch = 0.5f;   // 0 drawn in, 1 craned forward and up to watch
        float strike = 0.0f;        // 0..1 the lunge
        float stepPhase = 0.0f;     // the wading legs
        float wingBeat = 0.0f;      // the wings' phase in flight
        float flight = 0.0f;        // 0 standing, 1 flying
        float rest = 0.0f;          // 0..1 the night's hunch on one leg
        float waterDepth = 0.0f;    // the water over the feet
        float scale = 1.0f;
        std::uint8_t morph = 0U;    // 0 the dark slate morph, 1 the white
        ReefHeronBehavior behavior = ReefHeronBehavior::Stalk;
    };

    /**
     * @brief Four Pacific reef herons working the lagoon's edge: each wades a stretch of shore
     * in the shallow band, stalks along it, freezes to watch the water and strikes at fish;
     * a walker coming near flushes it off along the shore to land further on; at night it
     * stands hunched on one leg. Renderer-independent.
     */
    class ReefHeronGroup final
    {
    public:
        static constexpr std::size_t Count = 4U;
        static constexpr float MinimumDepth = 0.10f, MaximumDepth = 0.28f;   // the wading band, about knee deep
        static constexpr float StalkSpeed = 0.22f, FlySpeed = 5.5f, CruiseHeight = 3.2f;
        static constexpr float FlushDistance = 13.0f;
        static constexpr float PatrolRadians = 0.28f;   // the stretch of shore each works

        explicit ReefHeronGroup(const IslandTerrain& terrain);

        void SetNightness(float nightness) noexcept;
        /** @brief Something besides the walker that a wading heron will not let come near this
         *  frame: the resident on the shore, the quad, the bear, a boar or a crocodile, each
         *  flushing it from its own distance. */
        struct Disturber { float x = 0.0f, z = 0.0f, distance = 0.0f; };
        static constexpr std::size_t MaximumDisturbers = 24U;
        void SetDisturbers(std::span<const Disturber> disturbers) noexcept;
        /** @brief Where a heron may roost the night: perches on the mangroves and trees by the
         *  shore, the wreck, the rocks, the jetty's post. At dusk each flies to the nearest one
         *  no other heron holds (within RoostRange) and stands hunched there; at dawn it flies
         *  back to its stretch of shore. Without one it rests in the shallows. */
        struct Roost { float x = 0.0f, y = 0.0f, z = 0.0f; };
        static constexpr float RoostRange = 160.0f;
        void SetRoosts(std::span<const Roost> roosts);
        [[nodiscard]] std::size_t RoostedCount() const noexcept;
        void Update(float deltaSeconds, float observerX, float observerY, float observerZ) noexcept;

        [[nodiscard]] std::span<const ReefHeronState> Herons() const noexcept { return states_; }
        [[nodiscard]] std::uint32_t StrikeCount() const noexcept { return strikes_; }
        [[nodiscard]] std::uint32_t FlushCount() const noexcept { return flushes_; }
        /** @brief The last strike: where the bill went into the water, counted once, and how
         *  long ago (its fleck and ring last StrikeSeconds). */
        struct Strike { float x = 0.0f, y = 0.0f, z = 0.0f; std::uint32_t count = 0U; float ageSeconds = 0.0f; bool valid = false; };
        static constexpr float StrikeSeconds = 1.6f;
        [[nodiscard]] const Strike& LastStrike() const noexcept { return lastStrike_; }
        /** @brief QA: holds one heron frozen on the watch for a still capture. */
        void PreviewFreeze(std::size_t index) noexcept;

        /** @brief The point at the given bearing round the island where the water stands
         *  `depth` deep over the bed, searched outward from the shore; false where the shore
         *  drops away too steeply to wade. */
        [[nodiscard]] bool WadingPoint(float angleRadians, float depth, float& x, float& z) const noexcept;

    private:
        struct Agent
        {
            ReefHeronState state;
            float homeAngle = 0.0f, angle = 0.0f;
            float depth = 0.12f;            // the depth this bird likes to wade
            float direction = 1.0f;         // along the shore, +1 or -1
            float behaviorSeconds = 0.0f, behaviorLength = 3.0f;
            float startX = 0.0f, startY = 0.0f, startZ = 0.0f;   // the flight's take-off
            float targetAngle = 0.0f, targetX = 0.0f, targetY = 0.0f, targetZ = 0.0f;
            float flightSeconds = 0.0f, flightLength = 1.0f;
            bool struck = false;
            std::uint32_t random = 1U;
            enum class Flight : std::uint8_t { Flush, ToRoost, Home } flightKind = Flight::Flush;
            std::size_t roost = 0U;   // the roost held (roosts_.size() for none)
            bool roosted = false;
        };

        void Begin(Agent& agent, ReefHeronBehavior behavior, float length) noexcept;
        /** @brief Off along the shore away from (x, z); false where no landing was found. */
        bool Flush(Agent& agent, float fromX, float fromZ) noexcept;
        void FlyTo(Agent& agent, float x, float y, float z) noexcept;
        [[nodiscard]] std::size_t FreeRoostNear(const Agent& agent) const noexcept;
        void UpdateAgent(Agent& agent, float dt, float observerX, float observerZ, bool observerValid) noexcept;
        void PlaceOnShore(Agent& agent) noexcept;
        [[nodiscard]] float Random(Agent& agent) const noexcept;

        const IslandTerrain& terrain_;
        std::vector<Agent> agents_;
        std::vector<ReefHeronState> states_;
        float nightness_ = 0.0f;
        std::vector<Disturber> disturbers_;
        std::vector<Roost> roosts_;
        std::uint32_t strikes_ = 0U, flushes_ = 0U;
        Strike lastStrike_;
        std::size_t frozen_ = Count;   // the previewed heron, Count for none
    };
}
