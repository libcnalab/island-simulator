#pragma once

#include "IslandSimulator/QuadBike.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    /** One wheel of the quad this frame, as its traces need it: where its tyre meets the
     *  ground, how wide the tyre is, how hard it presses, and what it runs on. */
    struct WheelOnGround
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float width = 0.2f;
        float load = 0.0f;          // 0..1 spring compression
        bool down = false;          // touching the ground
        bool soft = false;          // sand or soil that takes a print
        bool wet = false;           // the wet sand the swash smooths
        bool dusty = false;         // dry enough to throw dust
        float waterDepth = 0.0f;    // metres of water over the ground
    };

    /** A piece of a tyre's track: a rut from one point to the next, as wide as the tyre. */
    struct TyreTrack
    {
        float x0 = 0.0f, z0 = 0.0f, x1 = 0.0f, z1 = 0.0f;
        float width = 0.2f;
        float strength = 0.0f;      // how deep it was pressed, 0..1
        float density = 0.0f;       // what is left of it now, 0..1
        float age = 0.0f;
        float life = 300.0f;
        bool active = false;
    };

    /** The quad's tyre tracks: every third of a metre a wheel rolls on sand or soil a piece of
     *  rut is laid behind it, pressed deeper under load; the dry ground keeps them five
     *  minutes, the wet sand at the water's edge one. Renderer-independent, deterministic. */
    class TyreTrackField final
    {
    public:
        static constexpr std::size_t Capacity = 480, Wheels = 4;
        static constexpr float SegmentLength = 0.32f, LifeSeconds = 300.0f, WetLifeSeconds = 60.0f;

        void Update(float seconds, std::span<const WheelOnGround> wheels) noexcept;
        [[nodiscard]] std::span<const TyreTrack> Tracks() const noexcept { return tracks_; }
        [[nodiscard]] std::uint32_t LaidCount() const noexcept { return laid_; }
        [[nodiscard]] std::size_t ActiveCount() const noexcept;

    private:
        struct Tracker { float x = 0.0f, z = 0.0f; bool valid = false; };
        std::array<TyreTrack, Capacity> tracks_{};
        std::array<Tracker, Wheels> trackers_{};
        std::size_t next_ = 0;
        std::uint32_t laid_ = 0;
    };

    /** What the quad's wheels throw: on dry sand and soil a plume of dust behind each tyre,
     *  growing with the speed (the rear, driven wheels throw most), swelling, settling and
     *  drifting downwind; through water a spray of droplets flung up and back that falls
     *  again within the second. Drawn as dust puffs. Renderer-independent, deterministic. */
    class WheelThrowField final
    {
    public:
        enum class Kind : std::uint8_t { Dust, Spray };
        static constexpr std::size_t Capacity = 96;
        static constexpr float DustLife = 1.8f, SprayLife = 0.8f;

        explicit WheelThrowField(Kind kind) noexcept : kind_(kind) {}
        void Update(float seconds, std::span<const WheelOnGround> wheels, BikeVec velocity, float windX,
                    float windZ) noexcept;
        [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept { return puffs_; }
        [[nodiscard]] std::uint32_t Emitted() const noexcept { return emitted_; }

    private:
        float Jitter() noexcept;
        Kind kind_;
        std::array<DustPuff, Capacity> puffs_{};
        std::array<BikeVec, Capacity> velocity_{};
        std::array<float, 4> pending_{};
        std::size_t next_ = 0;
        std::uint32_t emitted_ = 0;
        std::uint32_t jitter_ = 0x6d2b79f5U;
    };
}
