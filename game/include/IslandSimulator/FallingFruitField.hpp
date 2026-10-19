#pragma once
#include "IslandSimulator/MonkeyTroop.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

// Fruit knocked loose while a monkey picks in a fruiting crown: it drops from a lobe of that
// tree, lands on the ground below and lies there a while before it is gone. Renderer-independent.
struct FallingFruit {
    float x=0,y=0,z=0;
    float velocityY=0;
    float restSeconds=0;     // time lying on the ground; negative while still falling
    float groundY=0;
    std::uint8_t variant=0;  // canopy form: 2 orange, 5 purple
    bool active=false;
};

class FallingFruitField final {
public:
    static constexpr std::size_t Capacity=24;
    static constexpr float DropIntervalSeconds=4.5f;
    static constexpr float RestSeconds=90.0f;
    static constexpr float Gravity=9.8f;
    explicit FallingFruitField(const IslandTerrain& terrain) noexcept : terrain_(terrain) {}
    void Update(float deltaSeconds,std::span<const MonkeyState> monkeys) noexcept;
    [[nodiscard]] std::span<const FallingFruit> Fruit() const noexcept {return fruit_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] std::uint32_t DropCount() const noexcept {return drops_;}
private:
    void Drop(const MonkeyState& monkey) noexcept;
    const IslandTerrain& terrain_;
    std::array<FallingFruit,Capacity> fruit_{};
    std::array<float,16> pickTimers_{};
    std::uint32_t drops_=0;
};
}
