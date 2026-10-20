#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

// Spray off the offshore rocks: each time a breaker (the ocean's own breaker phase, so the
// burst lands with the foam) reaches a rock near the observer, a few puffs of spray are
// thrown up its seaward face, rise fast, drift downwind and are gone within the second and a
// half. The puffs are MistPuffs so the cascade's mist renderer draws them.
class SurfSprayField final {
public:
    static constexpr std::size_t Capacity=24,PuffsPerBurst=3;
    static constexpr float LifeSeconds=1.4f,RiseSpeed=1.6f,Reach=90.0f;
    static constexpr float BreakerSpace=0.46f,BreakerCoast=7.0f,BreakerRate=0.31f;   // the ocean's breaker phase
    explicit SurfSprayField(const IslandTerrain& terrain) noexcept:terrain_(terrain) {}
    void SetRocks(std::span<const RockPlacement> rocks);
    // seconds: the render clock the ocean's breakers run on; the observer bounds the work.
    void Update(float deltaSeconds,float seconds,float observerX,float observerZ) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::size_t RockCount() const noexcept {return rocks_.size();}
    [[nodiscard]] std::uint32_t BurstCount() const noexcept {return bursts_;}
    [[nodiscard]] static float BreakerPhase(float offshore,float coastAngle,float seconds) noexcept {
        return offshore*BreakerSpace+coastAngle*BreakerCoast-seconds*BreakerRate;
    }
private:
    struct Rock { float x=0,z=0,top=0,radius=0.5f,offshore=0,coastAngle=0,outX=1,outZ=0; float lastSine=0; };
    void Burst(const Rock& rock) noexcept;
    const IslandTerrain& terrain_;
    std::vector<Rock> rocks_;
    std::array<MistPuff,Capacity> puffs_{};
    std::uint32_t bursts_=0;
};
}
