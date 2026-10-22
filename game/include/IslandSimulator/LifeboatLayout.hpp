#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;
struct FreighterPlacement;

/** The grounded coaster's lifeboat, gone from its davits: washed up on the beach a little way
 *  along from the ship and lying keel up on the dry sand, settled over to one side. */
struct LifeboatPlacement {
    static constexpr float Length=7.2f,HalfBeam=1.15f,Depth=1.0f;
    float x=0,z=0,groundY=0;      // the boat's middle on the sand
    float dirX=1,dirZ=0;          // the bow's direction
    float list=0.12f;             // how far it has rolled onto one gunwale (radians)
    [[nodiscard]] std::array<float,2> At(float along,float across) const noexcept {
        return {{x+dirX*along-dirZ*across,z+dirZ*along+dirX*across}};
    }
    /** The hull blocks the way: bow, middle and stern. */
    [[nodiscard]] std::array<CircularObstacle,3> Obstacles() const noexcept;
};

class LifeboatLayout final {
public:
    /** The first open, level stretch of dry sand along the beach from the ship's ladder side (or,
     *  failing that, its other side), 14 to 40 m off its hull, clear of the obstacles and
     *  `keepAway` from each `avoid` point. */
    [[nodiscard]] static std::optional<LifeboatPlacement> FindNear(const IslandTerrain& terrain,
        const FreighterPlacement& ship,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=6.0f);
};
}
