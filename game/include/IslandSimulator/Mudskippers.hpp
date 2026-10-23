#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One mudskipper where it is this moment. */
struct Mudskipper {
    float x=0,y=0,z=0;      // on the mud (y the mud's surface, lifted through a hop)
    float heading=0;
    float hop=0;            // 0..1 through a hop (0 resting)
    float display=0;        // 0..1 the dorsal fin raised in display
    float scale=1;
    std::uint8_t tone=0;
};

/** The mud round the mangroves' roots at the stream mouth: a patch of wet grey-brown mud under
 *  each mangrove where the ground is low, and the mudskippers living on it, resting propped on
 *  their pectoral fins, skipping a few body-lengths in a hop now and then, raising their dorsal
 *  fins at each other, and skipping away from the walker's feet toward the water. Deterministic
 *  in the clock. */
class MudskipperColony final {
public:
    static constexpr int PerTree=4;
    static constexpr float MudRadius=6.8f;
    static constexpr float FleeRadius=2.6f;
    struct MudPatch {float x,z,radius;};

    MudskipperColony(const IslandTerrain& terrain,std::span<const CanopyPlacement> canopy);
    [[nodiscard]] std::span<const MudPatch> Patches() const noexcept {return patches_;}
    [[nodiscard]] std::size_t Count() const noexcept {return homes_.size();}
    /** The skippers at the clock's `seconds`, skipping away from the walker (written into `out`). */
    void At(float seconds,float walkerX,float walkerZ,const std::function<float(float,float)>& ground,
            std::vector<Mudskipper>& out) const;

private:
    struct Home {float x,z,towardWaterX,towardWaterZ;std::uint32_t seed;};
    std::vector<MudPatch> patches_;
    std::vector<Home> homes_;
};

}
