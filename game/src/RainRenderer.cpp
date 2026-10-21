#include "IslandSimulator/RainRenderer.hpp"
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
#include <cmath>
#include <cstdint>
#include <iostream>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        [[nodiscard]] std::uint32_t Mix(std::uint32_t v) noexcept
        {
            v ^= v >> 16U; v *= 0x7feb352dU; v ^= v >> 15U; v *= 0x846ca68bU; return v ^ (v >> 16U);
        }
        [[nodiscard]] float Unit(const int x, const int z, const int k, const std::uint32_t salt) noexcept
        {
            const std::uint32_t h = Mix(static_cast<std::uint32_t>(x) * 0x9e3779b9U
                ^ Mix(static_cast<std::uint32_t>(z) * 0x85ebca6bU ^ Mix(static_cast<std::uint32_t>(k) * 0xc2b2ae35U ^ salt)));
            return static_cast<float>(h >> 8U) * (1.0f / 16777216.0f);
        }
    }

    RainRenderer::RainRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumDrops * 6, BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(MaximumDrops) * 6U);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: rain falls in world-anchored streaks around the walker, slanted by the wind and faded at the edges of its volume";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    RainRenderer::~RainRenderer() = default;

    void RainRenderer::SetShelter(const float x, const float z, const float yawRadians, const float halfSpan,
                                  const float halfLength, const float eaveY, const float ridgeY) noexcept
    {
        shelter_ = {x, z, std::cos(yawRadians), std::sin(yawRadians), halfSpan, halfLength, eaveY, ridgeY};
        sheltered_ = std::isfinite(x) && std::isfinite(z) && halfSpan > 0.0f && halfLength > 0.0f;
    }

    void RainRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                            const float rain, const float wind, const float seconds)
    {
        visible_ = 0;
        if (!(rain > 0.01f) || !std::isfinite(camera.X) || !std::isfinite(seconds)) return;
        staging_.clear();
        const float slant = 0.10f + 0.12f * std::clamp(wind, 0.0f, 2.5f);   // horizontal metres per metre of fall
        Vector3 fall(wind::DirectionX * slant, -1.0f, wind::DirectionZ * slant);
        fall.Normalize();
        const int minX = static_cast<int>(std::floor((camera.X - Radius) / CellSize));
        const int maxX = static_cast<int>(std::floor((camera.X + Radius) / CellSize));
        const int minZ = static_cast<int>(std::floor((camera.Z - Radius) / CellSize));
        const int maxZ = static_cast<int>(std::floor((camera.Z + Radius) / CellSize));
        const float density = std::clamp(rain, 0.0f, 1.0f);
        const float time = std::fmod(seconds, 3600.0f);
        for (int cz = minZ; cz <= maxZ; ++cz) {
            for (int cx = minX; cx <= maxX; ++cx) {
                for (int k = 0; k < DropsPerCell; ++k) {
                    if (staging_.size() + 6U > static_cast<std::size_t>(MaximumDrops) * 6U) break;   // never past the buffer
                    if (Unit(cx, cz, k, 0x4a1u) >= density) continue;   // light rain: fewer drops
                    const float x = (static_cast<float>(cx) + Unit(cx, cz, k, 0x4a2u)) * CellSize;
                    const float z = (static_cast<float>(cz) + Unit(cx, cz, k, 0x4a3u)) * CellSize;
                    const float horizontal = std::hypot(x - camera.X, z - camera.Z);
                    if (horizontal > Radius) continue;
                    // A world-anchored fall: the drop's phase in a column of Height metres,
                    // placed in the layer that holds the camera.
                    const float speed = FallSpeed * (0.85f + 0.3f * Unit(cx, cz, k, 0x4a4u));
                    const float phase = std::fmod(Unit(cx, cz, k, 0x4a5u) * Height - time * speed, Height);
                    const float p = phase < 0.0f ? phase + Height : phase;
                    const float layer = std::floor((camera.Y - 0.5f * Height - p) / Height) + 1.0f;
                    const float y = p + layer * Height;
                    const float vertical = std::abs(y - camera.Y) / (0.5f * Height);
                    const float edge = std::clamp((1.0f - horizontal / Radius) * 4.0f, 0.0f, 1.0f)
                        * std::clamp((1.0f - vertical) * 4.0f, 0.0f, 1.0f);
                    if (edge <= 0.01f) continue;
                    if (sheltered_) {
                        // under the roof: in the roof's own frame, below its slope there
                        const float dx = x - shelter_.x, dz = z - shelter_.z;
                        const float across = std::abs(shelter_.cosYaw * dx - shelter_.sinYaw * dz);
                        const float along = std::abs(shelter_.sinYaw * dx + shelter_.cosYaw * dz);
                        if (across < shelter_.halfSpan && along < shelter_.halfLength
                            && y < shelter_.ridgeY - (shelter_.ridgeY - shelter_.eaveY) * across / shelter_.halfSpan) continue;
                    }
                    const Vector3 top(x, y, z);
                    const Vector3 bottom = top + fall * StreakLength;
                    const Vector3 toCamera = camera - top;
                    Vector3 side = Vector3::Cross(fall, toCamera);
                    const float distance = toCamera.Length();
                    if (side.LengthSquared() < 1e-6f || distance < 0.3f) continue;
                    side.Normalize();
                    side *= 0.006f + distance * 0.0011f;   // about a pixel wide at any distance
                    const float alpha = std::clamp(0.40f * edge * (0.6f + 0.4f * density), 0.0f, 1.0f);
                    const auto c = [alpha](const float v) { return static_cast<int>(std::lround(v * alpha * 255.0f)); };
                    const Color colour(c(0.74f), c(0.78f), c(0.83f), static_cast<int>(std::lround(alpha * 255.0f)));
                    const Color tail(c(0.40f), c(0.43f), c(0.47f), static_cast<int>(std::lround(alpha * 0.4f * 255.0f)));
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(top + side, tail);
                    staging_.emplace_back(bottom + side, colour);
                    staging_.emplace_back(top - side, tail); staging_.emplace_back(bottom + side, colour);
                    staging_.emplace_back(bottom - side, colour);
                    ++visible_;
                }
            }
        }
        if (staging_.empty()) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3));
    }
}
