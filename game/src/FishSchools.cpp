#include "IslandSimulator/FishSchools.hpp"

#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        constexpr float MinimumShoreDistance = -48.0f;
        constexpr float MaximumShoreDistance = -3.0f;
        constexpr float MinimumSeabedDepth = 0.85f;

        [[nodiscard]] float WrapAngle(float angle) noexcept
        {
            while (angle >= TwoPi) {
                angle -= TwoPi;
            }
            while (angle < 0.0f) {
                angle += TwoPi;
            }
            return angle;
        }

        [[nodiscard]] float SignedAngleDifference(float from, float to) noexcept
        {
            float difference = to - from;
            while (difference > std::numbers::pi_v<float>) {
                difference -= TwoPi;
            }
            while (difference < -std::numbers::pi_v<float>) {
                difference += TwoPi;
            }
            return difference;
        }

        [[nodiscard]] float FishDepth(const float seabedHeight, const float depthPhase,
                                      const float elapsedSeconds) noexcept
        {
            const float desired = -0.72f
                + std::sin(elapsedSeconds * 0.61f + depthPhase) * 0.22f;
            const float lower = seabedHeight + 0.34f;
            const float upper = -0.24f;
            return lower <= upper ? std::clamp(desired, lower, upper) : upper;
        }
    }

    FishSchools::FishSchools(const IslandTerrain& terrain)
        : terrain_(terrain)
    {
        schools_.resize(SchoolCount);
        agents_.resize(FishCount);
        states_.resize(FishCount);
        schoolStates_.resize(SchoolCount);
        for (std::size_t schoolIndex = 0; schoolIndex < schools_.size(); ++schoolIndex) {
            InitializeSchool(schoolIndex);
        }
        SyncSchoolStates();
        for (std::size_t fishIndex = 0; fishIndex < agents_.size(); ++fishIndex) {
            InitializeAgent(fishIndex);
            states_[fishIndex] = agents_[fishIndex].state;
        }
    }

    void FishSchools::InitializeSchool(const std::size_t schoolIndex)
    {
        School& school = schools_[schoolIndex];
        const float baseAngle = WrapAngle(-std::numbers::pi_v<float> * 0.5f
            + static_cast<float>(schoolIndex) * TwoPi / static_cast<float>(SchoolCount)
            + static_cast<float>(schoolIndex % 3U) * 0.037f);
        school.offshoreDistance = 17.0f + static_cast<float>(schoolIndex % 4U) * 3.4f;
        float bestScore = -1.0f;
        for (int offset = -6; offset <= 6; ++offset) {
            const float candidate = WrapAngle(baseAngle + static_cast<float>(offset) * .085f);
            const float radius = terrain_.ShoreRadius(candidate) + school.offshoreDistance;
            const float x = std::cos(candidate) * radius;
            const float z = std::sin(candidate) * radius;
            const auto sample = terrain_.Sample(x, z);
            const float score = IslandEnvironmentField::AquaticHabitatSuitability(
                x, z, sample.signedShoreDistance, sample.height);
            if (score > bestScore) {
                bestScore = score;
                school.routeAngle = candidate;
            }
        }
        school.routeSpeed = 0.30f + static_cast<float>(schoolIndex % 3U) * 0.075f;
        school.direction = schoolIndex % 2U == 0U ? 1 : -1;
        ResolveSchoolCentre(school);
    }

    void FishSchools::InitializeAgent(const std::size_t fishIndex)
    {
        Agent& agent = agents_[fishIndex];
        const std::size_t schoolIndex = fishIndex / FishPerSchool;
        const std::size_t memberIndex = fishIndex % FishPerSchool;
        const School& school = schools_[schoolIndex];
        const float radialLength = std::max(1.0f, std::hypot(school.centreX, school.centreZ));
        const float radialX = school.centreX / radialLength;
        const float radialZ = school.centreZ / radialLength;
        const float tangentX = -radialZ * static_cast<float>(school.direction);
        const float tangentZ = radialX * static_cast<float>(school.direction);
        agent.alongOffset = (static_cast<float>(memberIndex % 3U) - 1.0f) * 1.7f;
        agent.radialOffset = (memberIndex < 3U ? -1.0f : 1.0f)
            * (0.65f + static_cast<float>(memberIndex % 3U) * 0.18f);
        agent.wanderPhase = std::fmod(static_cast<float>(fishIndex) * 1.271f + 0.4f, TwoPi);
        agent.wanderFrequency = 0.43f + static_cast<float>(fishIndex % 5U) * 0.041f;
        agent.depthPhase = std::fmod(static_cast<float>(fishIndex) * 0.917f, TwoPi);
        agent.bodyPhase = std::fmod(static_cast<float>(fishIndex) * 1.733f, TwoPi);
        agent.bodySpeed = 6.1f + static_cast<float>(fishIndex % 6U) * 0.43f;
        agent.state.x = school.centreX + tangentX * agent.alongOffset
            + radialX * agent.radialOffset;
        agent.state.z = school.centreZ + tangentZ * agent.alongOffset
            + radialZ * agent.radialOffset;
        const TerrainSample sample = terrain_.Sample(agent.state.x, agent.state.z);
        agent.state.y = FishDepth(sample.height, agent.depthPhase, elapsedSeconds_);
        agent.state.headingRadians = std::atan2(tangentZ, tangentX)
            + std::sin(agent.wanderPhase) * 0.12f;
        agent.state.speed = 1.05f + static_cast<float>(fishIndex % 5U) * 0.11f;
        agent.state.bodyWave = std::sin(agent.bodyPhase);
        agent.state.scale = 0.76f + static_cast<float>(fishIndex % 6U) * 0.065f;
        agent.state.schoolIndex = static_cast<std::uint8_t>(schoolIndex);
        agent.state.colorVariant = static_cast<std::uint8_t>((schoolIndex + memberIndex) % 4U);
    }

    void FishSchools::ResolveSchoolCentre(School& school) const noexcept
    {
        float bestDistance = school.offshoreDistance;
        float bestScore = -1.0f;
        for (const float distance : {12.0f, 20.0f, 30.0f, 42.0f}) {
            const float candidateRadius = terrain_.ShoreRadius(school.routeAngle) + distance;
            const float candidateX = std::cos(school.routeAngle) * candidateRadius;
            const float candidateZ = std::sin(school.routeAngle) * candidateRadius;
            const auto candidateSample = terrain_.Sample(candidateX, candidateZ);
            const float score = IslandEnvironmentField::AquaticHabitatSuitability(
                candidateX, candidateZ, candidateSample.signedShoreDistance,
                candidateSample.height);
            if (candidateSample.height <= -MinimumSeabedDepth && score > bestScore) {
                bestScore = score;
                bestDistance = distance;
            }
        }
        school.offshoreDistance = bestDistance;
        float radius = terrain_.ShoreRadius(school.routeAngle) + bestDistance;
        float x = std::cos(school.routeAngle) * radius;
        float z = std::sin(school.routeAngle) * radius;
        TerrainSample sample = terrain_.Sample(x, z);
        for (int attempt = 0; attempt < 8 && sample.height > -MinimumSeabedDepth; ++attempt) {
            radius += 2.0f;
            x = std::cos(school.routeAngle) * radius;
            z = std::sin(school.routeAngle) * radius;
            sample = terrain_.Sample(x, z);
        }
        school.centreX = x;
        school.centreZ = z;
        school.centreY = FishDepth(sample.height, school.routeAngle, elapsedSeconds_);
    }

    void FishSchools::SetThreat(const float x, const float y, const float z, const bool valid) noexcept
    {
        threatValid_ = valid && std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
        if (threatValid_) { threatX_ = x; threatY_ = y; threatZ_ = z; }
    }

    void FishSchools::SetShelter(const float x, const float z, const bool valid) noexcept
    {
        shelterValid_ = valid && std::isfinite(x) && std::isfinite(z);
        if (shelterValid_) { shelterX_ = x; shelterZ_ = z; }
    }

    std::size_t FishSchools::ShelteringCount() const noexcept
    {
        std::size_t count = 0;
        for (const School& school : schools_) count += school.sheltering ? 1U : 0U;
        return count;
    }

    void FishSchools::Update(const float deltaSeconds, const float observerX,
                             const float observerY, const float observerZ) noexcept
    {
        threatParting_ = 0U;
        const float safeDelta = std::isfinite(deltaSeconds)
            ? std::clamp(deltaSeconds, 0.0f, 0.1f) : 0.0f;
        elapsedSeconds_ += safeDelta;
        if (breach_.valid) {
            breach_.ageSeconds += safeDelta;
            if (breach_.ageSeconds > BreachSeconds) breach_.valid = false;
        }
        ++frameIndex_;
        observerValid_ = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        observerX_ = observerX; observerY_ = observerY; observerZ_ = observerZ;
        fullSimulationCount_ = 0;
        reducedSimulationCount_ = 0;
        dormantSimulationCount_ = 0;

        for (std::size_t schoolIndex = 0; schoolIndex < schools_.size(); ++schoolIndex) {
            School& school = schools_[schoolIndex];
            const float deltaX = school.centreX - observerX;
            const float deltaY = school.centreY - observerY;
            const float deltaZ = school.centreZ - observerZ;
            const float distance = std::hypot(deltaY, std::hypot(deltaX, deltaZ));
            school.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
            const int population = static_cast<int>(FishPerSchool);
            switch (school.simulationTier) {
            case WildlifeSimulationTier::Full:
                fullSimulationCount_ += population;
                break;
            case WildlifeSimulationTier::Reduced:
                reducedSimulationCount_ += population;
                break;
            case WildlifeSimulationTier::Dormant:
                dormantSimulationCount_ += population;
                break;
            }
            for (std::size_t memberIndex = 0; memberIndex < FishPerSchool; ++memberIndex) {
                agents_[schoolIndex * FishPerSchool + memberIndex].state.simulationTier =
                    school.simulationTier;
            }

            school.accumulatedSeconds += safeDelta;
            const std::uint32_t divisor = WildlifeActivationPolicy::UpdateDivisor(
                school.simulationTier);
            if ((frameIndex_ + schoolIndex) % divisor == 0U) {
                UpdateSchool(schoolIndex, std::min(school.accumulatedSeconds, 0.5f));
                school.accumulatedSeconds = 0.0f;
            }
        }

        for (std::size_t fishIndex = 0; fishIndex < agents_.size(); ++fishIndex) {
            states_[fishIndex] = agents_[fishIndex].state;
        }
        SyncSchoolStates();
#ifndef NDEBUG
        assert(states_.size() == FishCount && schoolStates_.size() == SchoolCount);
        assert(fullSimulationCount_ + reducedSimulationCount_ + dormantSimulationCount_
               == static_cast<int>(FishCount));
        for (const FishSchoolState& school : schoolStates_) {
            assert(std::isfinite(school.centreX) && std::isfinite(school.centreY)
                   && std::isfinite(school.centreZ));
            assert(school.direction == -1 || school.direction == 1);
        }
        for (const FishState& state : states_) {
            assert(std::isfinite(state.x) && std::isfinite(state.y)
                   && std::isfinite(state.z) && std::isfinite(state.headingRadians));
            assert(std::isfinite(state.pitchRadians) && std::fabs(state.pitchRadians) <= 0.221f);
            assert(std::isfinite(state.bankRadians) && std::fabs(state.bankRadians) <= 0.281f);
            assert(std::isfinite(state.speed) && state.speed >= 0.0f);
            assert(std::isfinite(state.bodyWave) && std::fabs(state.bodyWave) <= 1.001f);
            assert(std::isfinite(state.scale) && state.scale > 0.0f);
            assert(state.schoolIndex < SchoolCount && state.colorVariant < 4U);
        }
#endif
    }

    void FishSchools::UpdateSchool(const std::size_t schoolIndex,
                                   const float deltaSeconds) noexcept
    {
        if (deltaSeconds <= 0.0f) {
            return;
        }
        School& school = schools_[schoolIndex];
        // Now and then a fish breaks the surface over a school the swimmer can see: a small
        // ring on the water where the school is, a little off its centre.
        if (school.simulationTier == WildlifeSimulationTier::Full) {
            school.nextBreachSeconds -= deltaSeconds;
            if (school.nextBreachSeconds <= 0.0f) {
                const float swing = static_cast<float>((breach_.count * 7U + schoolIndex * 3U) % 8U) * 0.785f;
                breach_ = {school.centreX + std::cos(swing) * 0.7f, school.centreZ + std::sin(swing) * 0.7f,
                           0.0f, breach_.count + 1U, true};
                school.nextBreachSeconds = BreachIntervalSeconds + static_cast<float>((schoolIndex * 5U + breach_.count) % 9U);
            }
        }
        // The shade under the jetty: a school passing within range turns in and holds there
        // a while, then goes on its way and does not turn in again for a time.
        if (shelterValid_ && !school.sheltering && school.nextShelterSeconds <= 0.0f
            && std::hypot(school.centreX - shelterX_, school.centreZ - shelterZ_) < ShelterRange) {
            school.sheltering = true;
            school.holdSeconds = ShelterSeconds;
            ++shelters_;
        }
        if (school.sheltering) {
            school.holdSeconds -= deltaSeconds;
            if (school.holdSeconds <= 0.0f || !shelterValid_) {
                school.sheltering = false;
                school.nextShelterSeconds = ShelterIntervalSeconds;
            }
        } else {
            school.nextShelterSeconds = std::max(0.0f, school.nextShelterSeconds - deltaSeconds);
        }
        if (school.sheltering) {
            // Into the shade and hold: the centre eases to the shelter point; the route angle
            // keeps step so the resume is seamless.
            const float toX = shelterX_ - school.centreX, toZ = shelterZ_ - school.centreZ;
            const float toLength = std::hypot(toX, toZ);
            const float step = std::min(toLength, ShelterApproachSpeed * deltaSeconds);
            if (toLength > 0.001f) {
                school.centreX += toX / toLength * step;
                school.centreZ += toZ / toLength * step;
            }
            school.routeAngle = WrapAngle(std::atan2(school.centreZ, school.centreX));
            const auto shelterSample = terrain_.Sample(school.centreX, school.centreZ);
            school.centreY = FishDepth(shelterSample.height, school.routeAngle, elapsedSeconds_);
        } else {
        const float routeRadius = std::max(1.0f, std::hypot(school.centreX, school.centreZ));
        const float oldAngle = school.routeAngle;
        const float angleStep = static_cast<float>(school.direction) * school.routeSpeed
            / routeRadius * deltaSeconds;
        school.routeAngle = WrapAngle(oldAngle + angleStep);
        ResolveSchoolCentre(school);
        const auto habitatSample = terrain_.Sample(school.centreX, school.centreZ);
        const float habitat = IslandEnvironmentField::AquaticHabitatSuitability(
            school.centreX, school.centreZ, habitatSample.signedShoreDistance,
            habitatSample.height);
        if (habitat < .04f) {
            school.direction = -school.direction;
            school.routeAngle = WrapAngle(oldAngle - angleStep);
            ResolveSchoolCentre(school);
        }
        }
        const float radialLength = std::max(1.0f, std::hypot(school.centreX, school.centreZ));
        const float radialX = school.centreX / radialLength;
        const float radialZ = school.centreZ / radialLength;
        const float tangentX = -radialZ * static_cast<float>(school.direction);
        const float tangentZ = radialX * static_cast<float>(school.direction);

        for (std::size_t memberIndex = 0; memberIndex < FishPerSchool; ++memberIndex) {
            Agent& agent = agents_[schoolIndex * FishPerSchool + memberIndex];
            const float localDrift = std::sin(elapsedSeconds_ * agent.wanderFrequency
                                              + agent.wanderPhase);
            float targetX = school.centreX
                + tangentX * (agent.alongOffset + localDrift * 0.75f)
                + radialX * (agent.radialOffset + localDrift * 0.32f);
            float targetZ = school.centreZ
                + tangentZ * (agent.alongOffset + localDrift * 0.75f)
                + radialZ * (agent.radialOffset + localDrift * 0.32f);
            // The school parts around the swimmer: a fish inside the parting distance has
            // its slot pushed straight away from them and hurries there.
            float hurry = 1.0f;
            if (observerValid_) {
                const float awayX = agent.state.x - observerX_;
                const float awayZ = agent.state.z - observerZ_;
                const float swimmerDistance = std::hypot(agent.state.y - observerY_,
                                                         std::hypot(awayX, awayZ));
                const float planar = std::max(std::hypot(awayX, awayZ), 0.05f);
                if (swimmerDistance < PartingDistance) {
                    const float push = (PartingDistance - swimmerDistance) * 1.8f + 0.6f;
                    targetX += awayX / planar * push;
                    targetZ += awayZ / planar * push;
                    hurry = 1.7f;
                }
            }
            // The shark through the school: the fish part around it further out than
            // around the swimmer, and hurry.
            if (threatValid_) {
                const float awayX = agent.state.x - threatX_;
                const float awayZ = agent.state.z - threatZ_;
                const float threatDistance = std::hypot(agent.state.y - threatY_, std::hypot(awayX, awayZ));
                const float planar = std::max(std::hypot(awayX, awayZ), 0.05f);
                if (threatDistance < ThreatPartingDistance) {
                    const float push = (ThreatPartingDistance - threatDistance) * 1.4f + 0.8f;
                    targetX += awayX / planar * push;
                    targetZ += awayZ / planar * push;
                    hurry = std::max(hurry, 1.9f);
                    ++threatParting_;
                }
            }
            const float toTargetX = targetX - agent.state.x;
            const float toTargetZ = targetZ - agent.state.z;
            const float targetDistance = std::hypot(toTargetX, toTargetZ);
            float desiredHeading = std::atan2(tangentZ, tangentX)
                + localDrift * 0.20f;
            if (targetDistance > 0.55f) {
                desiredHeading = std::atan2(toTargetZ, toTargetX);
            }
            const float headingDifference = SignedAngleDifference(
                agent.state.headingRadians, desiredHeading);
            agent.state.headingRadians += std::clamp(
                headingDifference, -1.55f * deltaSeconds, 1.55f * deltaSeconds);
            const float desiredBank = std::clamp(
                headingDifference * 0.58f, -0.28f, 0.28f);
            agent.state.bankRadians += std::clamp(
                desiredBank - agent.state.bankRadians,
                -1.1f * deltaSeconds, 1.1f * deltaSeconds);

            const float speedScale = std::clamp(0.78f + targetDistance * 0.22f, 0.78f, 1.32f)
                * hurry;
            const float proposedX = agent.state.x + std::cos(agent.state.headingRadians)
                * agent.state.speed * speedScale * deltaSeconds;
            const float proposedZ = agent.state.z + std::sin(agent.state.headingRadians)
                * agent.state.speed * speedScale * deltaSeconds;
            const TerrainSample proposedSample = terrain_.Sample(proposedX, proposedZ);
            if (proposedSample.signedShoreDistance <= MaximumShoreDistance
                && proposedSample.signedShoreDistance >= MinimumShoreDistance
                && proposedSample.height <= -0.55f) {
                agent.state.x = proposedX;
                agent.state.z = proposedZ;
            } else {
                const float correctionHeading = proposedSample.signedShoreDistance
                        > MaximumShoreDistance
                    ? std::atan2(radialZ, radialX)
                    : std::atan2(-radialZ, -radialX);
                agent.state.headingRadians += std::clamp(
                    SignedAngleDifference(agent.state.headingRadians, correctionHeading),
                    -2.4f * deltaSeconds, 2.4f * deltaSeconds);
            }

            const TerrainSample currentSample = terrain_.Sample(agent.state.x, agent.state.z);
            const float desiredDepth = FishDepth(currentSample.height, agent.depthPhase,
                                                 elapsedSeconds_);
            const float previousY = agent.state.y;
            agent.state.y += std::clamp(desiredDepth - agent.state.y,
                                        -0.9f * deltaSeconds, 0.9f * deltaSeconds);
            const float verticalSpeed = (agent.state.y - previousY) / deltaSeconds;
            const float desiredPitch = std::clamp(
                std::atan2(verticalSpeed, agent.state.speed * speedScale), -0.22f, 0.22f);
            agent.state.pitchRadians += std::clamp(
                desiredPitch - agent.state.pitchRadians,
                -0.72f * deltaSeconds, 0.72f * deltaSeconds);
            agent.bodyPhase = std::fmod(agent.bodyPhase
                                        + agent.bodySpeed * speedScale * deltaSeconds, TwoPi);
            agent.state.bodyWave = std::sin(agent.bodyPhase);
        }
    }

    std::span<const FishState> FishSchools::Fish() const noexcept
    {
        return states_;
    }

    std::span<const FishSchoolState> FishSchools::Schools() const noexcept
    {
        return schoolStates_;
    }

    void FishSchools::SyncSchoolStates() noexcept
    {
        for (std::size_t index = 0; index < schools_.size(); ++index) {
            const School& school = schools_[index];
            schoolStates_[index] = FishSchoolState{
                school.centreX,
                school.centreY,
                school.centreZ,
                school.direction,
                school.simulationTier,
            };
        }
    }
}
