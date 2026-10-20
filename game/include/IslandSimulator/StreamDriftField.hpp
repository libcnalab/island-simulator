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
    bool atSea=false;       // carried out of the mouth, drifting on the lagoon
    float seaSeconds=0;
    bool beached=false;     // washed up on the strand, lying on the sand
    float beachSeconds=0;
};

// Leaves drifting down the stream on the current: slow on the ordinary reach, hurried over
// the cascade lip and its drop, idling in the plunge pool, and gone when they reach the sea,
// when a new leaf enters near the source. Renderer-independent and deterministic.
class StreamDriftField final {
public:
    static constexpr std::size_t Capacity=14;
    static constexpr float ReachSpeed=0.32f,LipSpeed=1.1f,DropSpeed=1.8f,PoolSpeed=0.12f;
    // Out of the mouth a leaf drifts on the lagoon downwind for a while before it is lost.
    static constexpr float SeaDriftSpeed=0.2f,SeaSeconds=75.0f;
    // A leaf on the lagoon that drifts onto rising sand washes up and lies there a while.
    static constexpr float BeachSeconds=120.0f;
    [[nodiscard]] std::uint32_t BeachedCount() const noexcept {return beached_;}
    static constexpr float LipRun=4.0f;       // metres above the lip where the water hurries
    static constexpr float SurfaceLift=0.02f;
    explicit StreamDriftField(const IslandTerrain& terrain);
    void Update(float deltaSeconds) noexcept;
    [[nodiscard]] std::span<const DriftingLeaf> Leaves() const noexcept {return leaves_;}
    [[nodiscard]] std::uint32_t PassCount() const noexcept {return passes_;}
    /** @brief A leaf (or feather) that landed on the water at a course distance and lateral
     *  fraction joins the drift with its own colour, in a free slot or in the place of the
     *  leaf furthest along; false only when it cannot be placed on the water. */
    bool Adopt(float distance,float lateral,std::uint8_t variant) noexcept;
    [[nodiscard]] std::uint32_t AdoptCount() const noexcept {return adoptions_;}
    [[nodiscard]] std::size_t SeaCount() const noexcept;
    [[nodiscard]] const StreamCourse& Course() const noexcept {return course_;}
    // The surface speed of the current at a course distance.
    [[nodiscard]] float SpeedAt(float distance) const noexcept;
    // Resolves a course distance and lateral fraction to the water surface; false past the sea.
    [[nodiscard]] bool Surface(float distance,float lateral,float& x,float& y,float& z,float& headingRadians) const noexcept;
private:
    void Launch(std::size_t index,float distance) noexcept;
    const IslandTerrain& terrain_;
    StreamCourse course_;
    std::array<DriftingLeaf,Capacity> leaves_{};
    std::uint32_t passes_=0,launches_=0,adoptions_=0,beached_=0;
};
}
