#pragma once
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

/** @brief One animal running this frame: where it is, which way, how fast, how big, and
 *  whether the ground under it is dry enough to raise dust. */
struct HoofDustRunner { float x=0,y=0,z=0,headingRadians=0,speed=0,scale=1; bool dryGround=false; };

/** The dust a running animal kicks up: a boar trotting off, a deer bounding away, a goat
 *  sidestepping — on dry sand or bare soil, puffs of earth rise behind the hooves every
 *  few strides, drift on the breeze and are gone within the second. The puffs are
 *  DustPuffs so the rooting-dust renderer draws them. Renderer-independent, deterministic. */
class HoofDustField final {
public:
    static constexpr std::size_t Capacity=RootingDustField::Capacity,MaximumRunners=16;
    static constexpr float LifeSeconds=0.9f,ReleaseIntervalSeconds=0.16f,MinimumSpeed=1.4f,HoofBehind=0.45f;
    explicit HoofDustField(const IslandTerrain& terrain) noexcept:terrain_(terrain) {}
    /** @brief windSeconds: the gust fronts' clock; runners: the animals to consider this frame,
     *  the same order each frame (the release clocks are per slot). */
    void Update(float deltaSeconds,float windSeconds,std::span<const HoofDustRunner> runners) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    const IslandTerrain& terrain_;
    std::array<DustPuff,Capacity> puffs_{};
    std::array<float,MaximumRunners> clocks_{};
    std::uint32_t releases_=0;
};
}
