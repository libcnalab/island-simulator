#include "IslandSimulator/CrabColony.hpp"
#include "IslandSimulator/WrackLine.hpp"

#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        constexpr std::array<float, 6> ColonyAngles = {
            -2.72f, -1.78f, -0.18f, 0.68f, 1.58f, 2.52f};

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

    CrabColony::CrabColony(const IslandTerrain& terrain)
        : terrain_(terrain)
    {
        agents_.resize(CrabCount);
        states_.resize(CrabCount);
        homes_.resize(CrabCount);
        for (std::size_t index = 0; index < CrabCount; ++index) {
            InitializeAgent(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
        RebuildBurrows();
    }

    bool CrabColony::IsNearDebris(const CrabHomeRange& home,
        const std::span<const CrabHabitatCue> cues) noexcept
    {
        return std::ranges::any_of(cues, [&](const CrabHabitatCue cue) {
            return std::isfinite(cue.x) && std::isfinite(cue.z)
                && std::hypot(home.x - cue.x, home.z - cue.z) <= 3.25f;
        });
    }

    void CrabColony::SetDebrisHabitats(const std::span<const CrabHabitatCue> cues)
    {
        if (cues.empty()) throw std::invalid_argument("Crab debris habitat requires cues");
        CrabColony replacement(terrain_);
        std::array<bool, 6> selected{};
        for (std::size_t colony = 0; colony < ColonyAngles.size(); ++colony) {
            float best = std::numeric_limits<float>::infinity();
            for (std::size_t index = 0; index < cues.size(); ++index) {
                const auto cue = cues[index];
                if (!std::isfinite(cue.x) || !std::isfinite(cue.z))
                    throw std::invalid_argument("Crab debris cue must be finite");
                const auto sample = terrain_.Sample(cue.x, cue.z);
                const CrabHomeRange probe{cue.x, cue.z, 1.0f};
                if (sample.coastalRockiness >= .35f
                    || !replacement.IsValidHabitat(cue.x, cue.z, probe)) continue;
                bool separated = true;
                for (std::size_t previous = 0; previous < colony; ++previous)
                    separated = separated && std::hypot(cue.x - replacement.debrisAnchors_[previous].x,
                        cue.z - replacement.debrisAnchors_[previous].z) >= 28.0f;
                if (!separated) continue;
                const float difference = std::abs(WrapAngle(
                    std::atan2(cue.z, cue.x) - ColonyAngles[colony]));
                if (difference < best) {
                    best = difference;
                    replacement.debrisAnchors_[colony] = cue;
                    selected[colony] = true;
                }
            }
            if (!selected[colony]) throw std::runtime_error(
                "No separated sandy debris anchor for crab colony");
        }
        replacement.debrisAnchored_ = true;
        for (std::size_t index = 0; index < CrabCount; ++index) {
            replacement.InitializeAgent(index);
            const auto anchor = replacement.debrisAnchors_[index / 5U];
            if (std::hypot(replacement.agents_[index].home.x - anchor.x,
                           replacement.agents_[index].home.z - anchor.z) > 3.25f)
                throw std::runtime_error("Crab could not remain beside selected beach debris");
            replacement.states_[index] = replacement.agents_[index].state;
            replacement.homes_[index] = replacement.agents_[index].home;
        }
        replacement.RebuildBurrows();
        agents_.swap(replacement.agents_);
        states_.swap(replacement.states_);
        homes_.swap(replacement.homes_);
        debrisAnchors_ = replacement.debrisAnchors_;
        burrows_ = replacement.burrows_;
        debrisAnchored_ = true;
        frameIndex_ = 0;
    }

    void CrabColony::RebuildBurrows() noexcept
    {
        for (std::size_t colony = 0; colony < burrows_.size(); ++colony) {
            float x = 0.0f;
            float z = 0.0f;
            for (std::size_t member = 0; member < 5U; ++member) {
                const CrabHomeRange& home = homes_[colony * 5U + member];
                x += home.x;
                z += home.z;
            }
            x /= 5.0f;
            z /= 5.0f;
            burrows_[colony] = CrabBurrow{
                x, terrain_.Height(x, z) + 0.012f, z,
                0.24f + static_cast<float>(colony % 3U) * 0.035f};
        }
    }

    void CrabColony::InitializeAgent(const std::size_t index)
    {
        Agent& agent = agents_[index];
        const float sequence = static_cast<float>(index);
        const std::size_t colonyIndex = index / 5U;
        const float memberOffset = (static_cast<float>(index % 5U) - 2.0f) * 0.012f;
        const float authoredAngle = ColonyAngles[colonyIndex] + memberOffset;
        float angle = authoredAngle;
        const float landwardOffset = 3.4f + static_cast<float>(index % 6U) * 1.55f;
        const float tangentOffset = (static_cast<float>((index * 7U) % 9U) - 4.0f) * 0.42f;
        agent.home.radius = 5.8f + static_cast<float>(index % 4U) * 0.7f;
        const auto separatedFromEarlierColonyMembers=[&](const float x,const float z) {
            const std::size_t first=colonyIndex*5U;
            for(std::size_t previous=first;previous<index;++previous)
                if(std::hypot(x-agents_[previous].home.x,z-agents_[previous].home.z)<1.12f)
                    return false;
            return true;
        };
        bool foundHabitat = false;
        if (debrisAnchored_) {
            const auto anchor = debrisAnchors_[colonyIndex];
            angle = std::atan2(anchor.z, anchor.x);
            for (int attempt = 0; attempt < 64 && !foundHabitat; ++attempt) {
                const float radius = .58f + static_cast<float>(attempt) * .038f;
                const float theta = static_cast<float>(index) * 1.173f
                    + static_cast<float>(attempt) * 2.399963f;
                agent.home.x = anchor.x + std::cos(theta) * radius;
                agent.home.z = anchor.z + std::sin(theta) * radius;
                foundHabitat = IsValidHabitat(agent.home.x, agent.home.z, agent.home)
                    && separatedFromEarlierColonyMembers(agent.home.x,agent.home.z);
            }
        }
        for (int attempt = 0; attempt < 97 && !foundHabitat; ++attempt) {
            const float offset = static_cast<float>((attempt + 1) / 2) * 0.025f
                * (attempt % 2 == 0 ? -1.0f : 1.0f);
            angle = authoredAngle + offset;
            const float radius = terrain_.ShoreRadius(angle) - landwardOffset;
            agent.home.x = std::cos(angle) * radius - std::sin(angle) * tangentOffset;
            agent.home.z = std::sin(angle) * radius + std::cos(angle) * tangentOffset;
            const auto sample = terrain_.Sample(agent.home.x, agent.home.z);
            if (sample.coastalRockiness < 0.35f
                && IsValidHabitat(agent.home.x, agent.home.z, agent.home)
                && separatedFromEarlierColonyMembers(agent.home.x,agent.home.z)) {
                foundHabitat = true;
                break;
            }
        }
        if (!foundHabitat) throw std::runtime_error("No sandy shoreline habitat for beach crab");
        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + 0.075f;
        agent.state.bodyYawRadians = WrapAngle(angle + std::numbers::pi_v<float>);
        agent.preferredDirection = index % 2U == 0U ? 1 : -1;
        agent.state.travelHeadingRadians = WrapAngle(
            angle + static_cast<float>(agent.preferredDirection)
                * std::numbers::pi_v<float> * 0.5f);
        agent.state.scale = 0.60f + static_cast<float>(index % 7U) * 0.038f;
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 4U);
        agent.state.behavior = index % 4U == 0U ? CrabBehavior::Pause : CrabBehavior::Forage;
        agent.behaviorSeconds = 1.2f + static_cast<float>(index % 8U) * 0.47f;
        agent.movementPhase = std::fmod(sequence * 1.173f + 0.4f, TwoPi);
        agent.animationPhase = std::fmod(sequence * 2.071f, TwoPi);
        agent.state.clawPose = 0.2f + static_cast<float>(index % 5U) * 0.08f;
    }

    bool CrabColony::IsValidHabitat(const float x, const float z,
                                     const CrabHomeRange& home) const noexcept
    {
        const TerrainSample sample = terrain_.Sample(x, z);
        const float homeDistance = std::hypot(x - home.x, z - home.z);
        const bool beachSurface = sample.surface == TerrainSurface::WetSand
            || sample.surface == TerrainSurface::DrySand
            || sample.surface == TerrainSurface::CoastalGround;
        return beachSurface && sample.height >= -0.02f
            && sample.signedShoreDistance >= MinimumShoreDistance
            && sample.signedShoreDistance <= MaximumShoreDistance
            && sample.slopeRadians < 0.38f && homeDistance <= home.radius;
    }

    void CrabColony::SetWalker(const float x, const float z, const bool valid) noexcept
    {
        walkerValid_ = valid && std::isfinite(x) && std::isfinite(z);
        walkerX_ = walkerValid_ ? x : 0.0f;
        walkerZ_ = walkerValid_ ? z : 0.0f;
    }

    void CrabColony::Update(const float deltaSeconds, const float observerX,
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
                UpdateAgent(agent, index, std::min(agent.accumulatedSeconds, 0.5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
            }
            states_[index] = agent.state;
        }

#ifndef NDEBUG
        assert(fullSimulationCount_ + reducedSimulationCount_ + dormantSimulationCount_
               == static_cast<int>(CrabCount));
        for (const CrabState& crab : states_) {
            assert(std::isfinite(crab.x) && std::isfinite(crab.y) && std::isfinite(crab.z));
            assert(std::isfinite(crab.bodyYawRadians)
                   && std::isfinite(crab.travelHeadingRadians));
            assert(std::isfinite(crab.speed) && crab.speed >= 0.0f);
            assert(std::isfinite(crab.legPhase) && std::fabs(crab.legPhase) <= 1.001f);
            assert(std::isfinite(crab.clawPose)
                   && crab.clawPose >= 0.0f && crab.clawPose <= 1.0f);
        }
#endif
    }

    void CrabColony::UpdateAgent(Agent& agent, const std::size_t index,
                                 const float deltaSeconds,
                                 const float observerX, const float observerZ,
                                 const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;

        float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // The resident walking the beach frightens the crabs as the player does: the nearer
        // of the two is the one scattered from.
        float threatX = observerX, threatZ = observerZ;
        if (walkerValid_) {
            const float walkerDistance = std::hypot(agent.state.x - walkerX_, agent.state.z - walkerZ_);
            if (walkerDistance < observerDistance) {
                observerDistance = walkerDistance;
                threatX = walkerX_;
                threatZ = walkerZ_;
            }
        }
        const CrabBurrow& burrow = burrows_[std::min(index / 5U, burrows_.size() - 1U)];
        const bool nightfall = nightness_ > 0.6f;
        if (nightfall && agent.state.behavior != CrabBehavior::Hide) {
            // After dark: home to the colony's burrow at a run, and down it.
            const float toBurrowX = burrow.x - agent.state.x, toBurrowZ = burrow.z - agent.state.z;
            const float burrowDistance = std::hypot(toBurrowX, toBurrowZ);
            agent.state.behavior = CrabBehavior::Scuttle;
            agent.behaviorSeconds = 1.5f;
            agent.toBurrow = true;
            if (burrowDistance > 0.001f) agent.state.travelHeadingRadians = std::atan2(toBurrowZ, toBurrowX);
            agent.nightRun += deltaSeconds;
            // home, or (the way there blocked) dug in where it stands: these crabs dig their own
            if (burrowDistance < BurrowArriveDistance || agent.nightRun > 20.0f) {
                agent.ownHole = burrowDistance >= BurrowArriveDistance;
                agent.state.behavior = CrabBehavior::Hide;
                agent.clearSeconds = 0.0f;
                agent.nightRun = 0.0f;
                ++hides_;
            }
        } else if (observerDistance < ScuttleDistance && agent.state.behavior != CrabBehavior::Hide) {
            agent.state.behavior = CrabBehavior::Scuttle;
            agent.behaviorSeconds = 1.5f;
            float awayX = agent.state.x - threatX;
            float awayZ = agent.state.z - threatZ;
            const float awayLength = std::hypot(awayX, awayZ);
            if (awayLength > 0.001f) {
                awayX /= awayLength;
                awayZ /= awayLength;
                agent.state.travelHeadingRadians = std::atan2(awayZ, awayX);
            } else {
                agent.state.travelHeadingRadians += static_cast<float>(agent.preferredDirection)
                    * std::numbers::pi_v<float> * 0.5f;
            }
            // The colony's burrow, when it lies near and roughly along the line of flight, is
            // the refuge: the crab bolts for the hole and, at its mouth, goes down it.
            const float toBurrowX = burrow.x - agent.state.x, toBurrowZ = burrow.z - agent.state.z;
            const float burrowDistance = std::hypot(toBurrowX, toBurrowZ);
            agent.toBurrow = false;
            if (burrowDistance > 0.001f && burrowDistance < BurrowDashDistance) {
                const float along = (toBurrowX * std::cos(agent.state.travelHeadingRadians)
                    + toBurrowZ * std::sin(agent.state.travelHeadingRadians)) / burrowDistance;
                if (along > 0.3f || burrowDistance < BurrowArriveDistance) {
                    agent.toBurrow = true;
                    agent.state.travelHeadingRadians = std::atan2(toBurrowZ, toBurrowX);
                    if (burrowDistance < BurrowArriveDistance) {
                        agent.state.behavior = CrabBehavior::Hide;
                        agent.clearSeconds = 0.0f;
                        ++hides_;
                    }
                }
            }
        } else if (agent.state.behavior == CrabBehavior::Hide) {
            // Down the hole until the beach has been clear a while, then up and on.
            agent.clearSeconds = observerDistance < ScuttleDistance * 1.6f || nightfall
                ? 0.0f : agent.clearSeconds + deltaSeconds;
            if (agent.clearSeconds >= EmergeDelaySeconds && agent.state.hideAmount <= 0.0f) {
                agent.state.behavior = CrabBehavior::Pause;
                agent.behaviorSeconds = 1.0f;
                agent.toBurrow = false;
                agent.ownHole = false;
            }
        } else {
            agent.behaviorSeconds -= deltaSeconds;
            if (agent.behaviorSeconds <= 0.0f) {
                ++agent.transitionCount;
                if (agent.state.behavior == CrabBehavior::Pause) {
                    agent.state.behavior = CrabBehavior::Forage;
                    agent.behaviorSeconds = 3.0f
                        + static_cast<float>((agent.transitionCount + agent.state.colorVariant)
                                             % 5U) * 0.63f;
                    agent.preferredDirection *= -1;
                } else {
                    agent.state.behavior = CrabBehavior::Pause;
                    agent.behaviorSeconds = 1.2f
                        + static_cast<float>((agent.transitionCount * 3U
                                             + agent.state.colorVariant) % 6U) * 0.48f;
                    // Among the strandings there is food: the pause there is a feed.
                    agent.feedingPause = std::abs(terrain_.SignedShoreDistance(agent.state.x, agent.state.z)
                                                  - WrackLine::Centre(agent.state.x, agent.state.z)) < 0.9f;
                    if (agent.feedingPause) agent.behaviorSeconds *= 1.8f;
                }
            }
        }

        float targetSpeed = 0.0f;
        if (agent.state.behavior == CrabBehavior::Forage) {
            targetSpeed = 0.42f + agent.state.scale * 0.16f;
            const float homeAngle = std::atan2(agent.home.z, agent.home.x);
            const float tangentHeading = homeAngle
                + static_cast<float>(agent.preferredDirection)
                    * std::numbers::pi_v<float> * 0.5f;
            float desiredHeading = tangentHeading
                + std::sin(agent.movementPhase + agent.transitionCount * 0.7f) * 0.42f;
            // Every other bout works the wrack line: the heading bends up or down the beach
            // toward the strandings until the crab is on them, then runs along them.
            if ((agent.transitionCount / 2U) % 2U == 1U) {
                const float inland = terrain_.SignedShoreDistance(agent.state.x, agent.state.z);
                const float pull = std::clamp((WrackLine::Centre(agent.state.x, agent.state.z) - inland) * 0.6f,
                                              -0.9f, 0.9f);
                // Inland is toward the island centre: the opposite of the home's radial.
                const float tx = std::cos(tangentHeading), tz = std::sin(tangentHeading);
                const float nx = -std::cos(homeAngle), nz = -std::sin(homeAngle);
                desiredHeading = std::atan2(tz + nz * pull, tx + nx * pull);
            }
            agent.state.travelHeadingRadians = MoveAngle(
                agent.state.travelHeadingRadians, desiredHeading, deltaSeconds * 1.2f);
        } else if (agent.state.behavior == CrabBehavior::Scuttle) {
            targetSpeed = 1.85f + agent.state.scale * 0.35f;
        }
        if (agent.state.behavior == CrabBehavior::Hide) {
            // Into the hole: the body slides to the burrow's centre and sinks below the sand;
            // coming up is the reverse, the crab resting at the mouth until it goes on.
            const float target = agent.clearSeconds >= EmergeDelaySeconds ? 0.0f : 1.0f;
            agent.state.hideAmount += std::clamp(target - agent.state.hideAmount,
                                                 -deltaSeconds * 1.5f, deltaSeconds * 2.5f);
            if (target > 0.0f && !agent.ownHole) {
                const float pull = std::min(1.0f, deltaSeconds * 4.0f);
                agent.state.x += (burrow.x - agent.state.x) * pull;
                agent.state.z += (burrow.z - agent.state.z) * pull;
            }
            agent.state.speed = 0.0f;
        } else {
            agent.state.hideAmount = std::max(0.0f, agent.state.hideAmount - deltaSeconds * 1.5f);
        }
        agent.state.speed += std::clamp(targetSpeed - agent.state.speed,
                                        -deltaSeconds * 2.8f, deltaSeconds * 4.8f);

        const float homeDistance = std::hypot(
            agent.state.x - agent.home.x, agent.state.z - agent.home.z);
        if (homeDistance > agent.home.radius * 0.72f) {
            const float homeHeading = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
            agent.state.travelHeadingRadians = MoveAngle(
                agent.state.travelHeadingRadians, homeHeading, deltaSeconds * 2.4f);
        }

        const float candidateX = agent.state.x
            + std::cos(agent.state.travelHeadingRadians) * agent.state.speed * deltaSeconds;
        const float candidateZ = agent.state.z
            + std::sin(agent.state.travelHeadingRadians) * agent.state.speed * deltaSeconds;
        // Crabs keep their separation, except from one down the hole and when the move
        // draws them apart (crabs coming up out of one burrow start on top of each other).
        const bool separated = std::ranges::all_of(agents_, [&](const Agent& other) {
            const std::size_t otherIndex = static_cast<std::size_t>(
                &other - agents_.data());
            if (otherIndex == index || other.state.behavior == CrabBehavior::Hide) return true;
            const float candidateDistance = std::hypot(candidateX - other.state.x,
                                                       candidateZ - other.state.z);
            const float currentDistance = std::hypot(agent.state.x - other.state.x,
                                                     agent.state.z - other.state.z);
            return candidateDistance >= MinimumAgentSeparation
                || candidateDistance >= currentDistance;
        });
        if (IsValidHabitat(candidateX, candidateZ, agent.home) && separated) {
            agent.state.x = candidateX;
            agent.state.z = candidateZ;
        } else {
            const float homeHeading = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
            agent.state.travelHeadingRadians = MoveAngle(
                agent.state.travelHeadingRadians,
                homeHeading + static_cast<float>(agent.preferredDirection) * 0.62f,
                0.48f);
            agent.state.speed = std::min(agent.state.speed, 0.65f);
        }
        agent.state.bodyYawRadians = MoveAngle(
            agent.state.bodyYawRadians,
            agent.state.travelHeadingRadians
                - static_cast<float>(agent.preferredDirection)
                    * std::numbers::pi_v<float> * 0.5f,
            deltaSeconds * 3.5f);
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z) + 0.075f
            - 0.2f * agent.state.hideAmount;
        agent.movementPhase = std::fmod(agent.movementPhase + deltaSeconds * 0.73f, TwoPi);
        agent.animationPhase = std::fmod(
            agent.animationPhase + deltaSeconds * (4.0f + agent.state.speed * 8.0f), TwoPi);
        agent.state.legPhase = agent.state.speed > 0.08f
            ? std::sin(agent.animationPhase) : agent.state.legPhase * 0.82f;
        const float clawTarget = agent.state.behavior == CrabBehavior::Pause
            ? 0.60f + 0.16f * std::sin(agent.movementPhase * 1.7f)
            : 0.24f + 0.08f * std::sin(agent.movementPhase);
        agent.state.clawPose += std::clamp(
            clawTarget - agent.state.clawPose, -deltaSeconds * 1.8f, deltaSeconds * 1.8f);
        // The eyestalks fold flat on the scuttle for the burrow and down it, and rise again
        // as the crab comes up and stands.
        const bool foldEyes = agent.state.behavior == CrabBehavior::Scuttle
            || agent.state.behavior == CrabBehavior::Hide || agent.state.hideAmount > 0.05f;
        agent.state.eyeFold += std::clamp((foldEyes ? 1.0f : 0.0f) - agent.state.eyeFold,
                                          -deltaSeconds * 2.0f, deltaSeconds * 6.0f);
        // The feed: paused among the strandings, in the open, the crab works the food.
        const bool feedingNow = agent.state.behavior == CrabBehavior::Pause && agent.feedingPause
            && agent.state.hideAmount < 0.05f;
        agent.state.feeding += std::clamp((feedingNow ? 1.0f : 0.0f) - agent.state.feeding,
                                          -deltaSeconds * 3.0f, deltaSeconds * 3.0f);
        // A crab paused in the open waves a claw: a second into the pause, and every nine
        // seconds or so after (a little per crab), one claw is raised and swept up and down
        // for a couple of seconds; moving or going down the burrow drops the display.
        {
            const auto index = static_cast<std::size_t>(&agent - agents_.data());
            const bool displaying = agent.state.behavior == CrabBehavior::Pause && agent.state.hideAmount < 0.05f;
            agent.waveClock = displaying ? agent.waveClock + deltaSeconds : 0.0f;
            const float period = WavePeriodSeconds + static_cast<float>(index % 4U) * 1.5f;
            const float cycle = std::fmod(agent.waveClock, period);
            const bool waving = displaying && cycle >= WaveDelaySeconds && cycle < WaveDelaySeconds + WaveSeconds;
            if (waving && !agent.waving) {
                ++waves_;
                agent.state.waveSide = (waves_ + index) % 2U == 0U ? 1.0f : -1.0f;
                agent.state.wavePhase = 0.0f;
                // The pause holds through the display.
                agent.behaviorSeconds = std::max(agent.behaviorSeconds, WaveSeconds + 0.4f);
            }
            agent.waving = waving;
            if (waving) agent.state.wavePhase = std::fmod(agent.state.wavePhase + deltaSeconds * 11.0f, TwoPi);
            agent.state.waveAmount += std::clamp((waving ? 1.0f : 0.0f) - agent.state.waveAmount,
                                                 -deltaSeconds * 6.0f, deltaSeconds * 6.0f);
        }
    }

    std::size_t CrabColony::FeedingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.feeding > 0.5f ? 1U : 0U;
        return count;
    }

    std::size_t CrabColony::EyeFoldedCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.state.eyeFold > 0.5f ? 1U : 0U;
        return count;
    }

    std::size_t CrabColony::WavingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.waving ? 1U : 0U;
        return count;
    }

    std::span<const CrabState> CrabColony::Crabs() const noexcept
    {
        return states_;
    }

    std::size_t CrabColony::HiddenCount() const noexcept
    {
        std::size_t count = 0;
        for (const CrabState& crab : states_) count += crab.behavior == CrabBehavior::Hide ? 1U : 0U;
        return count;
    }

    std::span<const CrabHomeRange> CrabColony::Homes() const noexcept
    {
        return homes_;
    }
}
