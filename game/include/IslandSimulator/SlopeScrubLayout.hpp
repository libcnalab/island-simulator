#pragma once

#include "IslandSimulator/JungleLayout.hpp"
#include <span>
#include <vector>

namespace island
{
    class IslandTerrain;

    /**
     * Low coastal scrub on the ground every other plant rule leaves bare: the steep banks
     * (grass stops at 28 degrees, bushes at 30, ground plants at 32) and the strip between
     * the dry beach and the forest edge (bushes start 38 m from the shore), and the rocky
     * headlands (grass and bushes stop at a coastal rockiness of 0.6) that the terrain still
     * draws as smooth ground. Without it the bank rising behind the jetty's beach read as one
     * smooth bare slope. Inland, the open hillsides below the steep banks' slope carry sparse
     * shrubs too. Visual only (no colliders);
     * drawn with the jungle's bush meshes as extra instances.
     */
    class SlopeScrubLayout final
    {
    public:
        struct ClearCircle
        {
            float x = 0.0f;
            float z = 0.0f;
            float radius = 0.0f;
        };

        // A fine grid: a steep bank is narrow on the map but tall on screen, so it needs the
        // samples; the density per cell keeps the gentle beach back sparse.
        static constexpr float Spacing = 1.7f;
        static constexpr int Radius = 176;             // grid cells either side of the origin
        static constexpr float MinimumHeight = 1.8f;    // the beach back: above the dry beach
        static constexpr float SteepBankMinimumHeight = 1.0f, SteepBankShoreStart = 3.5f;   // a foredune
        static constexpr float SteepSlope = 0.45f;      // radians: where the other plants stop
        static constexpr float MaximumSlope = 0.95f;    // a cliff face holds no scrub
        static constexpr float BeachBackStart = 16.0f;  // metres from the shore
        static constexpr float BeachBackEnd = 40.0f;
        static constexpr float MinimumScale = 0.55f, MaximumScale = 0.95f;
        static constexpr float MaximumRockiness = 0.90f;   // grass and bushes stop at 0.60
        // Open inland hillsides between the grass's gentle ground and the steep banks: sparse
        // shrubs so a hill seen across a clearing is not one smooth painted slope.
        static constexpr float HillsideSlope = 0.24f, HillsideShoreStart = 40.0f, HillsideMinimumHeight = 4.0f;

        /** @brief The scrub on this terrain, clear of the reserved routes, the hut's clearing
         *  and the given circles (the goats' waterhole, the lookout, the jetty's root). */
        [[nodiscard]] static std::vector<BushPlacement> Generate(
            const IslandTerrain& terrain, std::span<const ClearCircle> clear);
        /** @brief Whether a site satisfies the scrub's ground rule (steep bank or beach back). */
        [[nodiscard]] static bool Accepts(float height, float slopeRadians, float shoreDistance,
                                          float rockiness) noexcept;
    };
}
