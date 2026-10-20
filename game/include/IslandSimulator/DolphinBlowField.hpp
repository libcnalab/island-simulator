#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/DolphinPod.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// The dolphins' blows: as a dolphin's back breaks the surface a puff of spray stands up from
// the blowhole, drifts off and is gone within the second. The puffs are MistPuffs so the
// cascade's mist renderer draws them. Renderer-independent and deterministic.
class DolphinBlowField final {
public:
    static constexpr std::size_t Capacity=8;
    static constexpr float SurfaceY=-0.15f,LifeSeconds=0.9f,RiseSpeed=1.2f,BlowholeAhead=0.8f,BlowholeUp=0.25f;
    static constexpr float MinimumIntervalSeconds=2.0f;
    void Update(float deltaSeconds,float windSeconds,std::span<const DolphinState> dolphins) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t BlowCount() const noexcept {return blows_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    struct Tracker { bool wasBelow=true; float sinceBlow=10.0f; };
    std::array<MistPuff,Capacity> puffs_{};
    std::array<Tracker,DolphinPod::DolphinCount> trackers_{};
    std::uint32_t blows_=0;
};
}
