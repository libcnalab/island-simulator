#pragma once
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** @brief One spot of exposed dry sand the wind can lift: on the windward beaches, a few
 *  metres above the wet line. */
struct BlownSandSource { float x=0,y=0,z=0,phase=0; };

/** Sand blown off the dry beach: while a gust front passes an exposed dry-sand source, pale
 *  puffs stream off it downwind, lifting a hand's height, swelling and gone within two
 *  seconds; between fronts the beach lies still. The puffs are DustPuffs so the rooting-dust
 *  renderer draws them in sand colour. Renderer-independent and deterministic. */
class BlownSandField final {
public:
    static constexpr std::size_t Capacity=48,MaximumSources=160;
    static constexpr float LifeSeconds=1.6f,ReleaseIntervalSeconds=0.3f,GustThreshold=0.55f;
    static constexpr float DriftSpeed=2.4f,RiseSpeed=0.12f,Reach=70.0f,MinimumExposure=0.5f;
    explicit BlownSandField(const IslandTerrain& terrain);
    /** @brief windSeconds: the gust fronts' clock; sources within Reach of the camera blow. */
    void Update(float deltaSeconds,float windSeconds,float cameraX,float cameraZ) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::span<const BlownSandSource> Sources() const noexcept {return sources_;}
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    const IslandTerrain& terrain_;
    std::vector<BlownSandSource> sources_;
    std::vector<float> clocks_;
    std::array<DustPuff,Capacity> puffs_{};
    std::uint32_t releases_=0;
};
}
