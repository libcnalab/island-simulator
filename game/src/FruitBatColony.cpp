#include "IslandSimulator/FruitBatColony.hpp"

#include "IslandSimulator/CanopyProfile.hpp"
#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace island
{
    namespace
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;

        [[nodiscard]] bool Separated(const std::vector<FruitBatRoost>& roosts,
                                     const float x, const float z) noexcept
        {
            return std::ranges::all_of(roosts, [=](const FruitBatRoost& roost) {
                return std::hypot(roost.x - x, roost.z - z) >= 20.0f;
            });
        }
    }

    FruitBatColony::FruitBatColony(const IslandTerrain& terrain,
                                   const std::span<const CanopyPlacement> canopy)
    {
        roosts_.reserve(Count);
        for (std::size_t index = 0; index < canopy.size() && roosts_.size() < Count; ++index) {
            const CanopyPlacement& tree = canopy[index];
            const float radius = std::hypot(tree.x, tree.z);
            if (index % 7U != 2U || tree.scale < .82f || tree.scale > 1.48f
                || radius < 45.0f || radius > 155.0f
                || terrain.SignedShoreDistance(tree.x, tree.z) < 45.0f
                || !Separated(roosts_, tree.x, tree.z)) {
                continue;
            }
            const float crownHeight = CanopyNominalHeight(tree.variant) * tree.scale;
            roosts_.push_back({
                tree.x + std::cos(tree.yawRadians + .7f) * .55f * tree.scale,
                // Hang beneath the lower crown rather than inside its opaque leaf cores.
                // The previous 70% nominal-height point hid both animals and their branch
                // contact from any plausible ground view.
                tree.y + crownHeight * .56f,
                tree.z + std::sin(tree.yawRadians + .7f) * .55f * tree.scale,
                tree.yawRadians + std::numbers::pi_v<float>});
            roosts_.back().tree = tree;
        }
        if (roosts_.size() != Count) {
            throw std::runtime_error("Unable to derive six separated fruit-bat canopy roosts");
        }

        agents_.resize(Count);
        states_.resize(Count);
        for (std::size_t index = 0; index < Count; ++index) {
            Agent& agent = agents_[index];
            agent.roost = roosts_[index];
            agent.orbitRadius = 8.5f + static_cast<float>(index % 3U) * 1.8f;
            agent.orbitPhase = std::fmod(.73f + static_cast<float>(index) * 2.39996323f, TwoPi);
            agent.orbitSpeed = .29f + static_cast<float>(index % 2U) * .035f;
            agent.flapPhase = static_cast<float>(index) * 1.37f;
            agent.state.scale = .84f + static_cast<float>(index % 3U) * .08f;
            agent.state.colorVariant = static_cast<std::uint8_t>(index % 3U);
            agent.state.behavior = index < MaximumDayFlights
                ? FruitBatBehavior::DayFlight : FruitBatBehavior::Roost;
            if (agent.state.behavior == FruitBatBehavior::Roost) {
                agent.state.x = agent.roost.x;
                agent.state.y = agent.roost.y - .38f * agent.state.scale;
                agent.state.z = agent.roost.z;
                agent.state.headingRadians = agent.roost.headingRadians;
                agent.state.hangAmount = 1.0f;
            } else {
                const float angle = agent.orbitPhase;
                agent.state.x = agent.roost.x + std::cos(angle) * agent.orbitRadius;
                agent.state.z = agent.roost.z + std::sin(angle) * agent.orbitRadius;
                agent.state.y = agent.roost.y + 3.2f + .7f * std::sin(angle * 1.7f);
                agent.state.headingRadians = angle + std::numbers::pi_v<float> * .5f;
                agent.state.speed = agent.orbitRadius * agent.orbitSpeed;
                agent.state.hangAmount = 0.0f;
                agent.state.wingPose = std::sin(agent.flapPhase + .8f);
                agent.airborne = 1.0f;
            }
            states_[index] = agent.state;
        }
    }

    std::size_t FruitBatColony::StretchingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.stretching ? 1U : 0U;
        return count;
    }

    std::size_t FruitBatColony::FlyingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.behavior == FruitBatBehavior::DayFlight ? 1U : 0U;
        return count;
    }

    std::size_t FruitBatColony::FanningCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.fanning ? 1U : 0U;
        return count;
    }

    void FruitBatColony::Update(const float deltaSeconds, const float observerX,
                                const float observerY, const float observerZ) noexcept
    {
        const bool valid = std::isfinite(deltaSeconds) && deltaSeconds >= 0.0f
            && std::isfinite(observerX) && std::isfinite(observerY) && std::isfinite(observerZ);
        const float boundedDelta = valid ? std::min(deltaSeconds, .1f) : 0.0f;
        fullSimulationCount_ = reducedSimulationCount_ = dormantSimulationCount_ = 0;
        for (std::size_t index = 0; index < agents_.size(); ++index) {
            Agent& agent = agents_[index];
            const float dx = agent.state.x - observerX;
            const float dy = agent.state.y - observerY;
            const float dz = agent.state.z - observerZ;
            agent.state.simulationTier = valid
                ? WildlifeActivationPolicy::Classify(
                    std::sqrt(dx * dx + dy * dy + dz * dz),
                    FullSimulationDistance, ReducedSimulationDistance)
                : WildlifeSimulationTier::Dormant;
            switch (agent.state.simulationTier) {
            case WildlifeSimulationTier::Full: ++fullSimulationCount_; break;
            case WildlifeSimulationTier::Reduced: ++reducedSimulationCount_; break;
            case WildlifeSimulationTier::Dormant: ++dormantSimulationCount_; break;
            }
            const auto divisor = WildlifeActivationPolicy::UpdateDivisor(agent.state.simulationTier);
            if (boundedDelta > 0.0f && frameIndex_ % divisor == index % divisor) {
                const float step = boundedDelta * static_cast<float>(divisor);
                agent.accumulatedSeconds += step;
                // By day two bats are out on their rounds; from dusk the whole colony is.
                const bool out = index < MaximumDayFlights || nightness_ > 0.55f;
                agent.airborne = std::clamp(agent.airborne + (out ? step : -step) / 3.5f, 0.0f, 1.0f);
                if (agent.airborne > 0.0f && agent.state.behavior != FruitBatBehavior::DayFlight) {
                    agent.state.behavior = FruitBatBehavior::DayFlight;
                    agent.state.hangAmount = 0.0f;
                } else if (agent.airborne <= 0.0f && agent.state.behavior == FruitBatBehavior::DayFlight) {
                    agent.state.behavior = FruitBatBehavior::Roost;
                    agent.state.x = agent.roost.x;
                    agent.state.y = agent.roost.y - .38f * agent.state.scale;
                    agent.state.z = agent.roost.z;
                    agent.state.headingRadians = agent.roost.headingRadians;
                    agent.state.hangAmount = 1.0f;
                }
                if (agent.state.behavior == FruitBatBehavior::DayFlight) {
                    // the night's rounds go wider, to the fruiting crowns further off
                    const float radius = agent.orbitRadius * (1.0f + 1.6f * nightness_);
                    const float angle = agent.orbitPhase + agent.accumulatedSeconds * agent.orbitSpeed;
                    const float orbitX = agent.roost.x + std::cos(angle) * radius;
                    const float orbitZ = agent.roost.z + std::sin(angle) * radius;
                    const float orbitY = agent.roost.y + 3.2f
                        + .7f * std::sin(angle * 1.7f + static_cast<float>(index));
                    // taking off or settling: drawn along a smooth path between the hang and the round
                    const float a = agent.airborne * agent.airborne * (3.0f - 2.0f * agent.airborne);
                    agent.state.x = agent.roost.x + (orbitX - agent.roost.x) * a;
                    agent.state.z = agent.roost.z + (orbitZ - agent.roost.z) * a;
                    agent.state.y = agent.roost.y - .38f * agent.state.scale + (orbitY - agent.roost.y + .38f * agent.state.scale) * a;
                    agent.state.headingRadians = angle + std::numbers::pi_v<float> * .5f;
                    agent.state.wingPose = std::sin(
                        agent.flapPhase + agent.accumulatedSeconds * 5.8f);
                } else {
                    // A hanging bat fans: every quarter minute or so (a little per bat) it
                    // half-opens its wings and beats them slowly for a few seconds to cool.
                    const float period = FanPeriodSeconds + 2.0f * static_cast<float>(index % 3U);
                    const float cycle = std::fmod(agent.accumulatedSeconds + 2.1f * static_cast<float>(index), period);
                    const bool fanning = cycle < FanSeconds;
                    if (fanning && !agent.fanning) ++fans_;
                    agent.fanning = fanning;
                    agent.state.fan += std::clamp((fanning ? 1.0f : 0.0f) - agent.state.fan, -step * 3.0f, step * 3.0f);
                    agent.state.wingPose = fanning ? std::sin(agent.accumulatedSeconds * FanBeatRate) : agent.state.wingPose * 0.8f;
                    // And now and then, between its fans, a hanging bat stretches one wing out
                    // and down for a couple of seconds and folds it again, the wings taking turns.
                    const float stretchPeriod = StretchPeriodSeconds + 3.0f * static_cast<float>(index % 2U);
                    const float stretchCycle = std::fmod(agent.accumulatedSeconds + 9.0f + 1.7f * static_cast<float>(index), stretchPeriod);
                    const bool stretching = stretchCycle < StretchSeconds && !fanning;
                    if (stretching && !agent.stretching) {
                        ++stretches_;
                        agent.state.stretchSide = -agent.state.stretchSide;
                    }
                    agent.stretching = stretching;
                    agent.state.stretch += std::clamp((stretching ? 1.0f : 0.0f) - agent.state.stretch, -step * 2.0f, step * 1.5f);
                }
            }
            states_[index] = agent.state;
        }
        ++frameIndex_;
    }
}
