#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One plastic bottle the sea washed up, lying on its side in the dry sand of the strandline. */
struct BeachBottle {
    enum class Kind : std::uint8_t {Water,Small,Soda,Crushed};
    float x=0,z=0;                 // the bottle's middle on the sand
    float groundY=0;
    float yaw=0;                   // its axis from the base toward the cap: (cos yaw, 0, sin yaw)
    float roll=0;                  // turned about its own axis
    float sink=0;                  // how deep it lies in the sand, as a fraction of its radius
    Kind kind=Kind::Water;
    std::uint8_t tint=0;           // the plastic: 0 clear, 1 blue, 2 green, 3 frosted with age
    std::uint8_t labelColour=0;
    float label=1.0f;              // how much of the label is left round it (0: none)
    bool cap=true;
    std::uint8_t capColour=0;
    [[nodiscard]] float Length() const noexcept;
    [[nodiscard]] float Radius() const noexcept;
};

class BeachLitterLayout final {
public:
    static constexpr float MinimumShore=2.5f,MaximumShore=9.5f;   // the band the high tides leave things in
    static constexpr float ObstacleClearance=0.35f;
    /** Washed-up PET bottles here and there round the island's strandline, a few dozen in all,
     *  more where the wind and the bays gather the flotsam, now and then two or three together;
     *  on dry level sand clear of the obstacles and `keepAway` metres from each `avoid` point. */
    [[nodiscard]] static std::vector<BeachBottle> Scatter(const IslandTerrain& terrain,
        std::span<const CircularObstacle> obstacles,std::span<const std::array<float,2>> avoid,
        float keepAway=5.0f);
};
}
