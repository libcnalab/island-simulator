#include "IslandSimulator/MineBatColony.hpp"
#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        constexpr float Pi = std::numbers::pi_v<float>;
        constexpr float TwoPi = 2.0f * Pi;
        // the three clusters on the dome (bearing round the chamber's oval) and the four bats'
        // places in each
        constexpr std::array<float, 3> ClusterBearings{{0.6f, 2.2f, 3.6f}};
        constexpr float ClusterReach = 0.38f;
        constexpr std::array<std::array<float, 2>, 4> ClusterPlaces{{
            {{-0.16f, -0.14f}}, {{0.15f, -0.12f}}, {{-0.13f, 0.15f}}, {{0.17f, 0.13f}}}};
        // the bats' feet this far under the dome's rock (its roughness reaches a few centimetres)
        constexpr float RoofGap = 0.06f;

        float Hash(std::uint32_t v) noexcept
        {
            v ^= v >> 16U; v *= 0x7feb352dU; v ^= v >> 15U; v *= 0x846ca68bU; v ^= v >> 16U;
            return static_cast<float>(v >> 8U) * (1.0f / 16777216.0f);
        }

        float Distance(const MinePoint& a, const MinePoint& b) noexcept
        {
            return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
        }
    }

    float MineBatColony::DomeUp(const float across, const float depth) noexcept
    {
        const float a = across / MineSite::ChamberHalfWidth;
        const float d = (depth - MineSite::ChamberCentre) / MineSite::ChamberHalfLength;
        const float inside = 1.0f - a * a - d * d;
        return inside <= 0.0f ? 0.0f : MineSite::ChamberHeight * std::sqrt(inside);
    }

    MineBatColony::MineBatColony(const IslandTerrain& terrain, const MineLayout& layout)
        : terrain_(terrain), layout_(layout)
    {
        const auto world = [&](const float across, const float up, const float depth) {
            return layout_.World(across, MineLayout::FloorRelative(depth) + up, depth);
        };
        // the clearing in front of the portal the bats hunt over, a few metres up
        {
            const MinePoint ground = layout_.World(0.0f, 0.0f, -14.0f);
            huntCentre_ = {ground.x, terrain_.Height(ground.x, ground.z) + 4.0f, ground.z};
        }
        const MinePoint outside = layout_.World(0.0f, 0.0f, -6.0f);
        const float outsideUp = terrain_.Height(outside.x, outside.z) + 2.8f - layout_.Entrance().y;
        agents_.resize(Count);
        states_.resize(Count);
        for (std::size_t i = 0; i < Count; ++i) {
            Agent& agent = agents_[i];
            const float bearing = ClusterBearings[i / 4U];
            const auto& place = ClusterPlaces[i % 4U];
            const float across = MineSite::ChamberHalfWidth * std::cos(bearing) * ClusterReach + place[0];
            const float depth = MineSite::ChamberCentre + MineSite::ChamberHalfLength * std::sin(bearing) * ClusterReach + place[1];
            agent.roost = world(across, DomeUp(across, depth) - RoofGap, depth);
            agent.route = {{world(across, DomeUp(across, depth) - 0.45f, depth),
                            world(0.0f, 2.2f, MineSite::ChamberCentre),
                            world(0.0f, 1.65f, MineSite::AditLength - 1.0f),
                            world(0.0f, 1.65f, 0.0f),
                            layout_.World(0.0f, outsideUp, -6.0f)}};
            agent.along[0] = 0.0f;
            for (std::size_t k = 1; k < agent.route.size(); ++k)
                agent.along[k] = agent.along[k - 1] + Distance(agent.route[k - 1], agent.route[k]);
            const auto seed = static_cast<std::uint32_t>(i) * 2654435761U + 0x51edU;
            agent.orbitRadius = 5.0f + 6.0f * Hash(seed);
            agent.orbitSpeed = (5.0f + 2.0f * Hash(seed ^ 0x77U)) / agent.orbitRadius * (i % 2U == 0U ? 1.0f : -1.0f);
            agent.orbitPhase = TwoPi * Hash(seed ^ 0x99U);
            agent.flapPhase = TwoPi * Hash(seed ^ 0x13U);
            agent.leaveAt = static_cast<float>(i) * LeaveSpacingSeconds;
            FruitBatState& state = states_[i];
            state.scale = Scale * (0.92f + 0.16f * Hash(seed ^ 0x5U));
            state.colorVariant = 3U;
            state.headingRadians = TwoPi * Hash(seed ^ 0x21U);
            state.simulationTier = WildlifeSimulationTier::Full;
            Place(agent, i, 0.0f);
        }
    }

    MinePoint MineBatColony::Roost(const std::size_t index) const noexcept
    {
        return index < agents_.size() ? agents_[index].roost : MinePoint{};
    }

    void MineBatColony::SetNightness(const float nightness) noexcept
    {
        nightness_ = std::isfinite(nightness) ? std::clamp(nightness, 0.0f, 1.0f) : 0.0f;
    }

    MinePoint MineBatColony::RoutePoint(const Agent& agent, const float s) const noexcept
    {
        const float clamped = std::clamp(s, 0.0f, agent.along.back());
        for (std::size_t k = 1; k < agent.route.size(); ++k) {
            if (clamped <= agent.along[k] || k + 1 == agent.route.size()) {
                const float span = std::max(1e-4f, agent.along[k] - agent.along[k - 1]);
                const float t = std::clamp((clamped - agent.along[k - 1]) / span, 0.0f, 1.0f);
                const MinePoint& a = agent.route[k - 1];
                const MinePoint& b = agent.route[k];
                return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
            }
        }
        return agent.route.back();
    }

    void MineBatColony::Update(const float deltaSeconds, const float observerX, const float observerY,
                               const float observerZ) noexcept
    {
        if (!std::isfinite(deltaSeconds) || deltaSeconds < 0.0f) return;
        const float dt = std::min(deltaSeconds, 0.1f);
        elapsed_ += dt;
        const bool dark = nightness_ >= LeaveNightness;
        if (!started_) {
            // the island loads at night: the bats are out already
            started_ = true;
            if (dark) {
                darkSince_ = elapsed_ - 1000.0f;
                for (Agent& agent : agents_) {agent.phase = Phase::Hunt; agent.snap = true;}
            }
        }
        if (dark && darkSince_ < 0.0f) darkSince_ = elapsed_;
        const bool dawn = nightness_ < ReturnNightness;
        if (dawn) darkSince_ = -1.0f;
        const MinePoint observer{observerX, observerY, observerZ};
        const bool observerFinite = std::isfinite(observerX) && std::isfinite(observerY) && std::isfinite(observerZ);
        for (std::size_t i = 0; i < agents_.size(); ++i) {
            Agent& agent = agents_[i];
            agent.clock += dt;
            // a walker under a cluster sets it flying; the bats keep flying while it stays
            const bool disturbed = observerFinite && observer.y < agent.roost.y + 0.5f
                && Distance(observer, agent.roost) < FlushDistance;
            switch (agent.phase) {
            case Phase::Hang:
                if (darkSince_ >= 0.0f && elapsed_ - darkSince_ >= agent.leaveAt) {
                    agent.phase = Phase::Out;
                    agent.routeS = 0.0f;
                } else if (disturbed) {
                    agent.phase = Phase::Flutter;
                    agent.flutterSeconds = 8.0f + 3.0f * Hash(static_cast<std::uint32_t>(i) * 31U + flushes_);
                    ++flushes_;
                }
                break;
            case Phase::Flutter:
                if (disturbed) agent.flutterSeconds = std::max(agent.flutterSeconds, 6.0f);
                agent.flutterSeconds -= dt;
                // at dusk a flying bat settles first and leaves from its place with the others
                if (darkSince_ >= 0.0f) agent.flutterSeconds = std::min(agent.flutterSeconds, 0.0f);
                break;
            case Phase::Out:
                if (dawn) {agent.phase = Phase::In; break;}
                agent.routeS += AditSpeed * dt;
                if (agent.routeS >= agent.along.back()) {
                    agent.phase = Phase::Hunt;
                    const MinePoint end = agent.route.back();
                    agent.orbitPhase = std::atan2(end.z - huntCentre_.z, end.x - huntCentre_.x);
                }
                break;
            case Phase::Hunt:
                if (dawn) agent.phase = Phase::Home;
                break;
            case Phase::Home:
                break;
            case Phase::In:
                if (darkSince_ >= 0.0f) {agent.phase = Phase::Out; break;}
                agent.routeS -= AditSpeed * dt;
                if (agent.routeS <= 0.0f) agent.phase = Phase::Hang;
                break;
            }
            Place(agent, i, dt);
        }
    }

    void MineBatColony::Place(Agent& agent, const std::size_t index, const float dt) noexcept
    {
        FruitBatState& state = states_[index];
        const MinePoint before{state.x, state.y, state.z};
        MinePoint at{};
        bool flying = true;
        const float weave = agent.clock * 1.9f + static_cast<float>(index) * 1.3f;
        switch (agent.phase) {
        case Phase::Hang:
            at = agent.roost;
            flying = false;
            break;
        case Phase::Flutter: {
            if (agent.flutterSeconds > 0.0f) {
                // round and round under the dome, rising and dipping
                agent.orbitPhase += dt * 2.3f * (index % 2U == 0U ? 1.0f : -1.0f);
                const float across = 1.9f * std::cos(agent.orbitPhase) * (0.85f + 0.15f * std::sin(weave));
                const float depth = MineSite::ChamberCentre + 2.8f * std::sin(agent.orbitPhase);
                at = layout_.World(across, MineLayout::FloorRelative(depth)
                    + std::min(2.3f + 0.35f * std::sin(weave * 1.4f), DomeUp(across, depth) - 0.4f), depth);
            } else {
                // settling back: straight to its place under the rock
                const float gap = Distance(before, agent.roost);
                const float step = std::min(gap, 3.0f * dt);
                at = gap > 1e-4f ? MinePoint{before.x + (agent.roost.x - before.x) * step / gap,
                                             before.y + (agent.roost.y - before.y) * step / gap,
                                             before.z + (agent.roost.z - before.z) * step / gap}
                                 : agent.roost;
                if (gap <= 0.08f) {agent.phase = Phase::Hang; at = agent.roost; flying = false;}
            }
            break;
        }
        case Phase::Out:
        case Phase::In: {
            at = RoutePoint(agent, agent.routeS);
            // a little weave along the adit, never near its walls or its roof
            const MinePoint local = layout_.Local(at);
            const float lateral = local.z > 0.5f && local.z < MineSite::AditLength ? 0.35f : 0.15f;
            at = layout_.World(local.x + lateral * std::sin(weave), local.y + 0.15f * std::sin(weave * 1.7f), local.z);
            break;
        }
        case Phase::Hunt: {
            agent.orbitPhase += agent.orbitSpeed * dt;
            const float radius = agent.orbitRadius * (0.8f + 0.2f * std::sin(weave * 0.7f));
            const float x = huntCentre_.x + std::cos(agent.orbitPhase) * radius;
            const float z = huntCentre_.z + std::sin(agent.orbitPhase) * radius;
            const MinePoint target{x, std::max(terrain_.Height(x, z) + 2.2f, huntCentre_.y + 1.3f * std::sin(weave * 1.1f)), z};
            // chasing its round, so the turn from the portal onto it has no jump
            const float follow = agent.snap ? 1.0f : std::min(1.0f, dt * 4.0f);
            at = {before.x + (target.x - before.x) * follow, before.y + (target.y - before.y) * follow,
                  before.z + (target.z - before.z) * follow};
            agent.snap = false;
            break;
        }
        case Phase::Home: {
            const MinePoint end = agent.route.back();
            const float gap = Distance(before, end);
            const float step = std::min(gap, 6.0f * dt);
            at = gap > 1e-4f ? MinePoint{before.x + (end.x - before.x) * step / gap,
                                         before.y + (end.y - before.y) * step / gap,
                                         before.z + (end.z - before.z) * step / gap}
                             : end;
            if (gap <= 0.6f) {agent.phase = Phase::In; agent.routeS = agent.along.back();}
            break;
        }
        }
        if (!flying) {
            state.x = at.x;
            state.y = at.y - 0.38f * state.scale;
            state.z = at.z;
        } else {
            state.x = at.x;
            state.y = at.y;
            state.z = at.z;
        }
        const float dx = state.x - before.x, dz = state.z - before.z;
        if (flying && dx * dx + dz * dz > 1e-6f) state.headingRadians = std::atan2(dz, dx);
        state.speed = dt > 0.0f ? std::sqrt(dx * dx + dz * dz) / dt : 0.0f;
        state.hangAmount = flying ? std::max(0.0f, state.hangAmount - dt * 4.0f) : std::min(1.0f, state.hangAmount + dt * 3.0f);
        if (dt <= 0.0f) state.hangAmount = flying ? 0.0f : 1.0f;
        agent.flapPhase = std::fmod(agent.flapPhase + dt * 2.0f * Pi * 9.0f, TwoPi);
        state.wingPose = flying ? std::sin(agent.flapPhase) : 0.0f;
        state.behavior = flying ? FruitBatBehavior::DayFlight : FruitBatBehavior::Roost;
        // only as bright as the mine makes it: daylight at the portal, little in the adit
        // between its lanterns, the crystals' glow in the chamber
        const float depth = layout_.Local({state.x, state.y, state.z}).z;
        state.light = depth <= 0.0f ? 1.0f
            : depth < MineSite::AditLength ? 1.0f - 0.62f * std::clamp(depth / 8.0f, 0.0f, 1.0f)
            : 0.6f;
    }

    std::size_t MineBatColony::HangingCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(agents_.begin(), agents_.end(),
            [](const Agent& agent) { return agent.phase == Phase::Hang; }));
    }

    std::size_t MineBatColony::FlutteringCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(agents_.begin(), agents_.end(),
            [](const Agent& agent) { return agent.phase == Phase::Flutter; }));
    }

    std::size_t MineBatColony::HuntingCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(agents_.begin(), agents_.end(),
            [](const Agent& agent) { return agent.phase == Phase::Hunt; }));
    }

    std::size_t MineBatColony::CommutingCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(agents_.begin(), agents_.end(),
            [](const Agent& agent) { return agent.phase == Phase::Out || agent.phase == Phase::In || agent.phase == Phase::Home; }));
    }
}
