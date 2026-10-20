#include "IslandSimulator/RainRingRenderer.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/OceanWaveSpectrum.hpp"
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
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int VerticesPerRing = RainRingRenderer::Segments * 6;
    }

    RainRingRenderer::RainRingRenderer(GraphicsDevice& device, const IslandTerrain& terrain)
        : device_(device), terrain_(terrain), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumRings * VerticesPerRing, BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(MaximumRings) * VerticesPerRing);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        const char* message = "island-simulator: rain on the sea: each drop sends out a ring on the waves round the walker, world-anchored";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    RainRingRenderer::~RainRingRenderer() = default;

    void RainRingRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float rain,
                                const float seconds)
    {
        visible_ = 0;
        if (!(rain > 0.02f) || camera.Y < 0.0f) return;
        staging_.clear();
        const int first = static_cast<int>(std::floor((camera.X - Radius) / rain_rings::CellSize));
        const int last = static_cast<int>(std::floor((camera.X + Radius) / rain_rings::CellSize));
        const int firstZ = static_cast<int>(std::floor((camera.Z - Radius) / rain_rings::CellSize));
        const int lastZ = static_cast<int>(std::floor((camera.Z + Radius) / rain_rings::CellSize));
        std::array<float, Segments + 1> cosines{}, sines{};
        for (int s = 0; s <= Segments; ++s) {
            const float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(s) / Segments;
            cosines[static_cast<std::size_t>(s)] = std::cos(angle);
            sines[static_cast<std::size_t>(s)] = std::sin(angle);
        }
        for (int cz = firstZ; cz <= lastZ && visible_ < MaximumRings; ++cz) {
            for (int cx = first; cx <= last && visible_ < MaximumRings; ++cx) {
                const auto ring = rain_rings::At(cx, cz, seconds, rain);
                if (!ring.active) continue;
                const float distance = std::hypot(ring.x - camera.X, ring.z - camera.Z);
                if (distance > Radius) continue;
                if (terrain_.Height(ring.x, ring.z) > -0.25f) continue;   // only over water, not the swash
                const float t = ring.progress;
                const float radius = rain_rings::MaximumRadius * (0.12f + 0.88f * std::sqrt(t));
                const float width = 0.014f + 0.012f * t;
                const float fade = (1.0f - t) * (1.0f - t) * (1.0f - std::clamp((distance - Radius * 0.6f) / (Radius * 0.4f), 0.0f, 1.0f));
                const int alpha = static_cast<int>(std::lround(95.0f * fade));
                if (alpha < 3) continue;
                // the ocean shader scales the waves by 0.78-1.22 with the shore's exposure: ride
                // on the higher of the two
                const float wave = SampleOceanWaves(ring.x, ring.z, seconds).height;
                const float y = std::max(wave * 0.78f, wave * 1.22f) + 0.025f;
                const Color pale(alpha * 222 / 255, alpha * 230 / 255, alpha * 236 / 255, alpha);
                for (int s = 0; s < Segments; ++s) {
                    const auto c0 = cosines[static_cast<std::size_t>(s)], s0 = sines[static_cast<std::size_t>(s)];
                    const auto c1 = cosines[static_cast<std::size_t>(s + 1)], s1 = sines[static_cast<std::size_t>(s + 1)];
                    const Vector3 i0(ring.x + c0 * (radius - width), y, ring.z + s0 * (radius - width));
                    const Vector3 o0(ring.x + c0 * (radius + width), y, ring.z + s0 * (radius + width));
                    const Vector3 i1(ring.x + c1 * (radius - width), y, ring.z + s1 * (radius - width));
                    const Vector3 o1(ring.x + c1 * (radius + width), y, ring.z + s1 * (radius + width));
                    staging_.emplace_back(i0, pale);
                    staging_.emplace_back(o0, pale);
                    staging_.emplace_back(o1, pale);
                    staging_.emplace_back(i0, pale);
                    staging_.emplace_back(o1, pale);
                    staging_.emplace_back(i1, pale);
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
