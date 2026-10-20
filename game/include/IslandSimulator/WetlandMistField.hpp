#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

/** A low mist over the seasonal wetland: ten broad faint puffs hanging a hand's breadth to
 *  a knee's height over the wet ground, breathing slowly, drifting with the trade wind and
 *  leaning in the gust fronts, each wrapping back to the windward edge as it leaves the
 *  wetland. The puffs are MistPuffs so the cascade's mist renderer draws them.
 *  Renderer-independent and deterministic. */
class WetlandMistField final {
public:
    static constexpr std::size_t Capacity=CascadeMistField::Capacity;
    static constexpr float DriftSpeed=0.10f,GustDrift=0.45f,HoverMinimum=0.12f,HoverMaximum=0.55f;
    explicit WetlandMistField(const IslandTerrain& terrain);
    /** @brief windSeconds: the gust fronts' clock. */
    void Update(float deltaSeconds,float windSeconds) noexcept;
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t WrapCount() const noexcept {return wraps_;}
    /** @brief Normalised distance from the wetland's centre: 1 at its edge. */
    [[nodiscard]] static float Extent(float x,float z) noexcept;
private:
    void Place(std::size_t index,float x,float z) noexcept;
    const IslandTerrain& terrain_;
    std::array<MistPuff,Capacity> puffs_{};
    std::array<float,Capacity> baseDensity_{},hover_{},phase_{};
    float clock_=0;
    std::uint32_t wraps_=0;
};
}
