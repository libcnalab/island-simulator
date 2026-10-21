#include "IslandSimulator/EaveDripRenderer.hpp"
#include "IslandSimulator/HutThatch.hpp"
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
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float Gravity = 9.81f, SplashSeconds = 0.14f, DrawDistance = 45.0f;
        constexpr int SplashSegments = 6;
        constexpr int VerticesPerDrop = 6 + SplashSegments * 6;
        float Unit(const int tip, const int drop, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(tip) * 0x9e3779b1U ^ static_cast<std::uint32_t>(drop) * 0x85ebca77U ^ salt * 0xc2b2ae3dU;
            h ^= h >> 15; h *= 0x2c1b3c6dU; h ^= h >> 12;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
    }

    EaveDripRenderer::EaveDripRenderer(GraphicsDevice& device, const HutPlacement& hut, const IslandTerrain& terrain)
        : device_(device), centre_(hut.x, hut.floorY + 1.5f, hut.z), effect_(std::make_unique<BasicEffect>(device))
    {
        // the drip edge: the tip of every leaf in the thatch's lowest row, on both slopes
        const float c = std::cos(hut.yawRadians), s = std::sin(hut.yawRadians);
        for (const int side : {-1, 1}) {
            for (int strip = 0; strip < 48; ++strip) {
                const auto tip = BuildHutThatchBlade(side, 6, strip).sections[3][1];
                const Vector3 at(hut.x + c * tip.x + s * tip.z, hut.floorY + tip.y, hut.z - s * tip.x + c * tip.z);
                const float ground = terrain.Height(at.X, at.Z);
                if (!(at.Y - ground > 0.3f)) continue;
                tips_.push_back({at, ground, std::sqrt(2.0f * (at.Y - ground) / Gravity)});
            }
        }
        const auto capacity = tips_.size() * DropsPerTip * VerticesPerDrop;
        vertices_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          static_cast<int>(std::max<std::size_t>(capacity, 6U)), BufferUsage::WriteOnly);
        staging_.reserve(capacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const std::string message = "island-simulator: in rain the hut's eaves drip from " + std::to_string(tips_.size())
            + " thatch tips, drops falling to splash in the drip line, near-curtains in a downpour";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message.c_str());
    }

    EaveDripRenderer::~EaveDripRenderer() = default;

    void EaveDripRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float rain,
                                const float seconds)
    {
        visible_ = 0;
        if (!(rain > 0.05f) || !std::isfinite(seconds) || tips_.empty()) return;
        const float range = Vector3::Distance(camera, centre_);
        if (range > DrawDistance) return;
        staging_.clear();
        const float wet = std::clamp(rain, 0.0f, 1.0f);
        const float time = std::fmod(seconds, 3600.0f);
        const float far = 1.0f - std::clamp((range - DrawDistance * 0.7f) / (DrawDistance * 0.3f), 0.0f, 1.0f);
        for (std::size_t i = 0; i < tips_.size(); ++i) {
            const Tip& tip = tips_[i];
            const int index = static_cast<int>(i);
            for (int d = 0; d < DropsPerTip; ++d) {
                // more of a tip's drops run the harder it rains
                if (Unit(index, d, 1U) * 0.85f + 0.15f * static_cast<float>(d) > wet + 0.10f) continue;
                // each drop falls, splashes, then waits for the thatch to gather the next
                const float gap = (0.25f + 0.9f * Unit(index, d, 2U)) * (1.4f - wet);
                const float cycle = tip.fallSeconds + SplashSeconds + gap;
                const float t = std::fmod(time + cycle * Unit(index, d, 3U), cycle);
                const Vector3 toCamera = camera - tip.at;
                const float distance = toCamera.Length();
                if (distance < 0.3f) continue;
                const float alpha = std::clamp((0.30f + 0.25f * wet) * far, 0.0f, 1.0f);
                const auto c = [alpha](const float v) { return static_cast<int>(std::lround(v * alpha * 255.0f)); };
                if (t < tip.fallSeconds) {
                    // a falling drop, streaked by its speed over a frame
                    const float y = tip.at.Y - 0.5f * Gravity * t * t;
                    const float streak = std::clamp(Gravity * t * 0.035f, 0.03f, 0.30f);
                    const Vector3 bottom(tip.at.X, y, tip.at.Z), top(tip.at.X, std::min(tip.at.Y, y + streak), tip.at.Z);
                    Vector3 side = Vector3::Cross(Vector3::Up, toCamera);
                    if (side.LengthSquared() < 1e-6f) continue;
                    side.Normalize();
                    side *= 0.004f + distance * 0.0011f;
                    const Color head(c(0.78f), c(0.82f), c(0.86f), static_cast<int>(std::lround(alpha * 255.0f)));
                    const Color tail(c(0.45f), c(0.48f), c(0.52f), static_cast<int>(std::lround(alpha * 0.35f * 255.0f)));
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(top + side, tail);
                    staging_.emplace_back(bottom + side, head);
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(bottom + side, head);
                    staging_.emplace_back(bottom - side, head);
                    ++visible_;
                } else if (t < tip.fallSeconds + SplashSeconds) {
                    // the splash: a small ring thrown out on the ground, thinning as it spreads
                    const float k = (t - tip.fallSeconds) / SplashSeconds;
                    const float outer = 0.025f + 0.07f * k, inner = outer * (0.55f + 0.3f * k);
                    const float a = alpha * (1.0f - k);
                    const Color ring(static_cast<int>(std::lround(0.80f * a * 255.0f)), static_cast<int>(std::lround(0.84f * a * 255.0f)),
                                     static_cast<int>(std::lround(0.88f * a * 255.0f)), static_cast<int>(std::lround(a * 255.0f)));
                    const Vector3 o(tip.at.X, tip.ground + 0.015f + 0.02f * k, tip.at.Z);
                    for (int sgm = 0; sgm < SplashSegments; ++sgm) {
                        const float a0 = 6.2831853f * static_cast<float>(sgm) / SplashSegments + 0.7f * Unit(index, d, 4U);
                        const float a1 = a0 + 6.2831853f / SplashSegments;
                        const Vector3 p0(std::cos(a0), 0.0f, std::sin(a0)), p1(std::cos(a1), 0.0f, std::sin(a1));
                        staging_.emplace_back(o + p0 * inner, ring); staging_.emplace_back(o + p0 * outer, ring);
                        staging_.emplace_back(o + p1 * outer, ring);
                        staging_.emplace_back(o + p0 * inner, ring); staging_.emplace_back(o + p1 * outer, ring);
                        staging_.emplace_back(o + p1 * inner, ring);
                    }
                    ++visible_;
                }
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
