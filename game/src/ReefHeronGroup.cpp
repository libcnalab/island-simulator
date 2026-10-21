#include "IslandSimulator/ReefHeronGroup.hpp"

#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float Pi = std::numbers::pi_v<float>;
        // Four stretches of the lagoon shore, spread round the island.
        constexpr float HomeAngles[ReefHeronGroup::Count] = {0.62f, 2.18f, 3.71f, 5.12f};

        [[nodiscard]] float WrapAngle(float angle) noexcept
        {
            angle = std::fmod(angle + Pi, 2.0f * Pi);
            if (angle < 0.0f) angle += 2.0f * Pi;
            return angle - Pi;
        }
    }

    ReefHeronGroup::ReefHeronGroup(const IslandTerrain& terrain) : terrain_(terrain)
    {
        agents_.resize(Count);
        states_.resize(Count);
        for (std::size_t index = 0; index < Count; ++index) {
            Agent& agent = agents_[index];
            agent.random = 0x9e3779b9U * static_cast<std::uint32_t>(index + 1U) ^ 0x51ed270bU;
            agent.depth = 0.14f + 0.10f * Random(agent);
            agent.direction = index % 2U == 0U ? 1.0f : -1.0f;
            agent.state.morph = static_cast<std::uint8_t>(index % 2U);
            agent.state.scale = 0.95f + 0.13f * Random(agent);
            // the first bearing near the home where the shore shelves gently enough to wade
            float x = 0.0f, z = 0.0f;
            float angle = HomeAngles[index];
            for (int attempt = 0; attempt < 120 && !WadingPoint(angle, agent.depth, x, z); ++attempt)
                angle = HomeAngles[index] + (attempt % 2 == 0 ? 1.0f : -1.0f) * 0.02f * static_cast<float>(attempt / 2 + 1);
            agent.homeAngle = agent.angle = angle;
            PlaceOnShore(agent);
            agent.state.headingRadians = agent.angle + agent.direction * 0.5f * Pi;   // along the shore
            Begin(agent, ReefHeronBehavior::Stalk, 4.0f + 6.0f * Random(agent));
            states_[index] = agent.state;
        }
    }

    float ReefHeronGroup::Random(Agent& agent) const noexcept
    {
        agent.random = agent.random * 1664525U + 1013904223U;
        return static_cast<float>(agent.random >> 8U) / 16777216.0f;
    }

    bool ReefHeronGroup::WadingPoint(const float angleRadians, const float depth, float& x, float& z) const noexcept
    {
        if (!std::isfinite(angleRadians) || !std::isfinite(depth)) return false;
        const float shore = terrain_.ShoreRadius(angleRadians);
        const float c = std::cos(angleRadians), s = std::sin(angleRadians);
        float previousDepth = -1.0f;
        for (float r = shore - 6.0f; r <= shore + 30.0f; r += 0.2f) {
            const float px = c * r, pz = s * r;
            const float here = -terrain_.Height(px, pz);
            if (here >= depth && previousDepth < depth && previousDepth > -1.0f) {
                // the wading band must shelve gently: no more than a metre of water a few
                // metres further out, or this is a drop-off, not a flat to wade
                if (-terrain_.Height(c * (r + 3.0f), s * (r + 3.0f)) > 1.2f) return false;
                x = px;
                z = pz;
                return true;
            }
            previousDepth = here;
        }
        return false;
    }

    void ReefHeronGroup::PlaceOnShore(Agent& agent) noexcept
    {
        float x = agent.state.x, z = agent.state.z;
        if (WadingPoint(agent.angle, agent.depth, x, z)) {
            agent.state.x = x;
            agent.state.z = z;
        }
        agent.state.y = terrain_.Height(agent.state.x, agent.state.z);
        agent.state.waterDepth = std::max(0.0f, -agent.state.y);
    }

    void ReefHeronGroup::Begin(Agent& agent, const ReefHeronBehavior behavior, const float length) noexcept
    {
        agent.state.behavior = behavior;
        agent.behaviorSeconds = 0.0f;
        agent.behaviorLength = length;
        agent.struck = false;
    }

    void ReefHeronGroup::SetDisturbers(const std::span<const Disturber> disturbers) noexcept
    {
        disturbers_.clear();
        for (const Disturber& disturber : disturbers) {
            if (disturbers_.size() >= MaximumDisturbers) break;
            if (std::isfinite(disturber.x) && std::isfinite(disturber.z) && std::isfinite(disturber.distance)
                && disturber.distance > 0.0f)
                disturbers_.push_back(disturber);
        }
    }

    void ReefHeronGroup::SetRoosts(const std::span<const Roost> roosts)
    {
        roosts_.clear();
        for (const Roost& roost : roosts)
            if (std::isfinite(roost.x) && std::isfinite(roost.y) && std::isfinite(roost.z)) roosts_.push_back(roost);
        for (Agent& agent : agents_) { agent.roost = roosts_.size(); agent.roosted = false; }
    }

    std::size_t ReefHeronGroup::RoostedCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) if (agent.roosted) ++count;
        return count;
    }

    std::size_t ReefHeronGroup::FreeRoostNear(const Agent& agent) const noexcept
    {
        std::size_t best = roosts_.size();
        float bestDistance = RoostRange * RoostRange;
        for (std::size_t index = 0; index < roosts_.size(); ++index) {
            bool held = false;
            for (const Agent& other : agents_)
                if (&other != &agent && other.roost == index) held = true;
            if (held) continue;
            const float dx = roosts_[index].x - agent.state.x, dz = roosts_[index].z - agent.state.z;
            const float distance = dx * dx + dz * dz;
            if (distance < bestDistance) { bestDistance = distance; best = index; }
        }
        return best;
    }

    void ReefHeronGroup::FlyTo(Agent& agent, const float x, const float y, const float z) noexcept
    {
        ReefHeronState& state = agent.state;
        agent.startX = state.x; agent.startY = state.y; agent.startZ = state.z;
        agent.targetX = x; agent.targetY = y; agent.targetZ = z;
        agent.flightLength = std::max(2.0f, std::hypot(x - state.x, z - state.z) / FlySpeed);
        agent.flightSeconds = 0.0f;
        Begin(agent, ReefHeronBehavior::Fly, agent.flightLength);
    }

    bool ReefHeronGroup::Flush(Agent& agent, const float fromX, const float fromZ) noexcept
    {
        // off along the shore, away from what flushed it
        const float fromAngle = std::atan2(fromZ, fromX);
        const float away = WrapAngle(agent.angle - fromAngle) >= 0.0f ? 1.0f : -1.0f;
        const float radius = std::max(terrain_.ShoreRadius(agent.angle), 10.0f);
        for (const float sense : {away, -away}) {
            const float hop = (45.0f + 25.0f * Random(agent)) / radius;
            const float target = std::clamp(agent.angle + sense * hop,
                                            agent.homeAngle - 0.6f, agent.homeAngle + 0.6f);
            float x = 0.0f, z = 0.0f;
            if (std::abs(target - agent.angle) > 0.05f && WadingPoint(target, agent.depth, x, z)) {
                agent.targetAngle = target;
                agent.flightKind = Agent::Flight::Flush;
                agent.roost = roosts_.size();
                agent.roosted = false;
                lastFlush_ = Takeoff{agent.state.x, agent.state.y + 0.5f, agent.state.z, lastFlush_.count + 1U};
                FlyTo(agent, x, terrain_.Height(x, z), z);
                ++flushes_;
                return true;
            }
        }
        return false;
    }

    void ReefHeronGroup::SetNightness(const float nightness) noexcept
    {
        nightness_ = std::isfinite(nightness) ? std::clamp(nightness, 0.0f, 1.0f) : 0.0f;
    }

    void ReefHeronGroup::PreviewFreeze(const std::size_t index) noexcept
    {
        frozen_ = index < Count ? index : Count;
    }

    void ReefHeronGroup::Update(const float deltaSeconds, const float observerX, const float observerY,
                                const float observerZ) noexcept
    {
        const float dt = std::isfinite(deltaSeconds) ? std::clamp(deltaSeconds, 0.0f, 0.1f) : 0.0f;
        if (lastStrike_.valid) {
            lastStrike_.ageSeconds += dt;
            if (lastStrike_.ageSeconds >= StrikeSeconds) lastStrike_.valid = false;
        }
        const bool observerValid = std::isfinite(observerX) && std::isfinite(observerY) && std::isfinite(observerZ);
        for (std::size_t index = 0; index < Count; ++index) {
            Agent& agent = agents_[index];
            if (index == frozen_) {
                // held on the watch, craned out over the water
                if (agent.state.behavior != ReefHeronBehavior::Freeze) Begin(agent, ReefHeronBehavior::Freeze, 1.0e6f);
                agent.state.neckStretch = 1.0f;
                agent.state.flight = agent.state.rest = agent.state.strike = 0.0f;
                PlaceOnShore(agent);
                agent.state.headingRadians = agent.angle + agent.direction * 0.5f * Pi;
            } else {
                UpdateAgent(agent, dt, observerX, observerZ,
                            observerValid && observerY < agent.state.y + 6.0f);
            }
            states_[index] = agent.state;
        }
    }

    void ReefHeronGroup::UpdateAgent(Agent& agent, const float dt, const float observerX, const float observerZ,
                                     const bool observerValid) noexcept
    {
        ReefHeronState& state = agent.state;
        agent.behaviorSeconds += dt;
        const float ease = 1.0f - std::exp(-2.5f * dt);

        // A walker close by flushes it, whatever it is doing on the ground; so does anything
        // else it will not let come near (the resident, the quad, a bear, boar or crocodile).
        // a heron roosting up in a tree lets the shore go on below it
        if (state.behavior != ReefHeronBehavior::Fly && !agent.roosted) {
            const auto near = [&](const float x, const float z, const float distance) {
                const float dx = x - state.x, dz = z - state.z;
                return dx * dx + dz * dz < distance * distance;
            };
            if (observerValid && near(observerX, observerZ, FlushDistance)) {
                Flush(agent, observerX, observerZ);
            } else {
                for (const Disturber& disturber : disturbers_) {
                    if (near(disturber.x, disturber.z, disturber.distance)) {
                        Flush(agent, disturber.x, disturber.z);
                        break;
                    }
                }
            }
        }

        if (state.behavior != ReefHeronBehavior::Fly) {
            if (nightness_ > 0.6f && state.behavior != ReefHeronBehavior::Rest) {
                // dusk: off to the nearest free roost, or rest in the shallows without one
                const std::size_t roost = FreeRoostNear(agent);
                if (roost < roosts_.size()) {
                    agent.roost = roost;
                    agent.flightKind = Agent::Flight::ToRoost;
                    FlyTo(agent, roosts_[roost].x, roosts_[roost].y, roosts_[roost].z);
                } else {
                    Begin(agent, ReefHeronBehavior::Rest, 1.0e6f);
                }
            } else if (nightness_ < 0.3f && state.behavior == ReefHeronBehavior::Rest) {
                if (agent.roosted) {
                    // dawn: back down to its stretch of shore
                    float x = 0.0f, z = 0.0f;
                    if (!WadingPoint(agent.angle, agent.depth, x, z)) {
                        agent.angle = agent.homeAngle;
                        (void)WadingPoint(agent.angle, agent.depth, x, z);
                    }
                    agent.roosted = false;
                    agent.roost = roosts_.size();
                    agent.targetAngle = agent.angle;
                    agent.flightKind = Agent::Flight::Home;
                    FlyTo(agent, x, terrain_.Height(x, z), z);
                } else {
                    Begin(agent, ReefHeronBehavior::Stalk, 4.0f + 6.0f * Random(agent));
                }
            }
        }

        float neckTarget = 0.55f;
        const float shoreTangent = agent.angle + agent.direction * 0.5f * Pi;
        switch (state.behavior) {
        case ReefHeronBehavior::Stalk: {
            const float radius = std::max(terrain_.ShoreRadius(agent.angle), 10.0f);
            const float next = agent.angle + agent.direction * StalkSpeed * dt / radius;
            float x = 0.0f, z = 0.0f;
            if (std::abs(next - agent.homeAngle) > PatrolRadians || !WadingPoint(next, agent.depth, x, z)) {
                agent.direction = -agent.direction;   // the end of its stretch: turn back
            } else {
                agent.angle = next;
                state.x = x;
                state.z = z;
            }
            state.y = terrain_.Height(state.x, state.z);
            state.stepPhase = std::fmod(state.stepPhase + dt * 2.0f * Pi * 0.9f, 2.0f * Pi);
            state.headingRadians = agent.angle + agent.direction * 0.5f * Pi;
            if (agent.behaviorSeconds >= agent.behaviorLength)
                Begin(agent, ReefHeronBehavior::Freeze, 2.0f + 4.0f * Random(agent));
            break;
        }
        case ReefHeronBehavior::Freeze:
            neckTarget = 1.0f;
            // half turned to the open water, watching
            state.headingRadians = shoreTangent - agent.direction * 0.7f;
            if (agent.behaviorSeconds >= agent.behaviorLength) {
                if (Random(agent) < 0.45f) Begin(agent, ReefHeronBehavior::Strike, 0.7f);
                else Begin(agent, ReefHeronBehavior::Stalk, 4.0f + 6.0f * Random(agent));
            }
            break;
        case ReefHeronBehavior::Strike: {
            neckTarget = 1.0f;
            const float t = std::clamp(agent.behaviorSeconds / agent.behaviorLength, 0.0f, 1.0f);
            state.strike = t < 0.4f ? std::sin(t / 0.4f * 0.5f * Pi) : 1.0f - (t - 0.4f) / 0.6f;
            if (!agent.struck && t >= 0.35f) {
                agent.struck = true;
                ++strikes_;
                const float fx = std::cos(state.headingRadians), fz = std::sin(state.headingRadians);
                lastStrike_ = Strike{state.x + fx * 0.42f * state.scale, 0.0f, state.z + fz * 0.42f * state.scale,
                                     lastStrike_.count + 1U, 0.0f, true};
            }
            if (agent.behaviorSeconds >= agent.behaviorLength) {
                state.strike = 0.0f;
                Begin(agent, ReefHeronBehavior::Freeze, 1.5f);
            }
            break;
        }
        case ReefHeronBehavior::Fly: {
            neckTarget = 0.0f;
            agent.flightSeconds += dt;
            const float t = std::clamp(agent.flightSeconds / agent.flightLength, 0.0f, 1.0f);
            state.x = agent.startX + (agent.targetX - agent.startX) * t;
            state.z = agent.startZ + (agent.targetZ - agent.startZ) * t;
            state.y = agent.startY + (agent.targetY - agent.startY) * t + std::sin(Pi * t) * CruiseHeight;
            state.headingRadians = std::atan2(agent.targetZ - agent.startZ, agent.targetX - agent.startX);
            state.flight = std::min(std::clamp(t / 0.12f, 0.0f, 1.0f), std::clamp((1.0f - t) / 0.12f, 0.0f, 1.0f));
            state.wingBeat = std::fmod(state.wingBeat + dt * 2.0f * Pi * 2.3f, 2.0f * Pi);
            if (t >= 1.0f) {
                state.flight = 0.0f;
                if (agent.flightKind == Agent::Flight::ToRoost) {
                    state.x = agent.targetX; state.y = agent.targetY; state.z = agent.targetZ;
                    agent.roosted = true;
                    Begin(agent, ReefHeronBehavior::Rest, 1.0e6f);
                } else {
                    agent.angle = agent.targetAngle;
                    PlaceOnShore(agent);
                    if (agent.flightKind == Agent::Flight::Home)
                        Begin(agent, ReefHeronBehavior::Stalk, 4.0f + 6.0f * Random(agent));
                    else
                        Begin(agent, ReefHeronBehavior::Freeze, 3.0f);   // alert a while after landing
                }
            }
            break;
        }
        case ReefHeronBehavior::Rest:
            neckTarget = 0.0f;
            break;
        }
        state.neckStretch += (neckTarget - state.neckStretch) * ease;
        state.rest += ((state.behavior == ReefHeronBehavior::Rest ? 1.0f : 0.0f) - state.rest) * ease * 0.5f;
        if (state.behavior != ReefHeronBehavior::Strike) state.strike = 0.0f;
        if (state.behavior != ReefHeronBehavior::Fly) state.waterDepth = std::max(0.0f, -state.y);
    }
}
