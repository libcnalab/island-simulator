#include "IslandSimulator/GeckoPopulation.hpp"

#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/VegetationLayout.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace island
{
    namespace
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        constexpr std::array<std::array<float, 2>, GeckoPopulation::Count> HabitatCentres = {{
            {{VegetationLayout::EastClearing.x - 12.0f, VegetationLayout::EastClearing.z + 4.0f}},
            {{VegetationLayout::EastClearing.x + 14.0f, VegetationLayout::EastClearing.z - 5.0f}},
            {{VegetationLayout::WestClearing.x - 10.0f, VegetationLayout::WestClearing.z + 3.0f}},
            {{VegetationLayout::WestClearing.x + 12.0f, VegetationLayout::WestClearing.z - 7.0f}},
            {{IslandEnvironmentField::SeasonalWetlandX - 18.0f,
              IslandEnvironmentField::SeasonalWetlandZ + 4.0f}},
            {{IslandEnvironmentField::SeasonalWetlandX + 18.0f,
              IslandEnvironmentField::SeasonalWetlandZ - 3.0f}},
            {{-42.0f, -74.0f}},
            {{48.0f, -58.0f}},
        }};

        [[nodiscard]] float WrapAngle(float value) noexcept
        {
            while (value > std::numbers::pi_v<float>) value -= TwoPi;
            while (value < -std::numbers::pi_v<float>) value += TwoPi;
            return value;
        }

        [[nodiscard]] float MoveAngle(float current, float target, float maximumStep) noexcept
        {
            const float difference = WrapAngle(target - current);
            return WrapAngle(current + std::clamp(difference, -maximumStep, maximumStep));
        }
    }

    GeckoPopulation::GeckoPopulation(const IslandTerrain& terrain) : terrain_(terrain)
    {
        for (std::size_t index = 0; index < Count; ++index) {
            Initialize(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
    }

    void GeckoPopulation::Initialize(const std::size_t index)
    {
        Agent& agent = agents_[index];
        const auto centre = HabitatCentres[index];
        agent.home.radius = 5.2f + static_cast<float>(index % 3U) * .45f;
        bool found = false;
        for (int attempt = 0; attempt < 96 && !found; ++attempt) {
            const float angle = static_cast<float>(attempt) * 2.39996323f
                + static_cast<float>(index) * .71f;
            const float radius = .4f + static_cast<float>(attempt / 12) * .72f;
            agent.home.x = centre[0] + std::cos(angle) * radius;
            agent.home.z = centre[1] + std::sin(angle) * radius;
            found = ValidGround(agent.home.x, agent.home.z, agent.home);
        }
        if (!found) throw std::runtime_error("No suitable inland gecko habitat");
        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .038f;
        agent.state.headingRadians = WrapAngle(.43f + static_cast<float>(index) * 1.37f);
        // Tokay-sized rather than monitor-lizard-sized: roughly 37--47 cm including tail.
        agent.state.scale = .34f + static_cast<float>(index % 4U) * .025f;
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 4U);
        agent.state.behavior = index % 4U == 0U ? GeckoBehavior::Scurry : GeckoBehavior::Rest;
        agent.behaviorSeconds = 3.2f + static_cast<float>(index % 5U) * 1.15f;
        agent.state.gaitPhase = static_cast<float>(index) * .83f;
    }

    bool GeckoPopulation::ValidGround(const float x, const float z,
                                      const GeckoHome& home) const noexcept
    {
        const auto sample = terrain_.Sample(x, z);
        const bool surface = sample.surface == TerrainSurface::CoastalGround
            || sample.surface == TerrainSurface::InteriorGround
            || sample.surface == TerrainSurface::RockyGround;
        return surface && sample.signedShoreDistance >= 34.0f
            && sample.slopeRadians < .52f
            && std::hypot(x - home.x, z - home.z) <= home.radius;
    }

    std::size_t GeckoPopulation::LickingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.licking ? 1U : 0U;
        return count;
    }

    std::size_t GeckoPopulation::WaggingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.behavior == GeckoBehavior::Stalk ? 1U : 0U;
        return count;
    }

    void GeckoPopulation::Update(const float deltaSeconds, const float observerX,
                                 const float observerY, const float observerZ) noexcept
    {
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, .1f) : 0.0f;
        const bool observerValid = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        ++frameIndex_;
        fullSimulationCount_ = reducedSimulationCount_ = dormantSimulationCount_ = 0;
        for (std::size_t index = 0; index < agents_.size(); ++index) {
            Agent& agent = agents_[index];
            const float distance = observerValid
                ? std::hypot(agent.state.y - observerY,
                    std::hypot(agent.state.x - observerX, agent.state.z - observerZ))
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
                UpdateAgent(agent, std::min(agent.accumulatedSeconds, .5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
            }
            states_[index] = agent.state;
        }
#ifndef NDEBUG
        assert(fullSimulationCount_ + reducedSimulationCount_ + dormantSimulationCount_
               == static_cast<int>(Count));
#endif
    }

    void GeckoPopulation::UpdateAgent(Agent& agent, const float deltaSeconds,
                                      const float observerX, const float observerZ,
                                      const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;
        const float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // A passing animal or the resident frightens a gecko as the walker does.
        float threatDistance = observerDistance, threatX = observerX, threatZ = observerZ;
        bool walkerThreat = false;
        for (const GrassWalker& body : walkers_) {
            if (body.strength <= 0.0f || !std::isfinite(body.x) || !std::isfinite(body.z)) continue;
            const float bodyDistance = std::hypot(agent.state.x - body.x, agent.state.z - body.z);
            if (bodyDistance < threatDistance) { threatDistance = bodyDistance; threatX = body.x; threatZ = body.z; walkerThreat = true; }
        }
        const bool hunting = agent.state.behavior == GeckoBehavior::Stalk
            || agent.state.behavior == GeckoBehavior::Lunge;
        agent.huntCooldown = std::max(0.0f, agent.huntCooldown - deltaSeconds);
        if (threatDistance < 2.7f
            && (agent.state.behavior == GeckoBehavior::Rest || agent.state.behavior == GeckoBehavior::Stalk)) {
            if (walkerThreat) ++walkerScatters_;
            agent.state.behavior = GeckoBehavior::Scurry;
            agent.behaviorSeconds = .75f;
            agent.prey = -1;
            agent.state.headingRadians = std::atan2(
                agent.state.z - threatZ, agent.state.x - threatX);
        } else if (hunting) {
            // Stalking: the slow approach, the heading held on the fly; from the last span
            // the lunge, then a rest. A fly gone from reach, or a stalk run out, ends it.
            agent.behaviorSeconds -= deltaSeconds;
            const bool preyValid = agent.prey >= 0 && static_cast<std::size_t>(agent.prey) < flies_.size();
            const float preyDistance = preyValid
                ? std::hypot(flies_[static_cast<std::size_t>(agent.prey)].x - agent.state.x,
                             flies_[static_cast<std::size_t>(agent.prey)].z - agent.state.z)
                : std::numeric_limits<float>::infinity();
            if (preyValid) {
                const FlyState& fly = flies_[static_cast<std::size_t>(agent.prey)];
                agent.state.headingRadians = MoveAngle(agent.state.headingRadians,
                    std::atan2(fly.z - agent.state.z, fly.x - agent.state.x), deltaSeconds * 4.0f);
            }
            if (agent.state.behavior == GeckoBehavior::Stalk) {
                if (!preyValid || preyDistance > HuntRange * 1.5f || agent.behaviorSeconds <= 0.0f) {
                    agent.state.behavior = GeckoBehavior::Rest;
                    agent.behaviorSeconds = 4.0f;
                    agent.huntCooldown = HuntCooldownSeconds * .4f;
                    agent.prey = -1;
                } else if (preyDistance < LungeReach) {
                    agent.state.behavior = GeckoBehavior::Lunge;
                    agent.behaviorSeconds = LungeSeconds;
                    lastLunge_ = {flies_[static_cast<std::size_t>(agent.prey)].x,
                                  flies_[static_cast<std::size_t>(agent.prey)].z, true};
                    ++lunges_;
                }
            } else if (agent.behaviorSeconds <= 0.0f) {
                agent.state.behavior = GeckoBehavior::Rest;
                agent.behaviorSeconds = 4.0f;
                agent.huntCooldown = HuntCooldownSeconds;
                agent.prey = -1;
            }
        } else {
            // A fly hanging low within reach of a resting gecko, on ground it accepts, is prey.
            if (agent.state.behavior == GeckoBehavior::Rest && agent.huntCooldown <= 0.0f) {
                float best = HuntRange;
                for (std::size_t i = 0; i < flies_.size(); ++i) {
                    const FlyState& fly = flies_[i];
                    if (!std::isfinite(fly.x) || !std::isfinite(fly.z) || !std::isfinite(fly.y)) continue;
                    const float flyDistance = std::hypot(fly.x - agent.state.x, fly.z - agent.state.z);
                    if (flyDistance >= best || fly.y > terrain_.Height(fly.x, fly.z) + .6f
                        || !ValidGround(fly.x, fly.z, agent.home)) continue;
                    best = flyDistance;
                    agent.prey = static_cast<int>(i);
                }
                if (agent.prey >= 0) {
                    agent.state.behavior = GeckoBehavior::Stalk;
                    agent.behaviorSeconds = StalkSeconds;
                }
            }
            agent.behaviorSeconds -= deltaSeconds;
            if (agent.state.behavior != GeckoBehavior::Stalk && agent.behaviorSeconds <= 0.0f) {
                ++agent.transitionCount;
                if (agent.state.behavior == GeckoBehavior::Scurry) {
                    agent.state.behavior = GeckoBehavior::Hidden;
                    agent.behaviorSeconds = 2.5f
                        + static_cast<float>(agent.transitionCount % 5U) * .75f;
                } else if (agent.state.behavior == GeckoBehavior::Hidden) {
                    agent.state.behavior = GeckoBehavior::Rest;
                    agent.behaviorSeconds = 5.0f
                        + static_cast<float>(agent.transitionCount % 6U) * 1.1f;
                } else {
                    agent.state.behavior = GeckoBehavior::Scurry;
                    agent.behaviorSeconds = 1.1f
                        + static_cast<float>(agent.transitionCount % 4U) * .22f;
                    agent.state.headingRadians = WrapAngle(agent.state.headingRadians
                        + 1.17f + static_cast<float>(agent.transitionCount % 3U) * .71f);
                }
            }
        }

        // The stalking tail wag: creeping on a fly, the tail tip sways wide and fast; the
        // lunge, or the hunt's end, stills it again.
        const bool stalking = agent.state.behavior == GeckoBehavior::Stalk;
        agent.state.tailWag += std::clamp((stalking ? 1.0f : 0.0f) - agent.state.tailWag,
            -deltaSeconds * 4.0f, deltaSeconds * 3.0f);
        if (agent.state.tailWag > 0.01f)
            agent.state.tailWagPhase = std::fmod(agent.state.tailWagPhase + deltaSeconds * TailWagRate, 6.2831853f);
        // The eye lick: resting, every eleven seconds or so (a little per gecko) the tongue
        // comes out and over one eye for half a second, the eyes taking turns.
        const bool resting = agent.state.behavior == GeckoBehavior::Rest;
        const float lickPeriod = LickPeriodSeconds + static_cast<float>(agent.transitionCount % 3U);
        agent.lickClock = resting ? agent.lickClock + deltaSeconds : 0.0f;
        if (agent.lickClock >= lickPeriod) agent.lickClock = 0.0f;
        const bool licking = resting && agent.lickClock >= 1.0f && agent.lickClock < 1.0f + LickSeconds;
        if (licking && !agent.licking) { ++licks_; agent.state.lickSide = -agent.state.lickSide; }
        agent.licking = licking;
        agent.state.eyeLick += std::clamp((licking ? 1.0f : 0.0f) - agent.state.eyeLick,
            -deltaSeconds * 6.0f, deltaSeconds * 8.0f);
        const float visibilityTarget = agent.state.behavior == GeckoBehavior::Hidden ? 0.0f : 1.0f;
        agent.state.visibility += std::clamp(visibilityTarget - agent.state.visibility,
            -deltaSeconds * 3.8f, deltaSeconds * 2.6f);
        agent.state.visibility = std::clamp(agent.state.visibility, 0.0f, 1.0f);
        const float targetSpeed = agent.state.behavior == GeckoBehavior::Scurry ? 1.35f
            : agent.state.behavior == GeckoBehavior::Stalk ? StalkSpeed
            : agent.state.behavior == GeckoBehavior::Lunge ? LungeSpeed : 0.0f;
        agent.state.speed += std::clamp(targetSpeed - agent.state.speed,
            -deltaSeconds * 4.0f, deltaSeconds * 6.0f);

        if (agent.state.behavior == GeckoBehavior::Scurry
            || agent.state.behavior == GeckoBehavior::Stalk
            || agent.state.behavior == GeckoBehavior::Lunge) {
            const float homeDistance = std::hypot(
                agent.state.x - agent.home.x, agent.state.z - agent.home.z);
            if (homeDistance > agent.home.radius * .72f) {
                const float homeHeading = std::atan2(
                    agent.home.z - agent.state.z, agent.home.x - agent.state.x);
                agent.state.headingRadians = MoveAngle(
                    agent.state.headingRadians, homeHeading, deltaSeconds * 3.2f);
            }
            const float candidateX = agent.state.x
                + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            const float candidateZ = agent.state.z
                + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            if (ValidGround(candidateX, candidateZ, agent.home)) {
                agent.state.x = candidateX;
                agent.state.z = candidateZ;
            } else {
                agent.state.headingRadians = std::atan2(
                    agent.home.z - agent.state.z, agent.home.x - agent.state.x);
                agent.state.speed = std::min(agent.state.speed, .45f);
            }
        }
        agent.state.gaitPhase = std::fmod(agent.state.gaitPhase
            + deltaSeconds * (2.2f + agent.state.speed * 8.0f), TwoPi);
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .038f;
    }
}
