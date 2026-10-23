#pragma once
#include "IslandSimulator/UfoWreckLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

struct AlienPoint {float x=0,y=0,z=0;};

/** A leg's joints in the world (hip, knee, the sole's centre on the ground). */
struct AlienLeg {AlienPoint hip,knee,foot;bool planted=true;};
/** An arm's joints in the world (shoulder, elbow, the hand). */
struct AlienArm {AlienPoint shoulder,elbow,hand;};

enum class AlienBehavior : std::uint8_t {Walk,Stand,Examine,TendHull,Scan,Watch,Startle};

/** The alien's whole pose this moment, everything the renderer needs. */
struct AlienState {
    float x=0,y=0,z=0;          // the ground point under it
    float heading=0;            // the way it faces (radians, atan2(z, x) of forward)
    float speed=0;
    float crouch=0;             // 0 upright .. 1 squatting on its heels
    float headYaw=0,headPitch=0;
    float armsUp=0;             // 0 .. 1 raised in alarm
    AlienBehavior behavior=AlienBehavior::Stand;
    AlienPoint pelvis,chest,neck,head;
    AlienPoint headRight,headUp,headAhead;   // unit axes of the head
    std::array<AlienLeg,2> legs{};           // left, right
    std::array<AlienArm,2> arms{};
};

/** The grey little visitor that came down with the saucer, picking over the wreck on foot: some
 *  1.3 m tall, a long thin body, a great head with two slanted black eyes, long arms to the knees
 *  and long three-fingered hands. It walks a loose ring round the saucer at 0.85 m/s, stops to
 *  look about, crouches over the scattered pieces of the skin and touches them, lays its hands on
 *  the hull to feel it, points a long arm at the dome, watches a walker who comes within 30 m
 *  and bolts round the far side of the saucer, arms up, from one within 7 m. Deterministic in the
 *  time it is given. */
class AlienMotion final {
public:
    static constexpr float Height=1.3f,WalkSpeed=0.85f,RunSpeed=1.7f,Thigh=0.4f,Shin=0.4f,UpperArm=0.34f,Forearm=0.36f;
    static constexpr float PelvisHeight=0.74f,WatchDistance=30.0f,StartleDistance=7.0f;
    static constexpr float RingMinimum=7.4f,RingMaximum=12.0f,HullTouchRadius=5.8f;

    AlienMotion(const IslandTerrain& terrain,const UfoWreckPlacement& wreck,std::span<const CircularObstacle> obstacles);
    void Update(float seconds) {Update(seconds,0.0f,0.0f,false);}
    /** With the walker (or the quad) at (observerX, observerZ): it watches within WatchDistance
     *  and bolts from within StartleDistance. */
    void Update(float seconds,float observerX,float observerZ,bool observerValid);
    /** QA: stands it at the wreck's local (forward, right) facing the hull, in `what`, held there
     *  for `seconds`. */
    void Place(float forward,float right,AlienBehavior what,float seconds);
    /** QA: holds it still in its present behavior for `seconds` from now. */
    void Hold(const float seconds) noexcept {holdSeconds_=std::max(holdSeconds_,seconds);}
    [[nodiscard]] const AlienState& State() const noexcept {return state_;}
    [[nodiscard]] CircularObstacle Obstacle() const noexcept {return {state_.x,state_.z,0.35f};}
    /** A point on it in the world from its own frame (x right, y up from the ground, z ahead). */
    [[nodiscard]] AlienPoint ToWorld(float right,float up,float ahead) const noexcept;
    [[nodiscard]] std::uint32_t DecisionCount() const noexcept {return decisions_;}
    [[nodiscard]] std::uint32_t StartleCount() const noexcept {return startles_;}
    /** The distinct behaviours it has been in (bit per AlienBehavior), for the QA log. */
    [[nodiscard]] std::uint32_t BehaviorsSeen() const noexcept {return seen_;}

private:
    [[nodiscard]] float Ground(float x,float z) const noexcept;
    [[nodiscard]] bool Free(float x,float z,float margin) const noexcept;
    bool PickRingGoal(std::uint32_t salt,float minRadius,float maxRadius,float& outX,float& outZ) const noexcept;
    void Decide();
    void Begin(AlienBehavior what,float seconds);
    void GoTo(float x,float z,AlienBehavior then,float thenSeconds,float speed);
    void Pose(float dt);
    const IslandTerrain& terrain_;
    UfoWreckPlacement wreck_;
    std::vector<CircularObstacle> obstacles_;   // those near the saucer
    AlienState state_;
    float goalX_=0,goalZ_=0,goalSpeed_=WalkSpeed;
    bool goingTo_=false;
    AlienBehavior then_=AlienBehavior::Stand;
    float thenSeconds_=0;
    float travelSeconds_=0,behaviorSeconds_=0,holdSeconds_=0,clock_=0,gait_=0,watchCooldown_=10.0f;
    float observerX_=0,observerZ_=0;
    bool observerValid_=false;
    float focusX_=0,focusZ_=0;      // what it examines or faces (a debris spot, the hull)
    std::uint32_t decisions_=0,startles_=0,seen_=0;
    std::array<std::array<float,3>,2> handLocal_{};   // each hand's pose in its own frame (right, up from the shoulder, ahead)
};
}
