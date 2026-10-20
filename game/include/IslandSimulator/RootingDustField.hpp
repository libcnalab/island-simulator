#pragma once
#include "IslandSimulator/BoarHerd.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

// One puff of soil thrown up by a rooting boar: where it hangs, its size and its density
// over a short life.
struct DustPuff {
    float x=0,y=0,z=0;
    float size=0.12f;
    float age=0;
    float density=0;
    bool active=false;
};

// The dust of the boars' rooting: while a boar stands with its snout among the roots a puff
// of soil is thrown up at the snout every so often, rising a little, swelling, drifting on
// the breeze and gone within the second. Renderer-independent and deterministic.
class RootingDustField final {
public:
    static constexpr std::size_t Capacity=18;
    static constexpr float LifeSeconds=1.1f,ReleaseIntervalSeconds=0.42f;
    static constexpr float SnoutAhead=0.95f,SnoutHeight=0.18f;
    explicit RootingDustField(const IslandTerrain& terrain) noexcept:terrain_(terrain) {}
    // windSeconds: the gust fronts' clock. Boars rooting (pose above 0.8) release puffs.
    void Update(float deltaSeconds,float windSeconds,std::span<const BoarState> boars) noexcept;
    [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
private:
    const IslandTerrain& terrain_;
    std::array<DustPuff,Capacity> puffs_{};
    std::array<float,BoarHerd::BoarCount> clocks_{};
    std::uint32_t releases_=0;
};
}
