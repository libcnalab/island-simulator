#include "IslandSimulator/FireflySwarm.hpp"
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
        float Unit(const std::uint32_t index, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = index * 0x9e3779b9U ^ salt * 0x85ebca6bU;
            h ^= h >> 16;
            h *= 0x7feb352dU;
            h ^= h >> 15;
            h *= 0x846ca68bU;
            h ^= h >> 16;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
    }

    FireflySwarm::FireflySwarm(const IslandTerrain& terrain) noexcept
    {
        constexpr float Tau = 2.0f * std::numbers::pi_v<float>;
        for (std::size_t i = 0; i < Count; ++i) {
            const auto n = static_cast<std::uint32_t>(i);
            Home& home = homes_[i];
            const bool stream = i < StreamCount;
            // three clusters along the stream and one at the wetland; within a cluster a
            // share of the flies keep the cluster's beat (they flash together)
            const std::uint32_t cluster = stream ? n / 30U : 3U;
            if (stream) {
                const float along = 10.0f + 26.0f * static_cast<float>(cluster) + 22.0f * Unit(n, 1U);
                const float x = StreamRoute::SourceX - along;
                const float side = Unit(n, 2U) < 0.5f ? -1.0f : 1.0f;
                home.x = x;
                home.z = StreamRoute::CentreZ(x) + side * (StreamRoute::HalfWidth(x) + 0.8f + 4.0f * Unit(n, 3U));
            } else {
                const float angle = Tau * Unit(n, 4U), r = 0.25f + 0.7f * Unit(n, 5U);
                home.x = IslandEnvironmentField::SeasonalWetlandX + std::cos(angle) * IslandEnvironmentField::SeasonalWetlandRadiusX * r;
                home.z = IslandEnvironmentField::SeasonalWetlandZ + std::sin(angle) * IslandEnvironmentField::SeasonalWetlandRadiusZ * r;
            }
            home.ground = std::max(terrain.Height(home.x, home.z), 0.0f);
            home.reachX = 0.8f + 1.8f * Unit(n, 6U);
            home.reachZ = 0.8f + 1.8f * Unit(n, 7U);
            home.rateX = 0.09f + 0.16f * Unit(n, 8U);
            home.rateZ = 0.08f + 0.15f * Unit(n, 9U);
            home.rateY = 0.05f + 0.10f * Unit(n, 10U);
            home.phaseX = Tau * Unit(n, 11U);
            home.phaseZ = Tau * Unit(n, 12U);
            home.phaseY = Tau * Unit(n, 13U);
            const bool inStep = Unit(n, 14U) < 0.45f;
            home.period = inStep ? 2.0f + 0.5f * static_cast<float>(cluster) : 1.6f + 1.9f * Unit(n, 15U);
            home.blinkPhase = inStep ? 0.31f * static_cast<float>(cluster) : Unit(n, 16U);
        }
    }

    void FireflySwarm::Update(const float seconds, const float nightness, const float rain) noexcept
    {
        const float dt = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.25f) : 0.0f;
        elapsed_ += dt;
        // out once it is properly dark, back under the leaves in the rain
        const float wanted = std::clamp((nightness - 0.55f) / 0.30f, 0.0f, 1.0f)
            * (1.0f - std::clamp((rain - 0.15f) / 0.25f, 0.0f, 1.0f));
        if (!settled_) {
            presence_ = wanted;
            settled_ = true;
        }
        presence_ += std::clamp(wanted - presence_, -dt * 0.25f, dt * 0.25f);
        const float t = elapsed_;
        for (std::size_t i = 0; i < Count; ++i) {
            const Home& home = homes_[i];
            Firefly& fly = flies_[i];
            fly.x = home.x + home.reachX * std::sin(t * home.rateX + home.phaseX)
                + 0.35f * std::sin(t * home.rateX * 3.1f + home.phaseZ);
            fly.z = home.z + home.reachZ * std::sin(t * home.rateZ + home.phaseZ)
                + 0.35f * std::sin(t * home.rateZ * 2.7f + home.phaseX);
            fly.y = home.ground + 0.55f + 0.9f * (0.5f + 0.5f * std::sin(t * home.rateY + home.phaseY));
            // a flash: a quick rise and a slower fade, once a period
            const float cycle = t / home.period + home.blinkPhase;
            const float since = (cycle - std::floor(cycle)) * home.period;
            float flash = 0.0f;
            if (since < FlashSeconds) {
                const float u = since / FlashSeconds;
                flash = u < 0.25f ? u / 0.25f : 1.0f - (u - 0.25f) / 0.75f;
            }
            // only a share of the swarm is out on a thin night
            const float share = static_cast<float>(i % 8U) / 8.0f;
            fly.glow = presence_ > share ? flash * std::clamp((presence_ - share) * 4.0f, 0.0f, 1.0f) : 0.0f;
        }
    }

    std::size_t FireflySwarm::GlowingCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(flies_.begin(), flies_.end(), [](const Firefly& f) { return f.glow > 0.05f; }));
    }
}
