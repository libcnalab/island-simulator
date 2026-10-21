#pragma once
#include <algorithm>
#include <cmath>

#include "IslandSimulator/DayNightCycle.hpp"
#include <cstdint>
#include <optional>
#include <string_view>

namespace island
{
    enum class WeatherKind : std::uint8_t
    {
        Clear,
        PartlyCloudy,   // the island's former fixed weather
        Overcast,
        Showers,
        Thunderstorm,
    };

    [[nodiscard]] std::string_view WeatherName(WeatherKind kind) noexcept;
    [[nodiscard]] std::optional<WeatherKind> WeatherFromName(std::string_view name) noexcept;

    /** @brief The weather of the moment, eased between spells. */
    struct WeatherState
    {
        WeatherKind kind = WeatherKind::PartlyCloudy;
        float cloudCover = 0.0f;   // -0.4 clear .. 0 the former scattered cumulus .. 1 overcast
        float rain = 0.0f;         // 0..1 falling rain
        float wetness = 0.0f;      // 0..1 wet ground: builds in rain, dries after
        float wind = 1.0f;         // gust strength relative to the former trade wind
        float flash = 0.0f;        // 0..1 a lightning flash lighting the scene now
        std::uint32_t strikes = 0; // lightning strikes so far
        float lastStrikeDistance = 0.0f;   // metres, for the thunder's delay
    };

    /**
     * The island's weather: spells of 1.5 to 5 game hours chained by plausible transitions
     * (fair weather tends to cloud over, showers to clear or build into a storm), seeded so a
     * run repeats, each eased into the next; a thunderstorm throws lightning every 5 to 22 real
     * seconds. Snow is deliberately absent: it cannot fall on a tropical island at sea level.
     */
    /** @brief The weather of the current frame, shared by the renderers (set by the game). */
    namespace weather_now
    {
        [[nodiscard]] const WeatherState& Current() noexcept;
        void Set(const WeatherState& state) noexcept;
        /** @brief A weather asked for on the command line (--weather); none when empty. */
        void SetRequested(std::optional<WeatherKind> kind) noexcept;
        [[nodiscard]] std::optional<WeatherKind> Requested() noexcept;
        /** @brief How high the sea runs with this weather's wind: 1 in the trade wind (and below),
         *  up to 1.7 in a storm's; the waves' heights and slopes scale by it. */
        /** @brief A bright moon casts soft shadows: the night's shadow strength from the
         *  moonlight (which the clouds already take down), up to 40 % under a full moon high. */
        inline void AddMoonShadows(DayLighting& light) noexcept
        {
            light.shadowStrength = std::max(light.shadowStrength,
                0.40f * std::clamp(light.moonLight, 0.0f, 1.0f) * (1.0f - std::clamp(light.sunVisibility, 0.0f, 1.0f)));
        }
        [[nodiscard]] inline float SeaState() noexcept
        {
            const float wind = Current().wind;
            return 1.0f + 0.7f * std::clamp(std::isfinite(wind) ? wind - 1.0f : 0.0f, 0.0f, 1.0f);
        }
    }

    class WeatherSystem final
    {
    public:
        static constexpr float MinimumSpellHours = 1.5f, MaximumSpellHours = 5.0f;

        explicit WeatherSystem(std::uint32_t seed = 0x5eed1e5U, WeatherKind start = WeatherKind::PartlyCloudy) noexcept;

        /** @brief Advances by real seconds while gameHours of the island's clock pass. */
        void Advance(float realSeconds, float gameHours) noexcept;
        /** @brief Holds one weather (QA and --weather); the state jumps to it at once. */
        void Force(WeatherKind kind) noexcept;
        void Release() noexcept { forced_ = false; }
        [[nodiscard]] bool Forced() const noexcept { return forced_; }
        [[nodiscard]] const WeatherState& State() const noexcept { return state_; }
        [[nodiscard]] WeatherKind Next() const noexcept { return next_; }
        [[nodiscard]] float SpellHoursLeft() const noexcept { return spellHoursLeft_; }

        /** @brief The targets one weather eases towards. */
        [[nodiscard]] static WeatherState Target(WeatherKind kind) noexcept;
        /** @brief The hour's light under this weather: overcast dims and greys the scene and the
         *  sky, weakens the sun's shadows and hides its disc; a lightning flash is added later. */
        static void ApplyTo(DayLighting& light, const WeatherState& weather) noexcept;

    private:
        [[nodiscard]] float Random() noexcept;
        [[nodiscard]] WeatherKind Choose(WeatherKind from) noexcept;
        WeatherState state_;
        WeatherKind next_ = WeatherKind::PartlyCloudy;
        std::uint32_t rng_;
        float spellHoursLeft_ = 3.0f;
        float strikeSeconds_ = 8.0f;
        float flashSeconds_ = 0.0f;
        float showerPhase_ = 0.0f;
        bool forced_ = false;
    };
}
