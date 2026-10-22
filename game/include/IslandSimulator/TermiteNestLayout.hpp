#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace island {

/** One arboreal termites' nest on a jungle tree's trunk: a dark lumpy carton of chewed wood and
 *  earth a couple of metres up, its covered mud runways running down the bark to the ground. */
struct TermiteNest {
    std::size_t tree=0;                    // the canopy tree it is on
    float localHeight=2.4f,angle=0.0f;     // up its rest mesh, and round it (mesh-local)
    std::array<float,3> bark{};            // the world point on the bark it is built on
    std::array<float,3> normal{};          // the bark's outward normal there
    float height=0.85f,width=0.62f;        // the nest's size
    float trunkRadius=0.4f;                // the trunk's world radius there
    std::uint32_t seed=0;
};

class TermiteNestLayout final {
public:
    static constexpr std::size_t MaximumCount=36;
    /** Nests on about one grown natural canopy tree in eight (not the mangroves or saplings),
     *  each on the bark's rest mesh 2.5 to 4.1 m up, facing a deterministic way round. */
    [[nodiscard]] static std::vector<TermiteNest> Place(std::span<const CanopyPlacement> canopy);
    /** A world point on a tree's rest bark (mesh-local height and angle), and its outward normal. */
    [[nodiscard]] static std::array<float,6> Bark(const CanopyPlacement& tree,float localHeight,float angle);
};
}
