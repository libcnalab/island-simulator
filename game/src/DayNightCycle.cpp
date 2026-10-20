#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float Pi = std::numbers::pi_v<float>;
        constexpr float Radians(const float degrees) noexcept { return degrees * Pi / 180.0f; }

        [[nodiscard]] float Smooth(const float edge0, const float edge1, const float x) noexcept
        {
            const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        }
        [[nodiscard]] DayVec3 Mix(const DayVec3& a, const DayVec3& b, const float t) noexcept
        {
            const float s = std::clamp(t, 0.0f, 1.0f);
            return {a.x + (b.x - a.x) * s, a.y + (b.y - a.y) * s, a.z + (b.z - a.z) * s};
        }
        [[nodiscard]] DayVec3 Scale(const DayVec3& a, const float s) noexcept { return {a.x * s, a.y * s, a.z * s}; }
        [[nodiscard]] DayVec3 Add(const DayVec3& a, const DayVec3& b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

        // The fit: at DefaultHour the hour angle is 30 degrees past noon; the latitude makes the
        // elevation match the former sun, the east axis rotation its azimuth.
        struct Fit { float latitude; float eastAxis; };
        [[nodiscard]] Fit FitToFormerSun() noexcept
        {
            const float length = std::sqrt(environment::SunDirectionX * environment::SunDirectionX
                + environment::SunDirectionY * environment::SunDirectionY
                + environment::SunDirectionZ * environment::SunDirectionZ);
            const float up = environment::SunDirectionY / length;
            const float hourAngle = Radians((DayNightCycle::DefaultHour - 12.0f) * 15.0f);
            const float latitude = std::acos(std::clamp(up / std::cos(hourAngle), -1.0f, 1.0f));
            const float east = -std::sin(hourAngle);
            const float north = -std::sin(latitude) * std::cos(hourAngle);
            const float target = std::atan2(environment::SunDirectionZ / length, environment::SunDirectionX / length);
            const float local = std::atan2(north, east);
            return {latitude, target - local};
        }
        const Fit fit = FitToFormerSun();

        // A body on the equinox path at an hour angle (radians, 0 at its noon).
        [[nodiscard]] DayVec3 OnPath(const float hourAngle) noexcept
        {
            const float east = -std::sin(hourAngle);
            const float north = -std::sin(fit.latitude) * std::cos(hourAngle);
            const float up = std::cos(fit.latitude) * std::cos(hourAngle);
            const float c = std::cos(fit.eastAxis), s = std::sin(fit.eastAxis);
            // east axis (c, s), north axis (-s, c) in world x, z
            return {east * c - north * s, up, east * s + north * c};
        }
        [[nodiscard]] float ElevationDegrees(const DayVec3& d) noexcept
        {
            return std::asin(std::clamp(d.y, -1.0f, 1.0f)) * 180.0f / Pi;
        }
    }

    namespace day_night_options
    {
        namespace { float requestedHour = -1.0f; }
        void SetRequestedHour(const float hour) noexcept
        {
            requestedHour = std::isfinite(hour) ? std::fmod(std::fmod(hour, 24.0f) + 24.0f, 24.0f) : -1.0f;
        }
        float RequestedHour() noexcept { return requestedHour; }
        namespace { float requestedDay = -1.0f; }
        void SetRequestedDay(const float day) noexcept
        {
            requestedDay = std::isfinite(day) && day >= 0.0f ? std::floor(day) : -1.0f;
        }
        float RequestedDay() noexcept { return requestedDay; }
    }

    namespace sun_light
    {
        namespace
        {
            [[nodiscard]] DayVec3 RaisedTo(const DayVec3& d, const float minimumDegrees) noexcept
            {
                const float horizontal = std::hypot(d.x, d.z);
                if (horizontal < 1e-5f) return {0.0f, 1.0f, 0.0f};
                const float elevation = std::max(std::asin(std::clamp(d.y, -1.0f, 1.0f)), Radians(minimumDegrees));
                return {d.x / horizontal * std::cos(elevation), std::sin(elevation), d.z / horizontal * std::cos(elevation)};
            }
            State& Mutable() noexcept
            {
                static State state{DayNightCycle::LightingAt(DayNightCycle::DefaultHour, 2.0f), {}};
                static bool initialised = false;
                if (!initialised) {
                    state.shading = ShadingDirection(state.lighting);
                    initialised = true;
                }
                return state;
            }
        }

        DayVec3 ShadingDirection(const DayLighting& lighting) noexcept
        {
            const DayVec3 sun = RaisedTo(lighting.toSun, MinimumShadingElevationDegrees);
            const DayVec3 moon = RaisedTo(lighting.toMoon, MinimumShadingElevationDegrees);
            const float sunWeight = Smooth(-6.0f, 2.0f, lighting.sunElevationDegrees);
            DayVec3 d = Mix(moon, sun, sunWeight);
            const float length = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
            if (length < 1e-5f) return sun;
            return {d.x / length, d.y / length, d.z / length};
        }

        const State& Current() noexcept { return Mutable(); }

        void Set(const DayLighting& lighting) noexcept
        {
            State& state = Mutable();
            state.lighting = lighting;
            state.shading = ShadingDirection(lighting);
        }

        DayVec3 UniformDirection() noexcept
        {
            const float former = std::sqrt(environment::SunDirectionX * environment::SunDirectionX
                + environment::SunDirectionY * environment::SunDirectionY
                + environment::SunDirectionZ * environment::SunDirectionZ);
            const DayVec3 d = Mutable().shading;
            return {d.x * former, d.y * former, d.z * former};
        }
    }

    DayNightCycle::DayNightCycle(const float startHour, const float startDay) noexcept
        : day_(std::max(startDay, 0.0f))
    {
        SetHour(startHour);
    }

    void DayNightCycle::SetHour(const float hour) noexcept
    {
        if (!std::isfinite(hour)) return;
        const float wrapped = std::fmod(std::fmod(hour, 24.0f) + 24.0f, 24.0f);
        day_ = std::floor(day_) + wrapped / 24.0f;
        hour_ = wrapped;
    }

    void DayNightCycle::Advance(const float realSeconds) noexcept
    {
        if (frozen_ || !std::isfinite(realSeconds) || realSeconds <= 0.0f) return;
        const float days = std::min(realSeconds, 10.0f) / SecondsPerDay;
        day_ += days;
        hour_ = (day_ - std::floor(day_)) * 24.0f;
    }

    float DayNightCycle::MoonPhase() const noexcept
    {
        const float cycles = day_ / LunarDays;
        return cycles - std::floor(cycles);
    }

    float DayNightCycle::LatitudeDegrees() noexcept { return fit.latitude * 180.0f / Pi; }
    float DayNightCycle::EastAxisDegrees() noexcept { return fit.eastAxis * 180.0f / Pi; }

    DayVec3 DayNightCycle::SunDirectionAt(const float hour) noexcept
    {
        return OnPath(Radians((hour - 12.0f) * 15.0f));
    }

    DayLighting DayNightCycle::LightingAt(const float hour, const float day) noexcept
    {
        DayLighting light;
        light.toSun = SunDirectionAt(hour);
        light.sunElevationDegrees = ElevationDegrees(light.toSun);
        // The moon trails the sun by its phase: new beside the sun, full opposite it.
        const float lunar = day / LunarDays;
        const float phase = lunar - std::floor(lunar);
        light.toMoon = OnPath(Radians((hour - 12.0f) * 15.0f) - 2.0f * Pi * phase);
        light.moonElevationDegrees = ElevationDegrees(light.toMoon);
        light.moonIllumination = 0.5f * (1.0f - std::cos(2.0f * Pi * phase));

        const float e = light.sunElevationDegrees;
        light.sunVisibility = Smooth(-2.0f, 8.0f, e);
        light.shadowStrength = Smooth(1.0f, 12.0f, e);
        light.moonLight = light.moonIllumination * Smooth(-2.0f, 15.0f, light.moonElevationDegrees);
        light.starVisibility = 1.0f - Smooth(-14.0f, -4.0f, e);

        // The scene's colour by the sun's height: full daylight above 20 degrees, a warm golden
        // hour down to the horizon, a short rose-violet dusk to -6 and a dark blue night that
        // the moon lightens with its phase.
        const DayVec3 daylight{1.0f, 1.0f, 1.0f};
        const DayVec3 golden{1.0f, 0.83f, 0.64f};
        const DayVec3 dusk{0.46f, 0.38f, 0.50f};
        // A game night: dark and blue, but the land still reads; the moon lifts it further.
        const DayVec3 night{0.13f, 0.165f, 0.29f};
        const DayVec3 moonlit{0.10f, 0.12f, 0.18f};
        DayVec3 grade;
        if (e >= 20.0f) grade = daylight;
        else if (e >= 2.0f) grade = Mix(golden, daylight, Smooth(2.0f, 20.0f, e));
        else if (e >= -6.0f) grade = Mix(dusk, golden, Smooth(-6.0f, 2.0f, e));
        else grade = Mix(night, dusk, Smooth(-14.0f, -6.0f, e));
        light.grade = Add(grade, Scale(moonlit, light.moonLight * (1.0f - Smooth(-6.0f, 2.0f, e))));

        // The sky: deep blue by day, a pale warm horizon at the golden hours, rose and indigo
        // through the dusk, near black-blue at night.
        const DayVec3 dayZenith{0.22f, 0.50f, 0.86f}, dayHorizon{0.70f, 0.86f, 0.95f};
        const DayVec3 goldenZenith{0.30f, 0.45f, 0.72f}, goldenHorizon{0.98f, 0.74f, 0.48f};
        const DayVec3 duskZenith{0.10f, 0.13f, 0.30f}, duskHorizon{0.78f, 0.42f, 0.38f};
        const DayVec3 nightZenith{0.012f, 0.018f, 0.045f}, nightHorizon{0.035f, 0.050f, 0.090f};
        if (e >= 20.0f) { light.skyZenith = dayZenith; light.skyHorizon = dayHorizon; }
        else if (e >= 2.0f) {
            const float t = Smooth(2.0f, 20.0f, e);
            light.skyZenith = Mix(goldenZenith, dayZenith, t); light.skyHorizon = Mix(goldenHorizon, dayHorizon, t);
        } else if (e >= -6.0f) {
            const float t = Smooth(-6.0f, 2.0f, e);
            light.skyZenith = Mix(duskZenith, goldenZenith, t); light.skyHorizon = Mix(duskHorizon, goldenHorizon, t);
        } else {
            const float t = Smooth(-14.0f, -6.0f, e);
            light.skyZenith = Mix(nightZenith, duskZenith, t); light.skyHorizon = Mix(nightHorizon, duskHorizon, t);
        }
        const DayVec3 moonGlow{0.03f, 0.045f, 0.08f};
        light.skyHorizon = Add(light.skyHorizon, Scale(moonGlow, light.moonLight * light.starVisibility));
        light.skyZenith = Add(light.skyZenith, Scale(moonGlow, 0.6f * light.moonLight * light.starVisibility));
        return light;
    }
}
