#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

namespace island {
// One of the walker's bare footprints: where it was pressed, which way, which foot, and how
// long ago (the sand takes it back over the trail's life).
struct Footprint {
    float x=0,z=0;
    float headingRadians=0;
    float age=0;
    float density=0;     // 1 fresh, fading to 0 as the print is lost
    bool left=false;
    bool active=false;
    std::uint8_t kind=0;   // 0 the walker's bare foot, 1 a cloven hoof
    float life=240.0f;     // how long the ground keeps this print (the swash takes wet sand's sooner)
    float strength=1.0f;   // how dark it was laid (a wet foot's print on planks, as wet as the foot)
    float y=std::numeric_limits<float>::quiet_NaN();   // laid on planks at this height; NaN: on the ground
};

// The walker's trail on the sand: a print every stride while walking on sand, alternating
// feet either side of the line of travel, the oldest fading and giving way to the newest.
// Renderer-independent and deterministic.
class FootprintTrail final {
public:
    static constexpr std::size_t Capacity=48;
    static constexpr float Stride=0.62f,FootAcross=0.11f,LifeSeconds=240.0f,MinimumSpeed=0.3f;
    // Prints on the wet sand at the water's edge are taken back by the swash within this.
    static constexpr float WetLifeSeconds=45.0f;
    // onSand: the walker's feet are on sand this frame (not swimming, not on a deck);
    // wetSand: that sand is the wet strip at the water's edge, whose prints go sooner.
    /** @brief deckY: finite when the feet are on planks (the jetty, the bridge, the lookout), whose
     *  prints are only the wet marks of feet come out of the water: laid at that height, as dark
     *  as deckWetness (0..1, none below 0.05), and drying off in DeckLifeSeconds. */
    void Update(float deltaSeconds,float x,float z,float velocityX,float velocityZ,bool onSand,bool wetSand=false,
                float deckY=std::numeric_limits<float>::quiet_NaN(),float deckWetness=0.0f) noexcept;
    static constexpr float DeckLifeSeconds=30.0f;
    [[nodiscard]] std::span<const Footprint> Prints() const noexcept {return prints_;}
    [[nodiscard]] std::uint32_t PrintCount() const noexcept {return laid_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    /** @brief The print this trail lays (0 bare foot, 1 cloven hoof) and its stride: the
     *  deer's and boars' hooves use the same trail with their own. */
    void SetKind(std::uint8_t kind) noexcept {kind_=kind;}
    void SetStride(float stride) noexcept {if(std::isfinite(stride)&&stride>0.05f) stride_=stride;}
    /** @brief The rain (0..1) beating the prints out: they age 1 + RainErosion times as fast in
     *  a downpour (a dry print lasts some forty seconds instead of four minutes). */
    static constexpr float RainErosion=5.0f;
    void SetRain(float rain) noexcept {rain_=std::isfinite(rain)?std::clamp(rain,0.0f,1.0f):0.0f;}
    [[nodiscard]] std::uint8_t Kind() const noexcept {return kind_;}
private:
    std::array<Footprint,Capacity> prints_{};
    std::uint8_t kind_=0;
    float stride_=Stride;
    float rain_=0;
    float travelled_=0;
    bool nextLeft_=true;
    std::uint32_t laid_=0;
};
}
