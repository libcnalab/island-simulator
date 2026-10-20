#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/CrabColony.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The bubbles of the beach crabs' feeding: a crab working the strandings blows a small
 *  bubble from its mouth every couple of seconds that rises a hand's breadth and pops
 *  within the second. The bubbles are MistPuffs so the dolphin-blow mist renderer draws
 *  them in white. Renderer-independent and deterministic. */
class CrabBubbleField final {
public:
    static constexpr std::size_t Capacity=CascadeMistField::Capacity;
    static constexpr float LifeSeconds=0.8f,RiseSpeed=0.14f,IntervalSeconds=1.6f;
    void Update(float deltaSeconds,std::span<const CrabState> crabs) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t BubbleCount() const noexcept {return bubbles_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<MistPuff,Capacity> puffs_{};
    std::array<float,CrabColony::CrabCount> clocks_{};
    std::uint32_t bubbles_=0;
};
}
