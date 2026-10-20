#pragma once

#include <cmath>
#include <cstdint>

namespace island::rain_rings
{
    /** Raindrops striking still water: each cell of a fixed world grid sends out a ring now and
     *  then (its own period and phase), so the rings stay where they are in the world as the
     *  walker moves; in lighter rain fewer cells take part. */
    inline constexpr float CellSize = 0.55f;
    inline constexpr float Life = 0.5f;           // seconds a ring takes to spread and fade
    inline constexpr float MaximumRadius = 0.22f;  // metres

    struct Ring
    {
        float x = 0.0f, z = 0.0f;
        float progress = 0.0f;   // 0 the drop strikes .. 1 the ring is gone
        bool active = false;
    };

    [[nodiscard]] inline float Hash(const std::int32_t cellX, const std::int32_t cellZ, const std::uint32_t salt) noexcept
    {
        std::uint32_t h = static_cast<std::uint32_t>(cellX) * 0x8da6b343U ^ static_cast<std::uint32_t>(cellZ) * 0xd8163841U ^ salt * 0xcb1ab31fU;
        h ^= h >> 13;
        h *= 0x5bd1e995U;
        h ^= h >> 15;
        return static_cast<float>(h & 0xffffffU) / 16777215.0f;
    }

    /** @brief The ring of cell (cellX, cellZ) at `seconds` in rain of strength `rain` (0..1). */
    [[nodiscard]] inline Ring At(const std::int32_t cellX, const std::int32_t cellZ, const double seconds, const float rain) noexcept
    {
        Ring ring;
        if (!(rain > 0.0f)) return ring;
        const float period = 1.2f + 1.2f * Hash(cellX, cellZ, 1U);
        const double cycle = seconds / period + Hash(cellX, cellZ, 2U);
        const double whole = std::floor(cycle);
        const float phase = static_cast<float>(cycle - whole) * period;   // seconds into this cycle
        if (phase >= Life) return ring;
        // each strike lands somewhere else in the cell, and only some cells strike in light rain
        const auto strike = static_cast<std::uint32_t>(static_cast<std::int64_t>(whole) & 0xffffffff);
        if (Hash(cellX, cellZ, 3U + strike) > rain) return ring;
        ring.x = (static_cast<float>(cellX) + 0.15f + 0.7f * Hash(cellX, cellZ, 0x1000U + strike)) * CellSize;
        ring.z = (static_cast<float>(cellZ) + 0.15f + 0.7f * Hash(cellX, cellZ, 0x2000U + strike)) * CellSize;
        ring.progress = phase / Life;
        ring.active = true;
        return ring;
    }
}
