#pragma once
#include "IslandSimulator/RootingDustField.hpp"
#include "IslandSimulator/SharkPatrol.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
/** The foam a shark's dorsal fin cuts when the patrol rides just under the surface: a
 *  puff of white laid at the fin every fifth of a second, spreading and fading on the
 *  water behind it within three seconds; nothing while the shark cruises deep or is away.
 *  The puffs are DustPuffs so the rooting-dust renderer draws them in foam white.
 *  Renderer-independent and deterministic. */
class FinWakeField final {
public:
    static constexpr std::size_t Capacity=RootingDustField::Capacity;
    static constexpr float LifeSeconds=3.0f,ReleaseIntervalSeconds=0.2f,CuttingDepth=-1.05f;
    /** @brief The shark this frame; the fin cuts the water while it is present, visible and
     *  riding above CuttingDepth. */
    void Update(float deltaSeconds,const SharkState& shark) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] static bool Cutting(const SharkState& shark) noexcept;
private:
    std::array<DustPuff,Capacity> puffs_{};
    float clock_=0;
    std::uint32_t releases_=0;
};
}
