#pragma once
#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

struct LighthousePoint {float x=0,y=0,z=0;};

/** Where the lighthouse stands: on a rise of rock or turf a little back from the shore, its
 *  lantern over the sea. Angles round its axis are measured from the direction out to sea,
 *  toward (seaZ, -seaX); the door in its foot faces inland (angle pi). It is walked: up stone
 *  steps to the plinth, through the door, up a stair winding round a newel inside the tower
 *  (climbing clockwise, the angle falling) and through a glazed door in the lantern out onto
 *  the gallery. */
struct LighthousePlacement {
    static constexpr float Pi=3.14159265f;
    static constexpr float BaseRadius=2.4f,TopRadius=1.6f,TowerHeight=16.5f,PlinthRadius=3.1f,PlinthHeight=0.8f;
    static constexpr float GalleryRadius=2.35f,LanternRadius=1.25f,LanternHeight=2.3f;
    static constexpr float RailHeight=1.0f,RailInset=0.08f,RailRadius=0.035f;   // the gallery's top rail
    // inside: the wall's thickness, the newel the stair winds round, the stair's rise a turn and
    // its steps; 16.5 m in five and a half turns
    static constexpr float WallThickness=0.3f,NewelRadius=0.16f,TurnRise=3.0f;
    static constexpr int StepsPerTurn=16;
    // the doors: in the tower's foot, inland; in the lantern, two panes wide, out onto the gallery
    static constexpr float DoorAngle=Pi,DoorHalfWidth=0.5f,DoorHeight=2.05f;
    static constexpr float LanternDoorAngle=-Pi*0.5f,LanternDoorHalfAngle=Pi/6.0f,LanternDoorHeight=2.0f;
    // the stair starts on the floor beside the door and ends 0.75 rad short of the lantern's door
    static constexpr float StairStart=Pi*0.5f+0.75f;
    // the stone steps up to the door from the ground inland: their half width and steepest slope
    static constexpr float StepsHalfWidth=0.75f,StepsSlope=0.55f;
    float x=0,z=0;
    float groundY=0;       // the highest ground under the plinth; the plinth's top is PlinthHeight above
    float seaX=1,seaZ=0;   // unit direction out to sea (the door faces inland)
    float stepsRun=1.5f;   // the steps' run inland from the plinth's edge to their foot
    float stepsFootY=0;    // the ground at their foot
    [[nodiscard]] float TowerBaseY() const noexcept {return groundY+PlinthHeight;}
    [[nodiscard]] float GalleryY() const noexcept {return TowerBaseY()+TowerHeight;}
    [[nodiscard]] float TowerTopY() const noexcept {return GalleryY()-0.22f;}
    /** The tower's outer face's radius at height y (tapering from its foot to its top). */
    [[nodiscard]] float OuterRadius(float y) const noexcept {
        const float t=std::clamp((y-TowerBaseY())/(TowerTopY()-TowerBaseY()),0.0f,1.0f);
        return BaseRadius+(TopRadius-BaseRadius)*std::pow(t,0.9f);
    }
    [[nodiscard]] float InnerRadius(float y) const noexcept {return OuterRadius(y)-WallThickness;}
    [[nodiscard]] float StairRisePerRadian() const noexcept {return TurnRise/(2.0f*Pi);}
    /** The stair's last tread's far edge, unwrapped (StairStart less its whole sweep). */
    [[nodiscard]] float StairEnd() const noexcept {return StairStart-(GalleryY()-TowerBaseY())/StairRisePerRadian();}
    /** A point at `angle` round the axis, `radius` out, at height y. */
    [[nodiscard]] LighthousePoint At(float angle,float radius,float y) const noexcept {
        return {x+(seaX*std::cos(angle)+seaZ*std::sin(angle))*radius,y,z+(seaZ*std::cos(angle)-seaX*std::sin(angle))*radius};
    }
    /** (radius, angle) of a point round the axis. */
    [[nodiscard]] std::array<float,2> Polar(float worldX,float worldZ) const noexcept {
        const float dx=worldX-x,dz=worldZ-z;
        return {{std::hypot(dx,dz),std::atan2(dx*seaZ-dz*seaX,dx*seaX+dz*seaZ)}};
    }
    /** The stair's heights at `angle` (one a turn), lowest first, and how many. */
    [[nodiscard]] int StairHeights(float angle,std::array<float,8>& heights) const noexcept;
    /** Whether the landing's floor stands at `angle` up in the lantern (not open over the stair). */
    [[nodiscard]] bool LandingFloor(float angle) const noexcept;
    /** The walkable surface under (x, z) for feet at feetY: the steps, the plinth, the floor, the
     *  stair, the lantern's floor, the gallery; false off the lighthouse. */
    [[nodiscard]] bool SupportHeight(float worldX,float worldZ,float feetY,float& height) const noexcept;
    /** Whether a walker's upright capsule standing at `feet` meets the plinth, the walls (but for
     *  the doors), the newel, the lens, the stair or the landing overhead, the lantern or the rail. */
    [[nodiscard]] bool IntersectsCapsule(LighthousePoint feet,float radius,float height) const noexcept;
    [[nodiscard]] float SafeTravelFraction(LighthousePoint from,LighthousePoint to,float radius,float height) const noexcept;
    /** Where a walker stands on the ground at the steps' foot. */
    [[nodiscard]] LighthousePoint StepsFoot() const noexcept;
    /** Whether (x, z) lies on the steps, `margin` wider either side. */
    [[nodiscard]] bool OnSteps(float worldX,float worldZ,float margin=0.0f) const noexcept;
    /** The lamp's centre in the lantern room. */
    [[nodiscard]] std::array<float,3> Lamp() const noexcept {return {{x,GalleryY()+LanternHeight*0.45f,z}};}
    [[nodiscard]] CircularObstacle Obstacle() const noexcept {return {x,z,PlinthRadius+0.2f};}
    /** Where a bird stands on the gallery's top rail over the sea, facing out over it. */
    [[nodiscard]] BirdPerchSite RailPerch() const noexcept {
        const float r=GalleryRadius-RailInset;
        return {x+seaX*r,GalleryY()+RailHeight+RailRadius,z+seaZ*r,std::atan2(seaZ,seaX),BirdPerchKind::Wreck};
    }
};

class LighthouseLayout final {
public:
    static constexpr float MinimumHeight=3.0f,MaximumHeight=24.0f,ClearRadius=3.0f,MaximumRise=1.2f;
    /** The highest level spot of rock or turf 10 to 25 m up from the shore round the island, clear
     *  of the obstacles by ClearRadius beyond the plinth and `keepAway` metres from each `avoid`
     *  point (the wreck, the jetty, the other landmarks). */
    [[nodiscard]] static std::optional<LighthousePlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=35.0f);
};
}
