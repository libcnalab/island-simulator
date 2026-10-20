#pragma once
#include "IslandSimulator/StreamCourse.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// One leaf afloat on the stream: its place along the course, its offset across the channel,
// and the resolved surface position and heading the renderer draws it at.
struct DriftingLeaf {
    float x=0,y=0,z=0;
    float yawRadians=0;
    float distance=0;       // metres along the course from the source
    float lateral=0;        // -1..1 of the channel half-width
    float spin=0;           // slow rotation on the surface
    float speed=0;          // current along-course speed, for diagnostics and tests
    std::uint8_t variant=0; // 0 green, 1 brown, 2 yellow
    bool active=false;
};

// Leaves drifting down the stream on the current: slow on the ordinary reach, hurried over
// the cascade lip and its drop, idling in the plunge pool, and gone when they reach the sea,
// when a new leaf enters near the source. Renderer-independent and deterministic.
class StreamDriftField final {
public:
    static constexpr std::size_t Capacity=14;
    static constexpr float ReachSpeed=0.32f,LipSpeed=1.1f,DropSpeed=1.8f,PoolSpeed=0.12f;
    static constexpr float LipRun=4.0f;       // metres above the lip where the water hurries
    static constexpr float SurfaceLift=0.02f;
    explicit StreamDriftField(const IslandTerrain& terrain);
    void Update(float deltaSeconds) noexcept;
    [[nodiscard]] std::span<const DriftingLeaf> Leaves() const noexcept {return leaves_;}
    [[nodiscard]] std::uint32_t PassCount() const noexcept {return passes_;}
    [[nodiscard]] const StreamCourse& Course() const noexcept {return course_;}
    // The surface speed of the current at a course distance.
    [[nodiscard]] float SpeedAt(float distance) const noexcept;
    // Resolves a course distance and lateral fraction to the water surface; false past the sea.
    [[nodiscard]] bool Surface(float distance,float lateral,float& x,float& y,float& z,float& headingRadians) const noexcept;
private:
    void Launch(std::size_t index,float distance) noexcept;
    StreamCourse course_;
    std::array<DriftingLeaf,Capacity> leaves_{};
    std::uint32_t passes_=0,launches_=0;
};
}
