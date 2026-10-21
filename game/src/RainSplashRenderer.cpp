#include "IslandSimulator/RainSplashRenderer.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RainRings.hpp"
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
#include <cstdint>
#include <iostream>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int Span = static_cast<int>(RainSplashRenderer::Radius / RainSplashRenderer::CellSize) + 1;
        // at most about a third of the cells splash at once in a downpour; the buffer holds that
        constexpr int MaximumSplashes = (2 * Span + 1) * (2 * Span + 1) / 3;
        constexpr int VerticesPerSplash = RainSplashRenderer::Droplets * 6;
        constexpr float Gravity = 9.81f;
    }

    RainSplashRenderer::RainSplashRenderer(GraphicsDevice& device, const IslandTerrain& terrain)
        : device_(device), terrain_(terrain), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumSplashes * VerticesPerSplash, BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(MaximumSplashes) * VerticesPerSplash);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: heavier rain splashes on the land round the walker, each drop throwing a small crown of droplets";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    RainSplashRenderer::~RainSplashRenderer() = default;

    void RainSplashRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float rain,
                                  const float seconds)
    {
        visible_ = 0;
        // a light rain's drops vanish into the ground; a heavier one's splash
        const float heavy = std::clamp((rain - 0.30f) / 0.50f, 0.0f, 1.0f);
        if (!(heavy > 0.0f) || !std::isfinite(seconds) || !std::isfinite(camera.X)) return;
        staging_.clear();
        const double time = std::fmod(static_cast<double>(seconds), 3600.0);
        const int cx = static_cast<int>(std::floor(camera.X / CellSize)), cz = static_cast<int>(std::floor(camera.Z / CellSize));
        for (int gz = cz - Span; gz <= cz + Span; ++gz) {
            for (int gx = cx - Span; gx <= cx + Span; ++gx) {
                if (visible_ >= MaximumSplashes) break;
                const float period = 0.45f + 0.60f * rain_rings::Hash(gx, gz, 0x51U);
                const double cycle = time / period + rain_rings::Hash(gx, gz, 0x52U);
                const double whole = std::floor(cycle);
                const float age = static_cast<float>(cycle - whole) * period;
                if (age >= Life) continue;
                const auto strike = static_cast<std::uint32_t>(static_cast<std::int64_t>(whole) & 0xffffffff);
                if (rain_rings::Hash(gx, gz, 0x53U + strike) > heavy) continue;
                const float x = (static_cast<float>(gx) + rain_rings::Hash(gx, gz, 0x3000U + strike)) * CellSize;
                const float z = (static_cast<float>(gz) + rain_rings::Hash(gx, gz, 0x4000U + strike)) * CellSize;
                const float ground = terrain_.Height(x, z);
                if (ground < 0.05f) continue;   // the sea and the swash take rings instead
                const Vector3 at(x, ground + 0.01f, z);
                const Vector3 toCamera = camera - at;
                const float distance = toCamera.Length();
                if (distance > Radius || distance < 0.3f) continue;
                const float k = age / Life;
                const float fade = (1.0f - k) * (1.0f - std::clamp((distance - Radius * 0.6f) / (Radius * 0.4f), 0.0f, 1.0f));
                const float alpha = std::clamp(0.55f * fade, 0.0f, 1.0f);
                if (alpha < 0.02f) continue;
                const auto c = [alpha](const float v) { return static_cast<int>(std::lround(v * alpha * 255.0f)); };
                const Color head(c(0.84f), c(0.87f), c(0.90f), static_cast<int>(std::lround(alpha * 255.0f)));
                const Color tail(c(0.60f), c(0.63f), c(0.66f), static_cast<int>(std::lround(alpha * 0.3f * 255.0f)));
                const float width = 0.003f + distance * 0.0011f;
                const float spin = 6.2831853f * rain_rings::Hash(gx, gz, 0x5000U + strike);
                for (int d = 0; d < Droplets; ++d) {
                    // each droplet flies out and up from the strike and falls back
                    const float angle = spin + 6.2831853f * static_cast<float>(d) / Droplets;
                    const float out = 0.55f + 0.35f * rain_rings::Hash(gx, gz, 0x6000U + strike * 4U + static_cast<std::uint32_t>(d));
                    const float up = 0.80f + 0.30f * rain_rings::Hash(gx, gz, 0x7000U + strike * 4U + static_cast<std::uint32_t>(d));
                    const auto position = [&](const float t) {
                        return at + Vector3(std::cos(angle) * out * t, std::max(0.0f, up * t - 0.5f * Gravity * t * t), std::sin(angle) * out * t);
                    };
                    const Vector3 tip = position(age), back = position(std::max(0.0f, age - 0.035f));
                    Vector3 along = tip - back;
                    if (along.LengthSquared() < 1e-8f) continue;
                    Vector3 side = Vector3::Cross(along, toCamera);
                    if (side.LengthSquared() < 1e-10f) continue;
                    side.Normalize();
                    side *= width;
                    staging_.emplace_back(back - side, tail); staging_.emplace_back(back + side, tail);
                    staging_.emplace_back(tip + side, head);
                    staging_.emplace_back(back - side, tail); staging_.emplace_back(tip + side, head);
                    staging_.emplace_back(tip - side, head);
                }
                ++visible_;
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
    }
}
