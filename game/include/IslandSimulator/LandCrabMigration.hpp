#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One red land crab on the march, where it is drawn this frame. */
struct MigratingCrab {
    float x=0,y=0,z=0;
    float heading=0;      // the way its front faces (it walks sideways to the sea, as crabs do)
    float stride=0;       // the legs' cycle, radians
    float scale=1;        // 0 while it comes up out of the litter or goes under the water
    std::uint8_t tone=0;  // 0..2 red, orange-red, a dark carmine
};

/** After the rain, on dark evenings, nights and dawns, the island's red land crabs march out of
 *  the forest to the sea in their thousands (as on Christmas Island): down lanes from 44 m inland
 *  to the waterline all round the coast, sideways, a few metres a minute, out of the litter at
 *  the lane's head and into the surf at its foot. Deterministic in the clock: a crab's place is
 *  worked out from its lane and the time, so only those near the eye are ever looked at. */
class LandCrabMigration final {
public:
    static constexpr int LaneCount=720,CrabsPerLane=40;   // some 25 000 on the whole coast
    static constexpr float LaneLength=44.0f;
    static constexpr float Speed=0.15f;          // metres a second, give or take a third
    static constexpr float ScatterRadius=1.4f;   // the crabs step aside from the walker's feet

    explicit LandCrabMigration(const IslandTerrain& terrain);

    /** How many of the crabs are out (0..1): the dark (0 by day, 1 at night) times the wet. */
    [[nodiscard]] static float Strength(float darkness,float wetness) noexcept;

    /** Appends the crabs on the march within `radius` of (x, z), the nearest lanes first, at most
     *  `cap` of them; `ground` gives the drawn ground's height. */
    void Gather(float seconds,float strength,float x,float z,float radius,float walkerX,float walkerZ,
                const std::function<float(float,float)>& ground,std::vector<MigratingCrab>& out,
                std::size_t cap) const;

    [[nodiscard]] std::size_t ActiveLaneCount() const noexcept {return lanes_.size();}

    struct Lane {float startX,startZ,endX,endZ;};
    [[nodiscard]] std::span<const Lane> Lanes() const noexcept {return lanes_;}

private:
    std::vector<Lane> lanes_;
};

}
