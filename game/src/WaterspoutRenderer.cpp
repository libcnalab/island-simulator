#include "IslandSimulator/WaterspoutRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
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
#include <cmath>
#include <iostream>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int Rings = 40, SprayRings = 8;
        constexpr int Capacity = (Rings + SprayRings) * 12;
    }

    WaterspoutRenderer::WaterspoutRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly))
    {
        staging_.reserve(Capacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: in a storm a waterspout now and then reaches down from the cloud base offshore, stands a few minutes and ropes out";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    WaterspoutRenderer::~WaterspoutRenderer() = default;

    void WaterspoutRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                  const WaterspoutState& spout, const float seconds)
    {
        drawCalls_ = 0;
        if (!spout.active || !(spout.strength > 0.05f)) return;
        const Vector3 base(spout.x, 0.0f, spout.z);
        const float distance = (base - camera).Length();
        if (distance > 1150.0f) return;
        const float p = spout.progress;
        // forming: the funnel reaches down from the cloud over the first tenth of its life;
        // roping out: over the last fifth it thins to a rope and bends further
        const float reach = std::clamp(p / 0.10f, 0.0f, 1.0f);
        const float rope = std::clamp((p - 0.80f) / 0.20f, 0.0f, 1.0f);
        const float fadeOut = 1.0f - std::clamp((p - 0.95f) / 0.05f, 0.0f, 1.0f);
        const Vector3 lean(std::cos(spout.x * 0.01f) * 0.08f, 0.0f, std::sin(spout.z * 0.01f) * 0.08f);
        const Vector3 bendAxis = Vector3::Normalize(Vector3(-std::sin(spout.x * 0.013f), 0.0f, std::cos(spout.x * 0.013f)));
        const auto axisAt = [&](const float s) {   // s 0 at the sea .. 1 at the cloud base
            const float wiggle = std::sin(s * 7.0f + seconds * 0.35f) * 3.0f;
            const float bend = std::sin(s * 3.14159265f) * (18.0f + 55.0f * rope) + wiggle;
            return base + Vector3(0.0f, Waterspout::CloudBase * s, 0.0f) + lean * (Waterspout::CloudBase * s) + bendAxis * bend;
        };
        const auto radiusAt = [&](const float s) {
            const float full = 3.2f + 24.0f * std::pow(s, 2.4f);
            return full * (1.0f - 0.65f * rope * (1.0f - s)) * (0.6f + 0.4f * spout.strength);
        };
        const float fogAmount = std::clamp((distance - 150.0f) / 900.0f, 0.0f, 1.0f) * 0.6f;
        const Vector3 fog(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue);
        const auto colour = [&](const Vector3& albedo, const float alpha) {
            const Vector3 c = albedo * (1.0f - fogAmount) + fog * fogAmount;
            const float a = std::clamp(alpha * spout.strength * fadeOut, 0.0f, 1.0f);
            const auto q = [a](float v) { return static_cast<int>(std::lround(std::clamp(v, 0.0f, 1.0f) * a * 255.0f)); };
            return Color(q(c.X), q(c.Y), q(c.Z), static_cast<int>(std::lround(a * 255.0f)));
        };
        staging_.clear();
        // The funnel: a ribbon facing the eye, each ring an edge-centre-edge strip, denser at
        // the edges, with slow bands turning up it; only the part reached down so far.
        const float lowest = 1.0f - reach;
        for (int r = 0; r < Rings; ++r) {
            const float s0 = lowest + (1.0f - lowest) * static_cast<float>(r) / Rings;
            const float s1 = lowest + (1.0f - lowest) * static_cast<float>(r + 1) / Rings;
            const Vector3 a0 = axisAt(s0), a1 = axisAt(s1);
            Vector3 tangent = a1 - a0;
            if (tangent.LengthSquared() < 1e-6f) continue;
            tangent.Normalize();
            Vector3 side = Vector3::Cross(tangent, camera - a0);
            if (side.LengthSquared() < 1e-6f) continue;
            side.Normalize();
            const float band0 = 0.85f + 0.15f * std::sin(s0 * 46.0f - seconds * 2.6f);
            const float band1 = 0.85f + 0.15f * std::sin(s1 * 46.0f - seconds * 2.6f);
            // the funnel's tip, where it is still reaching down, thins away
            const float tip0 = std::clamp((s0 - lowest) / 0.06f, 0.0f, 1.0f), tip1 = std::clamp((s1 - lowest) / 0.06f, 0.0f, 1.0f);
            const Vector3 grey(0.70f, 0.72f, 0.74f);
            const Color edge0 = colour(grey * band0, 0.72f * tip0), mid0 = colour(grey * band0 * 1.05f, 0.42f * tip0);
            const Color edge1 = colour(grey * band1, 0.72f * tip1), mid1 = colour(grey * band1 * 1.05f, 0.42f * tip1);
            const Vector3 l0 = a0 - side * radiusAt(s0), r0 = a0 + side * radiusAt(s0);
            const Vector3 l1 = a1 - side * radiusAt(s1), r1 = a1 + side * radiusAt(s1);
            staging_.emplace_back(l0, edge0); staging_.emplace_back(a0, mid0); staging_.emplace_back(a1, mid1);
            staging_.emplace_back(l0, edge0); staging_.emplace_back(a1, mid1); staging_.emplace_back(l1, edge1);
            staging_.emplace_back(a0, mid0); staging_.emplace_back(r0, edge0); staging_.emplace_back(r1, edge1);
            staging_.emplace_back(a0, mid0); staging_.emplace_back(r1, edge1); staging_.emplace_back(a1, mid1);
        }
        // The spray bush where it meets the sea, once it has touched down: wide and low, pale,
        // its top thinning away.
        if (reach >= 1.0f) {
            const float bush = 22.0f * (0.7f + 0.3f * spout.strength) * (1.0f - 0.5f * rope);
            for (int r = 0; r < SprayRings; ++r) {
                const float h0 = 16.0f * static_cast<float>(r) / SprayRings, h1 = 16.0f * static_cast<float>(r + 1) / SprayRings;
                const float w0 = bush * (1.0f - 0.45f * h0 / 16.0f), w1 = bush * (1.0f - 0.45f * h1 / 16.0f);
                const Vector3 a0 = base + Vector3(0.0f, h0, 0.0f), a1 = base + Vector3(0.0f, h1, 0.0f);
                Vector3 side = Vector3::Cross(Vector3::Up, camera - a0);
                if (side.LengthSquared() < 1e-6f) continue;
                side.Normalize();
                const Vector3 white(0.86f, 0.88f, 0.89f);
                const float al0 = 0.38f * (1.0f - h0 / 16.0f), al1 = 0.38f * (1.0f - h1 / 16.0f);
                const Color e0 = colour(white, al0 * 0.4f), m0 = colour(white, al0), e1 = colour(white, al1 * 0.4f), m1 = colour(white, al1);
                staging_.emplace_back(a0 - side * w0, e0); staging_.emplace_back(a0, m0); staging_.emplace_back(a1, m1);
                staging_.emplace_back(a0 - side * w0, e0); staging_.emplace_back(a1, m1); staging_.emplace_back(a1 - side * w1, e1);
                staging_.emplace_back(a0, m0); staging_.emplace_back(a0 + side * w0, e0); staging_.emplace_back(a1 + side * w1, e1);
                staging_.emplace_back(a0, m0); staging_.emplace_back(a1 + side * w1, e1); staging_.emplace_back(a1, m1);
            }
        }
        if (staging_.empty()) return;
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3));
        ++drawCalls_;
    }
}
