#pragma once
#include "IslandSimulator/FootprintTrail.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

struct ElephantPoint {float x=0,y=0,z=0;};

/** One leg's joints in the world: the shoulder (or hip), the elbow (or knee), the wrist (or
 *  hock), and the centre of the foot's sole on the ground; whether the foot is planted. */
struct ElephantLeg {
    ElephantPoint top,middle,low,foot;
    bool planted=true;
};

enum class ElephantBehavior : std::uint8_t {Walk,Stand,Graze,Drink,Dust,Watch,Trumpet,Doze};

/** The elephant's whole pose this moment, everything the renderer needs. */
struct ElephantState {
    float x=0,y=0,z=0;           // the body's ground point (y the ground under it)
    float heading=0;             // the way it faces (radians, atan2(z, x) of forward)
    float speed=0;
    float bodyHeight=2.1f;       // the line of the shoulder and hip joints over the ground
    float pitch=0;               // nose down positive
    float roll=0;                // the sway onto the planted side
    float headYaw=0,headPitch=0; // the head turned and raised
    float earSpread=0;           // 0 ears laid back on the neck .. 1 spread wide
    float trunkLift=0;           // 0 hanging .. 1 raised high (the trumpet)
    float trunkCurl=0;           // 0 .. 1 the tip curled back to the mouth
    float trunkReach=0;          // 0 .. 1 reaching down and forward to the ground
    float trunkSwing=0;          // its sideways swing, radians
    float tailSwing=0;
    float breath=0;
    float doze=0;                // 0..1 dozing on its feet, the head and trunk low
    ElephantBehavior behavior=ElephantBehavior::Stand;
    std::array<ElephantLeg,4> legs{};   // left fore, right fore, left hind, right hind
};

/** A bull Asian elephant (Elephas maximus) roaming the island alone, as the elephants of the
 *  Andaman Islands do: 2.9 m at the shoulder and the back arched highest behind it, some 5.5 m
 *  from the trunk's root to the tail, about 4.5 t, a tusker. It walks its own round of the
 *  island's coastal flats behind the beach at a slow 1.1 m/s in the lateral walk of its kind
 *  (left hind, left fore, right hind, right fore), each foot laying a round print; it stops to
 *  pull and eat grass, to dust itself, to drink where the stream comes out, and fans its ears
 *  in the heat; it watches the walker who comes near, its trunk raised to take the scent, trumpets
 *  at a walker who comes too close (and at the tyrannosaur), and dozes on its feet at night.
 *  Deterministic in the time it is given. */
class ElephantMotion final {
public:
    static constexpr float ShoulderHeight=2.9f,Length=5.5f,WalkSpeed=1.1f;
    static constexpr float StrideLength=2.1f;   // one foot's step forward and back again
    static constexpr float BodyHeight=2.1f;      // the joints' line over the ground
    static constexpr float UpperArmLength=1.0f,ForearmLength=.9f,WristHeight=.42f;
    static constexpr float FemurLength=1.02f,TibiaLength=.8f,HockHeight=.4f;
    static constexpr float ShoulderAhead=1.3f,HipBehind=1.3f,TrunkLength=1.95f;
    static constexpr float WatchDistance=32.0f,TrumpetDistance=11.0f;
    static constexpr std::size_t PrintCapacity=40;

    ElephantMotion(const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles);
    void Update(float seconds) {Update(seconds,0.0f,0.0f,false,0.0f,0.0f,0.0f,false);}
    /** With the walker (or the quad) at (observerX, observerZ) and the tyrannosaur at
     *  (rivalX, rivalZ): it watches the walker within WatchDistance and trumpets at the walker within
     *  TrumpetDistance (or at the tyrannosaur within 35 m) and then moves off; `nightness`
     *  (0 day .. 1 night) has it doze on its feet. */
    void Update(float seconds,float observerX,float observerZ,bool observerValid,float nightness,
                float rivalX,float rivalZ,bool rivalValid);
    /** Where it goes to drink (the stream's mouth): passing within 30 m, it goes and drinks. */
    void SetDrinkingPlace(float x,float z) noexcept {drinkX_=x;drinkZ_=z;hasDrink_=true;}
    /** QA: puts it at the route's waypoint nearest (x, z), standing still. */
    void PlaceNear(float x,float z);
    /** QA: holds it standing for `seconds` from now. */
    void HoldStanding(float seconds) noexcept {holdSeconds_=std::max(holdSeconds_,seconds);}
    /** QA: begins a trumpet now, held at its height, facing the way it faces. */
    void PreviewTrumpet();
    /** Whether the line from (x0, z0) to (x1, z1) passes clear of the trees and rocks near its
     *  round by `margin` (QA cameras look at it past them). */
    [[nodiscard]] bool ClearSight(float x0,float z0,float x1,float z1,float margin) const noexcept;

    /** Calls begun so far (trumpets and rumbles) and the last one's kind (0 rumble, 1 trumpet). */
    [[nodiscard]] std::uint32_t CallCount() const noexcept {return calls_;}
    [[nodiscard]] int LastCallKind() const noexcept {return lastCall_;}
    [[nodiscard]] const ElephantState& State() const noexcept {return state_;}
    [[nodiscard]] std::span<const ElephantPoint> Route() const noexcept {return route_;}
    [[nodiscard]] std::span<const Footprint> Prints() const noexcept {return prints_;}
    [[nodiscard]] std::uint32_t FootfallCount() const noexcept {return footfalls_;}
    /** The circles its body and head stand in (for the walker and the quad). */
    [[nodiscard]] std::array<CircularObstacle,3> Obstacles() const noexcept;
    /** A point on its body in the world, from its own frame (x right, y up from the joints'
     *  line, z ahead of the body's middle). */
    [[nodiscard]] ElephantPoint ToWorld(float right,float up,float ahead) const noexcept;
    /** The trunk's root on the face and its tip in the world, from the pose. */
    [[nodiscard]] std::array<ElephantPoint,16> TrunkPoints() const noexcept;
    /** The head's frame in the world: its pivot at the neck and its right, up and ahead axes. */
    struct HeadFrame {ElephantPoint pivot,right,up,ahead;};
    [[nodiscard]] HeadFrame Head() const noexcept;

private:
    [[nodiscard]] float Ground(float x,float z) const noexcept;
    void PoseLegs(float phase);
    void Begin(ElephantBehavior what,float seconds);
    const IslandTerrain& terrain_;
    std::vector<ElephantPoint> route_;
    std::vector<CircularObstacle> obstacles_;   // near its round, which it steers round
    std::size_t target_=1;
    ElephantState state_;
    float distance_=0;
    float walkSeconds_=0,holdSeconds_=0,clock_=0,activitySeconds_=0;
    float drinkCooldown_=0,noticeCooldown_=0,trumpetCooldown_=0,rumbleClock_=0,fleeSeconds_=0;
    float fleeX_=0,fleeZ_=0;
    float drinkX_=0,drinkZ_=0;
    bool hasDrink_=false,goingToDrink_=false,previewHold_=false;
    std::uint32_t calls_=0,decisions_=0;
    int lastCall_=0;
    std::array<bool,4> wasPlanted_{true,true,true,true};
    std::array<Footprint,PrintCapacity> prints_{};
    std::size_t nextPrint_=0;
    std::uint32_t footfalls_=0;
};

}
