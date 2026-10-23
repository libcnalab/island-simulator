#pragma once
#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One stack of the islet: a mass of dark rock out of the sea, steep-sided and flat-topped. */
struct IsletRock {
    float x=0,z=0,radius=4,height=6;
    std::uint32_t seed=0;
};

/** A rocky islet some 60 m off a headland where brown boobies nest: three or four stacks of
 *  basalt standing out of the sea, their tops white with guano. */
struct SeabirdIslet {
    float x=0,z=0;                       // the middle of the group
    float towardShoreX=0,towardShoreZ=0; // unit direction back to the island
    std::vector<IsletRock> rocks;
    /** The rocks' surface over (x, z) (the sea's floor where no rock stands: -4). */
    [[nodiscard]] float SurfaceY(float x,float z) const noexcept;
    /** How far the rocks' tops are level there (1 flat, 0 a cliff): where a bird can stand. */
    [[nodiscard]] float Flatness(float x,float z) const noexcept;
    [[nodiscard]] std::vector<CircularObstacle> Obstacles() const;
};

class SeabirdIsletLayout final {
public:
    static constexpr float SeaFloor=-4.0f;
    static constexpr float MinimumOffshore=48.0f,MaximumOffshore=70.0f;
    /** Off the most headland-like stretch of rocky coast, on the shelf in water some 2 m deep across the
     *  islet, at least `keepAway` metres from each of the `avoid` points (the wrecks, the jetty). */
    [[nodiscard]] static std::optional<SeabirdIslet> FindInWorld(const IslandTerrain& terrain,
        std::span<const std::array<float,2>> avoid,float keepAway=45.0f);
};

/** The islet's brown boobies: a pair or a lone bird on each nest site on the flat tops, standing,
 *  sitting on the nest, preening, stretching a wing, pointing the bill to the sky; and a few in
 *  the air round the islet, gliding on long pointed wings and now and then folding them to
 *  plunge into the sea. Deterministic in the clock: no state of its own. */
class BoobyColony final {
public:
    static constexpr std::size_t MaximumNests=16,FlyingCount=5;
    explicit BoobyColony(const SeabirdIslet& islet);
    [[nodiscard]] std::span<const std::array<float,4>> Nests() const noexcept {return nests_;}   // x, y, z, heading
    /** The colony's birds at the clock's `seconds` (written into `out`, which is cleared). */
    void At(float seconds,std::vector<BirdState>& out) const;
private:
    const SeabirdIslet* islet_;
    std::vector<std::array<float,4>> nests_;
};

}
