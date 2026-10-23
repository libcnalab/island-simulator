#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class ElephantMotion;

/** The water an elephant throws about bathing in the sea: each shower it swings over its back
 *  bursts from the trunk's tip as a fan of spray flying up and over the body and falling on it
 *  (MistPuffs, for the mist renderer), and foam spreads on the surface round its body where it
 *  wades, lies down and where the shower comes down (DustPuffs, for the fin-wake renderer).
 *  Renderer-independent and deterministic. */
class ElephantSplashField final {
public:
    static constexpr std::size_t SprayCapacity=24,FoamCapacity=RootingDustField::Capacity;
    static constexpr float SprayLifeSeconds=1.3f,FoamLifeSeconds=3.0f;
    void Update(float deltaSeconds,const ElephantMotion& elephant) noexcept;
    [[nodiscard]] std::span<const MistPuff> Spray() const noexcept {return spray_;}
    [[nodiscard]] std::span<const DustPuff> Foam() const noexcept {return foam_;}
    [[nodiscard]] std::size_t ActiveSpray() const noexcept;
    [[nodiscard]] std::size_t ActiveFoam() const noexcept;
private:
    void AddFoam(float x,float z,float size) noexcept;
    std::array<MistPuff,SprayCapacity> spray_{};
    std::array<std::array<float,3>,SprayCapacity> velocity_{};
    std::array<DustPuff,FoamCapacity> foam_{};
    std::uint32_t seenSprays_=0,released_=0;
    float foamClock_=0;
    bool primed_=false;
};
}
