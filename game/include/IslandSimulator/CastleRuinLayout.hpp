#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** Where the island's medieval castle ruin stands: a square curtain wall with a round tower at
 *  each corner, a gatehouse gap facing the sea and a ruined keep at the back, on a gentle rise
 *  inland. Local (right, forward): `forward` runs from the middle toward the gate and the sea. */
struct CastleRuinPlacement {
    static constexpr float Half=9.5f,Wall=1.3f,GateHalf=1.7f,TowerRadius=2.4f;
    static constexpr float KeepRight=-2.5f,KeepForward=-4.8f,KeepHalf=3.6f,KeepWall=1.2f;
    static constexpr float BreachFrom=2.0f,BreachTo=5.2f;   // the east wall's fallen stretch (right = +Half)
    float x=0,z=0;
    float faceX=1,faceZ=0;   // unit direction of the gate
    float groundY=0;         // the lowest ground under the curtain wall
    [[nodiscard]] float RightX() const noexcept {return faceZ;}
    [[nodiscard]] float RightZ() const noexcept {return -faceX;}
    [[nodiscard]] std::array<float,2> At(const float right,const float forward) const noexcept {
        return {{x+RightX()*right+faceX*forward,z+RightZ()*right+faceZ*forward}};
    }
    /** Circles down the middle of the standing walls, towers and keep; the gate and the breach
     *  are left open. */
    [[nodiscard]] std::vector<CircularObstacle> WallObstacles() const;
};

class CastleRuinLayout final {
public:
    static constexpr float MinimumShoreDistance=30.0f,MinimumHeight=5.0f,MaximumHeight=40.0f;
    static constexpr float MaximumRise=2.4f,ClearMargin=1.5f;
    /** The most level spot on the island's 6 m grid at least MinimumShoreDistance inland, with the
     *  ground varying less than MaximumRise over the walls' footprint, no obstacle within the
     *  walls and `keepAway` metres from each `avoid` point; the gate faces the sea. */
    [[nodiscard]] static std::optional<CastleRuinPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=45.0f);
};
}
