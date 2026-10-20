#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
class JungleLayout;

// One fallen twig on the forest floor: where it lies, which way, how long and thick, and
// how much it bends.
struct ForestTwigPlacement {
    float x=0,y=0,z=0;
    float yawRadians=0;
    float length=0.5f,radius=0.015f,bend=0.15f;
    std::uint8_t variant=0;
};

// Twigs scattered under the crowns: up to two for each canopy tree, a metre or three from
// the trunk on open forest floor, off the path and the stream. Deterministic.
class ForestTwigLayout final {
public:
    static constexpr std::size_t Capacity=240;
    static constexpr float PathClearance=3.0f,StreamClearance=2.5f,MinimumShoreDistance=20.0f;
    ForestTwigLayout(const IslandTerrain& terrain,const JungleLayout& jungle);
    [[nodiscard]] std::span<const ForestTwigPlacement> Twigs() const noexcept {return twigs_;}
    [[nodiscard]] static bool IsForestFloor(const IslandTerrain& terrain,float x,float z) noexcept;
private:
    std::vector<ForestTwigPlacement> twigs_;
};
}
