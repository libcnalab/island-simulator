#include "IslandSimulator/CoconutCrabGroup.hpp"

#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/FallingFruitField.hpp"
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
        constexpr float Pi = std::numbers::pi_v<float>;
        constexpr float TwoPi = 2.0f * Pi;
        constexpr std::array<float, CoconutCrabGroup::Count> Angles = {{
            -2.55f, -1.62f, -.64f, .44f, 1.48f, 2.53f}};
        float Wrap(float angle) noexcept
        {
            while (angle > Pi) angle -= TwoPi;
            while (angle < -Pi) angle += TwoPi;
            return angle;
        }
        float MoveAngle(float current, float target, float step) noexcept
        {
            return Wrap(current + std::clamp(Wrap(target - current), -step, step));
        }
    }

    CoconutCrabGroup::CoconutCrabGroup(const IslandTerrain& terrain)
        : terrain_(terrain)
    {
        for (std::size_t index = 0; index < Count; ++index) {
            Initialize(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
    }

    void CoconutCrabGroup::Initialize(const std::size_t index)
    {
        Agent& agent = agents_[index];
        agent.home.radius = 7.5f + static_cast<float>(index % 3U) * .8f;
        bool found = false;
        float angle = Angles[index];
        for (int attempt = 0; attempt < 120 && !found; ++attempt) {
            const float side = attempt % 2 == 0 ? 1.0f : -1.0f;
            angle = Angles[index] + side * static_cast<float>((attempt + 1) / 2) * .018f;
            const float inset = 31.0f + static_cast<float>(index % 3U) * 5.0f;
            const float radius = terrain_.ShoreRadius(angle) - inset;
            agent.home.x = std::cos(angle) * radius;
            agent.home.z = std::sin(angle) * radius;
            found = IsValid(agent.home.x, agent.home.z, agent.home);
        }
        if (!found) throw std::runtime_error("No coastal palm-zone habitat for coconut crab");
        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .080f;
        agent.state.headingRadians = Wrap(angle + Pi * .5f);
        agent.state.scale = .92f + static_cast<float>(index % 3U) * .06f;
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 4U);
        agent.state.behavior = index % 2U == 0U
            ? CoconutCrabBehavior::Rest : CoconutCrabBehavior::Forage;
        agent.behaviorSeconds = 2.0f + static_cast<float>(index) * .61f;
        agent.steeringPhase = .5f + static_cast<float>(index) * 1.23f;
        agent.state.clawPose = .25f + static_cast<float>(index % 3U) * .10f;
    }

    bool CoconutCrabGroup::IsValid(const float x, const float z,
                                   const CoconutCrabHome& home) const noexcept
    {
        const TerrainSample sample = terrain_.Sample(x, z);
        const bool ground = sample.surface == TerrainSurface::CoastalGround
            || sample.surface == TerrainSurface::InteriorGround
            || sample.surface == TerrainSurface::RockyGround;
        return ground && sample.height > .15f
            && sample.signedShoreDistance >= MinimumShoreDistance
            && sample.signedShoreDistance <= MaximumShoreDistance
            && sample.slopeRadians < .42f
            && std::hypot(x - home.x, z - home.z) <= home.radius;
    }

    void CoconutCrabGroup::Update(const float deltaSeconds,
                                  const float observerX, const float observerY,
                                  const float observerZ) noexcept
    {
        eaten_.clear();
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, .1f) : 0.0f;
        const bool observerValid = std::isfinite(observerX)
            && std::isfinite(observerY) && std::isfinite(observerZ);
        ++frameIndex_;
        fullCount_ = reducedCount_ = dormantCount_ = 0;
        for (std::size_t index = 0; index < Count; ++index) {
            Agent& agent = agents_[index];
            const float distance = observerValid
                ? std::hypot(agent.state.y - observerY,
                    std::hypot(agent.state.x - observerX, agent.state.z - observerZ))
                : std::numeric_limits<float>::infinity();
            agent.state.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
            switch (agent.state.simulationTier) {
            case WildlifeSimulationTier::Full: ++fullCount_; break;
            case WildlifeSimulationTier::Reduced: ++reducedCount_; break;
            case WildlifeSimulationTier::Dormant: ++dormantCount_; break;
            }
            agent.accumulatedSeconds += safeDelta;
            const auto divisor = WildlifeActivationPolicy::UpdateDivisor(
                agent.state.simulationTier);
            if ((frameIndex_ + index) % divisor == 0U) {
                UpdateAgent(agent, std::min(agent.accumulatedSeconds, .5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
            }
            states_[index] = agent.state;
        }
        assert(fullCount_ + reducedCount_ + dormantCount_ == static_cast<int>(Count));
    }

    // The nearest coconut lying still (not afloat) within the crab's range on ground it can
    // reach; the crab walks to it and works it.
    bool CoconutCrabGroup::ChooseNut(Agent& agent) noexcept
    {
        agent.nutTarget = -1;
        float best = std::numeric_limits<float>::max();
        for (std::size_t index = 0; index < lyingFruit_.size(); ++index) {
            const FallingFruit& nut = lyingFruit_[index];
            if (!nut.active || nut.variant != FallingFruitField::CoconutVariant || nut.restSeconds < 0.0f
                || nut.rolling || nut.afloat) continue;
            if (std::hypot(nut.x - agent.home.x, nut.z - agent.home.z) > agent.home.radius * 0.9f) continue;
            const float distance = std::hypot(nut.x - agent.state.x, nut.z - agent.state.z);
            if (distance >= best || !IsValid(nut.x, nut.z, agent.home)) continue;
            best = distance;
            agent.nutTarget = static_cast<int>(index);
        }
        return agent.nutTarget >= 0;
    }

    // The nearest living palm whose trunk stands inside the crab's range.
    bool CoconutCrabGroup::ChooseTrunk(Agent& agent) noexcept
    {
        agent.trunkTarget = -1;
        float best = std::numeric_limits<float>::max();
        for (std::size_t i = 0; i < palms_.size(); ++i) {
            const PalmPlacement& palm = palms_[i];
            if (palm.condition != PalmCondition::Living || palm.scale < .8f
                || !std::isfinite(palm.x) || !std::isfinite(palm.z)) continue;
            if (std::hypot(palm.x - agent.home.x, palm.z - agent.home.z) > agent.home.radius * .85f) continue;
            const float distance = std::hypot(palm.x - agent.state.x, palm.z - agent.state.z);
            if (distance < best) { best = distance; agent.trunkTarget = static_cast<int>(i); }
        }
        return agent.trunkTarget >= 0;
    }

    std::size_t CoconutCrabGroup::ClimbingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.behavior == CoconutCrabBehavior::Climb ? 1U : 0U;
        return count;
    }

    void CoconutCrabGroup::UpdateAgent(Agent& agent, const float deltaSeconds,
                                       const float observerX, const float observerZ,
                                       const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;
        const float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // On the trunk: up at the climbing speed, a pause at the top, down again, and off
        // onto the ground; a threat does not call a crab down off a trunk.
        if (agent.state.behavior == CoconutCrabBehavior::Climb) {
            if (!agent.descending) {
                agent.climbY = std::min(ClimbHeight, agent.climbY + ClimbSpeed * deltaSeconds);
                if (agent.climbY >= ClimbHeight) {
                    agent.climbPause -= deltaSeconds;
                    if (agent.climbPause <= 0.0f) agent.descending = true;
                }
            } else {
                agent.climbY = std::max(0.0f, agent.climbY - DescendSpeed * deltaSeconds);
            }
            const float onTrunk = std::clamp(agent.climbY / .6f, 0.0f, 1.0f);
            agent.state.climbAmount = agent.descending && agent.climbY <= 0.0f ? 0.0f : onTrunk;
            agent.state.speed = 0.0f;
            agent.state.working = false;
            agent.steeringPhase = std::fmod(agent.steeringPhase + deltaSeconds * .52f, TwoPi);
            agent.state.legPhase = std::sin(agent.climbY * 9.0f) * (agent.climbY > 0.0f && agent.climbY < ClimbHeight ? 1.0f : 0.0f);
            agent.state.clawPose = .40f + .10f * std::sin(agent.climbY * 9.0f);
            agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .080f + agent.climbY;
            if (agent.descending && agent.climbY <= 0.0f) {
                agent.state.behavior = CoconutCrabBehavior::Rest;
                agent.behaviorSeconds = 3.0f;
                agent.descending = false;
                agent.trunkTarget = -1;
            }
            return;
        }
        if (observerDistance < 5.5f
            && agent.state.behavior != CoconutCrabBehavior::Retreat) {
            agent.state.behavior = CoconutCrabBehavior::Retreat;
            agent.behaviorSeconds = 1.8f;
            agent.trunkTarget = -1;
            agent.state.headingRadians = std::atan2(
                agent.state.z - observerZ, agent.state.x - observerX);
        } else if ((agent.behaviorSeconds -= deltaSeconds) <= 0.0f) {
            ++agent.transitions;
            if (agent.state.behavior == CoconutCrabBehavior::Rest) {
                agent.state.behavior = CoconutCrabBehavior::Forage;
                agent.behaviorSeconds = 4.0f + static_cast<float>(agent.transitions % 4U) * .75f;
                // A coconut lying in range is the forage: the walk there and the work on it.
                // Every fourth forage without one is a climb: the walk to a trunk and up it.
                if (ChooseNut(agent)) agent.behaviorSeconds += 60.0f;
                else if (agent.transitions % 4U == 1U && ChooseTrunk(agent)) agent.behaviorSeconds += 40.0f;
            } else {
                agent.nutTarget = -1;
                agent.trunkTarget = -1;
                agent.state.working = false;
                agent.state.behavior = CoconutCrabBehavior::Rest;
                agent.behaviorSeconds = 1.8f + static_cast<float>(agent.transitions % 3U) * .60f;
            }
        }
        // On the way to a trunk: steer to the stand at its foot; there, the climb begins.
        bool atTrunk = false;
        if (agent.state.behavior == CoconutCrabBehavior::Forage && agent.trunkTarget >= 0) {
            const PalmPlacement& palm = palms_[static_cast<std::size_t>(agent.trunkTarget)];
            const float toX = palm.x - agent.state.x, toZ = palm.z - agent.state.z;
            const float distance = std::hypot(toX, toZ);
            if (distance > TrunkStand + .12f) {
                agent.state.headingRadians = MoveAngle(agent.state.headingRadians,
                    std::atan2(toZ, toX), deltaSeconds * 2.3f);
            } else {
                atTrunk = true;
                agent.state.behavior = CoconutCrabBehavior::Climb;
                agent.state.trunkX = palm.x;
                agent.state.trunkZ = palm.z;
                // Face the trunk from the stand, the body set to turn up it.
                agent.state.headingRadians = std::atan2(toZ, toX);
                agent.climbY = 0.0f;
                agent.climbPause = ClimbPauseSeconds;
                agent.descending = false;
                agent.state.speed = 0.0f;
                ++climbs_;
            }
        }
        agent.steeringPhase = std::fmod(agent.steeringPhase + deltaSeconds * .52f, TwoPi);
        // On a coconut: steer to it, stop over it and work it with the claws until it is gone.
        bool atNut = false;
        if (agent.state.behavior == CoconutCrabBehavior::Forage && agent.nutTarget >= 0) {
            if (static_cast<std::size_t>(agent.nutTarget) >= lyingFruit_.size()
                || !lyingFruit_[static_cast<std::size_t>(agent.nutTarget)].active) {
                agent.nutTarget = -1; agent.state.working = false;
            } else {
                const FallingFruit& nut = lyingFruit_[static_cast<std::size_t>(agent.nutTarget)];
                const float distance = std::hypot(nut.x - agent.state.x, nut.z - agent.state.z);
                atNut = distance < 0.45f;
                if (!atNut) {
                    agent.state.headingRadians = MoveAngle(agent.state.headingRadians,
                        std::atan2(nut.z - agent.state.z, nut.x - agent.state.x), deltaSeconds * 2.3f);
                    agent.state.working = false;
                } else {
                    agent.state.working = true;
                    agent.workSeconds += deltaSeconds;
                    if (agent.workSeconds >= WorkSeconds) {
                        eaten_.push_back(static_cast<std::size_t>(agent.nutTarget));
                        ++works_;
                        agent.nutTarget = -1; agent.workSeconds = 0.0f; agent.state.working = false;
                        agent.state.behavior = CoconutCrabBehavior::Rest;
                        agent.behaviorSeconds = 3.0f;
                    }
                }
            }
        } else {
            agent.state.working = false;
            agent.workSeconds = 0.0f;
        }
        const float targetSpeed = agent.state.behavior == CoconutCrabBehavior::Rest || atNut || atTrunk
            ? 0.0f : (agent.state.behavior == CoconutCrabBehavior::Retreat ? .95f : .30f);
        agent.state.speed += std::clamp(targetSpeed - agent.state.speed,
                                        -deltaSeconds * 1.5f, deltaSeconds * 2.2f);
        if (agent.nutTarget >= 0 || agent.trunkTarget >= 0) {
            // The nut or the trunk is inside the range; the walk to it is not a wander.
        } else if (std::hypot(agent.state.x - agent.home.x, agent.state.z - agent.home.z)
                > agent.home.radius * .72f) {
            agent.state.headingRadians = MoveAngle(agent.state.headingRadians,
                std::atan2(agent.home.z - agent.state.z,
                           agent.home.x - agent.state.x), deltaSeconds * 2.3f);
        } else if (agent.state.behavior == CoconutCrabBehavior::Forage) {
            agent.state.headingRadians = Wrap(agent.state.headingRadians
                + std::sin(agent.steeringPhase) * deltaSeconds * .55f);
        }
        const float nextX = agent.state.x
            + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        const float nextZ = agent.state.z
            + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        if (IsValid(nextX, nextZ, agent.home)) {
            agent.state.x = nextX;
            agent.state.z = nextZ;
        } else {
            agent.state.headingRadians = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
        }
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .080f;
        agent.state.climbAmount = 0.0f;
        agent.state.legPhase = std::sin(agent.steeringPhase * 3.0f)
            * std::clamp(agent.state.speed / .30f, 0.0f, 1.0f);
        agent.state.clawPose = agent.state.working
            ? .72f + .22f * std::sin(agent.workSeconds * 5.5f)
            : .32f + .18f * std::sin(agent.steeringPhase * .7f);
    }
}
