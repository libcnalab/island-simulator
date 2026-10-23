#pragma once
#include "IslandSimulator/FruitBatColony.hpp"
#include "IslandSimulator/GeckoPopulation.hpp"
#include "IslandSimulator/TermiteNestLayout.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace island {

/** One winged termite (an alate) in the nuptial flight. */
struct Alate {
    float x=0,y=0,z=0;
    float heading=0;     // the way it flies
    float flap=0;        // the wing beat, radians
    float scale=1;       // 0 as it leaves the nest's mouth or flies out of sight
};

/** At dusk after rain the arboreal termites' nests let out their winged reproductives: a
 *  fluttering column of alates rising from each nest's top and drifting away on the air. The
 *  island's small insect-eating bats come out of the mine and circle the columns, swooping
 *  through them, and a gecko waits on the trunk below each nest's mouth, snapping at those that
 *  stumble on their first flight. Deterministic in the clock (no state of its own): everything
 *  is worked out from the nests and the time, only for the nests near the eye. */
class TermiteSwarm final {
public:
    static constexpr int AlatesPerNest=220;
    static constexpr int BatsPerNest=2;
    static constexpr float Reach=45.0f;   // the nests swarming within this of the eye are worked out

    /** How strongly the nests are swarming (0..1): the hour round sunset (from the sun's
     *  elevation: from 5 degrees up, at full from 1 degree up to 5 down, over by 9 down) times
     *  the wet ground after rain (from 0.3 wetness, full at 0.7). */
    [[nodiscard]] static float Strength(float sunElevationDegrees,float wetness) noexcept;

    /** Appends the swarming alates, the hunting bats and the waiting geckos round the nests
     *  within Reach of the eye. */
    static void Gather(float seconds,float strength,std::span<const TermiteNest> nests,float eyeX,float eyeY,float eyeZ,
                       std::vector<Alate>& alates,std::vector<FruitBatState>& bats,std::vector<GeckoState>& geckos);

    /** The nest's mouth on top of it, where the alates come out. */
    [[nodiscard]] static std::array<float,3> Mouth(const TermiteNest& nest) noexcept;
};

}
