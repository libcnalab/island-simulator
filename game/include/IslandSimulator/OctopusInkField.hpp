#pragma once
#include "IslandSimulator/OctopusGroup.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// The ink an octopus leaves as it jets for its den: a dark cloud of a few puffs where it
// was, spreading and thinning over three seconds. DustPuffs, drawn by the rooting-dust
// renderer in ink colour.
class OctopusInkField final {
public:
    static constexpr std::size_t Capacity=12,PuffsPerCloud=4;
    static constexpr float LifeSeconds=3.0f;
    void Update(float deltaSeconds,const OctopusGroup::InkEvent& ink) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t CloudCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<DustPuff,Capacity> puffs_{};
    std::uint32_t seen_=0;
};
}
