#pragma once
#include "IslandSimulator/GoatHabitat.hpp"
#include "IslandSimulator/GoatPose.hpp"

namespace island {
enum class GoatBehavior { Graze,Walk,Rest,Retreat,Drink,Spar };
struct GoatState {
    float x=0,y=0,z=0,heading=0,speed=0,phase=0,grazingPose=0;
    float scale=1;
    // 0..1 lying down: a rest two seconds in, undisturbed, settles the goat onto folded legs,
    // body on the ground (the pose carries it); the walker's approach has it up at once.
    float lieAmount=0;
    // -1..1 the short tail swung to one side: every few seconds a goat flicks it side to
    // side, two beats in well under a second, still between flicks.
    float tailSwing=0;
    // The ear flick: magnitude 0..1 one ear flicked back at a fly and settling, its sign the
    // ear (+ right, - left); the ears take turns flick by flick.
    float earFlick=0;
    GoatBehavior behavior=GoatBehavior::Graze;
    GoatGroundPose pose{}; // Authoritative planted-foot pose shared with rendering.
    bool operator==(const GoatState&) const = default;
};
// Deterministic peaceful herd; simulation remains independent of the renderer.
class GoatHerd final {
public:
    static constexpr float HomeRadius=14,MaximumSpeed=1.2f;
    // Within this range a goat stops and faces the player; closer than 7 m it sidesteps away.
    static constexpr float AlertDistance=14.0f;
    // The sentinel (the first goat) watches further out; once it has stood alert for the
    // reaction delay the rest of the herd lifts their heads within the wider range too.
    static constexpr float SentinelAlertDistance=20.0f;
    static constexpr float HerdReactionSeconds=.7f;
    // A sudden sound (a coconut's thud) within this range makes a goat stand and face it.
    static constexpr float StartleDistance=30.0f,StartleSeconds=6.0f;
    // A sparring bout: two goats within SparRange walk together, stand nose to nose inside
    // SparReach and take turns lowering their heads to shove, for SparSeconds.
    static constexpr float SparSeconds=16.0f,SparReach=1.6f,SparContact=1.3f,SparRange=12.0f,SparTurnSeconds=1.5f;
    GoatHerd(const IslandTerrain& terrain,const JungleLayout& jungle,
        std::span<const CircularObstacle> obstacles);
    void Update(float dt,GoatPoint observer);
    [[nodiscard]] std::span<const GoatState> Goats() const noexcept { return states_; }
    [[nodiscard]] bool IsValidPosition(std::size_t index,float x,float z) const;
    // Goats whose range reaches a stream bank; they walk there to drink.
    [[nodiscard]] std::size_t DrinkerCount() const noexcept;
    [[nodiscard]] bool SentinelAlert() const noexcept {return sentinelAlertSeconds_>0;}
    // A sudden sound at a point; nearby goats stand alert facing it for a while.
    void NoteDisturbance(float x,float z) noexcept;
    [[nodiscard]] bool Startled() const noexcept {return disturbanceSeconds_>0;}
    [[nodiscard]] float SentinelAlertSeconds() const noexcept {return sentinelAlertSeconds_;}
    // Goats standing nose to nose with their sparring partner this moment.
    [[nodiscard]] std::size_t SparringCount() const noexcept;
    [[nodiscard]] std::uint32_t SparCount() const noexcept {return spars_;}
    [[nodiscard]] int SparPartner(std::size_t index) const noexcept {return index<GoatHabitat::Count?intents_[index].partner:-1;}
    // Lie-downs begun: a rest LieDelaySeconds in, with no walker, startle or retreat, settles
    // the goat onto folded legs and lengthens the rest by LieBonusSeconds; any disturbance
    // or the next walk has it up at RiseRate.
    [[nodiscard]] std::uint32_t LieCount() const noexcept {return lies_;}
    [[nodiscard]] std::size_t LyingCount() const noexcept;
    [[nodiscard]] bool Lying(std::size_t index) const noexcept {return index<GoatHabitat::Count&&intents_[index].lying;}
    // QA: holds one goat lying at rest for a still capture.
    void PreviewLie(std::size_t index);
    static constexpr float LieDelaySeconds=2.0f,LieBonusSeconds=8.0f,LieDownRate=.7f,RiseRate=2.0f;
    // Tail flicks begun, and the goats mid-flick: every WagPeriodSeconds (a little per goat)
    // the tail swings side to side WagBeats times over WagSeconds.
    [[nodiscard]] std::uint32_t WagCount() const noexcept {return wags_;}
    [[nodiscard]] std::size_t WaggingCount() const noexcept;
    static constexpr float WagPeriodSeconds=6.5f,WagSeconds=0.7f,WagBeats=2.0f;
    // Ear flicks begun (every EarFlickPeriodSeconds or so per goat, each EarFlickSeconds
    // long) and the goats mid-flick.
    [[nodiscard]] std::uint32_t EarFlickCount() const noexcept {return earFlicks_;}
    [[nodiscard]] std::size_t EarFlickingCount() const noexcept;
    static constexpr float EarFlickPeriodSeconds=4.4f,EarFlickSeconds=0.3f;
private:
    void FindDrinkingPoint(std::size_t index) noexcept;
    void Step(std::size_t index,float dt,GoatPoint observer,bool observerValid);
    bool TryMove(std::size_t index,float heading,float distance,bool& waitingForStep);
    struct Foot { GoatPoint position{},start{},target{};float progress=0;bool swinging=false; };
    using Feet=std::array<Foot,4>;
    [[nodiscard]] GoatGroundPose ContactPose(const GoatState& state,const Feet& feet) const;
    void AdvanceFeet(std::size_t index,float dt);
    const IslandTerrain& terrain_;
    RenderedTerrainSurface surface_;
    WorldCollisionGrid clearance_;
    std::array<GoatHome,GoatHabitat::Count> homes_{};
    std::array<GoatState,GoatHabitat::Count> states_{};
    std::array<Feet,GoatHabitat::Count> feet_{};
    struct Intent {
        float remaining=0,targetHeading=0,avoidance=0; std::uint32_t decisions=0;
        float drinkX=0,drinkZ=0,faceX=0,faceZ=0; bool hasDrink=false;
        int partner=-1; float sparClock=0;
        float restSeconds=0; bool lying=false;   // time into the undisturbed rest, and down
        float wagClock=0; bool wagging=false,wagSeeded=false; // time into the tail-flick cycle, mid-flick, and seeded
        float earClock=0,earSide=1; bool earFlicking=false;   // time into the ear-flick cycle, the ear, and mid-flick
    };
    std::array<Intent,GoatHabitat::Count> intents_{};
    float sentinelAlertSeconds_=0;
    float disturbanceX_=0,disturbanceZ_=0,disturbanceSeconds_=0;
    std::uint32_t spars_=0;
    std::uint32_t lies_=0;
    std::uint32_t wags_=0;
    std::uint32_t earFlicks_=0;
};
}
