#include "IslandSimulator/BoarGroundPose.hpp"
#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace island
{
    namespace
    {
        BoarJoint Cross(BoarJoint a, BoarJoint b) noexcept
        {
            return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
        }
        BoarJoint Unit(BoarJoint a)
        {
            const float length = std::hypot(a.x, a.y, a.z);
            if (!(length > 0.0001f)) throw std::invalid_argument("Degenerate boar ground basis");
            return {a.x/length, a.y/length, a.z/length};
        }
    }

    BoarJoint BoarGroundPose::ToWorld(const BoarJoint p) const noexcept
    {
        return {origin.x + scale * (right.x*p.x + up.x*p.y + forward.x*p.z),
                origin.y + scale * (right.y*p.x + up.y*p.y + forward.y*p.z),
                origin.z + scale * (right.z*p.x + up.z*p.y + forward.z*p.z)};
    }

    BoarGroundPose FitBoarGroundPose(const IslandTerrain& terrain,
                                    const RenderedTerrainSurface& surface, const BoarState& state)
    {
        if (!std::isfinite(state.x) || !std::isfinite(state.z)
            || !std::isfinite(state.headingRadians) || !std::isfinite(state.gaitPhase)
            || !std::isfinite(state.speed) || state.speed < 0.0f
            || !std::isfinite(state.scale) || state.scale <= 0.0f || state.scale > 2.0f
            || !std::isfinite(state.lieAmount)
            || std::abs(state.x) > TerrainMeshLayout::Extent - 4.0f
            || std::abs(state.z) > TerrainMeshLayout::Extent - 4.0f) {
            throw std::invalid_argument("Invalid boar rendered-terrain contact state");
        }
        BoarGroundPose result{};
        result.origin = {state.x, surface.Sample(state.x, state.z).height, state.z};
        const auto normal = terrain.Sample(state.x, state.z).normal;
        result.up = {normal.x, normal.y, normal.z};
        const BoarJoint horizontal{std::cos(state.headingRadians), 0, std::sin(state.headingRadians)};
        result.right = Unit(Cross(result.up, horizontal));
        result.forward = Unit(Cross(result.right, result.up));
        result.scale = state.scale;
        const float movement = std::min(1.0f, state.speed / 0.65f);
        const float phase = std::fmod(state.gaitPhase, 2.0f * std::numbers::pi_v<float>);
        result.bodyBob = std::abs(std::sin(phase * 2.0f)) * 0.018f * movement;

        for (std::size_t index = 0; index < result.legs.size(); ++index) {
            const bool front = index % 2 == 1, left = index < 2;
            const auto flat = EvaluateBoarLeg(state.gaitPhase, movement, result.bodyBob, front, left);
            const auto residual = [&](float localHeight) {
                // The sole centre is 0.025 model metres ahead of the ankle in both LOD meshes.
                const auto sole = result.ToWorld({flat.hoof.x, localHeight, flat.hoof.z + 0.025f});
                return sole.y - surface.Sample(sole.x, sole.z).height;
            };
            float low = -0.5f, high = 0.5f;
            const bool bracketed = residual(low) <= 0.0f && residual(high) >= 0.0f;
            // Solve along body-up, not world-up: the tilted basis moves X/Z with local Y.
            // Fixed iterations avoid tolerance-dependent platform divergence or unbounded work.
            for (int iteration = 0; iteration < 18; ++iteration) {
                const float middle = (low + high) * 0.5f;
                if (residual(middle) < 0.0f) low = middle;
                else high = middle;
            }
            const float ground = (low + high) * 0.5f;
            result.groundHeights[index] = ground;
            result.legs[index] = EvaluateBoarLeg(state.gaitPhase, movement, result.bodyBob,
                                                  front, left, ground);
            result.legs[index].reachLimited |= !bracketed;
        }
        // Lying: the body settles onto the ground and the legs fold under it, the fore
        // cannons lying back along the ground from knees set forward, the hind legs tucked
        // with the hooves brought forward under the belly; the standing chain and the folded
        // one blend by the lie amount.
        const float lie = std::clamp(state.lieAmount, 0.0f, 1.0f);
        if (lie > 0.0f) {
            const float drop = lie * 0.24f * state.scale;
            result.origin.y -= drop;
            result.bodyBob *= 1.0f - lie;
            const auto mix = [lie](const BoarJoint& a, const BoarJoint& b) {
                return BoarJoint{std::lerp(a.x, b.x, lie), std::lerp(a.y, b.y, lie), std::lerp(a.z, b.z, lie)};
            };
            for (std::size_t index = 0; index < result.legs.size(); ++index) {
                const bool front = index % 2 == 1;
                BoarLegPose& leg = result.legs[index];
                const BoarJoint root = leg.root;
                const float hoofX = root.x * 1.15f, hoofZ = front ? root.z - 0.18f : root.z + 0.36f;
                // The ground under the folded hoof's own spot, solved along body-up as the
                // standing hooves are, with the body already lowered.
                const auto residual = [&](float localHeight) {
                    const auto sole = result.ToWorld({hoofX, localHeight, hoofZ});
                    return sole.y - surface.Sample(sole.x, sole.z).height;
                };
                float low = -0.5f, high = 1.0f;
                for (int iteration = 0; iteration < 18; ++iteration) {
                    const float middle = (low + high) * 0.5f;
                    if (residual(middle) < 0.0f) low = middle;
                    else high = middle;
                }
                const float ground = (low + high) * 0.5f;
                result.groundHeights[index] = std::lerp(result.groundHeights[index], ground, lie);
                const BoarJoint knee{root.x * 1.08f, ground + 0.11f, root.z + (front ? 0.14f : 0.10f)};
                const BoarJoint hoof{hoofX, ground + BoarLegPose::HoofHeight, hoofZ};
                leg.knee = mix(leg.knee, knee);
                leg.hoof = mix(leg.hoof, hoof);
                leg.stance = true;
            }
        }
        return result;
    }
}
