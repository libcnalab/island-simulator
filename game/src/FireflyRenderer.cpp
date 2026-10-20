#include "IslandSimulator/FireflyRenderer.hpp"
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
        constexpr int Segments = 8;
        constexpr int VerticesPerFly = Segments * 3 * 2;   // a halo disc and a core disc
    }

    FireflyRenderer::FireflyRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          VerticesPerFly * static_cast<int>(FireflySwarm::Count), BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(VerticesPerFly) * FireflySwarm::Count);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: fireflies blink low over the stream's banks and the wetland on dry nights, part of each cluster flashing in step";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    FireflyRenderer::~FireflyRenderer() = default;

    void FireflyRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                               const std::span<const Firefly> flies)
    {
        visible_ = 0;
        staging_.clear();
        const auto disc = [&](const Vector3& centre, const Vector3& right, const Vector3& up, const float radius, const Color& middle) {
            const Color edge(0, 0, 0, 0);
            for (int i = 0; i < Segments; ++i) {
                const float a0 = 6.2831853f * static_cast<float>(i) / Segments, a1 = 6.2831853f * static_cast<float>(i + 1) / Segments;
                staging_.emplace_back(centre, middle);
                staging_.emplace_back(centre + (right * std::cos(a0) + up * std::sin(a0)) * radius, edge);
                staging_.emplace_back(centre + (right * std::cos(a1) + up * std::sin(a1)) * radius, edge);
            }
        };
        for (const Firefly& fly : flies) {
            if (!(fly.glow > 0.03f) || !std::isfinite(fly.x)) continue;
            const Vector3 at(fly.x, fly.y, fly.z);
            Vector3 toCamera = camera - at;
            const float distance = toCamera.Length();
            if (distance > DrawDistance || distance < 0.2f) continue;
            toCamera /= distance;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            const float fade = fly.glow * (1.0f - std::clamp((distance - 45.0f) / (DrawDistance - 45.0f), 0.0f, 1.0f));
            const auto c = [fade](float v) { return static_cast<int>(std::lround(std::clamp(v * fade, 0.0f, 1.0f) * 255.0f)); };
            // a point stays at least a few pixels across far off: its light, not its size
            const float core = std::max(0.03f, distance * 0.0016f);
            disc(at, right, up, core * 8.0f, Color(c(0.30f), c(0.42f), c(0.10f), 255));
            disc(at + toCamera * 0.01f, right, up, core, Color(c(0.92f), c(1.0f), c(0.55f), 255));
            ++visible_;
        }
        if (staging_.empty()) return;
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3));
    }
}
