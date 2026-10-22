#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One screw pine (Pandanus tectorius) on the coastal fringe: a slender trunk standing on a cone
 *  of stilt roots, forking into one to three branches, each ending in a spiral tuft of long
 *  sword-like leaves; leaning a little out toward the sea. */
struct PandanusPlacement {
    float x=0,z=0,groundY=0;
    float height=6.0f;          // to the tufts
    float lean=0.2f;            // radians, toward the sea
    float seaX=1,seaZ=0;        // unit direction out to sea
    std::uint8_t forks=2;       // branches at the fork
    std::uint32_t seed=0;
    bool fruiting=false;
    [[nodiscard]] CircularObstacle Obstacle() const noexcept {return {x,z,0.7f};}
};

class PandanusLayout final {
public:
    static constexpr std::size_t MaximumCount=56;
    static constexpr float MinimumSpacing=4.5f,InlandFrom=6.0f,InlandTo=30.0f;
    /** Screw pines in small groups round the island's coastal fringe, InlandFrom to InlandTo
     *  metres up from the shoreline on dry sand or turf, on gentle ground, MinimumSpacing apart,
     *  clear of the obstacles and `keepAway` metres from each `avoid` point. */
    [[nodiscard]] static std::vector<PandanusPlacement> Scatter(const IslandTerrain& terrain,
        std::span<const CircularObstacle> obstacles,std::span<const std::array<float,2>> avoid,float keepAway=10.0f);
};
}
