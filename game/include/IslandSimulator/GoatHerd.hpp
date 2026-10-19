#pragma once
#include "IslandSimulator/GoatHabitat.hpp"
#include "IslandSimulator/GoatPose.hpp"

namespace island {
enum class GoatBehavior { Graze,Walk,Rest,Retreat,Drink };
struct GoatState {
    float x=0,y=0,z=0,heading=0,speed=0,phase=0,grazingPose=0;
    float scale=1;
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
    };
    std::array<Intent,GoatHabitat::Count> intents_{};
    float sentinelAlertSeconds_=0;
    float disturbanceX_=0,disturbanceZ_=0,disturbanceSeconds_=0;
};
}
