#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/SeaTurtleGroup.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// A sea turtle's breath at the surface: one small puff of spray at the nose as the head
// comes up, drifting off and gone within the second. MistPuffs drawn by the cascade's mist
// renderer. Renderer-independent and deterministic.
class TurtleBreathField final {
public:
    static constexpr std::size_t Capacity=6;
    static constexpr float LifeSeconds=0.8f,RiseSpeed=0.5f,NoseAhead=0.45f;
    void Update(float deltaSeconds,float windSeconds,const SeaTurtleGroup::Breath& breath) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t BreathCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<MistPuff,Capacity> puffs_{};
    std::uint32_t seen_=0;
};
}
