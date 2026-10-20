#pragma once
#include "IslandSimulator/StreamCourse.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// One puff of spray at the cascade's foot: where it hangs, how big it has grown and how far
// through its short life it is (its density follows that).
struct MistPuff {
    float x=0,y=0,z=0;
    float size=0.3f;
    float age=0;
    float density=0;    // 0..1, rising then fading over the puff's life
    bool active=false;
};

// The mist at the cascade: spray thrown up where the drop meets the pool, rising, swelling
// and thinning as it drifts downstream on the breeze, laid over downwind by a passing gust
// front. Renderer-independent and deterministic.
class CascadeMistField final {
public:
    static constexpr std::size_t Capacity=10;
    static constexpr float LifeSeconds=2.6f,ReleaseIntervalSeconds=0.26f;
    static constexpr float RiseSpeed=0.9f,DriftSpeed=0.28f,GustLean=0.9f;
    explicit CascadeMistField(const IslandTerrain& terrain);
    // windSeconds: the gust fronts' clock (the render clock the shaders use).
    void Update(float deltaSeconds,float windSeconds) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] bool Present() const noexcept {return dropHeight_>0.3f;}
    [[nodiscard]] float FootX() const noexcept {return footX_;}
    [[nodiscard]] float FootY() const noexcept {return footY_;}
    [[nodiscard]] float FootZ() const noexcept {return footZ_;}
    [[nodiscard]] float DropHeight() const noexcept {return dropHeight_;}
    [[nodiscard]] float LastGust() const noexcept {return gust_;}
private:
    void Release(std::size_t index) noexcept;
    float footX_=0,footY_=0,footZ_=0,dropHeight_=0,halfWidth_=0.5f;
    float downX_=1,downZ_=0;    // the course direction below the drop
    std::array<MistPuff,Capacity> puffs_{};
    float releaseClock_=0,gust_=0;
    std::uint32_t releases_=0;
};
}
