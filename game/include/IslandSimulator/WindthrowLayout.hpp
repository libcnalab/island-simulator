#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <algorithm>
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
    /** The trunk's axis `along` it over `ground` (the drawn ground's height under x, z): resting
     *  on the ground's high points, held up at its foot by the plate. */
    template<class Ground>
    [[nodiscard]] std::array<float,3> Axis(const float along,const Ground& ground) const {
        float sum=0.0f;int count=0;
        for(float k=-1.5f;k<=1.51f;k+=0.75f) {
            const float a=std::clamp(along+k,0.0f,TrunkLength);
            const auto p=At(a,0.0f);
            sum+=ground(p[0],p[1])+0.82f*RadiusAt(a);++count;
        }
        const auto p=At(along,0.0f);
        const float r=RadiusAt(along);
        float y=std::max(sum/static_cast<float>(count),ground(p[0],p[1])+0.6f*r);
        if(along<1.4f) y=std::max(y,groundY+r+0.22f*(1.0f-std::max(along,0.0f)/1.4f));
        return {{p[0],y,p[1]}};
    }
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
