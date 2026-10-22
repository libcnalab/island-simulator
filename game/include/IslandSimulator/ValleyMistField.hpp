#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include <array>
#include <cstddef>
#include <span>

namespace island {
class IslandTerrain;

/** Morning mist in the stream's valley: after a dewy or a rained-on night, broad faint banks
 *  lie over the stream at first light, a knee's to a head's height over the ground, drifting
 *  slowly down the valley, and burn off as the sun climbs (gone by half past nine). The
 *  puffs are MistPuffs so the cascade's mist renderer draws them. Renderer-independent and
 *  deterministic. */
class ValleyMistField final {
public:
    static constexpr std::size_t Capacity=CascadeMistField::Capacity;
    static constexpr float DriftSpeed=0.12f,HoverMinimum=0.4f,HoverMaximum=1.8f;
    explicit ValleyMistField(const IslandTerrain& terrain);
    /** @brief hour: the clock's; dew and wetness (0..1): the night's dew and the rain's. */
    void Update(float deltaSeconds,float hour,float dew,float wetness) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    /** @brief 0..1 how thick the mist lies at an hour after a night of this dew and wetness. */
    [[nodiscard]] static float Strength(float hour,float dew,float wetness) noexcept;
private:
    void Place(std::size_t index,float along) noexcept;
    const IslandTerrain& terrain_;
    std::array<MistPuff,Capacity> puffs_{};
    std::array<float,Capacity> along_{},side_{},hover_{},baseDensity_{},phase_{};
    float clock_=0;
};
}
