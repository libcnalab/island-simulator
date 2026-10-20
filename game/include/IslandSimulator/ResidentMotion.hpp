#pragma once
#include "IslandSimulator/ResidentRoute.hpp"
#include <array>
namespace island {
struct ResidentFootFrame {
    ResidentPoint centre{},right{1,0,0},up{0,1,0},forward{0,0,1};
    float heading=0;
    bool operator==(const ResidentFootFrame&)const=default;
};
struct ResidentLegPose {
    static constexpr float UpperLength=.43f,LowerLength=.44f,AnkleHeight=.065f;
    ResidentPoint hip{},knee{},ankle{};
    ResidentFootFrame foot;
    bool stance=true,reachLimited=false;
    bool operator==(const ResidentLegPose&)const=default;
};
[[nodiscard]] ResidentLegPose SolveResidentLeg(ResidentPoint hip,const ResidentFootFrame& foot,
    ResidentPoint bendForward,bool stance);
struct ResidentPose {
    ResidentPoint origin{};
    float pelvisY=0,heading=0;
    std::array<ResidentLegPose,2> legs{};
    std::array<ResidentPoint,2> shoulders{},elbows{},wrists{};
    bool operator==(const ResidentPose&)const=default;
};
enum class ResidentMode {Walking,Pausing,Turning,Yielding};
struct ResidentState {
    float x=0,y=0,z=0,heading=0,speed=0,phase=0;
    float distanceTravelled=0;
    // 0..1 fishing: at the jetty-foot pause the rod is held out in the right hand with a line
    // to a float riding the shallows past the deck's end; eased in and out with the pause.
    float fishing=0;
    // While fishing: the float's rise and fall on the swell (metres), and 0..1 a lift of the
    // rod the resident gives now and then to check the line.
    float floatBob=0,rodLift=0;
    // The head's yaw off the body's heading (radians, left negative): turned to a player
    // within LookDistance and ahead of the resident, eased, and back to the front when
    // they leave that; the renderer turns the head and face by it.
    float lookYaw=0;
    // 0..1 the stretch: at every third pause the resident reaches both arms straight up
    // for a couple of seconds and lets them down; eased in and out.
    float stretch=0;
    std::size_t nextPoint=1,completedLaps=0,pauseCount=0,rejectedMoves=0;
    ResidentMode mode=ResidentMode::Pausing;
    ResidentPose pose;
    bool operator==(const ResidentState&)const=default;
};
// Exactly one peaceful adult. Rendering consumes this authoritative planted-foot pose;
// the controller itself remains independent of graphics and platform APIs.
class ResidentMotion final {
public:
    static constexpr float WalkingSpeed=.95f,SwingSeconds=.45f;
    ResidentMotion(const IslandTerrain& terrain,std::span<const CircularObstacle> solids,
                   std::span<const ResidentPoint> landmarks={},std::span<const ResidentPoint> landmarkFacings={});
    void Update(float seconds,ResidentPoint observer);
    [[nodiscard]] const ResidentState& State()const noexcept {return state_;}
    // True while a pause has a direction to face (the jetty foot looks out to sea).
    [[nodiscard]] bool LookingOut()const noexcept {return pauseFacing_&&pauseRemaining_>0;}
    // 0..1: how far the right hand is raised to a player who has stopped close by.
    [[nodiscard]] float WaveAmount()const noexcept {return wave_;}
    // 0..1 crouched: at a pause the resident squats, pelvis CrouchDrop lower with the knees
    // bent forward and both hands reaching to the front (tending the fire); the game asks for
    // it while they stand at the campfire, and the crouch eases in and out over a second.
    void SetCrouch(float target) noexcept;
    [[nodiscard]] float CrouchAmount()const noexcept {return crouch_;}
    // QA: holds the resident fully crouched for a still capture.
    void PreviewCrouch();
    static constexpr float CrouchDrop=.36f,CrouchInRate=1.0f,CrouchOutRate=1.4f;
    // The fishing spot: a pause with a view within FishingReach of it is a fishing pause.
    void SetFishingSpot(float x,float z) noexcept;
    [[nodiscard]] bool Fishing()const noexcept {return fishing_;}
    static constexpr float FishingReach=6.0f,FishingInRate=1.2f,FishingOutRate=2.5f;
    static constexpr float RodLiftPeriod=12.0f,RodLiftAt=9.0f,RodLiftSeconds=.8f;
    // The look: the head turns up to MaximumLookYaw to a player within LookDistance who is
    // within LookField of straight ahead, at LookRate; a player further round is not followed.
    static constexpr float LookDistance=14.0f,MaximumLookYaw=1.0f,LookField=1.35f,LookRate=2.5f;
    // The stretch: StretchSeconds of both arms up at the start of every third pause.
    [[nodiscard]] std::uint32_t StretchCount()const noexcept {return stretches_;}
    [[nodiscard]] bool Stretching()const noexcept {return stretchSeconds_>0;}
    static constexpr float StretchSeconds=2.0f,StretchInRate=1.4f,StretchOutRate=1.6f,StretchRaise=2.95f;
    // QA: holds the resident fishing, rod out, for a still capture.
    void PreviewFishing();
    [[nodiscard]] const ResidentRoute& Route()const noexcept {return route_;}
    [[nodiscard]] const RenderedTerrainSurface& Ground()const noexcept {return ground_;}
private:
    struct Foot {
        ResidentFootFrame current,start,target;
        float progress=1,duration=SwingSeconds;
        bool swinging=false;
    };
    [[nodiscard]] ResidentFootFrame GroundFoot(float x,float z,float heading)const;
    [[nodiscard]] bool FitPose(float x,float z,float heading,float speed,ResidentPose& pose)const;
    void Step(float seconds,ResidentPoint observer);
    void AdvanceFeet(float seconds);
    void StartStep(float targetX,float targetZ,float heading,std::size_t leg,float duration);
    const IslandTerrain& terrain_;
    RenderedTerrainSurface ground_;
    WorldCollisionGrid solids_;
    ResidentRoute route_;
    std::array<Foot,2> feet_{};
    ResidentState state_;
    float pauseRemaining_=3;
    float wave_=0,waveClock_=0;
    float stretchSeconds_=0;std::uint32_t stretches_=0;
    float crouch_=0,crouchTarget_=0;
    float fishSpotX_=0,fishSpotZ_=0;bool fishSpotValid_=false,fishing_=false;
    float fishClock_=0;
    float pauseFaceX_=0,pauseFaceZ_=0;bool pauseFacing_=false;
};
}
