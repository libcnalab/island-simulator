#pragma once

#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"

#include <array>
#include <vector>
#include <span>

namespace island {
struct FallingFruit;
class IslandTerrain;

struct BearPoint {float x=0,y=0,z=0;bool operator==(const BearPoint&)const=default;};
struct BearLegPose {
    static constexpr float UpperLength=.48f,LowerLength=.46f,PawHeight=.09f;
    BearPoint root{},joint{},paw{},soleNormal{0,1,0};
    bool stance=true,reachLimited=false;
    bool operator==(const BearLegPose&)const=default;
};
struct BearPose {
    BearPoint origin{},right{1,0,0},up{0,1,0},forward{0,0,1};
    float bodyBob=0;
    std::array<BearLegPose,4> legs{}; // left rear, left front, right rear, right front
    [[nodiscard]] BearPoint ToWorld(BearPoint local)const noexcept;
    [[nodiscard]] BearPoint ToLocal(BearPoint world)const noexcept;
    bool operator==(const BearPose&)const=default;
};
enum class BearBehavior {Idle,Walk,Trot,Forage,Rest,Withdraw};
struct BearState {
    float x=0,y=0,z=0,homeX=0,homeZ=0,heading=0,speed=0,phase=0;
    float foragePose=0,breathPhase=0,distanceTravelled=0;
    // 0..1 lying down: a rest two seconds in, undisturbed, settles the bear onto folded legs,
    // body on the ground (the pose carries it); the walker's approach has it up within a second.
    float lieAmount=0;
    // 0..1 the sniff: standing, the bear lifts its head nose-up and sniffs the air for a
    // couple of seconds early in each stand, and holds it up facing the walker in its
    // alert range (the renderer pitches the head by it).
    float sniff=0;
    // 0..1 the head shake: as a sniff ends (the bear not facing the walker) the head swings
    // side to side for a moment, shakePhase carrying the swing.
    float headShake=0,shakePhase=0;
    // 0..1 the ear flick: every few seconds one ear flicks back at a fly and settles, the
    // ears taking turns; earSide (+1 right, -1 left) is the ear.
    float earFlick=0,earSide=1;
    BearBehavior behavior=BearBehavior::Idle;
    BearPose pose{};
    bool operator==(const BearState&)const=default;
};

/** Exactly one peaceful heavy quadruped with world-locked terrain contact. */
class BearMotion final {
public:
    static constexpr float HomeRadius=34.0f,BodyClearance=1.55f,MaximumSpeed=1.65f;
    // Within this range the bear stops and faces the player; closer than 12 m it withdraws obliquely.
    static constexpr float AlertDistance=20.0f;
    BearMotion(const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles);
    void Update(float seconds,BearPoint observer);
    /** @brief Fruit and coconuts lying on the ground this frame; a forage may walk to one. */
    void SetLyingFruit(std::span<const FallingFruit> fruit) noexcept {lyingFruit_=fruit;}
    /** @brief Indices into that span the bear finished during the last update. */
    [[nodiscard]] std::span<const std::size_t> EatenFruit() const noexcept {return eaten_;}
    [[nodiscard]] std::uint32_t EatCount() const noexcept {return eats_;}
    static constexpr float EatSeconds=10.0f,FruitReach=1.1f;
    // Lie-downs begun: a rest LieDelaySeconds in with no walker in the alert range settles the
    // bear onto folded legs and lengthens the rest by LieBonusSeconds; an alert or the next
    // walk has it up at RiseRate.
    [[nodiscard]] std::uint32_t LieCount() const noexcept {return lies_;}
    [[nodiscard]] bool Lying() const noexcept {return lying_;}
    // QA: holds the bear lying at rest for a still capture.
    void PreviewLie();
    static constexpr float LieDelaySeconds=2.0f,LieBonusSeconds=8.0f,LieDownRate=.5f,RiseRate=1.5f,LieDrop=.42f;
    // Sniffs begun: SniffDelaySeconds into a stand (not foraging, lying or walking) the bear
    // lifts its head for SniffSeconds, again every SniffPeriodSeconds of the stand; facing
    // the walker in its alert range it holds the head up throughout.
    [[nodiscard]] std::uint32_t SniffCount() const noexcept {return sniffs_;}
    [[nodiscard]] bool Sniffing() const noexcept {return sniffing_;}
    static constexpr float SniffDelaySeconds=.6f,SniffSeconds=2.2f,SniffPeriodSeconds=9.0f;
    // Head shakes begun (one as each unalerted sniff ends, HeadShakeSeconds long) and
    // whether the head is shaking this moment.
    [[nodiscard]] std::uint32_t HeadShakeCount() const noexcept {return headShakes_;}
    [[nodiscard]] bool HeadShaking() const noexcept {return headShakeSeconds_>0;}
    static constexpr float HeadShakeSeconds=.6f,HeadShakeRate=14.0f;
    // Ear flicks begun (every EarFlickPeriodSeconds, each EarFlickSeconds) and mid-flick now.
    [[nodiscard]] std::uint32_t EarFlickCount() const noexcept {return earFlicks_;}
    [[nodiscard]] bool EarFlicking() const noexcept {return earFlicking_;}
    static constexpr float EarFlickSeconds=.3f,EarFlickPeriodSeconds=5.0f;
    [[nodiscard]] const BearState& State()const noexcept{return state_;}
    [[nodiscard]] bool IsValidPosition(float x,float z)const noexcept;
    [[nodiscard]] const RenderedTerrainSurface& Ground()const noexcept{return ground_;}
private:
    struct Foot {BearPoint position{},start{},target{};float progress=1,duration=.34f;bool swinging=false;};
    [[nodiscard]] bool AcceptsGround(float x,float z)const noexcept;
    [[nodiscard]] BearPose BodyFrame(const BearState& state)const;
    [[nodiscard]] BearPose ContactPose(const BearState& state,const std::array<Foot,4>& feet)const;
    [[nodiscard]] BearPoint NeutralFoot(const BearState& state,std::size_t leg,float ahead=0)const;
    void Step(float seconds,BearPoint observer,bool observerValid);
    void AdvanceFeet(float seconds);
    bool TryMove(float heading,float distance,bool& waitingForStep);
    void StartFoot(std::size_t leg,BearPoint target,float duration);
    const IslandTerrain& terrain_;
    RenderedTerrainSurface ground_;
    WorldCollisionGrid solids_;
    BearState state_;
    std::array<Foot,4> feet_{};
    float remaining_=2.5f,targetHeading_=0,avoidance_=0;
    std::uint32_t decisions_=0;
    std::span<const FallingFruit> lyingFruit_;
    std::vector<std::size_t> eaten_;
    std::uint32_t eats_=0;
    int fruitTarget_=-1;
    float eatSeconds_=0;
    float restSeconds_=0;
    bool lying_=false;
    std::uint32_t lies_=0;
    float sniffClock_=0;
    float headShakeSeconds_=0;
    float earClock_=0;bool earFlicking_=false;std::uint32_t earFlicks_=0;
    std::uint32_t headShakes_=0;
    bool sniffing_=false;
    std::uint32_t sniffs_=0;
    bool ChooseFruit() noexcept;
};
}
