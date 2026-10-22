#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** A jungle giant the wind threw: its trunk lying along the forest floor, its root plate torn up
 *  with it and standing on edge where the trunk's foot was, the pit it left behind the plate. */
struct WindthrowPlacement {
    static constexpr float TrunkLength=17.0f,BaseRadius=0.52f,TipRadius=0.3f;
    static constexpr float PlateHalfWidth=2.3f,PlateHeight=3.4f,PlateSink=0.35f;
    static constexpr float PitReach=2.6f;          // the pit, back from the plate
    float x=0,z=0;                // the trunk's foot, at the root plate's middle
    float dirX=1,dirZ=0;          // unit direction the trunk lies, away from the pit
    float groundY=0;              // the ground at the foot
    std::uint32_t seed=0;
    [[nodiscard]] float AcrossX() const noexcept {return -dirZ;}
    [[nodiscard]] float AcrossZ() const noexcept {return dirX;}
    /** The plan point `along` the trunk from its foot and `across` it. */
    [[nodiscard]] std::array<float,2> At(const float along,const float across) const noexcept {
        return {{x+dirX*along+AcrossX()*across,z+dirZ*along+AcrossZ()*across}};
    }
    /** The trunk's radius `along` it. */
    [[nodiscard]] static float RadiusAt(float along) noexcept;
    /** Circles along the trunk and the plate that block the walker and the animals. */
    [[nodiscard]] std::vector<CircularObstacle> Obstacles() const;
};

class WindthrowLayout final {
public:
    static constexpr float MinimumShoreDistance=40.0f,MaximumSlope=0.22f,MaximumRise=1.6f;
    static constexpr int MinimumStand=4;       // canopy trees within StandRadius: deep in the jungle
    static constexpr float StandRadius=16.0f;
    /** The deepest stand of jungle (most canopy trees round it) on the island's 5 m grid where a
     *  trunk TrunkLength long, its root plate and its pit lie clear of every obstacle, on gentle
     *  ground rising no more than MaximumRise along the trunk, `keepAway` metres from each of the
     *  `avoid` points; the trunk laid the way, of 24, it fits. */
    [[nodiscard]] static std::optional<WindthrowPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CanopyPlacement> canopy,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=14.0f);
};
}
