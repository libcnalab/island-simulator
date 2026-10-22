#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/CampfireSite.hpp"
#include "IslandSimulator/CanopyLightField.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"

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
                // The clearing home stands off the campfire with room to graze round it.
                && std::hypot(agent.home.x - CampfireSite::X, agent.home.z - CampfireSite::Z) > FireClearance + 3.0f
                && !obstacles_.IntersectsCircle(agent.home.x, agent.home.z, .72f);
        }
        if (!found) throw std::runtime_error("No safe open inland deer habitat");
        agent.state.x = agent.home.x;
        agent.state.z = agent.home.z;
        agent.state.y = StandHeight(agent.state.x, agent.state.z);
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

    float DeerHerd::StandHeight(const float x, const float z) const noexcept
    {
        return ground_ ? DrawnOrAnalyticHeight(*ground_, terrain_, x, z) : terrain_.Height(x, z);
    }

    void DeerHerd::SetGround(const RenderedTerrainSurface* ground) noexcept
    {
        ground_ = ground;
        for (Agent& agent : agents_) agent.state.y = StandHeight(agent.state.x, agent.state.z);
        for (std::size_t index = 0; index < Count; ++index) states_[index] = agents_[index].state;
    }

    float DeerHerd::LightAt(const float x, const float z) const noexcept
    {
        if (light_ == nullptr) return 1.0f;
        const float ground = terrain_.Height(x, z);
        return light_->SampleReceiver(x, ground + 0.6f, z, ground);
    }

    bool DeerHerd::FindShade(const Agent& agent, const std::size_t index, float& x, float& z) const noexcept
    {
        // Twelve bearings at two reaches; the shadiest valid spot inside the home's inner
        // margin (so the home pull does not override the walk) wins when it is shaded.
        float best = ShadedLevel;
        bool found = false;
        for (int ring = 0; ring < 2; ++ring) {
            const float reach = ring == 0 ? ShadeSeekRadius * 0.5f : ShadeSeekRadius;
            for (int bearing = 0; bearing < 12; ++bearing) {
                const float angle = static_cast<float>(bearing) * (TwoPi / 12.0f);
                const float px = agent.state.x + std::cos(angle) * reach;
                const float pz = agent.state.z + std::sin(angle) * reach;
                if (!IsValidPosition(index, px, pz)) continue;
                if (std::hypot(px - agent.home.x, pz - agent.home.z) > agent.home.radius * .6f) continue;
                const float level = LightAt(px, pz);
                if (level < best) { best = level; x = px; z = pz; found = true; }
            }
        }
        return found;
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
            && std::hypot(x - CampfireSite::X, z - CampfireSite::Z) > FireClearance
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
        const DeerState& sentinel = agents_[0].state;
        const bool sentinelSees = observerValid && std::hypot(sentinel.x - observerX,
            sentinel.z - observerZ) / observerThreat_ < SentinelAlertDistance;
        sentinelAlertSeconds_ = sentinelSees ? sentinelAlertSeconds_ + safeDelta : 0.0f;
        disturbanceSeconds_ = std::max(0.0f, disturbanceSeconds_ - safeDelta);
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

    std::size_t DeerHerd::TossingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.tossSeconds > 0.0f ? 1U : 0U;
        return count;
    }

    std::size_t DeerHerd::ChewingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.cud > 0.5f ? 1U : 0U;
        return count;
    }

    std::size_t DeerHerd::StampingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.stamping ? 1U : 0U;
        return count;
    }

    std::size_t DeerHerd::TwitchingCount() const noexcept
    {
        std::size_t count = 0;
        for (const Agent& agent : agents_) count += agent.twitching ? 1U : 0U;
        return count;
    }

    void DeerHerd::PreviewBed(const std::size_t index) noexcept
    {
        if (index >= Count) return;
        Agent& agent = agents_[index];
        agent.state.behavior = DeerBehavior::Rest;
        agent.state.speed = 0.0f;
        agent.state.grazeAmount = 0.0f;
        agent.state.bedAmount = 1.0f;
        agent.bedded = true;
        agent.seekingShade = false;
        agent.behaviorSeconds = 30.0f;
        states_[index] = agent.state;
    }

    void DeerHerd::NoteDisturbance(const float x, const float z) noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z)) return;
        disturbanceX_ = x;
        disturbanceZ_ = z;
        disturbanceSeconds_ = StartleSeconds;
    }

    void DeerHerd::UpdateAgent(Agent& agent, const float deltaSeconds,
                               const float observerX, const float observerZ,
                               const bool observerValid) noexcept
    {
        if (deltaSeconds <= 0.0f) return;
        const std::size_t index = static_cast<std::size_t>(&agent - agents_.data());
        // measured in the walker's terms: a louder, faster observer counts as nearer
        const float observerDistance = observerValid
            ? std::hypot(agent.state.x - observerX, agent.state.z - observerZ) / observerThreat_
            : std::numeric_limits<float>::infinity();
        bool alertFacing = false;
        if (observerDistance < 9.0f) {
            if (agent.state.behavior != DeerBehavior::Retreat) { ++bolts_; lastBolt_ = index; }
            agent.state.behavior = DeerBehavior::Retreat;
            agent.behaviorSeconds = 1.8f;
            // Bound away about seventy degrees off the direct line of escape: the deer keeps
            // the player in view over its shoulder instead of turning its rump to them.
            agent.targetHeadingRadians = Wrap(std::atan2(
                agent.state.z - observerZ, agent.state.x - observerX)
                + (index % 2U == 0U ? 1.22f : -1.22f));
        } else if (observerDistance < (index == 0U ? SentinelAlertDistance
            : index == 1U && sentinelAlertSeconds_ >= HerdReactionSeconds
                ? SentinelAlertDistance : AlertDistance)) {
            // Stand alert and face the approaching player; grazing resumes once they leave.
            // The antlered sentinel lifts its head first; its partner follows after the delay.
            agent.state.behavior = DeerBehavior::Rest;
            agent.behaviorSeconds = std::max(agent.behaviorSeconds, 0.5f);
            agent.targetHeadingRadians = std::atan2(
                observerZ - agent.state.z, observerX - agent.state.x);
            alertFacing = true;
        } else if (disturbanceSeconds_ > 0.0f && std::hypot(agent.state.x - disturbanceX_,
            agent.state.z - disturbanceZ_) < StartleDistance) {
            // A thud nearby: the deer stops and stands facing where it came from until the
            // startle fades, then grazing goes on.
            agent.state.behavior = DeerBehavior::Rest;
            agent.behaviorSeconds = std::max(agent.behaviorSeconds, 0.5f);
            agent.targetHeadingRadians = std::atan2(
                disturbanceZ_ - agent.state.z, disturbanceX_ - agent.state.x);
            alertFacing = true;
        } else if ((agent.behaviorSeconds -= deltaSeconds) <= 0.0f && nightness_ > 0.6f) {
            // After dark: bed down where it stands until morning.
            ++agent.transitions;
            agent.seekingShade = false;
            agent.state.behavior = DeerBehavior::Rest;
            agent.behaviorSeconds = 25.0f + static_cast<float>((static_cast<std::uint32_t>(index) * 7U + agent.transitions) % 10U);
            agent.bedded = true;
        } else if (agent.behaviorSeconds <= 0.0f) {
            ++agent.transitions;
            agent.seekingShade = false;
            const auto phase = (agent.transitions + static_cast<std::uint32_t>(index) * 3U) % 7U;
            agent.state.behavior = phase < 3U ? DeerBehavior::Graze
                : phase < 5U ? DeerBehavior::Walk : DeerBehavior::Rest;
            agent.behaviorSeconds = 3.5f + static_cast<float>((agent.transitions * 7U
                + static_cast<std::uint32_t>(index) * 5U) % 45U) * .12f;
            const float turn = -.85f + static_cast<float>((agent.transitions * 37U
                + static_cast<std::uint32_t>(index) * 19U) % 171U) * .01f;
            agent.targetHeadingRadians = Wrap(agent.state.headingRadians + turn);
            // The East pair keeps together: a straggler's next walk heads toward its partner.
            if (index < 2U && agent.state.behavior == DeerBehavior::Walk) {
                const DeerState& partner = agents_[1U - index].state;
                if (std::hypot(partner.x - agent.state.x, partner.z - agent.state.z) > CohesionDistance)
                    agent.targetHeadingRadians = std::atan2(partner.z - agent.state.z,
                                                            partner.x - agent.state.x);
            }
            // Every other rest of a deer with a reachable bank becomes a walk to the water.
            if (agent.hasDrink && phase == 5U && (agent.transitions / 7U) % 2U == 0U) {
                agent.state.behavior = DeerBehavior::Drink;
                agent.behaviorSeconds = 22.0f;
            }
            // In a downpour every turn is a rest (taken under cover below).
            const bool downpour = rain_ > RainShelterLevel;
            if (downpour && agent.state.behavior != DeerBehavior::Drink) {
                agent.state.behavior = DeerBehavior::Rest;
                agent.behaviorSeconds = std::max(agent.behaviorSeconds, 12.0f);
                agent.bedded = light_ != nullptr && LightAt(agent.state.x, agent.state.z) <= SunlitLevel;
            }
            // A rest that comes on sunlit ground (or in the open in the rain): the deer walks to
            // the nearest shade first (a crown, a palm head, a snag or a bush within ten metres)
            // and lies up there.
            if (agent.state.behavior == DeerBehavior::Rest && light_ != nullptr
                && LightAt(agent.state.x, agent.state.z) > SunlitLevel
                && FindShade(agent, index, agent.shadeX, agent.shadeZ)) {
                agent.state.behavior = DeerBehavior::Walk;
                agent.seekingShade = true;
                agent.bedded = false;
                agent.behaviorSeconds = 30.0f;
                if (downpour) ++rainShelters_;
                agent.targetHeadingRadians = std::atan2(agent.shadeZ - agent.state.z,
                                                        agent.shadeX - agent.state.x);
            }
        }
        if (agent.seekingShade) {
            if (agent.state.behavior != DeerBehavior::Walk) {
                agent.seekingShade = false;
            } else if (std::hypot(agent.state.x - agent.shadeX, agent.state.z - agent.shadeZ) < 0.8f) {
                // In the shade: the rest the deer set out for, a little longer for the walk.
                agent.seekingShade = false;
                agent.state.behavior = DeerBehavior::Rest;
                agent.behaviorSeconds = 6.0f + static_cast<float>((agent.transitions * 5U
                    + static_cast<std::uint32_t>(index) * 3U) % 30U) * .2f
                    + (rain_ > RainShelterLevel ? 20.0f : 0.0f);   // sitting the rain out
                agent.bedded = true;
                ++shadeRests_;
            } else {
                agent.targetHeadingRadians = std::atan2(agent.shadeZ - agent.state.z,
                                                        agent.shadeX - agent.state.x);
            }
        }
        // Bedded down for the shade rest; anything else (an alarm, a bolt, the next graze)
        // has the deer on its feet within half a second.
        if (agent.state.behavior != DeerBehavior::Rest) agent.bedded = false;
        agent.state.bedAmount += std::clamp((agent.bedded ? 1.0f : 0.0f) - agent.state.bedAmount,
            -RiseRate * deltaSeconds, BedDownRate * deltaSeconds);
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
        // The tail: flagged up at a bolt (the white flag), a flick every few seconds while
        // grazing, resting or drinking, hanging on the walk.
        agent.flickClock += deltaSeconds;
        const float flickPeriod = FlickPeriodSeconds
            + 1.3f * static_cast<float>(static_cast<int>(std::abs(agent.home.x)) % 4);
        if (agent.flickClock >= flickPeriod) agent.flickClock = 0.0f;
        const bool stillness = agent.state.behavior == DeerBehavior::Graze
            || agent.state.behavior == DeerBehavior::Rest || drinking;
        const float targetTail = agent.state.behavior == DeerBehavior::Retreat ? 1.0f
            : stillness && agent.flickClock < FlickSeconds ? 0.55f : 0.0f;
        agent.state.tailRaise += std::clamp(targetTail - agent.state.tailRaise,
            -6.0f * deltaSeconds, 8.0f * deltaSeconds);
        // The ears: every few seconds a deer not bolting swings one ear back and out at a
        // fly and lets it settle, the ears taking turns; a bolting deer holds both forward.
        agent.earClock += deltaSeconds;
        const float earPeriod = EarTwitchPeriodSeconds
            + 0.7f * static_cast<float>(static_cast<int>(std::abs(agent.home.z)) % 3);
        if (agent.earClock >= earPeriod) { agent.earClock = 0.0f; agent.state.earSide = -agent.state.earSide; }
        const bool twitching = agent.earClock < EarTwitchSeconds
            && agent.state.behavior != DeerBehavior::Retreat;
        if (twitching && !agent.twitching) ++earTwitches_;
        agent.twitching = twitching;
        agent.state.earTwitch += std::clamp((twitching ? 1.0f : 0.0f) - agent.state.earTwitch,
            -8.0f * deltaSeconds, 10.0f * deltaSeconds);
        // The stamp: standing alert at the walker (on its feet, not bedded) the deer lifts
        // its left forefoot and stamps it down every few seconds, a warning.
        const bool standingAlert = alertFacing && agent.state.behavior == DeerBehavior::Rest
            && agent.state.bedAmount < 0.2f;
        agent.stampClock = standingAlert ? agent.stampClock + deltaSeconds : 0.0f;
        if (agent.stampClock >= StampPeriodSeconds) agent.stampClock = 0.0f;
        const bool stamping = standingAlert && agent.stampClock >= 1.0f && agent.stampClock < 1.0f + StampSeconds;
        if (stamping && !agent.stamping) ++stamps_;
        agent.stamping = stamping;
        agent.state.stamp += std::clamp((stamping ? 1.0f : 0.0f) - agent.state.stamp,
            -8.0f * deltaSeconds, 10.0f * deltaSeconds);
        // The cud: at rest, undisturbed (standing or bedded), the deer chews, the muzzle
        // working side to side; the chew eases in over a second and stops with the next
        // move or an alert.
        const bool ruminating = agent.state.behavior == DeerBehavior::Rest && !alertFacing
            && agent.state.speed < 0.05f;
        agent.cud += std::clamp((ruminating ? 1.0f : 0.0f) - agent.cud, -2.0f * deltaSeconds, 1.0f * deltaSeconds);
        if (agent.cud > 0.01f) agent.chewPhase = std::fmod(agent.chewPhase + deltaSeconds * ChewRate, TwoPi);
        agent.state.chew = std::sin(agent.chewPhase) * agent.cud;
        // The head toss: as the deer comes up from a graze (its graze amount falling through
        // a half) it throws its head up and forward for a moment and settles it.
        const bool grazingNow = agent.state.grazeAmount > 0.5f;
        if (agent.wasGrazing && !grazingNow) { agent.tossSeconds = TossSeconds; ++tosses_; }
        agent.wasGrazing = grazingNow;
        agent.tossSeconds = std::max(0.0f, agent.tossSeconds - deltaSeconds);
        agent.state.headToss += std::clamp((agent.tossSeconds > 0.0f ? 1.0f : 0.0f) - agent.state.headToss,
            -6.0f * deltaSeconds, 10.0f * deltaSeconds);
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
        agent.state.y = StandHeight(agent.state.x, agent.state.z);
    }
}
