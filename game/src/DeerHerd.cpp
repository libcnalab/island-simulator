#include "IslandSimulator/DeerHerd.hpp"

#include "IslandSimulator/StreamRoute.hpp"
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
        // Both clearings lie about a hundred metres from the stream, so the fourth deer keeps
        // a stream-side range instead: nine metres off the channel on the upper reach, where
        // its drinking walk is short enough to be seen.
        std::array<float, 2> StreamSideCentre() noexcept
        {
            const float cx = StreamRoute::SourceX - 24.0f;
            const float cz = StreamRoute::CentreZ(cx);
            const float tangentZ = StreamRoute::CentreZ(cx - 1.0f) - cz;
            const float length = std::hypot(1.0f, tangentZ);
            return {{cx + tangentZ / length * 9.0f, cz + 1.0f / length * 9.0f}};
        }
        std::array<float, 2> Centre(const std::size_t index) noexcept
        {
            constexpr std::array<std::array<float, 2>, 3> Clearings = {{
                {{VegetationLayout::EastClearing.x - 5.0f, VegetationLayout::EastClearing.z + 2.0f}},
                {{VegetationLayout::EastClearing.x + 7.0f, VegetationLayout::EastClearing.z - 3.0f}},
                {{VegetationLayout::WestClearing.x - 4.0f, VegetationLayout::WestClearing.z + 2.0f}},
            }};
            return index < Clearings.size() ? Clearings[index] : StreamSideCentre();
        }

        float Wrap(const float angle) noexcept { return std::remainder(angle, TwoPi); }
        float Turn(const float current, const float target, const float limit) noexcept
        {
            return Wrap(current + std::clamp(Wrap(target - current), -limit, limit));
        }
    }

    DeerHerd::DeerHerd(const IslandTerrain& terrain,
                       const std::span<const CircularObstacle> obstacles)
        : terrain_(terrain), obstacles_(obstacles)
    {
        for (std::size_t index = 0; index < Count; ++index) {
            Initialize(index);
            states_[index] = agents_[index].state;
            homes_[index] = agents_[index].home;
        }
        for (std::size_t index = 0; index < Count; ++index) {
            FindDrinkingPoint(index);
        }
    }

    // The bank one metre outside the channel on the home's side, at the closest point of the
    // authored route that the deer may actually stand on; most homes lie beyond reach.
    void DeerHerd::FindDrinkingPoint(const std::size_t index) noexcept
    {
        Agent& agent = agents_[index];
        agent.hasDrink = false;
        float bestDistance = std::numeric_limits<float>::max();
        for (float along = 6.0f; along < StreamRoute::NominalLength - 6.0f; along += 1.0f) {
            const float cx = StreamRoute::SourceX - along;
            const float cz = StreamRoute::CentreZ(cx);
            const float tangentZ = StreamRoute::CentreZ(cx - 1.0f) - cz;
            const float length = std::hypot(1.0f, tangentZ);
            const float normalX = tangentZ / length, normalZ = 1.0f / length;
            const float side = (agent.home.z - cz) * normalZ + (agent.home.x - cx) * normalX < 0.0f
                ? -1.0f : 1.0f;
            const float offset = StreamRoute::HalfWidth(cx) + 1.05f;
            const float x = cx + normalX * offset * side, z = cz + normalZ * offset * side;
            const float homeDistance = std::hypot(x - agent.home.x, z - agent.home.z);
            if (homeDistance >= bestDistance || !IsValidPosition(index, x, z)) continue;
            bestDistance = homeDistance;
            agent.drinkX = x; agent.drinkZ = z; agent.faceX = cx; agent.faceZ = cz;
            agent.hasDrink = true;
        }
    }

    std::size_t DeerHerd::DrinkerCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.hasDrink ? 1U : 0U;
        return count;
    }

    bool DeerHerd::DrinkPoint(const std::size_t index, float& x, float& z) const noexcept
    {
        if (index >= Count || !agents_[index].hasDrink) return false;
        x = agents_[index].drinkX; z = agents_[index].drinkZ;
        return true;
    }

    void DeerHerd::Initialize(const std::size_t index)
    {
        Agent& agent = agents_[index];
        agent.home.radius = 13.0f + static_cast<float>(index % 2U) * 2.0f;
        bool found = false;
        for (int attempt = 0; attempt < 192 && !found; ++attempt) {
            const float angle = static_cast<float>(attempt) * 2.39996323f
                + static_cast<float>(index) * 1.13f;
            const float radius = .6f + static_cast<float>(attempt / 24) * 1.15f;
            agent.home.x = Centre(index)[0] + std::cos(angle) * radius;
            agent.home.z = Centre(index)[1] + std::sin(angle) * radius;
            const TerrainSample sample = terrain_.Sample(agent.home.x, agent.home.z);
            found = (sample.surface == TerrainSurface::InteriorGround
                     || sample.surface == TerrainSurface::CoastalGround)
                && sample.signedShoreDistance > 44.0f && sample.slopeRadians < .42f
                && !obstacles_.IntersectsCircle(agent.home.x, agent.home.z, .72f);
        }
        if (!found) throw std::runtime_error("No safe open inland deer habitat");
        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z);
        agent.state.headingRadians = Wrap(.4f + static_cast<float>(index) * 1.45f);
        agent.targetHeadingRadians = agent.state.headingRadians;
        agent.state.scale = index == 3U ? .84f : .94f + .035f * static_cast<float>(index);
        agent.state.coatVariant = static_cast<std::uint8_t>(index % 3U);
        agent.state.antlered = index == 0U;
        agent.state.behavior = index == 0U ? DeerBehavior::Walk
            : index % 3U == 2U ? DeerBehavior::Rest : DeerBehavior::Graze;
        agent.state.grazeAmount = agent.state.behavior == DeerBehavior::Graze ? 1.0f : 0.0f;
        agent.behaviorSeconds = 3.5f + static_cast<float>(index) * 1.2f;
        agent.state.gaitPhase = static_cast<float>(index) * 1.4f;
    }

    bool DeerHerd::IsValidPosition(const std::size_t index, const float x,
                                   const float z) const noexcept
    {
        if (index >= Count || !std::isfinite(x) || !std::isfinite(z)) return false;
        const TerrainSample sample = terrain_.Sample(x, z);
        const bool ground = sample.surface == TerrainSurface::InteriorGround
            || sample.surface == TerrainSurface::CoastalGround;
        return ground && sample.signedShoreDistance > 40.0f && sample.slopeRadians < .48f
            && std::hypot(x - homes_[index].x, z - homes_[index].z) <= homes_[index].radius
            && !obstacles_.IntersectsCircle(x, z, .72f);
    }

    void DeerHerd::Update(const float deltaSeconds, const float observerX,
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
                ? std::hypot(agent.state.y + 1.0f - observerY,
                    std::hypot(agent.state.x - observerX, agent.state.z - observerZ))
                : std::numeric_limits<float>::infinity();
            agent.state.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
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
    }

    void DeerHerd::UpdateAgent(Agent& agent, const float deltaSeconds,
                               const float observerX, const float observerZ,
                               const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;
        const std::size_t index = static_cast<std::size_t>(&agent - agents_.data());
        const float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        bool alertFacing = false;
        if (observerDistance < 9.0f) {
            agent.state.behavior = DeerBehavior::Retreat;
            agent.behaviorSeconds = 1.8f;
            // Bound away about seventy degrees off the direct line of escape: the deer keeps
            // the player in view over its shoulder instead of turning its rump to them.
            agent.targetHeadingRadians = Wrap(std::atan2(
                agent.state.z - observerZ, agent.state.x - observerX)
                + (index % 2U == 0U ? 1.22f : -1.22f));
        } else if (observerDistance < AlertDistance) {
            // Stand alert and face the approaching player; grazing resumes once they leave.
            agent.state.behavior = DeerBehavior::Rest;
            agent.behaviorSeconds = std::max(agent.behaviorSeconds, 0.5f);
            agent.targetHeadingRadians = std::atan2(
                observerZ - agent.state.z, observerX - agent.state.x);
            alertFacing = true;
        } else if ((agent.behaviorSeconds -= deltaSeconds) <= 0.0f) {
            ++agent.transitions;
            const auto phase = (agent.transitions + static_cast<std::uint32_t>(index) * 3U) % 7U;
            agent.state.behavior = phase < 3U ? DeerBehavior::Graze
                : phase < 5U ? DeerBehavior::Walk : DeerBehavior::Rest;
            agent.behaviorSeconds = 3.5f + static_cast<float>((agent.transitions * 7U
                + static_cast<std::uint32_t>(index) * 5U) % 45U) * .12f;
            const float turn = -.85f + static_cast<float>((agent.transitions * 37U
                + static_cast<std::uint32_t>(index) * 19U) % 171U) * .01f;
            agent.targetHeadingRadians = Wrap(agent.state.headingRadians + turn);
            // Every other rest of a deer with a reachable bank becomes a walk to the water.
            if (agent.hasDrink && phase == 5U && (agent.transitions / 7U) % 2U == 0U) {
                agent.state.behavior = DeerBehavior::Drink;
                agent.behaviorSeconds = 22.0f;
            }
        }
        bool drinking = false;
        if (agent.state.behavior == DeerBehavior::Drink) {
            const float bankDistance = std::hypot(agent.state.x - agent.drinkX,
                                                   agent.state.z - agent.drinkZ);
            drinking = bankDistance < 0.9f;
            agent.targetHeadingRadians = drinking
                ? std::atan2(agent.faceZ - agent.state.z, agent.faceX - agent.state.x)
                : std::atan2(agent.drinkZ - agent.state.z, agent.drinkX - agent.state.x);
        }
        // Straying past the home margin overrides the behavioural target before the one
        // bounded turn of this update.  A second Turn() here used to double the per-frame
        // heading change whenever a retreating or wandering deer was also heading home.
        const float homeDistance = std::hypot(agent.state.x - agent.home.x,
                                               agent.state.z - agent.home.z);
        // A standing, watching deer is not travelling, so the home pull waits until it moves.
        if (homeDistance > agent.home.radius * .68f && !alertFacing
            && agent.state.behavior != DeerBehavior::Drink) {
            agent.targetHeadingRadians = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
        }
        const float turnRate = agent.state.behavior == DeerBehavior::Retreat ? 1.45f : .92f;
        agent.state.headingRadians = Turn(agent.state.headingRadians,
            agent.targetHeadingRadians, turnRate * deltaSeconds);
        const float headingError = std::abs(Wrap(
            agent.targetHeadingRadians - agent.state.headingRadians));
        const float turnAlignment = std::clamp(
            (std::cos(headingError) + .18f) / 1.18f, 0.0f, 1.0f);
        const float targetSpeed = (agent.state.behavior == DeerBehavior::Retreat ? 3.1f
            : agent.state.behavior == DeerBehavior::Walk ? .78f
            : agent.state.behavior == DeerBehavior::Drink && !drinking ? .78f : 0.0f)
            * (.12f + .88f * turnAlignment);
        agent.state.speed += std::clamp(targetSpeed - agent.state.speed,
            -2.8f * deltaSeconds, 3.8f * deltaSeconds);
        // Drinking is the grazing head-down pose held at the water's edge.
        const float targetGraze = agent.state.behavior == DeerBehavior::Graze || drinking
            ? 1.0f : 0.0f;
        agent.state.grazeAmount += std::clamp(targetGraze - agent.state.grazeAmount,
            -1.8f * deltaSeconds, 1.3f * deltaSeconds);
        const float nextX = agent.state.x
            + std::cos(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        const float nextZ = agent.state.z
            + std::sin(agent.state.headingRadians) * agent.state.speed * deltaSeconds;
        if (IsValidPosition(index, nextX, nextZ)) {
            agent.state.x = nextX;
            agent.state.z = nextZ;
        } else {
            agent.targetHeadingRadians = std::atan2(
                agent.home.z - agent.state.z, agent.home.x - agent.state.x);
            agent.state.speed = std::min(agent.state.speed, .45f);
            agent.behaviorSeconds = std::min(agent.behaviorSeconds, .8f);
        }
        agent.state.gaitPhase = std::fmod(agent.state.gaitPhase
            + deltaSeconds * (1.4f + agent.state.speed * 4.2f), TwoPi);
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z);
    }
}
