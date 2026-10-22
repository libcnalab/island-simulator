#pragma once
#include "IslandSimulator/FishSchools.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <vector>

namespace island {
// A sea anemone on the seabed beside a coral head: where it stands, its size, and the surge's
// heading and phase that rock its tentacles.
struct ReefAnemone { float x=0,y=0,z=0,radius=0.12f,surgeHeading=0,phase=0; };

// Small reef fish over the coral heads near the observer: a cloud of six blue-green chromis or
// three yellow tangs over each head within Reach, milling a hand or two above the coral; the
// swimmer coming within HideDistance sends them down into it, and HideSeconds after the swimmer
// has gone they come out again. Only the heads nearest the observer are peopled (chosen again
// each second). Renderer-independent, deterministic.
class ReefFishField final {
public:
    static constexpr std::size_t HeadCount=12,FishPerHead=6,ClownfishPerAnemone=2,
        Capacity=HeadCount*(FishPerHead+ClownfishPerAnemone);
    static constexpr float Reach=30.0f,HideDistance=1.8f,HideSeconds=4.0f;
    static constexpr std::uint8_t ChromisVariant=4U,TangVariant=5U,ClownfishVariant=6U;
    // About one coral head in six has a sea anemone beside it, and in it a pair of clownfish
    // that hide among its tentacles from a swimmer within ClownHideDistance.
    static constexpr float AnemoneShare=0.18f,ClownHideDistance=1.2f;
    explicit ReefFishField(std::span<const ReefHabitatPlacement> habitat);
    void Update(float deltaSeconds,float observerX,float observerY,float observerZ) noexcept;
    [[nodiscard]] std::span<const FishState> Fish() const noexcept {return {fish_.data(),count_};}
    [[nodiscard]] std::size_t HeadsPeopled() const noexcept {return chosenCount_;}
    [[nodiscard]] std::size_t HidingCount() const noexcept;
    [[nodiscard]] std::size_t HeadPool() const noexcept {return heads_.size();}
    // Every anemone of the reef (static), nearest first is not implied: the renderer culls.
    [[nodiscard]] std::span<const ReefAnemone> Anemones() const noexcept {return anemones_;}
private:
    struct Head { float x=0,y=0,z=0,top=0; std::uint32_t seed=0; bool tangs=false; int anemone=-1; };
    std::vector<Head> heads_;
    std::vector<ReefAnemone> anemones_;
    std::array<std::size_t,HeadCount> chosen_{};
    std::array<float,HeadCount> hide_{},hiddenFor_{},clownHide_{};
    std::size_t chosenCount_=0;
    std::array<FishState,Capacity> fish_{};
    std::size_t count_=0;
    float clock_=0,chooseClock_=-1.0f;
};
}
