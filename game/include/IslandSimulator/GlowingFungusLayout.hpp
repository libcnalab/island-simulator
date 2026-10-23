#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/WindthrowLayout.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One clump of little glowing mushrooms (Mycena chlorophos, the Pacific islands' foxfire) on
 *  dead wood: pale buff caps on thin stalks by day, a soft yellow-green glow on dark nights. */
struct FungusCluster {
    std::array<float,3> at{};       // on the wood's surface
    std::array<float,3> normal{};   // the wood's outward normal there
    float spread=0.1f;              // the clump's radius over the wood
    std::uint8_t count=8;           // its mushrooms
    std::uint32_t seed=0;
};

class GlowingFungusLayout final {
public:
    static constexpr std::size_t MaximumClusters=80;
    /** Clumps on the low flanks of the windthrown giant's trunk (when there is one), on the
     *  jungle's fallen logs (not the wet stream-bank ones, nor any near the shore) and low on
     *  the rotting standing snags' trunks; `ground` is the drawn ground's height under (x, z). */
    [[nodiscard]] static std::vector<FungusCluster> Place(const IslandTerrain& terrain,const WindthrowPlacement* windthrow,
        std::span<const FallenLogPlacement> logs,const std::function<float(float,float)>& ground,
        std::span<const StandingSnagPlacement> snags={});
};
}
