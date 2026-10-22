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
        /** A breach instead of a roll: it bursts up out of the sea near upright, two thirds of
         *  its length clear, twists over and falls back on its side in a great splash (no blow). */
        bool breach = false;
    };

    /**
     * Humpback whales passing far offshore: every minute or so one surfaces 330-620 m out, blows
     * a tall plume of mist that drifts and thins, rolls its dark back through the swell and,
     * one roll in three, raises its flukes as it sounds; about one surfacing in five is a breach
     * instead, ending in a great splash. Renderer-independent and deterministic (a function of
     * the clock); the blows and the splashes are MistPuffs for the mist renderer.
     */
    class WhalePod final
    {
    public:
        static constexpr float RollSeconds = 9.0f, MinimumGapSeconds = 35.0f, MaximumGapSeconds = 85.0f;
        static constexpr std::size_t PuffCount = 6, SplashCount = 12;
        /** The fraction of a breach's run at which it falls back into the sea. */
        static constexpr float BreachImpact = 0.5f;

        void Update(float seconds, float windX, float windZ) noexcept;
        [[nodiscard]] const WhaleSurfacing& Surfacing() const noexcept { return surfacing_; }
        [[nodiscard]] std::span<const MistPuff> Blow() const noexcept { return puffs_; }
        [[nodiscard]] std::span<const MistPuff> Splash() const noexcept { return splash_; }
        [[nodiscard]] std::uint32_t BreachCount() const noexcept { return breaches_; }
        /** QA: a breach at (x, z) heading that way, `progress` through it. */
        void PreviewBreach(float x, float z, float headingRadians, float progress) noexcept;
        [[nodiscard]] std::uint32_t SurfacingCount() const noexcept { return surfacings_; }

    private:
        float Unit(std::uint32_t salt) const noexcept;
        WhaleSurfacing surfacing_{};
        std::array<MistPuff, PuffCount> puffs_{};
        std::array<MistPuff, SplashCount> splash_{};
        float splashAge_ = 99.0f, splashX_ = 0.0f, splashZ_ = 0.0f;
        std::uint32_t breaches_ = 0;
        float clock_ = 0.0f, nextSurfacing_ = 12.0f, blowAge_ = 99.0f;
        float blowX_ = 0.0f, blowZ_ = 0.0f;
        std::uint32_t surfacings_ = 0;
    };
}
