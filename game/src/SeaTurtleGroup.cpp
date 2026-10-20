#include "IslandSimulator/SeaTurtleGroup.hpp"

#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"

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
        // Three of five home ranges occupy the sheltered western bay. The remaining two
        // preserve restrained island-wide encounters while all five still require the
        // shared reef/seagrass field below.
        constexpr std::array<float, SeaTurtleGroup::TurtleCount> HomeAngles = {
            -2.92f, -2.62f, 2.72f, -0.18f, 1.16f};

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

    SeaTurtleGroup::SeaTurtleGroup(const IslandTerrain& terrain)
        : terrain_(terrain)
    {
        agents_.resize(TurtleCount);
        states_.resize(TurtleCount);
        homes_.resize(TurtleCount);
        for (std::size_t index = 0; index < TurtleCount; ++index) {
            InitializeAgent(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
    }

    void SeaTurtleGroup::InitializeAgent(const std::size_t index)
    {
        Agent& agent = agents_[index];
        const float baseAngle = HomeAngles[index];
        agent.home.radius = 11.5f + static_cast<float>(index % 3U) * 1.8f;
        float angle = baseAngle;
        float bestScore = -1.0f;
        for (int offset = -4; offset <= 4; ++offset) {
            const float candidateAngle = baseAngle + static_cast<float>(offset) * .10f;
            for (const float offshore : {11.0f, 18.0f, 26.0f, 35.0f}) {
                const float radius = terrain_.ShoreRadius(candidateAngle) + offshore;
                const float x = std::cos(candidateAngle) * radius;
                const float z = std::sin(candidateAngle) * radius;
                const auto sample = terrain_.Sample(x, z);
                if (sample.signedShoreDistance < MinimumShoreDistance
                    || sample.signedShoreDistance > MaximumShoreDistance
                    || sample.height > -.72f) continue;
                const float score = IslandEnvironmentField::AquaticHabitatSuitability(
                    x, z, sample.signedShoreDistance, sample.height);
                if (score > bestScore) {
                    bestScore = score;
                    angle = candidateAngle;
                    agent.home.x = x;
                    agent.home.z = z;
                }
            }
        }

        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.headingRadians = WrapAngle(
            angle + (index % 2U == 0U ? 1.0f : -1.0f)
                * std::numbers::pi_v<float> * 0.5f);
        agent.state.scale = 0.86f + static_cast<float>(index % 4U) * 0.075f;
        agent.state.colorVariant = static_cast<std::uint8_t>(index % 3U);
        agent.state.behavior = index == 2U
            ? SeaTurtleBehavior::Surface
            : (index % 2U == 0U ? SeaTurtleBehavior::Forage
                                 : SeaTurtleBehavior::Cruise);
        agent.behaviorSeconds = 4.5f + static_cast<float>(index) * 2.1f;
        agent.wanderPhase = std::fmod(static_cast<float>(index) * 1.731f + 0.4f, TwoPi);
        agent.depthPhase = std::fmod(static_cast<float>(index) * 2.219f + 0.8f, TwoPi);
        agent.flipperPhase = std::fmod(static_cast<float>(index) * 1.377f, TwoPi);
        const float seabed = terrain_.Height(agent.state.x, agent.state.z);
        agent.state.y = DesiredDepth(agent, seabed);
        agent.state.speed = agent.state.behavior == SeaTurtleBehavior::Cruise ? 0.72f : 0.28f;
        agent.state.flipperStroke = std::sin(agent.flipperPhase);
    }

    bool SeaTurtleGroup::IsValidHabitat(
        const float x, const float z, const SeaTurtleHomeRange& home) const noexcept
    {
        const TerrainSample sample = terrain_.Sample(x, z);
        const float homeDistance = std::hypot(x - home.x, z - home.z);
        return sample.signedShoreDistance >= MinimumShoreDistance
            && sample.signedShoreDistance <= MaximumShoreDistance
            && sample.height <= -0.72f && homeDistance <= home.radius
            && IslandEnvironmentField::AquaticHabitatSuitability(
                x, z, sample.signedShoreDistance, sample.height) >= .025f;
    }

    float SeaTurtleGroup::DesiredDepth(const Agent& agent,
                                       const float seabedHeight) const noexcept
    {
        const float lower = seabedHeight + 0.48f * agent.state.scale;
        constexpr float upper = -0.11f;
        if (agent.state.behavior == SeaTurtleBehavior::Surface) {
            return std::max(lower, upper);
        }
        if (agent.state.behavior == SeaTurtleBehavior::Forage) {
            return std::min(upper, lower + 0.18f);
        }
        const float cruiseDepth = -1.12f
            + std::sin(elapsedSeconds_ * 0.22f + agent.depthPhase) * 0.28f;
        return lower <= upper ? std::clamp(cruiseDepth, lower, upper) : upper;
    }

    void SeaTurtleGroup::Update(const float deltaSeconds, const float observerX,
                                const float observerY, const float observerZ) noexcept
    {
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, 0.1f) : 0.0f;
        const bool observerValid = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        elapsedSeconds_ += safeDelta;
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
                const float depthBefore = agent.state.y;
                UpdateAgent(agent, std::min(agent.accumulatedSeconds, 0.5f),
                            observerX, observerZ, observerValid);
                agent.accumulatedSeconds = 0.0f;
                // The head lifts through the surface for a breath on the way up.
                if (depthBefore < BreathDepth && agent.state.y >= BreathDepth
                    && agent.state.behavior == SeaTurtleBehavior::Surface) {
                    breath_ = {agent.state.x, agent.state.z, agent.state.headingRadians, agent.state.scale, breath_.count + 1U};
                    agent.blinkSeconds = BlinkSeconds;   // the eyes squeeze shut as the head breaks the surface
                }
            }
            states_[index] = agent.state;
        }

