#include "IslandSimulator/SailboatRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int Capacity = 600, GlowCapacity = 4 * 8 * 3;
        constexpr float DrawDistance = 1100.0f;
        // the hull's stations, stern to bow: along (m), half-beam (m), deck height above the water (m)
        constexpr std::array<std::array<float, 3>, 7> Stations{{
            {-5.5f, 1.25f, 0.92f}, {-3.8f, 1.60f, 0.88f}, {-1.6f, 1.72f, 0.86f}, {0.6f, 1.66f, 0.88f},
            {2.8f, 1.30f, 0.94f}, {4.4f, 0.74f, 1.02f}, {5.5f, 0.0f, 1.10f}}};
        // Rodrigues: v turned by `angle` about the unit `axis`
        Vector3 Turned(const Vector3& v, const Vector3& axis, const float angle) noexcept
        {
            const float c = std::cos(angle), s = std::sin(angle);
            return v * c + Vector3::Cross(axis, v) * s + axis * (Vector3::Dot(axis, v) * (1.0f - c));
        }
    }

    SailboatRenderer::SailboatRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly)),
          glow_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), GlowCapacity,
                                                      BufferUsage::WriteOnly))
    {
        staging_.reserve(Capacity);
        glowStaging_.reserve(GlowCapacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: now and then a yacht sails past well offshore, heeled under main and jib, its navigation lights burning at night";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    SailboatRenderer::~SailboatRenderer() = default;

    void SailboatRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const SailboatState& boat,
                                const float seaHeight, const float wind, const float seconds)
    {
        drawCalls_ = 0;
        drawn_ = false;
        if (!boat.active) return;
        const Vector3 base(boat.x, std::isfinite(seaHeight) ? seaHeight : 0.0f, boat.z);
        const float distance = (base - camera).Length();
        if (distance > DrawDistance) return;
        const Vector3 forward(std::cos(boat.headingRadians), 0.0f, std::sin(boat.headingRadians));
        const Vector3 right(-forward.Z, 0.0f, forward.X);   // starboard
        // The wind: from it the leeward side, the boom's swing (close in sailing towards the wind,
        // eased right out running before it) and the heel (most on a reach, more in a strong wind).
        const Vector3 blowing(wind::DirectionX, 0.0f, wind::DirectionZ);
        const float downwind = Vector3::Dot(forward, blowing);
        const float leeSign = Vector3::Dot(right, blowing) >= 0.0f ? 1.0f : -1.0f;
        const Vector3 lee = right * leeSign;
        const float swing = 0.26f + 0.95f * (0.5f + 0.5f * downwind);
        const float strength = std::clamp(std::isfinite(wind) ? wind : 1.0f, 0.5f, 2.5f);
        const float roll = std::sin(seconds * 0.9f + boat.x * 0.01f) * 0.035f;
        const float heel = leeSign * (0.10f + 0.14f * (1.0f - std::abs(downwind))) * strength + roll;
        // the boat's own frame, heeled about its fore-and-aft line
        const Vector3 up = Turned(Vector3::Up, forward, heel);
        const Vector3 side = Turned(right, forward, heel);
        const auto at = [&](const float along, const float across, const float height) {
            return base + forward * along + side * across + up * height;
        };
        const auto toSunState = sun_light::Current().shading;
        const Vector3 toSun = Vector3::Normalize(Vector3(toSunState.x, toSunState.y, toSunState.z));
        // the sea air hazes the boat toward the horizon's colour
        const float fogAmount = std::clamp((distance - 150.0f) / 700.0f, 0.0f, 1.0f) * 0.85f;
        const Vector3 fog(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue);
        staging_.clear();
        const auto triangle = [&](const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& albedo,
                                  const bool translucent = false) {
            Vector3 n = Vector3::Cross(b - a, c - a);
            if (n.LengthSquared() < 1e-10f) return;
            n.Normalize();
            if (Vector3::Dot(n, camera - a) < 0.0f) n = -n;   // the face the eye sees
            // a sail's cloth lets the sun through: lit from either face
            const float facing = Vector3::Dot(n, toSun);
            const float light = translucent ? 0.62f + 0.38f * std::abs(facing) : 0.45f + 0.55f * std::max(0.0f, facing);
            const Vector3 lit = albedo * light * (1.0f - fogAmount) + fog * fogAmount;
            const auto q = [](float x) { return static_cast<int>(std::lround(std::clamp(x, 0.0f, 1.0f) * 255.0f)); };
            const Color colour(q(lit.X), q(lit.Y), q(lit.Z), 255);
            staging_.emplace_back(a, colour); staging_.emplace_back(b, colour); staging_.emplace_back(c, colour);
        };
        const Vector3 hull(0.90f, 0.90f, 0.88f), stripe(0.10f, 0.16f, 0.30f), deck(0.62f, 0.56f, 0.46f),
            sail(0.93f, 0.91f, 0.86f), spar(0.55f, 0.56f, 0.58f), glass(0.12f, 0.14f, 0.16f);
        // The hull: each side from the deck edge down to below the water (the sea covers it), a
        // dark boot stripe just above the waterline; the deck between; the transom.
        for (std::size_t i = 0; i + 1 < Stations.size(); ++i) {
            const auto& s0 = Stations[i];
            const auto& s1 = Stations[i + 1];
            for (const float sideSign : {-1.0f, 1.0f}) {
                const Vector3 d0 = at(s0[0], sideSign * s0[1], s0[2]), d1 = at(s1[0], sideSign * s1[1], s1[2]);
                const Vector3 w0 = at(s0[0], sideSign * s0[1] * 0.92f, 0.12f), w1 = at(s1[0], sideSign * s1[1] * 0.92f, 0.12f);
                const Vector3 b0 = at(s0[0], sideSign * s0[1] * 0.55f, -0.6f), b1 = at(s1[0], sideSign * s1[1] * 0.55f, -0.6f);
                triangle(d0, d1, w1, hull); triangle(d0, w1, w0, hull);
                triangle(w0, w1, b1, stripe); triangle(w0, b1, b0, stripe);
            }
            triangle(at(s0[0], -s0[1], s0[2]), at(s1[0], -s1[1], s1[2]), at(s1[0], s1[1], s1[2]), deck);
            triangle(at(s0[0], -s0[1], s0[2]), at(s1[0], s1[1], s1[2]), at(s0[0], s0[1], s0[2]), deck);
        }
        {
            const auto& s = Stations.front();
            triangle(at(s[0], -s[1], s[2]), at(s[0], s[1], s[2]), at(s[0], s[1] * 0.55f, -0.6f), hull);
            triangle(at(s[0], -s[1], s[2]), at(s[0], s[1] * 0.55f, -0.6f), at(s[0], -s[1] * 0.55f, -0.6f), hull);
        }
        // The cabin top, its sides with a dark band of windows.
        {
            const float aft = -2.4f, fore = 1.0f, half = 0.95f, low = 0.86f, high = 1.42f;
            const Vector3 a0 = at(aft, -half, high), a1 = at(aft, half, high), f0 = at(fore, -half * 0.8f, high), f1 = at(fore, half * 0.8f, high);
            triangle(a0, f0, f1, hull); triangle(a0, f1, a1, hull);
            for (const float sideSign : {-1.0f, 1.0f}) {
                const float hs = half * sideSign;
                const Vector3 lowA = at(aft, hs, low), lowF = at(fore, hs * 0.8f, low);
                const Vector3 topA = at(aft, hs, high), topF = at(fore, hs * 0.8f, high);
                const Vector3 midA = at(aft, hs, 1.12f), midF = at(fore, hs * 0.8f, 1.12f);
                triangle(lowA, lowF, midF, hull); triangle(lowA, midF, midA, hull);
                triangle(midA, midF, topF, glass); triangle(midA, topF, topA, glass);
            }
            triangle(at(fore, -half * 0.8f, low), at(fore, half * 0.8f, low), at(fore, half * 0.8f, high), hull);
            triangle(at(fore, -half * 0.8f, low), at(fore, half * 0.8f, high), at(fore, -half * 0.8f, high), hull);
        }
        // The mast and the boom (thin flat spars, crossed so they show from any side).
        constexpr float MastAlong = 1.4f, MastHeight = 14.0f, BoomHeight = 2.2f, BoomLength = 4.6f;
        const Vector3 mastFoot = at(MastAlong, 0.0f, 0.9f), mastHead = at(MastAlong, 0.0f, MastHeight);
        for (const Vector3& width : {forward * 0.09f, side * 0.09f}) {
            triangle(mastFoot - width, mastFoot + width, mastHead + width * 0.6f, spar);
            triangle(mastFoot - width, mastHead + width * 0.6f, mastHead - width * 0.6f, spar);
        }
        const Vector3 gooseneck = at(MastAlong, 0.0f, BoomHeight);
        const Vector3 boomOut = Vector3::Normalize(-forward * std::cos(swing) + Turned(lee, forward, heel) * std::sin(swing));
        const Vector3 clew = gooseneck + boomOut * BoomLength;
        triangle(gooseneck - up * 0.07f, gooseneck + up * 0.07f, clew + up * 0.05f, spar);
        triangle(gooseneck - up * 0.07f, clew + up * 0.05f, clew - up * 0.05f, spar);
        // The mainsail, its belly filled out to leeward, and the jib from the masthead's fore
        // side down to the stem, sheeted in to leeward.
        {
            const Vector3 head = at(MastAlong, 0.0f, MastHeight - 0.4f), tack = at(MastAlong, 0.0f, BoomHeight + 0.1f);
            const Vector3 belly = (head + tack + clew) / 3.0f + Turned(lee, forward, heel) * 0.55f;
            triangle(tack, clew, belly, sail, true); triangle(clew, head, belly, sail, true); triangle(head, tack, belly, sail, true);
            const Vector3 jibHead = at(MastAlong, 0.0f, MastHeight - 1.8f), jibTack = at(5.3f, 0.0f, 1.15f);
            const Vector3 jibClew = at(-0.2f, 0.0f, 1.5f) + Turned(lee, forward, heel) * (0.9f + 1.4f * std::sin(swing));
            const Vector3 jibBelly = (jibHead + jibTack + jibClew) / 3.0f + Turned(lee, forward, heel) * 0.35f;
            triangle(jibTack, jibClew, jibBelly, sail, true); triangle(jibClew, jibHead, jibBelly, sail, true); triangle(jibHead, jibTack, jibBelly, sail, true);
        }
        masthead_ = mastHead + up * 0.15f;
        port_ = at(4.6f, -0.72f, 1.05f);
        starboard_ = at(4.6f, 0.72f, 1.05f);
        stern_ = at(-5.55f, 0.0f, 0.95f);
        drawn_ = true;
        if (staging_.empty()) return;
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3));
        ++drawCalls_;
    }

    void SailboatRenderer::DrawLights(const Matrix& view, const Matrix& projection, const Vector3& camera, const float nightness)
    {
        if (!drawn_ || !(nightness > 0.05f)) return;
        glowStaging_.clear();
        const auto light = [&](const Vector3& at, const Vector3& tint) {
            Vector3 toCamera = camera - at;
            const float distance = toCamera.Length();
            if (distance < 1.0f) return;
            toCamera /= distance;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            const float size = std::max(0.25f, distance * 0.0022f);   // never under a couple of pixels
            const float b = std::clamp(nightness, 0.0f, 1.0f);
            const auto q = [b](float v) { return static_cast<int>(std::lround(std::clamp(v * b, 0.0f, 1.0f) * 255.0f)); };
            const Color core(q(tint.X), q(tint.Y), q(tint.Z), 255), rim(0, 0, 0, 0);
            for (int s = 0; s < 8; ++s) {
                const float a0 = 6.2831853f * static_cast<float>(s) / 8.0f, a1 = 6.2831853f * static_cast<float>(s + 1) / 8.0f;
                glowStaging_.emplace_back(at, core);
                glowStaging_.emplace_back(at + (right * std::cos(a0) + up * std::sin(a0)) * size, rim);
                glowStaging_.emplace_back(at + (right * std::cos(a1) + up * std::sin(a1)) * size, rim);
            }
        };
        light(masthead_, Vector3(1.0f, 0.96f, 0.86f));
        light(stern_, Vector3(1.0f, 0.96f, 0.86f));
        light(port_, Vector3(1.0f, 0.12f, 0.08f));
        light(starboard_, Vector3(0.10f, 1.0f, 0.35f));
        if (glowStaging_.empty()) return;
        glow_->SetData(glowStaging_.data(), 0, static_cast<int>(glowStaging_.size()), SetDataOptions::Discard);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        device_.SetVertexBuffer(glow_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(glowStaging_.size() / 3));
        ++drawCalls_;
    }
}
