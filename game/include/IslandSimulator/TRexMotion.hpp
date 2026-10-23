#pragma once
#include "IslandSimulator/FootprintTrail.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

struct TRexPoint {float x=0,y=0,z=0;};

/** One hind leg's joints in the world: hip, knee, ankle, the ball of the foot and its three
 *  toes' tips (inner, middle, outer); whether the foot is planted. */
struct TRexLeg {
    TRexPoint hip,knee,ankle,ball;
    std::array<TRexPoint,3> toes{};
    bool planted=true;
};

enum class TRexBehavior : std::uint8_t {Walk,Stand,Sniff,Roar,Drink,Rest,Watch,Follow};

/** The tyrannosaur's whole pose this moment, everything the renderer needs. */
struct TRexState {
    float x=0,y=0,z=0;           // the hips' ground point (y the ground under the hips)
    float heading=0;             // the way it faces (radians, atan2(z, x) of forward)
    float speed=0;
    float hipHeight=3.0f;        // the hip joints over the ground (bobbing with the stride)
    float pitch=0;               // the body's pitch, nose down positive
    float roll=0;                // the sway onto the planted leg
    float neckYaw=0,neckPitch=0; // the head turned and raised (looking about)
    float jaw=0;                 // 0..1 the jaw open
    float tailSway=0;            // the tail's swing, radians at its root
    float breath=0;              // the breathing phase
    float lie=0;                 // 0..1 lying down to rest, the legs folded under it
    float crouch=0;              // 0..1 the body lowered and pitched down (to drink)
    TRexBehavior behavior=TRexBehavior::Stand;
    std::array<TRexLeg,2> legs{};   // left, right
};

/** A full-grown Tyrannosaurus rex walking the island alone: some 12 m from snout to tail and
 *  3.8 m at the hip, its body balanced level over the hips. It follows its own round of the
 *  island's open ground (the upper beaches and the clearings), at a heavy 1.5 m/s walk, its
 *  feet planted one after the other, each laying a three-toed print 0.7 m long; it stops now
 *  and then and stands, looking about. Deterministic in the time it is given. */
class TRexMotion final {
public:
    static constexpr float Length=12.2f,HipHeight=3.05f,WalkSpeed=1.5f;
    static constexpr float LieHipHeight=1.36f;  // lying: the belly and the pubic boot on the ground
    static constexpr float StrideLength=4.1f;   // one foot's step forward and back again
    static constexpr float FemurLength=1.32f,TibiaLength=1.22f,MetatarsusLength=.72f,ToeLength=.42f;
    static constexpr float HipAcross=.62f;      // the hip joints out from the midline
    static constexpr float BodyRadius=1.4f;     // the body and hips as a collision circle
    static constexpr std::size_t PrintCapacity=40;

    TRexMotion(const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles);
    void Update(float seconds) {Update(seconds,0.0f,0.0f,false,0.0f);}
    /** With the walker (or the quad) at (observerX, observerZ): it stops and watches the walker
     *  within 60 m, follows at a distance now and then (never nearer than FollowDistance), and
     *  is roused from its rest within 18 m; `nightness` (0 day .. 1 night) has it lie up for
     *  the night. */
    void Update(float seconds,float observerX,float observerZ,bool observerValid,float nightness);
    static constexpr float WatchDistance=60.0f,FollowDistance=20.0f;
    /** Where it goes to drink (the stream's mouth): on its round near it, it stops and drinks. */
    void SetDrinkingPlace(float x,float z) noexcept {drinkX_=x;drinkZ_=z;hasDrink_=true;}
    /** QA: begins a roar now (`boom`: the closed-mouth call), or the lie-down, held. */
    void PreviewRoar(bool boom);
    void PreviewRest();
    /** Calls begun so far (roars and booms), the last one's kind (1 roar, 2 boom), breaths. */
    [[nodiscard]] std::uint32_t CallCount() const noexcept {return calls_;}
    [[nodiscard]] int LastCallKind() const noexcept {return lastCall_;}
    [[nodiscard]] std::uint32_t BreathCount() const noexcept {return breaths_;}
    /** QA: puts it at the route's waypoint nearest (x, z), standing still. */
    void PlaceNear(float x,float z);
    /** QA: holds it standing for `seconds` from now. */
    void HoldStanding(float seconds) noexcept {holdSeconds_=std::max(holdSeconds_,seconds);}

    [[nodiscard]] const TRexState& State() const noexcept {return state_;}
    [[nodiscard]] std::span<const TRexPoint> Route() const noexcept {return route_;}
    [[nodiscard]] std::span<const Footprint> Prints() const noexcept {return prints_;}
    /** Footfalls so far (each planted foot), for the ground's shake and the thud. */
    [[nodiscard]] std::uint32_t FootfallCount() const noexcept {return footfalls_;}
    [[nodiscard]] TRexPoint LastFootfall() const noexcept {return lastFootfall_;}
    /** The circles its body and tail stand in (for the walker and the quad). */
    [[nodiscard]] std::array<CircularObstacle,3> Obstacles() const noexcept;
    /** A point on its body in the world, from its own frame (x right, y up from the hip joints,
     *  z ahead). */
    [[nodiscard]] TRexPoint ToWorld(float right,float up,float ahead) const noexcept;

private:
    [[nodiscard]] float Ground(float x,float z) const noexcept;
    void PoseLegs(float seconds);
    const IslandTerrain& terrain_;
    std::vector<TRexPoint> route_;
    std::size_t target_=1;
    TRexState state_;
    float distance_=0;               // walked, for the stride's phase
    float walkSeconds_=0,holdSeconds_=0,clock_=0;
    float activitySeconds_=0,breathClock_=0,drinkCooldown_=0,followSeconds_=0,followCooldown_=30.0f,noticeCooldown_=0;
    float drinkX_=0,drinkZ_=0;
    bool hasDrink_=false,previewHold_=false;
    std::uint32_t calls_=0,breaths_=0,decisions_=0;
    int lastCall_=0;
    void BeginStand(TRexBehavior what,float seconds);
    std::array<bool,2> wasPlanted_{true,true};
    std::array<Footprint,PrintCapacity> prints_{};
    std::size_t nextPrint_=0;
    std::uint32_t footfalls_=0;
    TRexPoint lastFootfall_{};
};

}
