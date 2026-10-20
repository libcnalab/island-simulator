#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/CaveSite.hpp"
#include "IslandSimulator/FallingFruitField.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace island
{
    namespace
    {
        constexpr float Tau = 2.0f * std::numbers::pi_v<float>;
        float Wrap(float angle) noexcept { return std::remainder(angle, Tau); }
        float Sequence(std::uint32_t seed) noexcept
        {
            seed ^= seed >> 16U;
            seed *= 0x7feb352dU;
            seed ^= seed >> 15U;
            return static_cast<float>(seed & 0x00ffffffU) / 16777216.0f;
        }
    }

    BoarHerd::BoarHerd(const IslandTerrain& terrain,
                       const std::span<const CircularObstacle> obstacles)
        : terrain_(terrain), obstacles_(obstacles.begin(), obstacles.end())
    {
        for (const auto& obstacle : obstacles_) {
            if (!std::isfinite(obstacle.x) || !std::isfinite(obstacle.z)
                || !std::isfinite(obstacle.radius) || obstacle.radius < 0.0f) {
                throw std::invalid_argument("Boar habitat requires finite nonnegative obstacles");
            }
        }
        for (std::size_t index = 0; index < BoarCount; ++index) Initialize(index);
    }

    void BoarHerd::SetObstacles(const std::span<const CircularObstacle> obstacles)
    {
        BoarHerd replacement(terrain_, obstacles);
        obstacles_.swap(replacement.obstacles_);
        agents_ = replacement.agents_;
        states_ = replacement.states_;
        frame_ = 0;
    }

    bool BoarHerd::IsValidGround(const float x, const float z, const bool allowDrainage) const noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z) || CaveSite::Reserves(x,z,CollisionRadius)) return false;
        const auto sample = terrain_.Sample(x, z);
        const auto environment = IslandEnvironmentField::SampleInland(
            x, z, sample.signedShoreDistance);
        // The stream corridor is the drainage field itself: ordinary foraging keeps out of it,
        // but a boar walking to its wallow is allowed onto the bank.
        if (sample.signedShoreDistance < MinimumShoreDistance
            || sample.height < 2.0f || sample.slopeRadians > MaximumSlope
            || environment.moisture < MinimumHabitatMoisture
            || (!allowDrainage && environment.drainage > MaximumHabitatDrainage)
            || environment.saltExposure > MaximumHabitatSaltExposure) return false;
        for (const auto& obstacle : obstacles_) {
            if (std::hypot(x - obstacle.x, z - obstacle.z)
                < obstacle.radius + CollisionRadius) return false;
        }
        return true;
    }

    void BoarHerd::Initialize(const std::size_t index)
    {
        // Three pairs occupy the vegetated banks of the shared western drainage corridor.
        // They are offset well outside the channel itself, but remain in genuinely damp
        // interior ground instead of three arbitrary radial sectors.
        constexpr std::array<std::array<float, 2>, 3> Regions = {{
            {-72.0f, -49.0f}, {-92.0f, -46.0f}, {-112.0f, -42.0f}}};
        auto& agent = agents_[index];
        auto& state = agent.state;
        const auto& region = Regions[index / 2U];
        bool found = false;
        for (unsigned attempt = 0; attempt < 256; ++attempt) {
            const float angle = static_cast<float>(attempt) * 2.39996323f
                + static_cast<float>(index) * 1.37f;
            const float radius = 3.0f + std::sqrt(static_cast<float>(attempt)) * 1.5f;
            const float x = region[0] + std::cos(angle) * radius;
            const float z = region[1] + std::sin(angle) * radius;
            bool separated = true;
            for (std::size_t previous = 0; previous < index; ++previous) {
                separated &= std::hypot(x - states_[previous].x, z - states_[previous].z) > 4.0f;
            }
            if (separated && IsValidGround(x, z)) {
                state.x = state.homeX = x;
                state.z = state.homeZ = z;
                found = true;
                break;
            }
        }
        if (!found) throw std::runtime_error("No valid inland home for procedural boar");
        state.y = terrain_.Height(state.x, state.z);
        state.scale = index % 2U == 0U ? 1.0f : 0.66f;
        state.colorVariant = static_cast<std::uint8_t>(index % 3U);
        state.headingRadians = Wrap(static_cast<float>(index) * 1.71f);
        state.behavior = index % 2U == 0U ? BoarBehavior::Forage : BoarBehavior::Rest;
        agent.remainingSeconds = 2.0f + static_cast<float>(index) * 0.8f;
        agent.targetHeading = state.headingRadians;
        agent.decisions = static_cast<std::uint32_t>(index) * 173U;
        FindWallow(agent);
        states_[index] = state;
    }

    // The bank a metre outside the channel on the home's side, at the closest point of the
    // authored stream route inside the range that the habitat rules allow a boar to stand on.
    void BoarHerd::FindWallow(Agent& agent) noexcept
    {
        const BoarState& state = agent.state;
        agent.hasWallow = false;
        float best = std::numeric_limits<float>::max();
        for (float along = 6.0f; along < StreamRoute::NominalLength - 6.0f; along += 1.0f) {
            const float cx = StreamRoute::SourceX - along;
            const float cz = StreamRoute::CentreZ(cx);
            const float tangentZ = StreamRoute::CentreZ(cx - 1.0f) - cz;
            const float length = std::hypot(1.0f, tangentZ);
            const float normalX = tangentZ / length, normalZ = 1.0f / length;
            const float side = (state.homeZ - cz) * normalZ + (state.homeX - cx) * normalX < 0.0f ? -1.0f : 1.0f;
            const float offset = StreamRoute::HalfWidth(cx) + 1.1f;
            const float x = cx + normalX * offset * side, z = cz + normalZ * offset * side;
            const float homeDistance = std::hypot(x - state.homeX, z - state.homeZ);
            if (homeDistance >= best || homeDistance > HomeRadius * 0.9f || !IsValidGround(x, z, true)) continue;
            best = homeDistance;
            agent.wallowX = x; agent.wallowZ = z; agent.faceX = cx; agent.faceZ = cz;
            agent.hasWallow = true;
        }
    }

    std::size_t BoarHerd::WallowerCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.hasWallow ? 1U : 0U;
        return count;
    }

    // The nearest trunk (a static obstacle) inside the range, approached from the boar's side
    // to a standing point just outside the collision envelope on valid ground.
    // The nearest fruit or coconut lying within the boar's range on ground it can stand on;
    // the boar roots at the fruit itself, nose toward it along its approach.
    bool BoarHerd::ChooseFruitTarget(Agent& agent) noexcept
    {
        const BoarState& state = agent.state;
        agent.fruitTarget = -1;
        float best = std::numeric_limits<float>::max();
        for (std::size_t index = 0; index < lyingFruit_.size(); ++index) {
            const FallingFruit& fruit = lyingFruit_[index];
            if (!fruit.active || fruit.restSeconds < 0.0f || fruit.rolling) continue;
            if (std::hypot(fruit.x - state.homeX, fruit.z - state.homeZ) > HomeRadius * 0.9f) continue;
            const float distance = std::hypot(fruit.x - state.x, fruit.z - state.z);
            if (distance < 0.01f || distance >= best || !IsValidGround(fruit.x, fruit.z)) continue;
            best = distance;
            agent.fruitTarget = static_cast<int>(index);
            agent.rootX = fruit.x; agent.rootZ = fruit.z;
            agent.trunkX = fruit.x + (fruit.x - state.x) / distance;
            agent.trunkZ = fruit.z + (fruit.z - state.z) / distance;
            agent.hasRoot = true;
        }
        return agent.fruitTarget >= 0;
    }

    void BoarHerd::ChooseRootingSite(Agent& agent) noexcept
    {
        const BoarState& state = agent.state;
        float best = std::numeric_limits<float>::max();
        for (const auto& obstacle : obstacles_) {
            if (obstacle.radius < 0.12f || obstacle.radius > 1.6f) continue;
            const float distance = std::hypot(obstacle.x - state.x, obstacle.z - state.z);
            if (distance < 0.01f || distance >= best) continue;
            const float standOff = obstacle.radius + CollisionRadius + 0.12f;
            const float x = obstacle.x + (state.x - obstacle.x) / distance * standOff;
            const float z = obstacle.z + (state.z - obstacle.z) / distance * standOff;
            if (std::hypot(x - state.homeX, z - state.homeZ) > HomeRadius * 0.9f
                || !IsValidGround(x, z)) continue;
            best = distance;
            agent.rootX = x; agent.rootZ = z; agent.trunkX = obstacle.x; agent.trunkZ = obstacle.z;
            agent.hasRoot = true;
        }
    }

    void BoarHerd::Update(const float deltaSeconds, const float observerX,
                         const float observerY, const float observerZ) noexcept
    {
        const float dt = std::isfinite(deltaSeconds) ? std::clamp(deltaSeconds, 0.0f, 0.1f) : 0.0f;
        const bool valid = std::isfinite(observerX) && std::isfinite(observerY)
            && std::isfinite(observerZ);
        ++frame_;
        eatenFruit_.clear();
        for (std::size_t index = 0; index < BoarCount; ++index) {
            auto& agent = agents_[index];
            const auto& state = agent.state;
            const float distance = valid
                ? std::hypot(std::hypot(state.x - observerX, state.z - observerZ), state.y - observerY)
                : std::numeric_limits<float>::infinity();
            agent.state.simulationTier = WildlifeActivationPolicy::Classify(
                distance, FullSimulationDistance, ReducedSimulationDistance);
            agent.pendingSeconds = std::min(agent.pendingSeconds + dt, 0.5f);
            const auto divisor = WildlifeActivationPolicy::UpdateDivisor(agent.state.simulationTier);
            if ((frame_ + index) % divisor == 0 && dt > 0.0f) {
                // Bound spatial increments even for distant, throttled agents. No obstacle tunneling.
                while (agent.pendingSeconds > 0.00001f) {
                    const float step = std::min(agent.pendingSeconds, 1.0f / 30.0f);
                    Step(agent, step, observerX, observerZ, valid && distance < 15.0f);
                    agent.pendingSeconds -= step;
                }
            }
            states_[index] = agent.state;
        }
    }

    void BoarHerd::Step(Agent& agent, const float dt, const float observerX,
                       const float observerZ, const bool observerValid) noexcept
    {
        auto& state = agent.state;
        const float observerDistance = observerValid
            ? std::hypot(state.x - observerX, state.z - observerZ)
            : std::numeric_limits<float>::infinity();
        agent.remainingSeconds -= dt;
        const bool oddAgent = (static_cast<std::size_t>(&agent - agents_.data()) % 2U) == 1U;
        bool alertFacing = false;
        if (observerDistance < 6.0f) {
            state.behavior = BoarBehavior::Retreat;
            agent.remainingSeconds = 2.0f;
            // Trot off about seventy degrees from the direct line of escape so the boar
            // presents its flank, keeping the player in view, rather than its rump.
            if (observerDistance > 0.001f) {
                agent.targetHeading = Wrap(std::atan2(state.z - observerZ, state.x - observerX)
                    + (oddAgent ? -1.22f : 1.22f));
            }
        } else if (observerDistance < AlertDistance) {
            // Stand and face the approaching player; foraging resumes once they leave.
            state.behavior = BoarBehavior::Rest;
            agent.remainingSeconds = std::max(agent.remainingSeconds, 0.5f);
            agent.targetHeading = std::atan2(observerZ - state.z, observerX - state.x);
            alertFacing = true;
        } else if (agent.remainingSeconds <= 0.0f) {
            const float choice = Sequence(++agent.decisions);
            state.behavior = state.behavior == BoarBehavior::Wander
                ? BoarBehavior::Forage
                : (choice < 0.25f ? BoarBehavior::Rest : BoarBehavior::Wander);
            agent.remainingSeconds = 3.0f + choice * 6.0f;
            agent.targetHeading = Wrap(state.headingRadians + (choice - 0.5f) * 3.4f);
            agent.hasRoot = false;
            agent.fruitTarget = -1;
            if (state.behavior == BoarBehavior::Forage) {
                // Fallen fruit and coconuts outrank the trunk bases while any lie in range.
                if (!ChooseFruitTarget(agent)) ChooseRootingSite(agent);
                if (agent.hasRoot) agent.remainingSeconds += 6.0f;
            }
            // Every third rest of a boar with a reachable bank becomes a walk to the water
            // to wallow: the trip plus the time in the mud.
            if (state.behavior == BoarBehavior::Rest && agent.hasWallow && agent.decisions % 3U == 0U) {
                state.behavior = BoarBehavior::Wallow;
                agent.remainingSeconds = WallowSeconds
                    + std::hypot(agent.wallowX - state.x, agent.wallowZ - state.z) / 0.5f;
            }
        }
        bool wallowing = false;
        if (state.behavior == BoarBehavior::Wallow) {
            const float bankDistance = std::hypot(state.x - agent.wallowX, state.z - agent.wallowZ);
            wallowing = bankDistance < 0.5f;
            agent.targetHeading = wallowing
                ? std::atan2(agent.faceZ - state.z, agent.faceX - state.x)
                : std::atan2(agent.wallowZ - state.z, agent.wallowX - state.x);
        }
        bool rooting = false;
        if (state.behavior == BoarBehavior::Forage && agent.hasRoot) {
            // Walk to the trunk's root flare, then stand with the snout among the roots.
            const float siteDistance = std::hypot(state.x - agent.rootX, state.z - agent.rootZ);
            rooting = siteDistance < 0.4f;
            if (rooting && agent.fruitTarget >= 0) {
                // Nose on the fruit: it is eaten, and the rooting pose carries on over the spot.
                if (static_cast<std::size_t>(agent.fruitTarget) < lyingFruit_.size())
                    eatenFruit_.push_back(static_cast<std::size_t>(agent.fruitTarget));
                ++eatCount_;
                agent.fruitTarget = -1;
            }
            agent.targetHeading = rooting
                ? std::atan2(agent.trunkZ - state.z, agent.trunkX - state.x)
                : std::atan2(agent.rootZ - state.z, agent.rootX - state.x);
        }
        if (!alertFacing && state.behavior != BoarBehavior::Wallow
            && std::hypot(state.x - state.homeX, state.z - state.homeZ) > HomeRadius * 0.72f) {
            agent.targetHeading = std::atan2(state.homeZ - state.z, state.homeX - state.x);
        }
        state.headingRadians = Wrap(state.headingRadians + std::clamp(
            Wrap(agent.targetHeading - state.headingRadians), -dt * 2.1f, dt * 2.1f));
        const float targetSpeed = state.behavior == BoarBehavior::Retreat ? 2.8f
            : state.behavior == BoarBehavior::Wander ? 0.85f
            : state.behavior == BoarBehavior::Forage
                ? (agent.hasRoot ? (rooting ? 0.0f : 0.42f) : 0.11f)
            : state.behavior == BoarBehavior::Wallow ? (wallowing ? 0.0f : 0.5f) : 0.0f;
        state.speed += std::clamp(targetSpeed - state.speed, -dt * 3.5f, dt * 2.4f);
        const float x = state.x + std::cos(state.headingRadians) * state.speed * dt;
        const float z = state.z + std::sin(state.headingRadians) * state.speed * dt;
        bool separated = true;
        for (const auto& other : agents_) {
            if (&other == &agent) continue;
            const float clearance = (state.scale + other.state.scale) * 0.72f;
            const float previousDistance = std::hypot(state.x - other.state.x, state.z - other.state.z);
            const float nextDistance = std::hypot(x - other.state.x, z - other.state.z);
            if (nextDistance < clearance && nextDistance < previousDistance) separated = false;
        }
        // The wallow trip may cross the stream's drainage ground, and a boar already on it
        // (even one startled off its trip) may keep walking until it is back on ordinary
        // habitat; nothing else takes a boar into the channel.
        const bool strictlyValid = IsValidGround(x, z);
        const bool allowance = state.behavior == BoarBehavior::Wallow || agent.inCorridor;
        if (separated && std::hypot(x - state.homeX, z - state.homeZ) <= HomeRadius
            && (strictlyValid || (allowance && IsValidGround(x, z, true)))) {
            agent.inCorridor = !strictlyValid;
            state.gaitPhase = std::fmod(state.gaitPhase + state.speed * dt * 7.0f, Tau);
            state.x = x;
            state.z = z;
        } else {
            state.speed = 0.0f;
            agent.targetHeading = Wrap(state.headingRadians + 1.1f);
        }
        state.y = terrain_.Height(state.x, state.z);
        state.inChannel = agent.inCorridor;
        // In the mud the boar drops its snout like rooting; the coat wets while it stays and
        // dries off over a few minutes once it leaves.
        const float rootTarget = state.behavior == BoarBehavior::Forage || wallowing ? 1.0f : 0.0f;
        state.rootingPose += std::clamp(rootTarget - state.rootingPose, -dt * 1.8f, dt * 1.2f);
        state.mudAmount = wallowing ? std::min(1.0f, state.mudAmount + dt / 6.0f)
            : std::max(0.0f, state.mudAmount - dt / MudDrySeconds);
    }
}
