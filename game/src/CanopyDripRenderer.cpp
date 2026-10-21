#include "IslandSimulator/CanopyDripRenderer.hpp"
#include "IslandSimulator/CanopyLightField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
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
        constexpr int Span = static_cast<int>(CanopyDripRenderer::Radius / CanopyDripRenderer::CellSize) + 1;
        constexpr int SplashSegments = 6;
        // at most about a quarter of the cells have a drop out at once
        constexpr int MaximumDrops = (2 * Span + 1) * (2 * Span + 1) / 4;
        constexpr int VerticesPerDrop = SplashSegments * 6;
        constexpr float Gravity = 9.81f, SplashSeconds = 0.18f;
        float Unit(const int x, const int z, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(x) * 0x8da6b343U ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ salt * 0xcb1ab31fU;
            h ^= h >> 13; h *= 0x5bd1e995U; h ^= h >> 15;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
    }

    CanopyDripRenderer::CanopyDripRenderer(GraphicsDevice& device, const IslandTerrain& terrain, const CanopyLightField& light)
        : device_(device), terrain_(terrain), light_(light), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumDrops * VerticesPerDrop, BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(MaximumDrops) * VerticesPerDrop);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: the forest goes on dripping after the rain, big drops falling from the crowns to splash on the floor";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    CanopyDripRenderer::~CanopyDripRenderer() = default;

    void CanopyDripRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float dripping,
                                  const float seconds)
    {
        visible_ = 0;
        const float wet = std::isfinite(dripping) ? std::clamp(dripping, 0.0f, 1.0f) : 0.0f;
        if (wet < 0.02f || !std::isfinite(seconds) || !std::isfinite(camera.X)) return;
        staging_.clear();
        const double time = std::fmod(static_cast<double>(seconds), 3600.0);
        const int cx = static_cast<int>(std::floor(camera.X / CellSize)), cz = static_cast<int>(std::floor(camera.Z / CellSize));
        for (int gz = cz - Span; gz <= cz + Span; ++gz) {
            for (int gx = cx - Span; gx <= cx + Span; ++gx) {
                if (visible_ >= MaximumDrops) break;
                if (Unit(gx, gz, 1U) > 0.60f) continue;   // a leaf tip here or not
                const float x = (static_cast<float>(gx) + Unit(gx, gz, 2U)) * CellSize;
                const float z = (static_cast<float>(gz) + Unit(gx, gz, 3U)) * CellSize;
                const float range = std::hypot(x - camera.X, z - camera.Z);
                if (range > Radius) continue;
                // only under the crowns: where the canopy shades the floor
                const float under = std::clamp((0.97f - light_.Sample(x, z)) / 0.25f, 0.0f, 1.0f);
                if (under <= 0.0f) continue;
                const float ground = terrain_.Height(x, z);
                if (ground < 0.2f) continue;
                const float drop = 3.0f + 6.0f * Unit(gx, gz, 4U);   // the leaf's height over the floor
                const float fall = std::sqrt(2.0f * drop / Gravity);
                // a leaf gathers its drop and lets it go: wetter leaves drip more often
                const float gap = (0.8f + 3.0f * Unit(gx, gz, 5U)) / (0.35f + 0.65f * wet);
                const float cycle = fall + SplashSeconds + gap;
                const double phase = time / cycle + Unit(gx, gz, 6U);
                const auto strike = static_cast<std::uint32_t>(static_cast<std::int64_t>(std::floor(phase)) & 0xffffffff);
                if (Unit(gx, gz, 7U + strike) > wet * under) continue;
                const float t = static_cast<float>(phase - std::floor(phase)) * cycle;
                if (t > fall + SplashSeconds) continue;
                const Vector3 base(x, ground, z);
                const Vector3 toCamera = camera - base;
                const float distance = toCamera.Length();
                if (distance < 0.3f) continue;
                const float fade = 1.0f - std::clamp((range - Radius * 0.6f) / (Radius * 0.4f), 0.0f, 1.0f);
                if (t < fall) {
                    // the drop falling, streaked by its speed
                    const float y = ground + drop - 0.5f * Gravity * t * t;
                    const float streak = std::clamp(Gravity * t * 0.03f, 0.04f, 0.32f);
                    const Vector3 bottom(x, y, z), top(x, std::min(ground + drop, y + streak), z);
                    Vector3 side = Vector3::Cross(Vector3::Up, camera - bottom);
                    if (side.LengthSquared() < 1e-6f) continue;
                    side.Normalize();
                    side *= 0.004f + (camera - bottom).Length() * 0.0011f;
                    const float a = 0.55f * fade;
                    const auto c = [a](float v) { return static_cast<int>(std::lround(v * a * 255.0f)); };
                    const Color head(c(0.80f), c(0.84f), c(0.86f), static_cast<int>(std::lround(a * 255.0f)));
                    const Color tail(c(0.45f), c(0.48f), c(0.50f), static_cast<int>(std::lround(a * 0.35f * 255.0f)));
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(top + side, tail);
                    staging_.emplace_back(bottom + side, head);
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(bottom + side, head);
                    staging_.emplace_back(bottom - side, head);
                } else {
                    // the splash: a small ring thrown out on the floor, thinning as it spreads
                    const float k = (t - fall) / SplashSeconds;
                    const float outer = 0.03f + 0.08f * k, inner = outer * (0.55f + 0.3f * k);
                    const float a = 0.45f * fade * (1.0f - k);
                    const auto c = [a](float v) { return static_cast<int>(std::lround(v * a * 255.0f)); };
                    const Color ring(c(0.80f), c(0.84f), c(0.86f), static_cast<int>(std::lround(a * 255.0f)));
                    const Vector3 o(x, ground + 0.02f + 0.02f * k, z);
                    for (int s = 0; s < SplashSegments; ++s) {
                        const float a0 = 6.2831853f * static_cast<float>(s) / SplashSegments, a1 = a0 + 6.2831853f / SplashSegments;
                        const Vector3 p0(std::cos(a0), 0.0f, std::sin(a0)), p1(std::cos(a1), 0.0f, std::sin(a1));
                        staging_.emplace_back(o + p0 * inner, ring); staging_.emplace_back(o + p0 * outer, ring);
                        staging_.emplace_back(o + p1 * outer, ring);
                        staging_.emplace_back(o + p0 * inner, ring); staging_.emplace_back(o + p1 * outer, ring);
                        staging_.emplace_back(o + p1 * inner, ring);
                    }
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
