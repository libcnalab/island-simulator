#include "IslandSimulator/WeatherSystem.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace island
{
    namespace
    {
        [[nodiscard]] float Ease(const float value, const float target, const float seconds, const float timeConstant) noexcept
        {
            return target + (value - target) * std::exp(-seconds / timeConstant);
        }
    }

    namespace weather_now
    {
        namespace
        {
            WeatherState& Mutable() noexcept { static WeatherState state; return state; }
            std::optional<WeatherKind>& RequestedKind() noexcept { static std::optional<WeatherKind> kind; return kind; }
            bool& RequestedWet() noexcept { static bool wet = false; return wet; }
        }
        const WeatherState& Current() noexcept { return Mutable(); }
        void Set(const WeatherState& state) noexcept { Mutable() = state; }
        void SetRequested(const std::optional<WeatherKind> kind) noexcept { RequestedKind() = kind; }
        std::optional<WeatherKind> Requested() noexcept { return RequestedKind(); }
        void SetRequestedWetGround(const bool wet) noexcept { RequestedWet() = wet; }
        bool RequestedWetGround() noexcept { return RequestedWet(); }
    }

    std::string_view WeatherName(const WeatherKind kind) noexcept
    {
        switch (kind) {
        case WeatherKind::Clear: return "clear";
        case WeatherKind::PartlyCloudy: return "partly-cloudy";
        case WeatherKind::Overcast: return "overcast";
        case WeatherKind::Showers: return "showers";
        case WeatherKind::Thunderstorm: return "thunderstorm";
        }
        return "partly-cloudy";
    }

    std::optional<WeatherKind> WeatherFromName(const std::string_view name) noexcept
    {
        for (const auto kind : {WeatherKind::Clear, WeatherKind::PartlyCloudy, WeatherKind::Overcast,
                                WeatherKind::Showers, WeatherKind::Thunderstorm})
            if (WeatherName(kind) == name) return kind;
        if (name == "rain") return WeatherKind::Showers;
        if (name == "storm") return WeatherKind::Thunderstorm;
        return std::nullopt;
    }

    WeatherState WeatherSystem::Target(const WeatherKind kind) noexcept
    {
        WeatherState t;
        t.kind = kind;
        switch (kind) {
        case WeatherKind::Clear: t.cloudCover = -0.4f; t.rain = 0.0f; t.wind = 0.8f; break;
        case WeatherKind::PartlyCloudy: t.cloudCover = 0.0f; t.rain = 0.0f; t.wind = 1.0f; break;
        case WeatherKind::Overcast: t.cloudCover = 0.85f; t.rain = 0.0f; t.wind = 1.15f; break;
        case WeatherKind::Showers: t.cloudCover = 0.75f; t.rain = 0.55f; t.wind = 1.35f; break;
        case WeatherKind::Thunderstorm: t.cloudCover = 1.0f; t.rain = 1.0f; t.wind = 2.0f; break;
        }
        return t;
    }

    WeatherSystem::WeatherSystem(const std::uint32_t seed, const WeatherKind start) noexcept
        : rng_(seed == 0U ? 0x9e3779b9U : seed)
    {
        state_ = Target(start);
        state_.wetness = start == WeatherKind::Showers || start == WeatherKind::Thunderstorm ? 0.8f : 0.0f;
        next_ = Choose(start);
        spellHoursLeft_ = MinimumSpellHours + (MaximumSpellHours - MinimumSpellHours) * Random();
    }

    float WeatherSystem::Random() noexcept
    {
        rng_ ^= rng_ << 13U; rng_ ^= rng_ >> 17U; rng_ ^= rng_ << 5U;
        return static_cast<float>(rng_ >> 8U) * (1.0f / 16777216.0f);
    }

    WeatherKind WeatherSystem::Choose(const WeatherKind from) noexcept
    {
        // Rows: the chance of each next weather (clear, partly, overcast, showers, storm).
        static constexpr std::array<std::array<float, 5>, 5> chances{{
            {0.00f, 0.70f, 0.20f, 0.10f, 0.00f},
            {0.35f, 0.00f, 0.30f, 0.25f, 0.10f},
            {0.00f, 0.40f, 0.00f, 0.40f, 0.20f},
            {0.00f, 0.50f, 0.30f, 0.00f, 0.20f},
            {0.00f, 0.00f, 0.40f, 0.60f, 0.00f},
        }};
        const auto& row = chances[static_cast<std::size_t>(from)];
        float pick = Random();
        for (std::size_t i = 0; i < row.size(); ++i) {
            if (pick < row[i]) return static_cast<WeatherKind>(i);
            pick -= row[i];
        }
        return WeatherKind::PartlyCloudy;
    }

    void WeatherSystem::Force(const WeatherKind kind) noexcept
    {
        forced_ = true;
        const auto target = Target(kind);
        state_.kind = kind;
        state_.cloudCover = target.cloudCover;
        state_.rain = target.rain;
        state_.wind = target.wind;
        state_.wetness = target.rain > 0.0f || weather_now::RequestedWetGround() ? 0.85f : 0.0f;
        next_ = kind;
    }

    void WeatherSystem::Advance(const float realSeconds, const float gameHours) noexcept
    {
        if (!std::isfinite(realSeconds) || realSeconds <= 0.0f) return;
        const float dt = std::min(realSeconds, 1.0f);
        if (!forced_ && std::isfinite(gameHours) && gameHours > 0.0f) {
            spellHoursLeft_ -= gameHours;
            if (spellHoursLeft_ <= 0.0f) {
                state_.kind = next_;
                next_ = Choose(next_);
                spellHoursLeft_ = MinimumSpellHours + (MaximumSpellHours - MinimumSpellHours) * Random();
            }
        }
        const auto target = Target(state_.kind);
        // Showers come and go within their spell; a storm's rain stays heavy.
        showerPhase_ += dt * 0.021f;
        const float showerPulse = state_.kind == WeatherKind::Showers
            ? std::clamp(0.35f + 0.9f * std::sin(showerPhase_ * 6.2831853f), 0.0f, 1.0f) : 1.0f;
        state_.cloudCover = Ease(state_.cloudCover, target.cloudCover, dt, 75.0f);
        state_.rain = Ease(state_.rain, target.rain * showerPulse, dt, 18.0f);
        state_.wind = Ease(state_.wind, target.wind, dt, 40.0f);
        // Wet ground: soaked in about a minute of heavy rain, dry a few minutes after it.
        state_.wetness = state_.rain > 0.05f
            ? std::min(1.0f, state_.wetness + dt * state_.rain / 55.0f)
            : std::max(0.0f, state_.wetness - dt / 220.0f);
        if (forced_ && weather_now::RequestedWetGround()) state_.wetness = std::max(state_.wetness, 0.85f);
        // Lightning in a storm: a double flicker of about a fifth of a second.
        flashSeconds_ = std::max(0.0f, flashSeconds_ - dt);
        if (state_.kind == WeatherKind::Thunderstorm && state_.cloudCover > 0.7f) {
            strikeSeconds_ -= dt;
            if (strikeSeconds_ <= 0.0f) {
                ++state_.strikes;
                state_.lastStrikeDistance = 400.0f + 3600.0f * Random();
                flashSeconds_ = 0.22f;
                strikeSeconds_ = 5.0f + 17.0f * Random();
            }
        }
        const float f = flashSeconds_;
        state_.flash = f > 0.0f ? (f > 0.14f ? 1.0f : f > 0.10f ? 0.25f : f / 0.10f) : 0.0f;
    }

    void WeatherSystem::ApplyTo(DayLighting& light, const WeatherState& weather) noexcept
    {
        const float cover = std::clamp(weather.cloudCover, 0.0f, 1.0f);
        // Overcast: the direct sun is hidden, so its shadows weaken and its disc is gone; the
        // scene dims a little and turns grey; the sky goes to a lit grey.
        light.shadowStrength *= 1.0f - 0.85f * cover;
        light.sunVisibility *= 1.0f - 0.9f * cover;
        const float grey = 0.30f * cover + 0.15f * std::clamp(weather.rain, 0.0f, 1.0f);
        const float lum = 0.3f * light.grade.x + 0.59f * light.grade.y + 0.11f * light.grade.z;
        const auto toGrey = [&](float c) { return (c + (lum - c) * 0.5f * cover) * (1.0f - grey); };
        light.grade = {toGrey(light.grade.x), toGrey(light.grade.y), toGrey(light.grade.z)};
        const DayVec3 overcastZenith{0.50f, 0.54f, 0.58f}, overcastHorizon{0.66f, 0.69f, 0.72f};
        const auto mixSky = [&](DayVec3 a, const DayVec3& b) {
            // the grey sky keeps the hour's brightness (a dark grey night, a lit grey day)
            const float brightness = 0.3f * a.x + 0.59f * a.y + 0.11f * a.z;
            const float scale = brightness / 0.55f;
            a.x += (b.x * scale - a.x) * cover; a.y += (b.y * scale - a.y) * cover; a.z += (b.z * scale - a.z) * cover;
            return a;
        };
        light.skyZenith = mixSky(light.skyZenith, overcastZenith);
        light.skyHorizon = mixSky(light.skyHorizon, overcastHorizon);
        light.skyAntiHorizon = mixSky(light.skyAntiHorizon, overcastHorizon);
        light.starVisibility *= 1.0f - cover;
        light.moonLight *= 1.0f - 0.8f * cover;
    }
}
