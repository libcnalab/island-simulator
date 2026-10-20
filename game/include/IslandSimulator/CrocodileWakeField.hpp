#pragma once
#include "IslandSimulator/CrocodileGroup.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The wake a swimming crocodile draws on the estuary: a faint puff of foam laid at the
 *  snout every third of a second while a crocodile is under way at the surface, spreading
 *  and fading on the water behind it within two and a half seconds; nothing from a
 *  crocodile on the bank, lying still in the water, or simulated dormant. The puffs are
 *  DustPuffs so the fin-wake renderer draws them in foam white. Renderer-independent and
 *  deterministic. */
class CrocodileWakeField final {
public:
    static constexpr std::size_t Capacity=RootingDustField::Capacity;
    static constexpr float LifeSeconds=2.5f,ReleaseIntervalSeconds=0.33f,MinimumSpeed=0.15f;
    void Update(float deltaSeconds,std::span<const CrocodileState> crocodiles) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] static bool Cutting(const CrocodileState& crocodile) noexcept;
private:
    std::array<DustPuff,Capacity> puffs_{};
    std::array<float,CrocodileGroup::CrocodileCount> clocks_{};
    std::uint32_t releases_=0;
};
}
