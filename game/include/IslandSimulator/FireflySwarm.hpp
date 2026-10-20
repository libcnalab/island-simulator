#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island
{
    class IslandTerrain;

    /** One firefly this moment: where it hovers and how bright its lantern is (0 dark .. 1). */
    struct Firefly
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float glow = 0.0f;
    };

    /**
     * Fireflies on warm, still nights: drifting low over the stream's banks and round the
     * seasonal wetland, each blinking its own slow rhythm, part of every cluster flashing in
     * step; they come out after dusk and keep to cover in the rain. Renderer-independent and
     * deterministic (positions and blinks are functions of the time).
     */
    class FireflySwarm final
    {
    public:
        static constexpr std::size_t Count = 120, StreamCount = 90;
        static constexpr float FlashSeconds = 0.55f;

        explicit FireflySwarm(const IslandTerrain& terrain) noexcept;
        /** @param nightness 0 by day .. 1 at night; @param rain 0..1. */
        void Update(float seconds, float nightness, float rain) noexcept;
        [[nodiscard]] std::span<const Firefly> Flies() const noexcept { return flies_; }
        [[nodiscard]] float Presence() const noexcept { return presence_; }
        [[nodiscard]] std::size_t GlowingCount() const noexcept;

    private:
        struct Home
        {
            float x = 0.0f, z = 0.0f, ground = 0.0f;
            float reachX = 1.5f, reachZ = 1.5f;
            float rateX = 0.2f, rateZ = 0.17f, rateY = 0.11f;
            float phaseX = 0.0f, phaseZ = 0.0f, phaseY = 0.0f;
            float period = 3.0f, blinkPhase = 0.0f;
        };
        std::array<Home, Count> homes_{};
        std::array<Firefly, Count> flies_{};
        float presence_ = 0.0f;
        float elapsed_ = 0.0f;
        bool settled_ = false;   // the first update starts at the hour's presence
    };
}
