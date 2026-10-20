#include "IslandSimulator/ButterflyMeadow.hpp"

#include "IslandSimulator/FallingFruitField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/VegetationLayout.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        constexpr float GoldenAngle = 2.39996322972865332f;

        struct HabitatDefinition
        {
            float x;
            float z;
            float radius;
        };

        constexpr std::array<HabitatDefinition, ButterflyMeadow::HabitatCount> Habitats = {{
            {VegetationLayout::EastClearing.x - 3.0f,
             VegetationLayout::EastClearing.z + 1.5f, 11.5f},
            {VegetationLayout::EastClearing.x + 17.0f,
             VegetationLayout::EastClearing.z + 11.0f, 7.5f},
            {VegetationLayout::WestClearing.x + 2.0f,
             VegetationLayout::WestClearing.z - 1.0f, 9.5f}
        }};

        [[nodiscard]] float WrapAngle(float value) noexcept
        {
            while (value > std::numbers::pi_v<float>) value -= TwoPi;
            while (value < -std::numbers::pi_v<float>) value += TwoPi;
            return value;
        }

        [[nodiscard]] float MoveAngle(const float current, const float target,
                                      const float maximumStep) noexcept
        {
            const float difference = WrapAngle(target - current);
            return WrapAngle(current + std::clamp(difference, -maximumStep, maximumStep));
        }
    }

    ButterflyMeadow::ButterflyMeadow(const IslandTerrain& terrain)
        : terrain_(terrain)
    {
        for (std::size_t index = 0; index < flowerPatches_.size(); ++index) {
            const HabitatDefinition& habitat = Habitats[index];
            constexpr float Probe = 0.5f;
            flowerPatches_[index] = ButterflyFlowerPatch{
                habitat.x, terrain_.Height(habitat.x, habitat.z), habitat.z,
                1.8f + static_cast<float>(index) * 0.22f,
                (terrain_.Height(habitat.x + Probe, habitat.z)
                    - terrain_.Height(habitat.x - Probe, habitat.z)) / (2.0f * Probe),
                (terrain_.Height(habitat.x, habitat.z + Probe)
                    - terrain_.Height(habitat.x, habitat.z - Probe)) / (2.0f * Probe)};
        }
        agents_.resize(ButterflyCount);
        states_.resize(ButterflyCount);
        homes_.resize(ButterflyCount);
        for (std::size_t index = 0; index < ButterflyCount; ++index) {
            InitializeAgent(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
    }

    void ButterflyMeadow::SetFlowerBushes(const std::span<const BushPlacement> bushes)
    {
        for (auto& sites : bushSites_) sites.clear();
        for (const BushPlacement& bush : bushes) {
            // Variants 1 and 2 carry hibiscus and plumeria blossoms; the others are plain.
            if (bush.variant != 1U && bush.variant != 2U) continue;
            if (!std::isfinite(bush.x) || !std::isfinite(bush.z) || !std::isfinite(bush.scale)) continue;
            for (std::size_t habitat = 0; habitat < HabitatCount; ++habitat) {
                const HabitatDefinition& definition = Habitats[habitat];
                if (std::hypot(bush.x - definition.x, bush.z - definition.z)
                    > definition.radius + BushExcursionReach) continue;
                if (bushSites_[habitat].size() >= 6U) break;
                // Blossoms sit on the crown: hover just above it, clear of the leaves.
                bushSites_[habitat].push_back({bush.x, bush.z, std::clamp(0.62f * bush.scale + 0.30f, 0.5f, 1.6f)});
                break;
            }
        }
    }

    std::size_t ButterflyMeadow::FlowerBushCount() const noexcept
    {
        std::size_t count = 0;
        for (const auto& sites : bushSites_) count += sites.size();
        return count;
    }

    // The nearest fruit that has lain long enough to ferment, within the habitat's excursion
    // reach and on ground the butterfly may hover over; coconuts never draw them.
    bool ButterflyMeadow::FindFermentingFruit(const Agent& agent, NectarSite& site) const noexcept
    {
        float best = std::numeric_limits<float>::max();
        bool found = false;
        for (const FallingFruit& fruit : lyingFruit_) {
            if (!fruit.active || fruit.restSeconds < FermentSeconds || fruit.rolling
                || fruit.variant == FallingFruitField::CoconutVariant) continue;
            const float fromHome = std::hypot(fruit.x - agent.home.x, fruit.z - agent.home.z);
            if (fromHome > agent.home.radius + BushExcursionReach) continue;
            const float distance = std::hypot(fruit.x - agent.state.x, fruit.z - agent.state.z);
            if (distance >= best) continue;
            // Fruit lies under the crowns, on forest floor the clearing rule would refuse;
            // the hover there only needs inland, gentle ground.
            const TerrainSample sample = terrain_.Sample(fruit.x, fruit.z);
            if (sample.signedShoreDistance < MinimumInlandDistance || sample.slopeRadians >= 0.48f) continue;
            best = distance;
            site = {fruit.x, fruit.z, FruitHoverHeight};
            found = true;
        }
        return found;
    }

    bool ButterflyMeadow::NectarTarget(const std::size_t index, float& x, float& z, float& height) const noexcept
    {
        if (index >= agents_.size()) return false;
        x = agents_[index].nectarX; z = agents_[index].nectarZ; height = agents_[index].nectarHeight;
        return true;
    }

    void ButterflyMeadow::InitializeAgent(const std::size_t index)
    {
        Agent& agent = agents_[index];
        const HabitatDefinition& habitat = Habitats[index / 6U];
        agent.home = {habitat.x, habitat.z, habitat.radius};
        const float sequence = static_cast<float>(index);
        const float angle = std::fmod(0.37f + sequence * GoldenAngle, TwoPi);
        const float nectarRadius = 0.35f + static_cast<float>(index % 6U) * 0.22f;
        agent.nectarX = habitat.x + std::cos(angle) * nectarRadius;
        agent.nectarZ = habitat.z + std::sin(angle) * nectarRadius;
        agent.patchX = agent.nectarX;
        agent.patchZ = agent.nectarZ;
        agent.nectarHeight = 0.34f;
        float placementRadius = 1.6f + static_cast<float>(index % 6U) * 0.72f;
        agent.state.x = agent.home.x + std::cos(angle) * placementRadius;
        agent.state.z = agent.home.z + std::sin(angle) * placementRadius;
        for (int attempt = 0;
             attempt < 5 && !IsValidHabitat(agent.state.x, agent.state.z, agent.home);
             ++attempt) {
            placementRadius *= 0.55f;
            agent.state.x = agent.home.x + std::cos(angle) * placementRadius;
            agent.state.z = agent.home.z + std::sin(angle) * placementRadius;
        }
        if (!IsValidHabitat(agent.state.x, agent.state.z, agent.home)) {
            agent.state.x = agent.home.x;
            agent.state.z = agent.home.z;
        }

        agent.state.headingRadians = WrapAngle(angle + std::numbers::pi_v<float> * 0.5f);
        agent.state.scale = 0.16f + static_cast<float>(index % 5U) * 0.012f;
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 5U);
        agent.state.behavior = index % 6U == 0U
            ? ButterflyBehavior::Rest
            : (index % 4U == 0U ? ButterflyBehavior::NectarHover
                                 : ButterflyBehavior::Flutter);
        agent.behaviorSeconds = 1.4f + static_cast<float>(index % 7U) * 0.63f;
        agent.baseAltitude = 0.62f + static_cast<float>(index % 6U) * 0.14f;
        agent.turnPhaseA = std::fmod(sequence * 1.311f + 0.4f, TwoPi);
        agent.turnPhaseB = std::fmod(sequence * 2.137f + 1.2f, TwoPi);
        agent.verticalPhase = std::fmod(sequence * 1.773f + 0.7f, TwoPi);
        agent.wingPhase = std::fmod(sequence * 2.419f, TwoPi);
        const float terrainHeight = terrain_.Height(agent.state.x, agent.state.z);
        if (agent.state.behavior == ButterflyBehavior::Rest) {
            agent.state.y = terrainHeight + 0.16f;
            agent.state.wingFold = 0.86f;
        } else {
            agent.state.y = terrainHeight + agent.baseAltitude;
            agent.state.wingFold = 0.35f + 0.30f * std::sin(agent.wingPhase);
        }
    }

    bool ButterflyMeadow::IsValidHabitat(
        const float x, const float z, const ButterflyHomeRange& home,
        const float reachRadius) const noexcept
    {
        const TerrainSample sample = terrain_.Sample(x, z);
        const float homeDistance = std::hypot(x - home.x, z - home.z);
        const bool vegetatedGround = sample.surface == TerrainSurface::CoastalGround
            || sample.surface == TerrainSurface::InteriorGround;
        return vegetatedGround && sample.signedShoreDistance >= MinimumInlandDistance
            && sample.slopeRadians < 0.48f && homeDistance <= std::max(home.radius, reachRadius);
    }

    void ButterflyMeadow::Update(const float deltaSeconds, const float observerX,
                                 const float observerY, const float observerZ) noexcept
    {
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, 0.1f) : 0.0f;
        const bool observerValid = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        ++frameIndex_;
        fullSimulationCount_ = 0;
        reducedSimulationCount_ = 0;
        dormantSimulationCount_ = 0;

        for (std::size_t index = 0; index < agents_.size(); ++index) {
            Agent& agent = agents_[index];
            const float distance = observerValid
                ? std::hypot(agent.state.y - observerY,
                             std::hypot(agent.state.x - observerX,
                                        agent.state.z - observerZ))
                : std::numeric_limits<float>::infinity();
            agent.state.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
            switch (agent.state.simulationTier) {
            case WildlifeSimulationTier::Full: ++fullSimulationCount_; break;
            case WildlifeSimulationTier::Reduced: ++reducedSimulationCount_; break;
            case WildlifeSimulationTier::Dormant: ++dormantSimulationCount_; break;
            }

            agent.accumulatedSeconds += safeDelta;
            const std::uint32_t divisor = WildlifeActivationPolicy::UpdateDivisor(
                agent.state.simulationTier);
            if ((frameIndex_ + index) % divisor == 0U) {
                UpdateAgent(agent, std::min(agent.accumulatedSeconds, 0.5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
            }
            states_[index] = agent.state;
        }

#ifndef NDEBUG
        assert(fullSimulationCount_ + reducedSimulationCount_ + dormantSimulationCount_
               == static_cast<int>(ButterflyCount));
        for (const ButterflyState& butterfly : states_) {
            assert(std::isfinite(butterfly.x) && std::isfinite(butterfly.y)
                   && std::isfinite(butterfly.z));
            assert(std::isfinite(butterfly.headingRadians));
            assert(std::isfinite(butterfly.speed) && butterfly.speed >= 0.0f);
            assert(std::isfinite(butterfly.wingFold)
                   && butterfly.wingFold >= 0.0f && butterfly.wingFold <= 1.0f);
            assert(std::isfinite(butterfly.scale) && butterfly.scale > 0.0f);
        }
#endif
    }

    void ButterflyMeadow::UpdateAgent(Agent& agent, const float deltaSeconds,
                                      const float observerX, const float observerZ,
                                      const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;

        float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // A passing animal or the resident frightens a butterfly as the walker does: the
        // nearest body is the one scattered from.
        float threatX = observerX, threatZ = observerZ;
        for (const GrassWalker& body : walkers_) {
            if (body.strength <= 0.0f || !std::isfinite(body.x) || !std::isfinite(body.z)) continue;
            const float distance = std::hypot(agent.state.x - body.x, agent.state.z - body.z);
            if (distance < observerDistance) { observerDistance = distance; threatX = body.x; threatZ = body.z; }
        }
        if (observerDistance < 2.6f) {
            if (agent.state.behavior != ButterflyBehavior::Flutter || agent.behaviorSeconds < 2.0f) ++scatters_;
            agent.state.behavior = ButterflyBehavior::Flutter;
            agent.behaviorSeconds = 2.3f;
            const float awayX = agent.state.x - threatX;
            const float awayZ = agent.state.z - threatZ;
            if (std::hypot(awayX, awayZ) > 0.001f) {
                agent.state.headingRadians = std::atan2(awayZ, awayX);
            }
        } else {
            agent.behaviorSeconds -= deltaSeconds;
            if (agent.behaviorSeconds <= 0.0f) {
                ++agent.transitionCount;
                switch (agent.state.behavior) {
                case ButterflyBehavior::Rest:
                    agent.state.behavior = ButterflyBehavior::Flutter;
                    agent.behaviorSeconds = 5.0f
                        + static_cast<float>(agent.transitionCount % 6U) * 0.81f;
                    break;
                case ButterflyBehavior::Flutter:
                    agent.state.behavior = agent.transitionCount % 3U == 0U
                        ? ButterflyBehavior::Rest : ButterflyBehavior::NectarHover;
                    agent.behaviorSeconds = agent.state.behavior == ButterflyBehavior::Rest
                        ? 1.6f + static_cast<float>(agent.transitionCount % 5U) * 0.52f
                        : 1.3f + static_cast<float>(agent.transitionCount % 4U) * 0.46f;
                    if (agent.state.behavior == ButterflyBehavior::NectarHover) {
                        // Every other nectar visit goes to a flowering bush near the habitat
                        // when it has one; the rest return to the ground patch. A bush beyond
                        // a few metres is an excursion: keep fluttering toward it first.
                        const std::size_t index = static_cast<std::size_t>(&agent - agents_.data());
                        const auto& sites = bushSites_[index / 6U];
                        NectarSite fruitSite;
                        const bool toFruit = agent.transitionCount % 4U == 0U && FindFermentingFruit(agent, fruitSite);
                        if (toFruit) ++fruitVisits_;
                        if (toFruit || (!sites.empty() && agent.transitionCount % 2U == 1U)) {
                            // Fermenting fruit on the ground takes every other ground-patch
                            // visit (the nectar decisions fall on even counts): a trip down to
                            // it, hovering low over the fruit.
                            const NectarSite& site = toFruit ? fruitSite
                                : sites[(index + agent.transitionCount / 2U) % sites.size()];
                            const float toward = std::atan2(agent.state.z - site.z, agent.state.x - site.x);
                            agent.nectarX = site.x + std::cos(toward) * 0.45f;
                            agent.nectarZ = site.z + std::sin(toward) * 0.45f;
                            agent.nectarHeight = site.height;
                            agent.reachRadius = std::hypot(site.x - agent.home.x, site.z - agent.home.z) + 2.5f;
                            const float trip = std::hypot(agent.nectarX - agent.state.x, agent.nectarZ - agent.state.z);
                            if (trip > 3.0f) {
                                agent.state.behavior = ButterflyBehavior::Flutter;
                                agent.travelling = true;
                                agent.behaviorSeconds = trip / 0.40f + 4.0f;
                            } else {
                                agent.behaviorSeconds += 1.2f;
                            }
                        } else {
                            agent.nectarX = agent.patchX;
                            agent.nectarZ = agent.patchZ;
                            agent.nectarHeight = 0.34f;
                        }
                    }
                    break;
                case ButterflyBehavior::NectarHover:
                    agent.state.behavior = agent.transitionCount % 2U == 0U
                        ? ButterflyBehavior::Rest : ButterflyBehavior::Flutter;
                    agent.behaviorSeconds = agent.state.behavior == ButterflyBehavior::Rest
                        ? 1.4f + static_cast<float>(agent.transitionCount % 5U) * 0.48f
                        : 4.8f + static_cast<float>(agent.transitionCount % 6U) * 0.77f;
                    break;
                }
            }
        }

        agent.turnPhaseA = std::fmod(agent.turnPhaseA + deltaSeconds * 1.17f, TwoPi);
        agent.turnPhaseB = std::fmod(agent.turnPhaseB + deltaSeconds * 0.43f, TwoPi);
        agent.verticalPhase = std::fmod(agent.verticalPhase + deltaSeconds * 2.1f, TwoPi);
        const float targetSpeed = agent.state.behavior == ButterflyBehavior::Flutter
            ? 0.46f + static_cast<float>(agent.state.colorVariant) * 0.035f
            : (agent.state.behavior == ButterflyBehavior::NectarHover ? 0.10f : 0.0f);
        agent.state.speed += std::clamp(
            targetSpeed - agent.state.speed, -deltaSeconds * 0.75f, deltaSeconds * 1.25f);

        if (agent.state.behavior != ButterflyBehavior::Rest) {
            const float homeDistance = std::hypot(
                agent.state.x - agent.home.x, agent.state.z - agent.home.z);
            if (agent.travelling) {
                // Out to the bush: once there, settle into the hover.
                const float remaining = std::hypot(agent.nectarX - agent.state.x, agent.nectarZ - agent.state.z);
                if (remaining < 1.0f || agent.behaviorSeconds <= 0.0f) {
                    agent.travelling = false;
                    agent.state.behavior = ButterflyBehavior::NectarHover;
                    agent.behaviorSeconds = 4.0f + static_cast<float>(agent.transitionCount % 4U) * 0.8f;
                }
            }
            // Back inside the home range, the excursion allowance lapses.
            if (!agent.travelling && agent.state.behavior != ButterflyBehavior::NectarHover
                && homeDistance <= agent.home.radius) agent.reachRadius = 0.0f;
            if (agent.state.behavior == ButterflyBehavior::NectarHover || agent.travelling) {
                const float nectarHeading = std::atan2(
                    agent.nectarZ - agent.state.z, agent.nectarX - agent.state.x);
                agent.state.headingRadians = MoveAngle(
                    agent.state.headingRadians, nectarHeading, deltaSeconds * 2.8f);
            } else if (homeDistance > agent.home.radius * 0.66f) {
                const float homeHeading = std::atan2(
                    agent.home.z - agent.state.z, agent.home.x - agent.state.x);
                agent.state.headingRadians = MoveAngle(
                    agent.state.headingRadians, homeHeading, deltaSeconds * 1.9f);
            } else {
                const float turnRate = std::sin(agent.turnPhaseA) * 1.05f
                    + std::sin(agent.turnPhaseB) * 0.58f;
                agent.state.headingRadians = WrapAngle(
                    agent.state.headingRadians + turnRate * deltaSeconds);
            }

            const float candidateX = agent.state.x
                + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            const float candidateZ = agent.state.z
                + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            if (IsValidHabitat(candidateX, candidateZ, agent.home, agent.reachRadius)) {
                agent.state.x = candidateX;
                agent.state.z = candidateZ;
            } else {
                agent.state.headingRadians = std::atan2(
                    agent.home.z - agent.state.z, agent.home.x - agent.state.x);
                agent.state.speed = std::min(agent.state.speed, 0.22f);
            }
        }

        const float groundHeight = terrain_.Height(agent.state.x, agent.state.z);
        float targetHeight = groundHeight + 0.16f;
        if (agent.state.behavior == ButterflyBehavior::Flutter) {
            const float irregularLift = std::sin(agent.verticalPhase) * 0.32f
                + std::sin(agent.turnPhaseA * 1.7f) * 0.12f;
            targetHeight = groundHeight
                + std::clamp(agent.baseAltitude + irregularLift, 0.42f, 1.75f);
        } else if (agent.state.behavior == ButterflyBehavior::NectarHover) {
            targetHeight = groundHeight + agent.nectarHeight
                + std::sin(agent.verticalPhase * 1.4f) * 0.08f;
        }
        agent.state.y += std::clamp(
            targetHeight - agent.state.y, -deltaSeconds * 0.82f, deltaSeconds * 0.82f);

        const float wingRate = agent.state.behavior == ButterflyBehavior::Rest
            ? 1.2f : (agent.state.behavior == ButterflyBehavior::NectarHover ? 24.0f : 31.0f);
        agent.wingPhase = std::fmod(agent.wingPhase + deltaSeconds * wingRate, TwoPi);
        float wingTarget = 0.86f + std::sin(agent.wingPhase) * 0.05f;
        if (agent.state.behavior != ButterflyBehavior::Rest) {
            wingTarget = 0.12f + (0.5f + 0.5f * std::sin(agent.wingPhase)) * 0.78f;
        }
        agent.state.wingFold += std::clamp(
            wingTarget - agent.state.wingFold, -deltaSeconds * 12.0f, deltaSeconds * 12.0f);
        agent.state.wingFold = std::clamp(agent.state.wingFold, 0.0f, 1.0f);
    }

    std::span<const ButterflyState> ButterflyMeadow::Butterflies() const noexcept
    {
        return states_;
    }

    std::span<const ButterflyHomeRange> ButterflyMeadow::Homes() const noexcept
    {
        return homes_;
    }
}
