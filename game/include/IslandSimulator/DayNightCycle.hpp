#pragma once

#include <cstdint>

namespace island
{
    /** @brief A unit direction or an RGB triple. */
    struct DayVec3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
    };

    /** @brief Everything the renderers need from the time of day. */
    struct DayLighting
    {
        DayVec3 toSun;                 // unit vector towards the sun (below the horizon at night)
        DayVec3 toMoon;                // unit vector towards the moon
        float sunElevationDegrees = 0.0f;
        float moonElevationDegrees = 0.0f;
        float sunVisibility = 0.0f;    // 0..1 direct sunlight (fades through sunrise and sunset)
        float moonIllumination = 0.0f; // 0..1 lit fraction of the disc (phase)
        float moonLight = 0.0f;        // 0..1 moonlight reaching the ground (phase x altitude)
        float starVisibility = 0.0f;   // 0..1
        DayVec3 grade;                 // scene colour multiplier: (1,1,1) by day, warm at the golden hours, dark blue at night
        DayVec3 skyZenith;             // sky colour overhead
        DayVec3 skyHorizon;            // sky colour at the horizon (toward the sun)
        DayVec3 skyAntiHorizon;        // and at the horizon opposite the sun (cool at the golden hours, rose at dusk)
        float shadowStrength = 1.0f;   // 0..1 sun shadows (none at night)
        float hour = 14.0f;            // the hour this light is for (the stars wheel with it)
    };

    /**
     * The island's clock: 24 game hours in SecondsPerDay real seconds (24 minutes by default),
     * a sun crossing the sky from the east to the west with a short tropical twilight, and a
     * moon whose phase turns through LunarDays game days. The sun's path is fitted so that at
     * DefaultHour (14:00) it stands exactly where the island's former fixed afternoon sun was
     * (environment::SunDirection), so geometry lit at build time stays correct then.
     */
    class DayNightCycle final
    {
    public:
        static constexpr float SecondsPerDay = 24.0f * 60.0f;   // 24 real minutes per game day
        static constexpr float DefaultHour = 14.0f;              // the former fixed sun's hour
        static constexpr float LunarDays = 8.0f;                 // a short lunar month, for visible phases

        explicit DayNightCycle(float startHour = DefaultHour, float startDay = 2.0f) noexcept;

        /** @brief Advances the clock by real seconds (the game runs SecondsPerDay per day). */
        void Advance(float realSeconds) noexcept;
        /** @brief Holds the clock still (captures and tests); Advance does nothing while held. */
        void SetFrozen(bool frozen) noexcept { frozen_ = frozen; }
        [[nodiscard]] bool Frozen() const noexcept { return frozen_; }
        void SetHour(float hour) noexcept;

        [[nodiscard]] float Hour() const noexcept { return hour_; }          // 0..24
        [[nodiscard]] float Day() const noexcept { return day_; }            // whole and fractional days elapsed
        [[nodiscard]] float MoonPhase() const noexcept;                      // 0 new, 0.5 full
        [[nodiscard]] DayLighting Lighting() const noexcept { return LightingAt(hour_, day_); }

        /** @brief The lighting at any hour of any day, without a clock (tests, previews). */
        [[nodiscard]] static DayLighting LightingAt(float hour, float day) noexcept;
        /** @brief Unit direction to the sun at an hour (the equinox path at the fitted latitude). */
        [[nodiscard]] static DayVec3 SunDirectionAt(float hour) noexcept;
        /** @brief The fitted latitude (degrees) and the world rotation of the east axis. */
        [[nodiscard]] static float LatitudeDegrees() noexcept;
        /** @brief A direction fixed among the stars, given as the sky stands at midnight, turned
         *  to where it stands at an hour: the whole sky wheels about the celestial pole (at the
         *  latitude's height over the north horizon) with the sun, fifteen degrees an hour. */
        [[nodiscard]] static DayVec3 SkyAt(const DayVec3& fixed, float hour) noexcept;
        [[nodiscard]] static float EastAxisDegrees() noexcept;

    private:
        float hour_ = DefaultHour;
        float day_ = 0.0f;
        bool frozen_ = false;
    };

    /**
     * The light of the current frame, shared by the renderers (set once per frame by the game
     * from its clock). Surfaces are shaded from the `shading` direction: the sun by day, the
     * moon by night, blended through the dusk and never lower than 15 degrees, so no shadow
     * or slope term divides by a vanishing sun height near the horizon.
     */
    /** @brief A start hour asked for on the command line (--time-of-day); negative: none. */
    namespace day_night_options
    {
        void SetRequestedHour(float hour) noexcept;
        [[nodiscard]] float RequestedHour() noexcept;
        /** @brief A game day asked for (--game-day; the moon's phase follows it); negative: none. */
        void SetRequestedDay(float day) noexcept;
        [[nodiscard]] float RequestedDay() noexcept;
    }

    namespace sun_light
    {
        inline constexpr float MinimumShadingElevationDegrees = 15.0f;
        struct State
        {
            DayLighting lighting;
            DayVec3 shading;
        };
        [[nodiscard]] DayVec3 ShadingDirection(const DayLighting& lighting) noexcept;
        [[nodiscard]] const State& Current() noexcept;
        /** @brief The hour's sky against the reference afternoon's, divided by the night grade so
         *  what is drawn before the grade comes out at the hour's sky colour after it: the sky's
         *  horizon and zenith scales, which the sea's reflected sky shares. */
        struct SkyScales { DayVec3 horizon, zenith, antiHorizon; };
        [[nodiscard]] SkyScales SkyScalesOf(const DayLighting& lighting) noexcept;
        void Set(const DayLighting& lighting) noexcept;
        /** @brief The shading direction scaled to the former constant's length, so at 14:00 the
         *  uploaded uniform equals environment::SunDirection exactly. */
        [[nodiscard]] DayVec3 UniformDirection() noexcept;
    }
}
