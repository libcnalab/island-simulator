#pragma once
#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cmath>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;

/** Where the lighthouse stands: on a rise of rock or turf a little back from the shore, its
 *  lantern over the sea. */
struct LighthousePlacement {
    static constexpr float BaseRadius=2.4f,TopRadius=1.6f,TowerHeight=16.5f,PlinthRadius=3.1f,PlinthHeight=0.8f;
    static constexpr float GalleryRadius=2.35f,LanternRadius=1.25f,LanternHeight=2.3f;
    static constexpr float RailHeight=1.0f,RailInset=0.08f,RailRadius=0.035f;   // the gallery's top rail
    float x=0,z=0;
    float groundY=0;       // the highest ground under the plinth; the plinth's top is PlinthHeight above
    float seaX=1,seaZ=0;   // unit direction out to sea (the door faces inland)
    [[nodiscard]] float TowerBaseY() const noexcept {return groundY+PlinthHeight;}
    [[nodiscard]] float GalleryY() const noexcept {return TowerBaseY()+TowerHeight;}
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
