#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

/** Where a stranded whale's bleached skeleton lies on the beach: the back of its skull on the
 *  upper beach's dry sand, the spine running along the shore toward the tail. */
struct WhaleSkeletonPlacement {
    static constexpr float SkullLength=3.9f;   // rostrum tip forward of the skull's back
    static constexpr float SpineLength=11.2f;  // skull's back to the last tail vertebra
    float x=0,z=0;                             // the back of the skull, on the sand
    float dirX=1,dirZ=0;                       // unit direction from the skull back along the spine
    float seaX=0,seaZ=1;                       // unit direction across it toward the sea
    /** The skull and the jaws' bows block the walker and the animals; the ribs and the
     *  spine are stepped over or through. */
    [[nodiscard]] std::array<CircularObstacle,3> Obstacles() const noexcept;
    /** A point at `along` metres from the skull's back toward the tail (negative: toward the
     *  rostrum's tip) and `seaward` metres across toward the sea. */
    [[nodiscard]] std::array<float,2> At(float along,float seaward) const noexcept {
        return {{x+dirX*along+seaX*seaward,z+dirZ*along+seaZ*seaward}};
    }
    /** The spine's line across the sand, seaward of the axis: a lazy S as the carcass settled. */
    [[nodiscard]] static float SpineAcross(float along) noexcept;
};

class WhaleSkeletonLayout final {
public:
    static constexpr float InlandFromShore=8.5f;   // the skull's back this far up from the shoreline
    static constexpr float ClearRadius=8.0f;       // no other obstacle's edge within this of the bones
    /** The first stretch of open, level dry sand round the island (from the north-east, sunwise)
     *  that holds the whole skeleton clear of the obstacles and at least `keepAway` metres from
     *  each of the points in `avoid` (the jetty's root, the turtle nests). */
    [[nodiscard]] static std::optional<WhaleSkeletonPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=16.0f);
    /** Whether a placement lies wholly on level dry sand clear of the obstacles. */
    [[nodiscard]] static bool Fits(const IslandTerrain& terrain,const WhaleSkeletonPlacement& placement,
                                   std::span<const CircularObstacle> obstacles);
};
}
