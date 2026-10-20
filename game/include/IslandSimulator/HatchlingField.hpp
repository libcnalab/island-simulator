#pragma once
#include <algorithm>
#include <cmath>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

struct HatchlingNest { float x=0,z=0; };

// One turtle hatchling on its scramble: a position on the sand (or just afloat), a heading
// and a flipper phase, and whether it has reached the water and is swimming off.
struct Hatchling {
    float x=0,y=0,z=0;
    float headingRadians=0;
    float speed=0;
    float flipperPhase=0;
    float swimSeconds=0;    // time afloat; the hatchling is gone after SwimSeconds
    bool swimming=false;
    bool active=false;
};

// Turtle hatchlings: every so often a clutch breaks out of one of the nests and scrambles
// down the sand toward the sea, each a little off the others' line, wobbling and pausing;
// at the water's edge they swim out and are gone. Renderer-independent and deterministic.
class HatchlingField final {
public:
    static constexpr std::size_t Capacity=28,ClutchSize=12;
    static constexpr float FirstEmergenceSeconds=0.0f,EmergenceIntervalSeconds=140.0f;
    static constexpr float ScrambleSpeed=0.22f,SwimSpeed=0.45f,SwimSeconds=14.0f;
    explicit HatchlingField(const IslandTerrain& terrain) noexcept:terrain_(terrain) {}
    void SetNests(std::span<const HatchlingNest> nests);
    void Update(float deltaSeconds) noexcept;
    // How dark it is (0 by day .. 1 at night): once the game sets it, a clutch breaks out
    // only after dark, as hatchlings do (the cool sand, and the moonlit sea to steer by);
    // a clutch due by day waits for the night. Never set (the tests), any time will do.
    void SetNightness(float nightness) noexcept {nightness_=std::isfinite(nightness)?std::clamp(nightness,0.0f,1.0f):0.0f;}
    [[nodiscard]] std::span<const Hatchling> Hatchlings() const noexcept {return hatchlings_;}
    [[nodiscard]] std::size_t NestCount() const noexcept {return nests_.size();}
    [[nodiscard]] std::uint32_t ClutchCount() const noexcept {return clutches_;}
    [[nodiscard]] std::uint32_t ReachedSeaCount() const noexcept {return reachedSea_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    void Emerge(std::size_t nest) noexcept;
    const IslandTerrain& terrain_;
    std::vector<HatchlingNest> nests_;
    std::array<Hatchling,Capacity> hatchlings_{};
    float clock_=0,nextEmergence_=FirstEmergenceSeconds;
    float nightness_=-1;   // unset: emerge at any hour
    std::uint32_t clutches_=0,reachedSea_=0;
};
}