#ifndef NDEBUG
        assert(fullSimulationCount_ + reducedSimulationCount_ + dormantSimulationCount_
               == static_cast<int>(TurtleCount));
        for (const SeaTurtleState& turtle : states_) {
            assert(std::isfinite(turtle.x) && std::isfinite(turtle.y)
                   && std::isfinite(turtle.z));
            assert(std::isfinite(turtle.headingRadians));
            assert(std::isfinite(turtle.pitchRadians)
                   && std::fabs(turtle.pitchRadians) <= 0.241f);
            assert(std::isfinite(turtle.bankRadians)
                   && std::fabs(turtle.bankRadians) <= 0.221f);
            assert(std::isfinite(turtle.speed) && turtle.speed >= 0.0f);
            assert(std::isfinite(turtle.flipperStroke)
                   && std::fabs(turtle.flipperStroke) <= 1.001f);
            assert(std::isfinite(turtle.scale) && turtle.scale > 0.0f);
        }
#endif
    }

    std::size_t SeaTurtleGroup::BlinkingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.blinking ? 1U : 0U;
        return count;
    }

    void SeaTurtleGroup::UpdateAgent(Agent& agent, const float deltaSeconds,
                                     const float observerX, const float observerZ,
                                     const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;

        agent.behaviorSeconds -= deltaSeconds;
        if (agent.behaviorSeconds <= 0.0f) {
            ++agent.transitionCount;
            switch (agent.state.behavior) {
            case SeaTurtleBehavior::Cruise:
                agent.state.behavior = agent.transitionCount % 4U == 0U
                    ? SeaTurtleBehavior::Surface : SeaTurtleBehavior::Forage;
                agent.behaviorSeconds = agent.state.behavior == SeaTurtleBehavior::Surface
                    ? 4.8f + static_cast<float>(agent.transitionCount % 3U) * 0.9f
                    : 7.0f + static_cast<float>(agent.transitionCount % 5U) * 1.2f;
                break;
            case SeaTurtleBehavior::Forage:
                agent.state.behavior = SeaTurtleBehavior::Cruise;
                agent.behaviorSeconds = 13.0f
                    + static_cast<float>(agent.transitionCount % 6U) * 1.7f;
                break;
            case SeaTurtleBehavior::Surface:
                agent.state.behavior = SeaTurtleBehavior::Cruise;
                agent.behaviorSeconds = 15.0f
                    + static_cast<float>(agent.transitionCount % 5U) * 2.0f;
                break;
            }
        }

        const float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        // The shark within its distance: a surfacing turtle dives, and the flight is from
        // the shark rather than the swimmer.
        const float sharkDistance = threatValid_
            ? std::hypot(agent.state.x - threatX_, agent.state.z - threatZ_)
            : std::numeric_limits<float>::infinity();
        const bool sharkNear = sharkDistance < ThreatDistance;
        if (sharkNear && agent.state.behavior == SeaTurtleBehavior::Surface) {
            agent.state.behavior = SeaTurtleBehavior::Cruise;
            agent.behaviorSeconds = 15.0f;
            ++threatDives_;
        }
        float desiredHeading = agent.state.headingRadians
            + std::sin(elapsedSeconds_ * 0.19f + agent.wanderPhase) * 0.42f;
        const float homeDistance = std::hypot(
            agent.state.x - agent.home.x, agent.state.z - agent.home.z);
        if (homeDistance > agent.home.radius * 0.67f) {
            desiredHeading = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
        } else if (sharkNear && sharkDistance < observerDistance) {
            desiredHeading = std::atan2(
                agent.state.z - threatZ_, agent.state.x - threatX_);
        } else if (observerDistance < 4.0f) {
            desiredHeading = std::atan2(
                agent.state.z - observerZ, agent.state.x - observerX);
        }
        // A non-circular coast can put the initial tangent against a shallow bank.
        // Steer before reaching it; at the home centre there is no homeward direction,
        // so use the offshore radial direction instead of atan2(0, 0).
        if (!IsValidHabitat(agent.state.x + std::cos(desiredHeading),
                            agent.state.z + std::sin(desiredHeading), agent.home)) {
            desiredHeading = homeDistance > 1.0f
                ? std::atan2(agent.home.z - agent.state.z, agent.home.x - agent.state.x)
                : std::atan2(agent.state.z, agent.state.x);
        }
        const float headingBefore = agent.state.headingRadians;
        agent.state.headingRadians = MoveAngle(
            agent.state.headingRadians, desiredHeading, deltaSeconds * 0.52f);
        const float headingChange = WrapAngle(agent.state.headingRadians - headingBefore);
        const float targetBank = std::clamp(headingChange / deltaSeconds * 0.32f,
                                            -0.22f, 0.22f);
        agent.state.bankRadians += std::clamp(
            targetBank - agent.state.bankRadians,
            -deltaSeconds * 0.42f, deltaSeconds * 0.42f);

        const float targetSpeed = (agent.state.behavior == SeaTurtleBehavior::Cruise
            ? 0.72f + static_cast<float>(agent.state.colorVariant) * 0.055f
            : (agent.state.behavior == SeaTurtleBehavior::Surface ? 0.42f : 0.24f))
            + (sharkNear ? 0.35f : 0.0f);
        agent.state.speed += std::clamp(
            targetSpeed - agent.state.speed, -deltaSeconds * 0.34f, deltaSeconds * 0.34f);
        const float candidateX = agent.state.x
            + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        const float candidateZ = agent.state.z
            + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        if (IsValidHabitat(candidateX, candidateZ, agent.home)) {
            agent.state.x = candidateX;
            agent.state.z = candidateZ;
        } else {
            agent.state.headingRadians = MoveAngle(
                agent.state.headingRadians,
                desiredHeading,
                deltaSeconds * 1.25f);
            agent.state.speed = std::min(agent.state.speed, 0.34f);
        }

        const float seabed = terrain_.Height(agent.state.x, agent.state.z);
        const float targetDepth = DesiredDepth(agent, seabed);
        const float previousY = agent.state.y;
        const float verticalRate = agent.state.behavior == SeaTurtleBehavior::Surface
            ? 0.42f : 0.26f;
        agent.state.y += std::clamp(
            targetDepth - agent.state.y, -verticalRate * deltaSeconds,
            verticalRate * deltaSeconds);
        const float verticalSpeed = (agent.state.y - previousY) / deltaSeconds;
        const float targetPitch = std::clamp(
            std::atan2(verticalSpeed, std::max(0.18f, agent.state.speed)), -0.24f, 0.24f);
        agent.state.pitchRadians += std::clamp(
            targetPitch - agent.state.pitchRadians,
            -deltaSeconds * 0.36f, deltaSeconds * 0.36f);

        const float flipperRate = agent.state.behavior == SeaTurtleBehavior::Forage
            ? 1.7f : (agent.state.behavior == SeaTurtleBehavior::Surface ? 2.5f : 3.2f);
        agent.flipperPhase = std::fmod(
            agent.flipperPhase + deltaSeconds * flipperRate, TwoPi);
        agent.state.flipperStroke = std::sin(agent.flipperPhase);
        // Grazing: foraging low over the bottom the head dips to it in a bite every couple
        // of seconds, each noted at the beak, on the bottom, for the sand it stirs.
        const bool grazing = agent.state.behavior == SeaTurtleBehavior::Forage
            && agent.state.y - terrain_.Height(agent.state.x, agent.state.z) < GrazeHeight;
        if (grazing) {
            agent.grazeClock += deltaSeconds;
            const float cycle = std::fmod(agent.grazeClock, BiteSeconds);
            const float want = cycle < BiteDipSeconds ? std::sin(cycle / BiteDipSeconds * 3.14159265f) : 0.0f;
            if (want > 0.95f && !agent.bitten) {
                agent.bitten = true;
                const float beakX = agent.state.x + std::cos(agent.state.headingRadians) * 0.8f * agent.state.scale;
                const float beakZ = agent.state.z + std::sin(agent.state.headingRadians) * 0.8f * agent.state.scale;
                bite_ = {beakX, terrain_.Height(beakX, beakZ), beakZ, agent.state.scale, bite_.count + 1U};
            }
            if (cycle >= BiteDipSeconds) agent.bitten = false;
            agent.state.graze += std::clamp(want - agent.state.graze, -deltaSeconds * 4.0f, deltaSeconds * 4.0f);
        } else {
            agent.grazeClock = 0.0f;
            agent.bitten = false;
            agent.state.graze = std::max(0.0f, agent.state.graze - deltaSeconds * 3.0f);
        }
        // The blink: a squeeze of the eyes at each breath (set where the breath is noted),
        // and a slow blink every dozen seconds or so otherwise.
        agent.blinkClock += deltaSeconds;
        if (agent.blinkClock >= BlinkPeriodSeconds + agent.state.scale * 4.0f) {
            agent.blinkClock = 0.0f;
            agent.blinkSeconds = BlinkSeconds;
        }
        const bool blinking = agent.blinkSeconds > 0.0f;
        if (blinking && !agent.blinking) ++blinks_;
        agent.blinking = blinking;
        agent.blinkSeconds = std::max(0.0f, agent.blinkSeconds - deltaSeconds);
        agent.state.blink += std::clamp((blinking ? 1.0f : 0.0f) - agent.state.blink,
                                        -deltaSeconds * 8.0f, deltaSeconds * 12.0f);
    }

    void SeaTurtleGroup::SetThreat(const float x, const float z, const bool valid) noexcept
    {
        threatValid_ = valid && std::isfinite(x) && std::isfinite(z);
        if (threatValid_) { threatX_ = x; threatZ_ = z; }
    }

    std::span<const SeaTurtleState> SeaTurtleGroup::Turtles() const noexcept
    {
        return states_;
    }

    std::span<const SeaTurtleHomeRange> SeaTurtleGroup::Homes() const noexcept
    {
        return homes_;
    }
}
