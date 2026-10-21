#include "IslandSimulator/SeagrassRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/TerrainShaderNoise.hpp"
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
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int MaximumBlades = 12, Segments = 3;
        constexpr int Capacity = 60000;
        float Unit(const int x, const int z, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(x) * 0x8da6b343U ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ salt * 0xcb1ab31fU;
            h ^= h >> 13; h *= 0x5bd1e995U; h ^= h >> 15;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        std::int64_t BucketKey(const int bx, const int bz) noexcept
        {
            return (static_cast<std::int64_t>(bx) << 32) ^ static_cast<std::int64_t>(static_cast<std::uint32_t>(bz));
        }
        using terrain_noise::Smooth;
        using terrain_noise::ValueNoise;
        // How strongly the terrain paints seagrass meadow at (x, z) over a bed `depth` down.
        float MeadowAt(const float x, const float z, const float depth) noexcept
        {
            const float meadowField = ValueNoise(x * 0.038f + 43.7f, z * 0.038f - 21.6f);
            const float medium = ValueNoise(x * 0.31f + 17.3f, z * 0.31f - 9.7f);
            const float meadow = Smooth(-0.16f, 0.17f, meadowField + medium * 0.22f)
                * Smooth(0.45f, 0.85f, depth) * (1.0f - Smooth(1.8f, 2.6f, depth));
            const float broad = ValueNoise(x * 0.060f, z * 0.060f);
            const float length = std::max(std::hypot(x, z), 1.0f);
            const float arcX = x / length * 250.0f, arcZ = z / length * 250.0f;
            const float channel = ValueNoise(arcX * 0.065f + 5.1f + broad * 0.65f, arcZ * 0.065f + 19.3f + broad * 0.65f);
            const float submerged = Smooth(0.25f, 0.65f, depth) * (1.0f - Smooth(5.0f, 8.0f, depth));
            return meadow * submerged * (1.0f - Smooth(0.10f, 0.28f, channel));
        }
    }

    SeagrassRenderer::SeagrassRenderer(GraphicsDevice& device, const IslandTerrain& terrain)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly))
    {
        staging_.reserve(Capacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        // the meadows lie in a ring of lagoon round the island: scan it once
        constexpr float Extent = 330.0f;
        const int cells = static_cast<int>(2.0f * Extent / Spacing);
        for (int gz = 0; gz < cells; ++gz) {
            for (int gx = 0; gx < cells; ++gx) {
                const float x = -Extent + (static_cast<float>(gx) + Unit(gx, gz, 1U)) * Spacing;
                const float z = -Extent + (static_cast<float>(gz) + Unit(gx, gz, 2U)) * Spacing;
                const float radius = std::hypot(x, z);
                if (radius < 150.0f || radius > Extent) continue;
                const float height = terrain.Height(x, z);
                if (height > -0.45f || height < -2.6f) continue;
                const float suits = MeadowAt(x, z, -height);
                if (suits <= 0.08f || Unit(gx, gz, 3U) > suits) continue;
                Clump clump;
                clump.x = x; clump.y = height; clump.z = z;
                clump.size = 0.20f + 0.25f * Unit(gx, gz, 4U) * (0.5f + 0.5f * suits);
                clump.phase = 6.2831853f * Unit(gx, gz, 5U);
                clump.blades = static_cast<std::uint8_t>(7 + static_cast<int>(Unit(gx, gz, 6U) * (MaximumBlades - 7)));
                const auto index = static_cast<std::uint32_t>(clumps_.size());
                clumps_.push_back(clump);
                buckets_[BucketKey(static_cast<int>(std::floor(x / BucketSize)), static_cast<int>(std::floor(z / BucketSize)))].push_back(index);
            }
        }
        const std::string message = "island-simulator: seagrass meadows of " + std::to_string(clumps_.size())
            + " clumps sway with the surge on the lagoon floor";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message.c_str());
    }

    SeagrassRenderer::~SeagrassRenderer() = default;

    void SeagrassRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float seconds,
                                const float seaState)
    {
        visible_ = 0;
        if (clumps_.empty() || !std::isfinite(seconds) || !std::isfinite(camera.X)) return;
        staging_.clear();
        const auto toSunState = sun_light::Current().shading;
        const float sunUp = std::clamp(toSunState.y, 0.0f, 1.0f);
        const float surge = 0.16f * std::clamp(std::isfinite(seaState) ? seaState : 1.0f, 1.0f, 1.8f);
        const int b0x = static_cast<int>(std::floor((camera.X - Radius) / BucketSize)), b1x = static_cast<int>(std::floor((camera.X + Radius) / BucketSize));
        const int b0z = static_cast<int>(std::floor((camera.Z - Radius) / BucketSize)), b1z = static_cast<int>(std::floor((camera.Z + Radius) / BucketSize));
        for (int bz = b0z; bz <= b1z; ++bz) {
            for (int bx = b0x; bx <= b1x; ++bx) {
                const auto found = buckets_.find(BucketKey(bx, bz));
                if (found == buckets_.end()) continue;
                for (const std::uint32_t index : found->second) {
                    const Clump& clump = clumps_[index];
                    const Vector3 base(clump.x, clump.y, clump.z);
                    const Vector3 toCamera = camera - base;
                    const float distance = toCamera.Length();
                    if (distance > Radius || distance < 0.2f) continue;
                    if (staging_.size() + static_cast<std::size_t>(clump.blades) * Segments * 6U > Capacity) break;
                    const float fade = 1.0f - std::clamp((distance - Radius * 0.7f) / (Radius * 0.3f), 0.0f, 1.0f);
                    // deeper, darker; the light from above dims with the hour
                    const float depth = std::clamp(-clump.y, 0.0f, 4.0f);
                    const float lit = (0.55f + 0.45f * sunUp) * (1.0f - 0.12f * depth);
                    Vector3 side = Vector3::Cross(Vector3::Up, toCamera);
                    if (side.LengthSquared() < 1e-6f) continue;
                    side.Normalize();
                    // the surge: the whole clump leans back and forth on the swell's period
                    const float sway = std::sin(seconds * 0.9f + clump.phase + clump.x * 0.05f) * surge;
                    for (int blade = 0; blade < clump.blades; ++blade) {
                        const float a = clump.phase * 3.0f + 2.39996f * static_cast<float>(blade);
                        const float spread = 0.05f + 0.05f * static_cast<float>(blade % 3);
                        const Vector3 root = base + Vector3(std::cos(a) * spread, 0.01f, std::sin(a) * spread);
                        // towards the edge of the radius the blades shrink into the bed (no popping)
                        const float height = clump.size * fade * (0.75f + 0.35f * static_cast<float>((blade * 7) % 5) / 4.0f);
                        const float width = 0.012f + 0.004f * static_cast<float>(blade % 2);
                        const Vector3 lean(std::cos(a) * 0.12f + sway, 0.0f, std::sin(a) * 0.12f + sway * 0.6f);
                        Vector3 previous = root;
                        for (int s = 0; s < Segments; ++s) {
                            const float t0 = static_cast<float>(s) / Segments, t1 = static_cast<float>(s + 1) / Segments;
                            const Vector3 p1 = root + Vector3(0.0f, height * t1, 0.0f) + lean * (height * t1 * t1);
                            const auto shade = [&](float t) {
                                const float g = lit * (0.55f + 0.45f * t);
                                const auto c = [g](float v) { return static_cast<int>(std::lround(std::clamp(v * g, 0.0f, 1.0f) * 255.0f)); };
                                return Color(c(0.22f), c(0.30f), c(0.11f), 255);
                            };
                            const Color c0 = shade(t0), c1 = shade(t1);
                            const float w0 = width * (1.0f - 0.35f * t0), w1 = width * (1.0f - 0.35f * t1);
                            staging_.emplace_back(previous - side * w0, c0); staging_.emplace_back(previous + side * w0, c0);
                            staging_.emplace_back(p1 + side * w1, c1);
                            staging_.emplace_back(previous - side * w0, c0); staging_.emplace_back(p1 + side * w1, c1);
                            staging_.emplace_back(p1 - side * w1, c1);
                            previous = p1;
                        }
                    }
                    ++visible_;
                }
            }
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
    }
}
