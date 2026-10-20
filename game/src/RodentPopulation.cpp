#include "IslandSimulator/RodentPopulation.hpp"

#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/VegetationLayout.hpp"

#include <algorithm>
#include <array>
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
        constexpr std::array<std::array<float, 2>, RodentPopulation::Count> Centres = {{
            {{VegetationLayout::EastClearing.x - 14.0f, VegetationLayout::EastClearing.z + 7.0f}},
            {{VegetationLayout::EastClearing.x + 13.0f, VegetationLayout::EastClearing.z + 5.0f}},
            {{VegetationLayout::WestClearing.x - 12.0f, VegetationLayout::WestClearing.z + 5.0f}},
            {{VegetationLayout::WestClearing.x + 13.0f, VegetationLayout::WestClearing.z - 5.0f}},
            {{-35.0f, -72.0f}}, {{42.0f, -64.0f}}, {{-18.0f, 52.0f}}, {{29.0f, 61.0f}},
            {{-68.0f, 22.0f}}, {{72.0f, 12.0f}}, {{-53.0f, -31.0f}}, {{57.0f, -26.0f}},
            {{IslandEnvironmentField::SeasonalWetlandX - 18.0f,
              IslandEnvironmentField::SeasonalWetlandZ + 8.0f}},
            {{IslandEnvironmentField::SeasonalWetlandX + 17.0f,
              IslandEnvironmentField::SeasonalWetlandZ + 5.0f}},
            {{VegetationLayout::WestClearing.x - 17.0f, VegetationLayout::WestClearing.z - 2.0f}},
            {{VegetationLayout::WestClearing.x + 16.0f, VegetationLayout::WestClearing.z + 4.0f}},
            {{-83.0f, -12.0f}}, {{-76.0f, 34.0f}}, {{63.0f, 38.0f}}, {{78.0f, -18.0f}},
        }};

        float Wrap(const float angle) noexcept { return std::remainder(angle, TwoPi); }
        float Turn(const float current, const float target, const float limit) noexcept
        {
            return Wrap(current + std::clamp(Wrap(target - current), -limit, limit));
        }
    }

    RodentPopulation::RodentPopulation(const IslandTerrain& terrain) : terrain_(terrain)
    {
        for (std::size_t index = 0; index < Count; ++index) {
            Initialize(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
    }

    void RodentPopulation::Initialize(const std::size_t index)
    {
        Agent& agent = agents_[index];
        agent.state.species = index < MouseCount ? RodentSpecies::Mouse : RodentSpecies::Rat;
        agent.home.radius = agent.state.species == RodentSpecies::Mouse
            ? 4.2f + .35f * static_cast<float>(index % 3U)
            : 6.0f + .45f * static_cast<float>(index % 3U);
        bool found = false;
        for (int attempt = 0; attempt < 160 && !found; ++attempt) {
            const float angle = static_cast<float>(attempt) * 2.39996323f
                + static_cast<float>(index) * .73f;
            const float radius = .35f + static_cast<float>(attempt / 20) * .72f;
            agent.home.x = Centres[index][0] + std::cos(angle) * radius;
            agent.home.z = Centres[index][1] + std::sin(angle) * radius;
            const TerrainSample sample = terrain_.Sample(agent.home.x, agent.home.z);
            const bool dry = sample.surface == TerrainSurface::InteriorGround
                || sample.surface == TerrainSurface::CoastalGround
                || sample.surface == TerrainSurface::RockyGround;
            found = dry && sample.signedShoreDistance > 30.0f
                && sample.slopeRadians < .55f;
        }
        if (!found) throw std::runtime_error("No safe inland rodent habitat");
        agent.state.x = agent.home.x;agent.state.z = agent.home.z;
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .018f;
        agent.state.headingRadians = Wrap(.2f + static_cast<float>(index) * 1.17f);
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 4U);
        agent.state.behavior = index % 5U == 0U ? RodentBehavior::Forage : RodentBehavior::Rest;
        agent.behaviorSeconds = 2.5f + .65f * static_cast<float>(index % 7U);
        agent.state.gaitPhase = static_cast<float>(index) * .81f;
        agent.state.sniffPhase = static_cast<float>(index) * .57f;
    }

    bool RodentPopulation::IsValidPosition(const std::size_t index, const float x,
                                           const float z) const noexcept
    {
        if (index >= Count || !std::isfinite(x) || !std::isfinite(z)) return false;
        const TerrainSample sample = terrain_.Sample(x, z);
        const bool dry = sample.surface == TerrainSurface::InteriorGround
            || sample.surface == TerrainSurface::CoastalGround
            || sample.surface == TerrainSurface::RockyGround;
        return dry && sample.signedShoreDistance > 28.0f && sample.slopeRadians < .60f
            && std::hypot(x - homes_[index].x, z - homes_[index].z) <= homes_[index].radius;
    }

    void RodentPopulation::Update(const float deltaSeconds, const float observerX,
                                  const float observerY, const float observerZ) noexcept
    {
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, .1f) : 0.0f;
        const bool observerValid = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        ++frameIndex_;
        for (std::size_t index = 0; index < Count; ++index) {
            Agent& agent = agents_[index];
            const float distance = observerValid
                ? std::hypot(agent.state.y - observerY,
                    std::hypot(agent.state.x - observerX, agent.state.z - observerZ))
                : std::numeric_limits<float>::infinity();
            agent.state.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
            agent.accumulatedSeconds += safeDelta;
            const auto divisor = WildlifeActivationPolicy::UpdateDivisor(
                agent.state.simulationTier);
            if ((frameIndex_ + index) % divisor == 0U) {
                UpdateAgent(agent, index, std::min(agent.accumulatedSeconds, .5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
            }
            states_[index] = agent.state;
        }
    }

    // The nearest fruit lying still within FruitRange of the rodent, inside its range and on
    // ground it accepts; coconuts are beyond a rodent.
    int RodentPopulation::FindFruit(const Agent& agent, const std::size_t index) const noexcept
    {
        int best = -1;
        float bestDistance = FruitRange;
        for (std::size_t i = 0; i < lyingFruit_.size(); ++i) {
            const FallingFruit& fruit = lyingFruit_[i];
            if (!fruit.active || fruit.restSeconds < 0.0f || fruit.rolling || fruit.afloat
                || fruit.variant == FallingFruitField::CoconutVariant) continue;
            const float distance = std::hypot(fruit.x - agent.state.x, fruit.z - agent.state.z);
            if (distance >= bestDistance) continue;
            if (std::hypot(fruit.x - agent.home.x, fruit.z - agent.home.z) > agent.home.radius * .95f) continue;
            if (!IsValidPosition(index, fruit.x, fruit.z)) continue;
            bestDistance = distance;
            best = static_cast<int>(i);
        }
        return best;
    }

    std::size_t RodentPopulation::WhiskingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.whiskerTwitch > 0.9f ? 1U : 0U;
        return count;
    }

    std::size_t RodentPopulation::GroomingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.grooming ? 1U : 0U;
        return count;
    }

    std::size_t RodentPopulation::FeedingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.fruitTarget >= 0 && agent.feedSeconds > 0.0f ? 1U : 0U;
        return count;
    }

    void RodentPopulation::UpdateAgent(Agent& agent, const std::size_t index,
                                       const float deltaSeconds, const float observerX,
                                       const float observerZ, const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;
        float distance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // A passing animal or the resident frightens a rodent as the walker does.
        float threatX = observerX, threatZ = observerZ;
        bool walkerThreat = false;
        for (const GrassWalker& body : walkers_) {
            if (body.strength <= 0.0f || !std::isfinite(body.x) || !std::isfinite(body.z)) continue;
            const float bodyDistance = std::hypot(agent.state.x - body.x, agent.state.z - body.z);
            if (bodyDistance < distance) { distance = bodyDistance; threatX = body.x; threatZ = body.z; walkerThreat = true; }
        }
        // A snake's head near is the worst of threats: the rodent bolts from it and goes to
        // ground for longer.
        bool snakeThreat = false;
        for (const SnakeState& snake : snakes_) {
            const SnakePoint& head = snake.body.front();
            if (!std::isfinite(head.x) || !std::isfinite(head.z)) continue;
            const float headDistance = std::hypot(agent.state.x - head.x, agent.state.z - head.z);
            if (headDistance < SnakeFleeDistance && headDistance < distance) {
                distance = headDistance; threatX = head.x; threatZ = head.z;
                walkerThreat = false; snakeThreat = true;
            }
        }
        const float fleeDistance = snakeThreat ? SnakeFleeDistance
            : (agent.state.species == RodentSpecies::Mouse ? 2.5f : 3.3f);
        if (distance < fleeDistance && agent.state.behavior != RodentBehavior::Hidden) {
            if (walkerThreat && agent.state.behavior != RodentBehavior::Scurry) ++walkerScatters_;
            if (snakeThreat && agent.state.behavior != RodentBehavior::Scurry) ++snakeScatters_;
            agent.state.behavior = RodentBehavior::Scurry;
            agent.behaviorSeconds = snakeThreat ? 1.1f : .75f;
            agent.snakeBolt = snakeThreat || (agent.snakeBolt && agent.state.behavior == RodentBehavior::Scurry);
            agent.fruitTarget = -1;
            agent.feedSeconds = 0.0f;
            agent.state.headingRadians = std::atan2(
                agent.state.z - threatZ, agent.state.x - threatX);
        } else if ((agent.behaviorSeconds -= deltaSeconds) <= 0.0f) {
            ++agent.transitions;
            if (agent.state.behavior == RodentBehavior::Scurry) {
                agent.state.behavior = RodentBehavior::Hidden;
                agent.behaviorSeconds = (agent.snakeBolt ? SnakeHideSeconds : 1.8f)
                    + .45f * static_cast<float>(agent.transitions % 5U);
                agent.snakeBolt = false;
            } else if (agent.state.behavior == RodentBehavior::Hidden) {
                agent.state.behavior = RodentBehavior::Rest;
                agent.behaviorSeconds = 2.8f + .55f * static_cast<float>(agent.transitions % 6U);
            } else {
                agent.state.behavior = agent.state.behavior == RodentBehavior::Rest
                    ? RodentBehavior::Forage : RodentBehavior::Rest;
                agent.behaviorSeconds = 2.2f + .5f * static_cast<float>(agent.transitions % 7U);
                agent.state.headingRadians = Wrap(agent.state.headingRadians
                    + .65f + .31f * static_cast<float>(agent.transitions % 5U));
                agent.fruitTarget = -1;
                agent.feedSeconds = 0.0f;
                // Fruit lying near: the forage is a walk to it and a feed there.
                if (agent.state.behavior == RodentBehavior::Forage) {
                    agent.fruitTarget = FindFruit(agent, index);
                    if (agent.fruitTarget >= 0) { agent.behaviorSeconds = 30.0f; ++fruitVisits_; }
                }
            }
        }
        // On the way to a fruit, and at it: the fruit gone, the forage is an ordinary one.
        bool feeding = false;
        if (agent.fruitTarget >= 0) {
            const std::size_t fruitIndex = static_cast<std::size_t>(agent.fruitTarget);
            if (fruitIndex >= lyingFruit_.size() || !lyingFruit_[fruitIndex].active
                || agent.state.behavior != RodentBehavior::Forage) {
                agent.fruitTarget = -1;
                agent.feedSeconds = 0.0f;
                agent.behaviorSeconds = std::min(agent.behaviorSeconds, 1.0f);
            } else {
                const FallingFruit& fruit = lyingFruit_[fruitIndex];
                const float toFruit = std::hypot(fruit.x - agent.state.x, fruit.z - agent.state.z);
                feeding = toFruit < FruitReach;
                if (feeding) {
                    agent.feedSeconds += deltaSeconds;
                    if (agent.feedSeconds >= FeedSeconds) {
                        agent.fruitTarget = -1;
                        agent.feedSeconds = 0.0f;
                        agent.state.behavior = RodentBehavior::Rest;
                        agent.behaviorSeconds = 2.0f;
                        feeding = false;
                    }
                }
            }
        }
        const float visibilityTarget = agent.state.behavior == RodentBehavior::Hidden ? 0.0f : 1.0f;
        agent.state.visibility += std::clamp(visibilityTarget - agent.state.visibility,
            -4.5f * deltaSeconds, 3.0f * deltaSeconds);
        const float targetSpeed = agent.state.behavior == RodentBehavior::Scurry
            ? (agent.state.species == RodentSpecies::Mouse ? 1.65f : 1.45f)
            : agent.state.behavior == RodentBehavior::Forage && !feeding ? .28f : 0.0f;
        agent.state.speed += std::clamp(targetSpeed - agent.state.speed,
            -4.0f * deltaSeconds, 6.0f * deltaSeconds);
        const float homeDistance = std::hypot(agent.state.x - agent.home.x,
                                               agent.state.z - agent.home.z);
        if (homeDistance > agent.home.radius * .72f) {
            agent.state.headingRadians = Turn(agent.state.headingRadians,
                std::atan2(agent.home.z - agent.state.z, agent.home.x - agent.state.x),
                3.2f * deltaSeconds);
        } else if (agent.fruitTarget >= 0 && !feeding) {
            const FallingFruit& fruit = lyingFruit_[static_cast<std::size_t>(agent.fruitTarget)];
            agent.state.headingRadians = Turn(agent.state.headingRadians,
                std::atan2(fruit.z - agent.state.z, fruit.x - agent.state.x), 4.0f * deltaSeconds);
        } else if (agent.state.behavior == RodentBehavior::Forage && !feeding) {
            agent.state.headingRadians = Wrap(agent.state.headingRadians
                + std::sin(agent.state.sniffPhase * .7f) * .45f * deltaSeconds);
        }
        if (agent.state.speed > .001f) {
            const float nextX = agent.state.x
                + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            const float nextZ = agent.state.z
                + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
            if (IsValidPosition(index, nextX, nextZ)) {
                agent.state.x = nextX;agent.state.z = nextZ;
            } else {
                agent.state.headingRadians = std::atan2(
                    agent.home.z - agent.state.z, agent.home.x - agent.state.x);
                agent.state.speed = std::min(agent.state.speed, .24f);
            }
        }
        agent.state.gaitPhase = std::fmod(agent.state.gaitPhase
            + deltaSeconds * (2.0f + 11.0f * agent.state.speed), TwoPi);
        // Feeding, the sniff becomes a nibble: the same motion, three times as fast.
        agent.state.sniffPhase = std::fmod(agent.state.sniffPhase + deltaSeconds * (feeding ? 8.1f : 2.7f), TwoPi);
        // Grooming: a rest a little in, the rodent in the open, has it sit up and wash its
        // face with both forepaws for a couple of seconds; a scurry or the next forage ends it.
        const bool resting = agent.state.behavior == RodentBehavior::Rest && agent.state.visibility > .5f;
        agent.restSeconds = resting ? agent.restSeconds + deltaSeconds : 0.0f;
        const bool grooming = resting && agent.restSeconds >= GroomDelaySeconds
            && agent.restSeconds < GroomDelaySeconds + GroomSeconds;
        if (grooming && !agent.grooming) ++grooms_;
        agent.grooming = grooming;
        agent.state.groom += std::clamp((grooming ? 1.0f : 0.0f) - agent.state.groom,
            -4.0f * deltaSeconds, 3.0f * deltaSeconds);
        if (agent.state.groom > .01f)
            agent.state.groomPhase = std::fmod(agent.state.groomPhase + deltaSeconds * GroomStrokeRate, TwoPi);
        // The whiskers: swept with the sniff, fully on the forage and at the fruit, a little
        // at rest, still down the burrow.
        const float whiskerTarget = agent.state.visibility < .5f ? 0.0f
            : (agent.state.behavior == RodentBehavior::Forage || feeding) ? 1.0f : 0.3f;
        agent.state.whiskerTwitch += std::clamp(whiskerTarget - agent.state.whiskerTwitch,
            -3.0f * deltaSeconds, 3.0f * deltaSeconds);
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + .018f;
    }
}
