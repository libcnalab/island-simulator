#pragma once
#include "IslandSimulator/RootingDustField.hpp"
#include "IslandSimulator/SeaTurtleGroup.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The sand a grazing turtle stirs: at each bite it takes on the bottom a puff of sand
 *  rises at its beak, drifts a little and settles within two seconds. The puffs are
 *  DustPuffs so the stingrays' sand renderer draws them. Renderer-independent and
 *  deterministic. */
class TurtleGrazeField final {
public:
    static constexpr std::size_t Capacity=12;
    static constexpr float LifeSeconds=1.8f,RiseSpeed=0.25f;
    /** @brief The last bite the group noted; a new count raises a puff at it. */
    void Update(float deltaSeconds,const SeaTurtleGroup::Bite& bite) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t BiteCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    std::array<DustPuff,Capacity> puffs_{};
    std::uint32_t seen_=0;
};
}
