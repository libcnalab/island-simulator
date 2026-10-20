#pragma once
#include "IslandSimulator/MonkeyTroop.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

// Fruit knocked loose while a monkey picks in a fruiting crown: it drops from a lobe of that
// tree, lands on the ground below and lies there a while before it is gone. Renderer-independent.
struct FallingFruit {
    float x=0,y=0,z=0;
    float velocityY=0;
    float velocityX=0,velocityZ=0;   // the bounce and the roll downhill after it
    bool rolling=false;              // on the ground but still moving
    bool afloat=false;               // a coconut that struck the shallows, drifting on the sea
    float restSeconds=0;     // time lying on the ground; negative while still falling
    float groundY=0;
    std::uint8_t variant=0;  // canopy form: 2 orange, 5 purple; 9 a coconut from a palm
    bool active=false;
};

class FallingFruitField final {
public:
    static constexpr std::size_t Capacity=24;
    static constexpr float DropIntervalSeconds=4.5f;
    static constexpr float RestSeconds=90.0f;
    static constexpr float Gravity=9.8f;
    static constexpr std::uint8_t CoconutVariant=9;
    // A gust front over a coconut palm may shake a nut loose: this fraction of the fronts
    // that pass a palm within CoconutRange of the observer drop one.
    static constexpr float CoconutChance=0.10f;
    static constexpr float CoconutRange=80.0f;
    static constexpr float CoconutRestSeconds=240.0f;
    explicit FallingFruitField(const IslandTerrain& terrain) noexcept : terrain_(terrain) {}
    void SetPalms(std::span<const PalmPlacement> palms) noexcept { palms_=palms; }
    void Update(float deltaSeconds,std::span<const MonkeyState> monkeys) noexcept;
    // Palms shed in gusts near the observer; seconds is the world clock the gust field uses.
    void UpdateCoconuts(float seconds,float observerX,float observerZ) noexcept;
    /** @brief Drops one nut from this palm's crown now (the gust path and the tests use it). */
    void ShakeCoconut(const PalmPlacement& palm,std::uint32_t seed) noexcept;
    /** @brief Coconuts that struck the shallows and later washed up on the sand. */
    [[nodiscard]] std::uint32_t BeachedCount() const noexcept {return beached_;}
    static constexpr float DriftSpeed=0.22f,AfloatLimitSeconds=480.0f;
    [[nodiscard]] std::uint32_t CoconutDropCount() const noexcept {return coconutDrops_;}
    /** @brief The last coconut to hit the ground, counted so a landing is noted once. */
    struct Landing { float x=0,y=0,z=0; std::uint32_t count=0; };
    [[nodiscard]] const Landing& LastCoconutLanding() const noexcept {return landing_;}
    [[nodiscard]] std::span<const FallingFruit> Fruit() const noexcept {return fruit_;}
    /** @brief Removes one fruit (eaten); indices are positions in Fruit(). */
    void Consume(std::size_t index) noexcept { if(index<fruit_.size()&&fruit_[index].active) { fruit_[index].active=false; ++consumed_; } }
    [[nodiscard]] std::uint32_t ConsumedCount() const noexcept {return consumed_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] std::uint32_t DropCount() const noexcept {return drops_;}
    /** @brief How far the last fruit to come to rest rolled from where it first struck. */
    [[nodiscard]] float LastRollDistance() const noexcept {return lastRoll_;}
    static constexpr float RollStopSpeed=0.05f;
    static constexpr float RollPull=0.25f,RollDrag=2.2f,RollResistance=0.4f,RollSeconds=3.0f;
private:
    void Drop(const MonkeyState& monkey) noexcept;
    FallingFruit* FreeSlot() noexcept;
    const IslandTerrain& terrain_;
    std::span<const PalmPlacement> palms_;
    std::vector<float> palmGust_;       // last gust strength seen at each palm
    std::uint32_t coconutDrops_=0;
    std::uint32_t consumed_=0;
    std::uint32_t beached_=0;
    Landing landing_;
    std::array<FallingFruit,Capacity> fruit_{};
    std::array<float,16> pickTimers_{};
    std::uint32_t drops_=0;
    float lastRoll_=0;
    std::array<float,Capacity> strikeX_{},strikeZ_{};
    [[nodiscard]] float GroundOffset(const FallingFruit& f) const noexcept {return f.variant==CoconutVariant?0.10f:0.06f;}
    void SlopeAt(float x,float z,float& slopeX,float& slopeZ) const noexcept;
};
}
