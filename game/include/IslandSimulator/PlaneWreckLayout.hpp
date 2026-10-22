#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

/** Where a Second World War fighter came down: in the open of a clearing inland, its nose dug in,
 *  the tail broken off behind the cockpit and the left wing torn away beside it. */
struct PlaneWreckPlacement {
    float x=0,z=0;            // the wing roots' centre on the ground
    float headingX=1,headingZ=0;   // unit direction the nose points
    float groundY=0;
    [[nodiscard]] std::array<float,2> At(float forward,float right) const noexcept {
        return {{x+headingX*forward+headingZ*right,z+headingZ*forward-headingX*right}};
    }
    /** The fuselage, the broken tail and the torn wing block the way. */
    [[nodiscard]] std::array<CircularObstacle,4> Obstacles() const noexcept;
};

class PlaneWreckLayout final {
public:
    static constexpr float MinimumShoreDistance=30.0f,MaximumRise=1.3f;
    /** The most open level spot inland (furthest from any obstacle, at least 7 m) on a 6 m grid,
     *  `keepAway` metres from each `avoid` point (the hut, the fire, the other landmarks). */
    [[nodiscard]] static std::optional<PlaneWreckPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=35.0f);
};
}
