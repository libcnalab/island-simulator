#pragma once
#include "IslandSimulator/RootingDustField.hpp"
#include "IslandSimulator/StingrayGroup.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// The sand a settled stingray throws up as it bursts away from the swimmer: a ring of puffs
// where it lay, rising slowly through the water and gone within two seconds. The puffs are
// DustPuffs so the rooting-dust renderer draws them in sand colour.
class StingraySandField final {
public:
    static constexpr std::size_t Capacity=12,PuffsPerBurst=5;
    static constexpr float LifeSeconds=1.8f,RiseSpeed=0.3f;
    void Update(float deltaSeconds,const StingrayGroup::StingrayBurst& burst) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t BurstCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<DustPuff,Capacity> puffs_{};
    std::uint32_t seen_=0;
};
}
