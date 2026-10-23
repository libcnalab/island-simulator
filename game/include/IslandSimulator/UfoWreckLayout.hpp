#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

/** Where a flying saucer came down: in the open of a clearing inland, sliding in along a furrow
 *  of scorched earth and stopping slewed over, tilted, its far rim sunk into the ground, its
 *  cracked dome up and a landing strut bent out. Local (forward, right): `forward` is the way
 *  it was sliding, so the furrow lies behind it. */
struct UfoWreckPlacement {
    static constexpr float HullRadius=5.3f,Tilt=0.27f,Roll=0.12f,Sink=0.85f,FurrowLength=17.0f;
    float x=0,z=0;                 // the hull's centre over the ground
    float headingX=1,headingZ=0;   // unit direction it was sliding
    float groundY=0;               // the ground at the hull's middle
    [[nodiscard]] std::array<float,2> At(const float forward,const float right) const noexcept {
        return {{x+headingX*forward+headingZ*right,z+headingZ*forward-headingX*right}};
    }
    /** The hull, the buried rim and the bent strut block the way. */
    [[nodiscard]] std::array<CircularObstacle,3> Obstacles() const noexcept;
    /** Where the pieces of its skin lie scattered (forward, right), for the alien to look over. */
    static constexpr std::size_t DebrisCount=6;
    [[nodiscard]] static std::array<float,2> DebrisSpot(std::size_t index) noexcept;
};

class UfoWreckLayout final {
public:
    static constexpr float MinimumShoreDistance=30.0f,MaximumRise=1.6f,MinimumClearance=9.0f;
    /** The most open level spot inland (furthest from any obstacle, at least MinimumClearance) on
     *  a 6 m grid, `keepAway` metres from each `avoid` point (the hut, the fire, the other
     *  landmarks). */
    [[nodiscard]] static std::optional<UfoWreckPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=40.0f);
};
}
