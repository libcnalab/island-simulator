#include "IslandSimulator/WhaleRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
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
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int Rings = 14, Segments = 12;
        constexpr int Capacity = Rings * Segments * 6 + 2 * 3 * 12;
    }

    WhaleRenderer::WhaleRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(Capacity));
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        const char* message = "island-simulator: humpback whales pass far offshore, blowing, rolling their backs through the swell and now and then raising their flukes";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    WhaleRenderer::~WhaleRenderer() = default;

    void WhaleRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const WhaleSurfacing& whale)
    {
        drawCalls_ = 0;
        if (!whale.active) return;
        const Vector3 base(whale.x, 0.0f, whale.z);
        const float distance = (base - camera).Length();
        if (distance > 900.0f) return;
        const float p = whale.progress;
        // the back rolls up through the surface and under again: the arc's crest passes from
        // the head toward the tail as the whale goes on down
        const float roll = std::sin(std::numbers::pi_v<float> * std::clamp((p - 0.08f) / 0.80f, 0.0f, 1.0f));
        const Vector3 forward(std::cos(whale.headingRadians), 0.0f, std::sin(whale.headingRadians));
        const Vector3 side(-forward.Z, 0.0f, forward.X);
        const auto toSunState = sun_light::Current().shading;
        const Vector3 toSun = Vector3::Normalize(Vector3(toSunState.x, toSunState.y, toSunState.z));
        // the sea air hazes a whale hundreds of metres off toward the horizon's colour
        const float fogAmount = std::clamp((distance - 120.0f) / 620.0f, 0.0f, 1.0f) * 0.85f;
        const Vector3 fog(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue);
        const auto shade = [&](const Vector3& normal, const Vector3& albedo) {
            const float light = 0.42f + 0.58f * std::max(0.0f, Vector3::Dot(normal, toSun));
            const Vector3 c = albedo * light * (1.0f - fogAmount) + fog * fogAmount;
            const auto b = [](float x) { return static_cast<int>(std::lround(std::clamp(x, 0.0f, 1.0f) * 255.0f)); };
            return Color(b(c.X), b(c.Y), b(c.Z), 255);
        };
        const Vector3 skin(0.20f, 0.23f, 0.26f), belly(0.40f, 0.42f, 0.44f);
        staging_.clear();
        // The back: a long half-ellipsoid, 12 m nose to tail and 3 m across, its top risen
        // above the swell by the roll (at most 1.1 m), the crest sliding back as it rolls.
        constexpr float Length = 12.0f, HalfWidth = 1.55f, Height = 1.9f;
        const float crestShift = (p - 0.5f) * 5.0f;
        const float rise = -1.4f + 2.5f * roll;
        const auto surfacePoint = [&](float u, float a) {   // u along -1 tail .. 1 head, a round the back 0..pi
            const float along = u * Length * 0.5f;
            const float girth = std::sqrt(std::max(0.0f, 1.0f - u * u)) * (u < 0.0f ? 1.0f + 0.35f * u : 1.0f);
            const float x = std::cos(a) * HalfWidth * girth;
            const float y = std::sin(a) * Height * girth + rise - 0.06f * (along + crestShift) * (along + crestShift) / 6.0f;
            return base + forward * along + side * x + Vector3(0.0f, y, 0.0f);
        };
        for (int r = 0; r < Rings; ++r) {
            const float u0 = -1.0f + 2.0f * static_cast<float>(r) / Rings, u1 = -1.0f + 2.0f * static_cast<float>(r + 1) / Rings;
            for (int s = 0; s < Segments; ++s) {
                const float a0 = std::numbers::pi_v<float> * static_cast<float>(s) / Segments;
                const float a1 = std::numbers::pi_v<float> * static_cast<float>(s + 1) / Segments;
                const Vector3 v00 = surfacePoint(u0, a0), v01 = surfacePoint(u0, a1), v10 = surfacePoint(u1, a0), v11 = surfacePoint(u1, a1);
                Vector3 n = Vector3::Cross(v10 - v00, v01 - v00);
                if (n.LengthSquared() < 1e-8f) continue;
                n.Normalize();
                if (n.Y < 0.0f) n = -n;
                const Vector3 albedo = (a0 < 0.4f || a0 > 2.7f) ? belly : skin;
                const Color c = shade(n, albedo);
                staging_.emplace_back(v00, c); staging_.emplace_back(v10, c); staging_.emplace_back(v11, c);
                staging_.emplace_back(v00, c); staging_.emplace_back(v11, c); staging_.emplace_back(v01, c);
            }
        }
        // The small dorsal fin two-thirds of the way back.
        {
            const Vector3 root = surfacePoint(-0.38f, std::numbers::pi_v<float> * 0.5f);
            const Vector3 tip = root + Vector3(0.0f, 0.45f, 0.0f) - forward * 0.5f;
            const Color c = shade(Vector3::Up, skin);
            staging_.emplace_back(root + forward * 0.45f, c); staging_.emplace_back(root - forward * 0.35f, c); staging_.emplace_back(tip, c);
        }
        // The flukes as it sounds: raised clear of the water at the end of the roll, the
        // pale undersides toward the eye, then slipping under.
        if (whale.flukes && p > 0.72f) {
            const float lift = std::sin(std::numbers::pi_v<float> * std::clamp((p - 0.72f) / 0.28f, 0.0f, 1.0f));
            const Vector3 stock = base - forward * (Length * 0.5f - 1.0f) + Vector3(0.0f, -0.6f + 3.4f * lift, 0.0f);
            const Vector3 notch = stock + Vector3(0.0f, 0.9f, 0.0f);
            const Vector3 leftTip = notch + side * 2.1f + Vector3(0.0f, 0.55f, 0.0f) - forward * 0.3f;
            const Vector3 rightTip = notch - side * 2.1f + Vector3(0.0f, 0.55f, 0.0f) - forward * 0.3f;
            const Color dark = shade(-forward, skin), pale = shade(forward, Vector3(0.62f, 0.62f, 0.60f));
            const Vector3 trailingLeft = notch + side * 1.0f + Vector3(0.0f, 0.15f, 0.0f);
            const Vector3 trailingRight = notch - side * 1.0f + Vector3(0.0f, 0.15f, 0.0f);
            for (const Color& c : {dark, pale}) {
                const float offset = &c == &dark ? 0.02f : -0.02f;
                const Vector3 o = forward * offset;
                staging_.emplace_back(stock + o, c); staging_.emplace_back(leftTip + o, c); staging_.emplace_back(trailingLeft + o, c);
                staging_.emplace_back(stock + o, c); staging_.emplace_back(trailingLeft + o, c); staging_.emplace_back(notch + o, c);
                staging_.emplace_back(stock + o, c); staging_.emplace_back(rightTip + o, c); staging_.emplace_back(trailingRight + o, c);
                staging_.emplace_back(stock + o, c); staging_.emplace_back(trailingRight + o, c); staging_.emplace_back(notch + o, c);
            }
            // the tail stock down into the water
            const Color c = shade(side, skin);
            const Vector3 under = stock - forward * 0.0f + Vector3(0.0f, -2.5f, 0.0f) + forward * 1.2f;
            staging_.emplace_back(stock + side * 0.35f, c); staging_.emplace_back(stock - side * 0.35f, c); staging_.emplace_back(under, c);
        }
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
}
