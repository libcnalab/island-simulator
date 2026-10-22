#pragma once
#include "IslandSimulator/FishSchools.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include <array>
#include <cstddef>
#include <span>
#include <vector>

namespace island {
// Small reef fish over the coral heads near the observer: a cloud of six blue-green chromis or
// three yellow tangs over each head within Reach, milling a hand or two above the coral; the
// swimmer coming within HideDistance sends them down into it, and HideSeconds after the swimmer
// has gone they come out again. Only the heads nearest the observer are peopled (chosen again
// each second). Renderer-independent, deterministic.
class ReefFishField final {
public:
    static constexpr std::size_t HeadCount=12,FishPerHead=6,Capacity=HeadCount*FishPerHead;
    static constexpr float Reach=30.0f,HideDistance=1.8f,HideSeconds=4.0f;
    static constexpr std::uint8_t ChromisVariant=4U,TangVariant=5U;
    explicit ReefFishField(std::span<const ReefHabitatPlacement> habitat);
    void Update(float deltaSeconds,float observerX,float observerY,float observerZ) noexcept;
    [[nodiscard]] std::span<const FishState> Fish() const noexcept {return {fish_.data(),count_};}
    [[nodiscard]] std::size_t HeadsPeopled() const noexcept {return chosenCount_;}
    [[nodiscard]] std::size_t HidingCount() const noexcept;
    [[nodiscard]] std::size_t HeadPool() const noexcept {return heads_.size();}
private:
    struct Head { float x=0,y=0,z=0,top=0; std::uint32_t seed=0; bool tangs=false; };
    std::vector<Head> heads_;
    std::array<std::size_t,HeadCount> chosen_{};
    std::array<float,HeadCount> hide_{},hiddenFor_{};
    std::size_t chosenCount_=0;
    std::array<FishState,Capacity> fish_{};
    std::size_t count_=0;
    float clock_=0,chooseClock_=-1.0f;
};
}
