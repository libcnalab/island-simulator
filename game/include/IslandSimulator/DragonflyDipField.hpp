#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/DragonflySwarm.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The fleck of spray a dragonfly's dip to the water throws: one small puff at the touch,
 *  rising a hand's breadth and gone within the second. The puffs are MistPuffs so the
 *  cascade's mist renderer draws them. Renderer-independent and deterministic. */
class DragonflyDipField final {
public:
    static constexpr std::size_t Capacity=CascadeMistField::Capacity;
    static constexpr float LifeSeconds=0.7f,RiseSpeed=0.25f;
    void Update(float deltaSeconds,const DragonflySwarm::Dip& dip) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t DipCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<MistPuff,Capacity> puffs_{};
    std::uint32_t seen_=0;
};
}
