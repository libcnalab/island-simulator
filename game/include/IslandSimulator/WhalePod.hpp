#pragma once

#include "IslandSimulator/CascadeMistField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    /** One surfacing whale this moment: where it rolls, which way, how far through the roll
     *  (0 the blow .. 1 under again), and whether this roll ends with the flukes raised. */
    struct WhaleSurfacing
    {
        float x = 0.0f, z = 0.0f, headingRadians = 0.0f;
        float progress = 0.0f;
        bool flukes = false;
        bool active = false;
    };

    /**
     * Humpback whales passing far offshore: every minute or so one surfaces 330-620 m out, blows
     * a tall plume of mist that drifts and thins, rolls its dark back through the swell and,
     * one roll in three, raises its flukes as it sounds. Renderer-independent and deterministic
     * (a function of the clock); the blows are MistPuffs for the mist renderer.
     */
    class WhalePod final
    {
    public:
        static constexpr float RollSeconds = 9.0f, MinimumGapSeconds = 35.0f, MaximumGapSeconds = 85.0f;
        static constexpr std::size_t PuffCount = 6;

        void Update(float seconds, float windX, float windZ) noexcept;
        [[nodiscard]] const WhaleSurfacing& Surfacing() const noexcept { return surfacing_; }
        [[nodiscard]] std::span<const MistPuff> Blow() const noexcept { return puffs_; }
        [[nodiscard]] std::uint32_t SurfacingCount() const noexcept { return surfacings_; }

    private:
        float Unit(std::uint32_t salt) const noexcept;
        WhaleSurfacing surfacing_{};
        std::array<MistPuff, PuffCount> puffs_{};
        float clock_ = 0.0f, nextSurfacing_ = 12.0f, blowAge_ = 99.0f;
        float blowX_ = 0.0f, blowZ_ = 0.0f;
        std::uint32_t surfacings_ = 0;
    };
}
