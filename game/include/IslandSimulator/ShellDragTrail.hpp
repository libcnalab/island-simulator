#pragma once
#include "IslandSimulator/HermitCrabGroup.hpp"
#include <array>
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
};

// The hermit crabs' drag lines: every few centimetres a foraging crab moves, a segment is
// laid behind it; a line lasts a minute and a half. Renderer-independent, deterministic.
class ShellDragTrail final {
public:
    static constexpr std::size_t Capacity=200;
    static constexpr float SegmentLength=0.08f,LifeSeconds=90.0f,MinimumSpeed=0.015f;
    void Update(float deltaSeconds,std::span<const HermitCrabState> crabs) noexcept;
    [[nodiscard]] std::span<const DragSegment> Segments() const noexcept {return segments_;}
    [[nodiscard]] std::uint32_t LaidCount() const noexcept {return laid_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    struct Tracker { float x=0,z=0; bool valid=false; };
    std::array<DragSegment,Capacity> segments_{};
    std::array<Tracker,HermitCrabGroup::Count> trackers_{};
    std::uint32_t laid_=0;
};
}
