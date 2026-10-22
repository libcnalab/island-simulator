#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace island {

/** Steam off the sun-warmed wet ground: when the sun breaks out after rain, thin wisps rise from
 *  the open, sunlit ground round the walker, swelling and thinning as they climb a metre or two
 *  and lean away downwind, until the ground has dried. The puffs are MistPuffs so the wetland
 *  mist renderer draws them. Renderer-independent and deterministic. */
class SunSteamField final {
public:
    static constexpr std::size_t Capacity=40;
    static constexpr float NearClear=3.0f,Reach=24.0f,LifeSeconds=5.5f,RiseSpeed=0.32f,DriftSpeed=0.22f;
    static constexpr float SpawnPerSecond=8.0f;   // at full strength
    /** A candidate spot: true if it is open, sunlit dry land (not water, not under the crowns),
     *  with its ground height. */
    using GroundProbe=std::function<bool(float x,float z,float& groundY)>;
    /** @brief 0..1 how strongly the ground steams: wet ground, the rain stopped, the sun out
     *  (shadowStrength, which the clouds take down) and high enough to warm it. */
    [[nodiscard]] static float Strength(float wetness,float rain,float sunOut,float sunElevationDegrees) noexcept;
    /** @brief Advances the wisps round (x, z) and starts new ones at `strength`; windX/windZ is
     *  where the breeze blows toward. */
    void Update(float deltaSeconds,float x,float z,float strength,float windX,float windZ,const GroundProbe& probe);
    [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept {return puffs_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] std::uint32_t StartedCount() const noexcept {return started_;}
private:
    std::array<MistPuff,Capacity> puffs_{};
    std::array<float,Capacity> peak_{};
    float pending_=0;
    std::uint32_t seed_=0x51ea7e11U,started_=0;
    std::size_t next_=0;
    bool warmed_=false;   // the first update runs a wisp's life ahead, so the steam is not seen starting
};
}
