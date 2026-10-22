#pragma once

#include "IslandSimulator/GeckoPopulation.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace island::hut_geckos
{
    /** House geckos on the hut's front wall by the lantern at night: as the light draws the
     *  moths, two small geckos come out on the boards beside it, rest, creep towards the light
     *  with their tails twitching, dart at a moth and lick their lips, and creep back; each on
     *  its own cycle, deterministic in the clock. By day they are gone into the cracks. */
    inline constexpr std::size_t Count = 2;
    inline constexpr float WallFace = 1.735f;   // the front wall boards' outer face, hut-local z
    inline constexpr float Scale = 0.14f;       // about 17 cm nose to tail

    [[nodiscard]] inline std::array<GeckoState, Count> At(const HutPlacement& hut, const float seconds,
                                                          const float nightness) noexcept
    {
        std::array<GeckoState, Count> geckos{};
        const float shown = std::clamp((nightness - 0.45f) / 0.25f, 0.0f, 1.0f);
        const float c = std::cos(hut.yawRadians), s = std::sin(hut.yawRadians);
        // hut-local (x, y on the wall) to the world; the wall faces local +z
        const auto place = [&](GeckoState& gecko, const float x, const float y) {
            gecko.x = hut.x + c * x + s * WallFace;
            gecko.y = hut.floorY + y;
            gecko.z = hut.z - s * x + c * WallFace;
        };
        constexpr std::array<std::array<float, 4>, Count> Paths{{
            {0.35f, 1.42f, 0.60f, 1.66f},   // home, then the spot by the lantern (local x, y)
            {1.32f, 1.30f, 1.04f, 1.60f}}};
        for (std::size_t i = 0; i < Count; ++i) {
            GeckoState& gecko = geckos[i];
            const float period = 19.0f + 6.0f * static_cast<float>(i);
            const float t = std::isfinite(seconds) ? std::fmod(seconds + 7.3f * static_cast<float>(i), period) : 0.0f;
            const float ax = Paths[i][0], ay = Paths[i][1], bx = Paths[i][2], by = Paths[i][3];
            // the dart: a few centimetres further towards the lantern (local 0.80, 1.85)
            const float lx = 0.80f - bx, ly = 1.85f - by, ll = std::hypot(lx, ly);
            const float dx = lx / ll * 0.06f, dy = ly / ll * 0.06f;
            float x = ax, y = ay, travelled = 0.0f, mx = bx - ax, my = by - ay;
            const float path = std::hypot(mx, my);
            gecko.behavior = GeckoBehavior::Rest;
            if (t < 5.0f) {
                x = ax; y = ay;
            } else if (t < 11.0f) {   // creeping up on the light, tail twitching
                const float k = (t - 5.0f) / 6.0f;
                x = ax + mx * k; y = ay + my * k; travelled = path * k;
                gecko.behavior = GeckoBehavior::Stalk;
                gecko.tailWag = 0.7f;
                gecko.speed = path / 6.0f;
            } else if (t < 11.6f) {   // the dart and back
                const float k = t < 11.15f ? (t - 11.0f) / 0.15f : 1.0f - (t - 11.15f) / 0.45f;
                x = bx + dx * k; y = by + dy * k; travelled = path;
                mx = dx; my = dy;
                gecko.behavior = GeckoBehavior::Lunge;
            } else if (t < 14.0f) {
                x = bx; y = by; travelled = path;
                gecko.lipLick = t < 12.1f ? std::sin((t - 11.6f) / 0.5f * 3.14159265f) : 0.0f;
            } else {   // creeping back home
                const float k = (t - 14.0f) / (period - 14.0f);
                x = bx - mx * k; y = by - my * k; travelled = path * (1.0f + k);
                mx = -mx; my = -my;
                gecko.speed = path / (period - 14.0f);
            }
            place(gecko, x, y);
            // heading on the wall: from straight up towards local +x
            gecko.headingRadians = std::atan2(mx, my);
            gecko.gaitPhase = travelled * 60.0f;
            gecko.tailWagPhase = (std::isfinite(seconds) ? std::fmod(seconds, 600.0f) : 0.0f) * GeckoPopulation::TailWagRate;
            gecko.visibility = shown;
            gecko.scale = Scale;
            gecko.simulationTier = WildlifeSimulationTier::Full;
            gecko.colorVariant = static_cast<std::uint8_t>(i == 0 ? 3U : 1U);   // tan and brown
            gecko.wallNormalX = s;
            gecko.wallNormalZ = c;
        }
        return geckos;
    }

    /** House geckos chirp: now and then through the night a gecko gives its run of sharp
     *  "chik"s from where it rests at home, ChirpAt seconds into its cycle, on about one cycle
     *  in three (chosen by a hash of the cycle). Returns the gecko that chirps after `from` and
     *  up to `to` seconds on the same clock as At, or Count if none does (by day, or with the
     *  clock running backwards). */
    inline constexpr float ChirpAt = 2.5f;
    [[nodiscard]] inline std::size_t ChirpBetween(const float from, const float to, const float nightness) noexcept
    {
        if (!(nightness > 0.7f) || !std::isfinite(from) || !std::isfinite(to) || !(to > from) || to < 0.0f) return Count;
        for (std::size_t i = 0; i < Count; ++i) {
            const float period = 19.0f + 6.0f * static_cast<float>(i), lead = 7.3f * static_cast<float>(i);
            const float cycle = std::floor((to + lead) / period);
            const float at = cycle * period + ChirpAt - lead;
            const auto chosen = static_cast<std::uint32_t>(cycle) * 2654435761U + static_cast<std::uint32_t>(i) * 40503U;
            if (at > from && at <= to && (chosen >> 13U) % 3U == 0U) return i;
        }
        return Count;
    }
}
