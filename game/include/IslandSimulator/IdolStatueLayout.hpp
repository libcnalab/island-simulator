#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

/** Where the island's stone god stands: a weathered tiki idol on its platform of fitted basalt
 *  blocks, on a level rise inland with the sea before it. */
struct IdolStatuePlacement {
    static constexpr float PlatformHalfWidth=2.5f,PlatformHalfDepth=1.7f,PlatformHeight=0.55f;
    static constexpr float StatueHeight=3.35f;
    float x=0,z=0;          // the platform's centre
    float faceX=1,faceZ=0;  // unit direction the god looks (out toward the sea)
    float groundY=0;        // the highest ground under the platform; its top is PlatformHeight above
    [[nodiscard]] float TopY() const noexcept {return groundY+PlatformHeight;}
    [[nodiscard]] CircularObstacle Obstacle() const noexcept {return {x,z,3.1f};}
};

class IdolStatueLayout final {
public:
    static constexpr float MinimumShoreDistance=25.0f,MinimumHeight=8.0f,MaximumHeight=45.0f;
    static constexpr float ClearRadius=4.5f,MaximumRise=0.7f;
    /** The highest level, clear spot on the island's grid (6 m apart) that is at least
     *  MinimumShoreDistance inland, MinimumHeight..MaximumHeight up, with no obstacle's edge
     *  within ClearRadius of the platform's corners and at least `keepAway` metres from each of
     *  the `avoid` points (the hut, the fire, the other landmarks); the god faces the sea. */
    [[nodiscard]] static std::optional<IdolStatuePlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=30.0f);
};
}
