#pragma once
#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The mud a boar flings shaking on leaving its wallow: at each new shake a ring of
 *  flecks leaves the flanks sideways, arcs out and falls to the ground within the second.
 *  The flecks are DustPuffs so the rooting-dust renderer draws them in soil brown.
 *  Renderer-independent and deterministic. */
class MudFleckField final {
public:
    static constexpr std::size_t Capacity=RootingDustField::Capacity;
    static constexpr std::size_t FlecksPerShake=8;
    static constexpr float LifeSeconds=0.8f,FlingSpeed=1.6f,Gravity=4.0f;
    void Update(float deltaSeconds,const BoarHerd::Shake& shake) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t ShakeCount() const noexcept {return seen_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    struct Motion { float vx=0,vy=0,vz=0; };
    std::array<DustPuff,Capacity> puffs_{};
    std::array<Motion,Capacity> motion_{};
    std::uint32_t seen_=0;
};
}
