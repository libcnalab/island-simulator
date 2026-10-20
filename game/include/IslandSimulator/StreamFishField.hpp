#pragma once
#include "IslandSimulator/StreamCourse.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// One small fish in the plunge pool: where it holds in the pool (course distance, lateral
// fraction, depth under the surface) and the resolved position and heading it is drawn at.
struct StreamFish {
    float x=0,y=0,z=0;
    float yawRadians=0;     // heading; the fish faces upstream at rest
    float distance=0,lateral=0,depth=0;
    float holdDistance=0,holdLateral=0;   // the station it returns to
    float dartSeconds=0;    // remaining dart; 0 while holding
    float nextDartSeconds=0;
    float tailPhase=0;
    bool hiding=false;      // scattered to the deep by the walker
    bool active=false;
};

// Small fish holding station in the cascade's plunge pool: they face upstream with a slow
// sway, dart a metre and drift back now and then, and scatter to the deepest water when
// the walker comes near, returning once they are left alone. Renderer-independent.
class StreamFishField final {
public:
    static constexpr std::size_t Capacity=10;
    static constexpr float ScatterDistance=4.0f;   // walker this close sends them to the deep
    static constexpr float DartLength=1.2f,DartSeconds=0.6f,ReturnSpeed=0.35f;
    static constexpr float MinimumDepth=0.06f,MaximumDepth=0.22f;
    explicit StreamFishField(const IslandTerrain& terrain);
    void Update(float deltaSeconds,float observerX,float observerZ) noexcept;
    [[nodiscard]] std::span<const StreamFish> Fish() const noexcept {return fish_;}
    [[nodiscard]] const StreamCourse& Course() const noexcept {return course_;}
    [[nodiscard]] std::uint32_t ScatterCount() const noexcept {return scatters_;}
    [[nodiscard]] std::uint32_t DartCount() const noexcept {return darts_;}
    // The pool's course-distance span and its deepest station's distance.
    [[nodiscard]] float PoolStart() const noexcept {return poolStart_;}
    [[nodiscard]] float PoolEnd() const noexcept {return poolEnd_;}
    [[nodiscard]] float DeepDistance() const noexcept {return deepDistance_;}
    // Resolves a course distance, lateral fraction and depth to a point in the water.
    [[nodiscard]] bool Place(float distance,float lateral,float depth,float& x,float& y,float& z,float& headingRadians) const noexcept;
private:
    StreamCourse course_;
    std::array<StreamFish,Capacity> fish_{};
    float poolStart_=0,poolEnd_=0,deepDistance_=0;
    std::uint32_t scatters_=0,darts_=0;
    bool wasNear_=false;
};
}
