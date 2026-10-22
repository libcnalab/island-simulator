#pragma once
#include "IslandSimulator/CrocodileGroup.hpp"
#include "IslandSimulator/HermitCrabGroup.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// One short piece of the line a hermit crab's shell drags in the sand, from where the crab
// was to where it is, fading as the sand dries and the wind levels it.
struct DragSegment {
    float x0=0,z0=0,x1=0,z1=0;
    float age=0;
    float density=0;
    bool active=false;
    float halfWidth=0.011f;   // a hermit's shell line; a crocodile's belly and tail drag is wide
};

// The hermit crabs' drag lines: every few centimetres a foraging crab moves, a segment is
// laid behind it; a line lasts a minute and a half. Renderer-independent, deterministic.
class ShellDragTrail final {
public:
    static constexpr std::size_t Capacity=200;
    static constexpr float SegmentLength=0.08f,LifeSeconds=90.0f,MinimumSpeed=0.015f;
    void Update(float deltaSeconds,std::span<const HermitCrabState> crabs) noexcept;
    /** @brief The rain (0..1): the grooves age as the footprints do in it (FootprintTrail::RainErosion). */
    void SetRain(float rain) noexcept {rain_=std::isfinite(rain)?std::clamp(rain,0.0f,1.0f):0.0f;}
    [[nodiscard]] std::span<const DragSegment> Segments() const noexcept {return segments_;}
    [[nodiscard]] std::uint32_t LaidCount() const noexcept {return laid_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    struct Tracker { float x=0,z=0; bool valid=false; };
    std::array<DragSegment,Capacity> segments_{};
    std::array<Tracker,HermitCrabGroup::Count> trackers_{};
    std::uint32_t laid_=0;
    float rain_=0;
};

// The crocodiles' slides on the estuary's sand and mud: hauled out and crawling, a crocodile's
// belly and tail drag a broad groove behind it, a piece every quarter metre; the sand keeps it
// five minutes (the swash or the river's rise takes it then). Renderer-independent.
class CrocodileDragTrail final {
public:
    static constexpr std::size_t Capacity=160;
    static constexpr float SegmentLength=0.25f,LifeSeconds=300.0f,MinimumSpeed=0.04f,HalfWidth=0.07f;
    // dryGround: the ground under each crocodile this frame takes a mark (sand or mud, not water)
    void Update(float deltaSeconds,std::span<const CrocodileState> crocodiles,std::span<const bool> dryGround) noexcept;
    void SetRain(float rain) noexcept {rain_=std::isfinite(rain)?std::clamp(rain,0.0f,1.0f):0.0f;}
    [[nodiscard]] std::span<const DragSegment> Segments() const noexcept {return segments_;}
    [[nodiscard]] std::uint32_t LaidCount() const noexcept {return laid_;}
private:
    struct Tracker { float x=0,z=0; bool valid=false; };
    std::array<DragSegment,Capacity> segments_{};
    std::array<Tracker,CrocodileGroup::CrocodileCount> trackers_{};
    std::uint32_t laid_=0;
    float rain_=0;
};
}
