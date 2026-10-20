#include "IslandSimulator/MorningMistField.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        float Unit(const int x, const int z, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(x) * 0x8da6b343U ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ salt * 0xcb1ab31fU;
            h ^= h >> 13;
            h *= 0x5bd1e995U;
            h ^= h >> 15;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        float Smooth(const float a, const float b, const float v) noexcept
        {
            const float t = std::clamp((v - a) / (b - a), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        }
    }

    MorningMistField::MorningMistField(const IslandTerrain& terrain)
    {
        // The low damp ground: the stream's valley, the wetland and the coastal flats, flat
        // enough for the cold air to pool, never on the sand at the water's edge.
        constexpr int Radius = 22;
        for (int gz = -Radius; gz <= Radius && sites_.size() < MaximumSites; ++gz) {
            for (int gx = -Radius; gx <= Radius && sites_.size() < MaximumSites; ++gx) {
                const float x = (static_cast<float>(gx) + (Unit(gx, gz, 1U) - 0.5f) * 0.8f) * GridSpacing;
                const float z = (static_cast<float>(gz) + (Unit(gx, gz, 2U) - 0.5f) * 0.8f) * GridSpacing;
                const TerrainSample sample = terrain.Sample(x, z);
                if (!std::isfinite(sample.height) || sample.height < 0.6f || sample.height > 11.0f || sample.slopeRadians > 0.25f)
                    continue;
                const auto inland = IslandEnvironmentField::SampleInland(x, z, sample.signedShoreDistance);
                const float coastalFlat = Smooth(6.0f, 12.0f, sample.signedShoreDistance) * (1.0f - Smooth(40.0f, 60.0f, sample.signedShoreDistance));
                const float valley = std::max(inland.moisture, inland.drainage);
                const float wetland = 1.0f - Smooth(0.8f, 1.6f, std::hypot((x - IslandEnvironmentField::SeasonalWetlandX) / IslandEnvironmentField::SeasonalWetlandRadiusX,
                                                                          (z - IslandEnvironmentField::SeasonalWetlandZ) / IslandEnvironmentField::SeasonalWetlandRadiusZ));
                const float damp = std::max({valley, wetland, coastalFlat * 0.75f}) * (1.0f - Smooth(5.0f, 11.0f, sample.height) * 0.6f);
                if (Unit(gx, gz, 3U) > damp) continue;
                Site site;
                site.x = x;
                site.z = z;
                site.ground = sample.height;
                site.size = 5.0f + 4.5f * Unit(gx, gz, 4U);
                site.hover = 0.9f + 1.4f * Unit(gx, gz, 5U);
                site.phase = 6.2831853f * Unit(gx, gz, 6U);
                site.rate = 0.05f + 0.08f * Unit(gx, gz, 7U);
                site.weight = 0.55f + 0.45f * damp;
                sites_.push_back(site);
            }
        }
        puffs_.resize(sites_.size());
        Update(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float MorningMistField::Thickness(const float hour, const float wind, const float rain) noexcept
    {
        if (!std::isfinite(hour)) return 0.0f;
        // it gathers before dawn, lies thickest at sunrise and burns off by mid-morning
        const float h = std::fmod(std::fmod(hour, 24.0f) + 24.0f, 24.0f);
        const float dawn = Smooth(4.4f, 5.7f, h) * (1.0f - Smooth(7.2f, 8.8f, h));
        const float still = 1.0f - Smooth(1.15f, 1.6f, std::isfinite(wind) ? wind : 1.0f);
        const float dry = 1.0f - Smooth(0.2f, 0.5f, std::isfinite(rain) ? rain : 0.0f);
        return dawn * still * dry;
    }

    void MorningMistField::Update(const float seconds, const float thickness, const float windX, const float windZ) noexcept
    {
        const float dt = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.25f) : 0.0f;
        clock_ += dt;
        // the whole bank creeps downwind a few metres and back as the air stirs
        drift_ = std::sin(clock_ * 0.021f) * 4.0f;
        driftX_ = std::isfinite(windX) ? windX : 0.0f;
        driftZ_ = std::isfinite(windZ) ? windZ : 0.0f;
        const float thick = std::isfinite(thickness) ? std::clamp(thickness, 0.0f, 1.0f) : 0.0f;
        for (std::size_t i = 0; i < sites_.size(); ++i) {
            const Site& site = sites_[i];
            MistPuff& puff = puffs_[i];
            const float breathe = 0.5f + 0.5f * std::sin(clock_ * site.rate + site.phase);
            const float wander = std::sin(clock_ * site.rate * 0.7f + site.phase * 1.7f);
            puff.x = site.x + driftX_ * drift_ + driftZ_ * wander * 2.5f;
            puff.z = site.z + driftZ_ * drift_ - driftX_ * wander * 2.5f;
            puff.y = site.ground + site.hover * (0.85f + 0.3f * breathe);
            puff.size = site.size * (0.85f + 0.3f * breathe) * (0.7f + 0.3f * thick);
            puff.age = 0.0f;
            puff.density = thick * site.weight * (0.65f + 0.35f * breathe);
            puff.active = puff.density > 0.01f;
        }
    }
}
