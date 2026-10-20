#pragma once

#include "IslandSimulator/CascadeMistField.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island
{
    class IslandTerrain;

    /**
     * Morning mist: after a still night the low, damp ground breathes out banks of mist that
     * lie over the stream's valley, the wetland and the coastal flats in the first light and
     * burn off as the sun climbs. Broad faint puffs at fixed sites on that ground (a world grid,
     * so the mist stays put as the walker moves), each swelling, drifting a few metres with
     * the wind and breathing; a strong wind or rain disperses them. MistPuffs, drawn by the
     * mist bank renderer. Renderer-independent and deterministic.
     */
    class MorningMistField final
    {
    public:
        static constexpr std::size_t MaximumSites = 260;
        static constexpr float GridSpacing = 14.0f;

        explicit MorningMistField(const IslandTerrain& terrain);
        /** @brief The mist for this hour (0..24) and weather: how thick it lies now (0..1). */
        [[nodiscard]] static float Thickness(float hour, float wind, float rain) noexcept;
        void Update(float seconds, float thickness, float windX, float windZ) noexcept;
        [[nodiscard]] std::span<const MistPuff> Puffs() const noexcept { return puffs_; }
        [[nodiscard]] std::size_t SiteCount() const noexcept { return sites_.size(); }

    private:
        struct Site
        {
            float x = 0.0f, z = 0.0f, ground = 0.0f;
            float size = 4.0f, hover = 1.2f, phase = 0.0f, rate = 0.1f, weight = 1.0f;
        };
        std::vector<Site> sites_;
        std::vector<MistPuff> puffs_;
        float clock_ = 0.0f, drift_ = 0.0f, driftX_ = 0.0f, driftZ_ = 0.0f;
    };
}
