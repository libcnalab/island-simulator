#include "IslandSimulator/SlopeScrubLayout.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace island
{
    namespace
    {
        [[nodiscard]] std::uint32_t Mix(std::uint32_t value) noexcept
        {
            value ^= value >> 16U; value *= 0x7feb352dU;
            value ^= value >> 15U; value *= 0x846ca68bU;
            return value ^ (value >> 16U);
        }
        [[nodiscard]] float Unit(const int gridX, const int gridZ, const std::uint32_t salt) noexcept
        {
            const std::uint32_t h = Mix(static_cast<std::uint32_t>(gridX) * 0x9e3779b9U
                                        ^ Mix(static_cast<std::uint32_t>(gridZ) * 0x85ebca6bU ^ salt));
            return static_cast<float>(h >> 8U) * (1.0f / 16777216.0f);
        }
        // Broad patches: scrub grows in clumps with open ground between them.
        [[nodiscard]] float Patchiness(const float x, const float z) noexcept
        {
            return 0.5f + 0.25f * std::sin(x * 0.061f + std::sin(z * 0.043f) * 1.7f)
                 + 0.25f * std::sin(z * 0.087f - x * 0.029f + 0.6f);
        }
    }

    bool SlopeScrubLayout::Accepts(const float height, const float slopeRadians, const float shoreDistance,
                                   const float rockiness) noexcept
    {
        // Scrub holds the rocky headlands too (sea grape and naupaka on a rocky coast); only
        // the barest rock is left to the shader's cliff.
        if (!std::isfinite(height) || slopeRadians > MaximumSlope || rockiness > MaximumRockiness) return false;
        // A steep bank holds scrub from just above the tide line (a foredune); the gentle
        // beach back only above the dry sand.
        const bool steepBank = slopeRadians >= SteepSlope && shoreDistance >= SteepBankShoreStart
            && height >= SteepBankMinimumHeight;
        const bool beachBack = shoreDistance >= BeachBackStart && shoreDistance < BeachBackEnd && height >= MinimumHeight;
        return steepBank || beachBack;
    }

    std::vector<BushPlacement> SlopeScrubLayout::Generate(const IslandTerrain& terrain,
                                                                        const std::span<const ClearCircle> clear)
    {
        std::vector<BushPlacement> scrub;
        scrub.reserve(2000U);
        for (int gridZ = -Radius; gridZ <= Radius; ++gridZ) {
            for (int gridX = -Radius; gridX <= Radius; ++gridX) {
                const float x = (static_cast<float>(gridX) + (Unit(gridX, gridZ, 0x51ab0001U) - 0.5f) * 0.8f) * Spacing;
                const float z = (static_cast<float>(gridZ) + (Unit(gridX, gridZ, 0x51ab0002U) - 0.5f) * 0.8f) * Spacing;
                const TerrainSample sample = terrain.Sample(x, z);
                if (!Accepts(sample.height, sample.slopeRadians, sample.signedShoreDistance, sample.coastalRockiness))
                    continue;
                if (VegetationLayout::IsReservedRoute(x, z) || HutLayout::ReservesDecorativeClearing(x, z, 1.4f))
                    continue;
                const auto environment = IslandEnvironmentField::SampleInland(x, z, sample.signedShoreDistance);
                if (environment.drainage > 0.80f) continue;   // the stream's own banks
                bool blocked = false;
                for (const auto& circle : clear)
                    if (std::hypot(x - circle.x, z - circle.z) < circle.radius) { blocked = true; break; }
                if (blocked) continue;
                // Clumped: denser on the steep banks, thinner on the beach back.
                const float steep = std::clamp((sample.slopeRadians - 0.30f) / 0.30f, 0.0f, 1.0f);
                const float rocky = std::clamp((sample.coastalRockiness - 0.60f) / 0.30f, 0.0f, 1.0f);
                const float density = std::clamp((0.045f + 0.30f * steep) * (0.45f + 0.95f * Patchiness(x, z))
                                                  * (1.0f - 0.35f * rocky), 0.0f, 0.60f);
                if (Unit(gridX, gridZ, 0x51ab0003U) >= density) continue;
                BushPlacement bush;
                bush.x = x;
                bush.y = sample.height + 0.012f;
                bush.z = z;
                bush.yawRadians = Unit(gridX, gridZ, 0x51ab0004U) * 2.0f * std::numbers::pi_v<float>;
                // Smaller on the steep banks, where the scrub clings low.
                bush.scale = (MinimumScale + (MaximumScale - MinimumScale) * Unit(gridX, gridZ, 0x51ab0005U))
                    * (1.0f - 0.15f * steep);
                bush.variant = static_cast<std::uint8_t>(Unit(gridX, gridZ, 0x51ab0006U) * JungleLayout::BushVariantCount)
                    % JungleLayout::BushVariantCount;
                scrub.push_back(bush);
            }
        }
        return scrub;
    }
}
