#include "IslandSimulator/SunbeamMoteRenderer.hpp"
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
        constexpr int Span = static_cast<int>(SunbeamMoteRenderer::Radius / SunbeamMoteRenderer::CellSize) + 1;
        constexpr int MaximumMotes = (2 * Span + 1) * (2 * Span + 1);
        constexpr int Segments = 5;
        float Hash(const int x, const int z, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(x) * 0x8da6b343U ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ salt * 0xcb1ab31fU;
            h ^= h >> 13; h *= 0x5bd1e995U; h ^= h >> 15;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
    }

    SunbeamMoteRenderer::SunbeamMoteRenderer(GraphicsDevice& device, const IslandTerrain& terrain, const CanopyLightField& light)
        : device_(device), terrain_(terrain), light_(light), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumMotes * Segments * 3, BufferUsage::WriteOnly))
    {
        staging_.reserve(static_cast<std::size_t>(MaximumMotes) * Segments * 3U);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: dust and pollen drift glinting in the forest's sunbeams round the walker";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    SunbeamMoteRenderer::~SunbeamMoteRenderer() = default;

    void SunbeamMoteRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float seconds,
                                   const float directSun, const Vector3& toSun)
    {
        visible_ = 0;
        if (!(directSun > 0.05f) || !std::isfinite(seconds)) return;
        // Only in the forest: the walker's own neighbourhood must be mostly shaded.
        float around = 0.0f;
        for (int k = 0; k < 6; ++k) {
            const float a = 1.0471976f * static_cast<float>(k);
            around += light_.Sample(camera.X + std::cos(a) * 5.0f, camera.Z + std::sin(a) * 5.0f);
        }
        around /= 6.0f;
        const float forest = std::clamp((0.98f - around) / 0.20f, 0.0f, 1.0f);
        if (forest <= 0.0f) return;
        staging_.clear();
        const int cx = static_cast<int>(std::floor(camera.X / CellSize)), cz = static_cast<int>(std::floor(camera.Z / CellSize));
        for (int gz = cz - Span; gz <= cz + Span; ++gz) {
            for (int gx = cx - Span; gx <= cx + Span; ++gx) {
                if (Hash(gx, gz, 1U) > 0.65f) continue;   // not every cell holds a mote
                const float drift = seconds * (0.10f + 0.08f * Hash(gx, gz, 2U));
                const float x = (static_cast<float>(gx) + Hash(gx, gz, 3U)) * CellSize + 0.35f * std::sin(drift + 6.0f * Hash(gx, gz, 4U));
                const float z = (static_cast<float>(gz) + Hash(gx, gz, 5U)) * CellSize + 0.35f * std::cos(drift * 0.8f + 6.0f * Hash(gx, gz, 6U));
                const float ground = terrain_.Height(x, z);
                if (ground < 0.3f) continue;
                const float y = ground + 0.3f + 2.2f * Hash(gx, gz, 7U) + 0.25f * std::sin(drift * 1.3f);
                const Vector3 at(x, y, z);
                Vector3 toCamera = camera - at;
                const float distance = toCamera.Length();
                if (distance > Radius || distance < 0.25f) continue;
                // Only where it is seen against the dark of the forest: a glint is lost against
                // the bright sky, but drawn additively there it piled up into a star-like point.
                // Looking down at a mote (it lies below the eye) puts the floor and the trunks
                // behind it.
                const float below = (camera.Y - y) / distance;
                const float againstDark = std::clamp((below - 0.02f) / 0.10f, 0.0f, 1.0f);
                if (againstDark <= 0.0f) continue;
                // in a gap of sunlight: the light field bright here
                const float sunlit = std::clamp((light_.SampleReceiver(x, y, z, ground) - 0.45f) / 0.25f, 0.0f, 1.0f);
                if (sunlit <= 0.0f) continue;
                toCamera /= distance;
                Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
                if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
                right.Normalize();
                const Vector3 up = Vector3::Cross(toCamera, right);
                // a mote glints as it turns
                const float glint = 0.6f + 0.4f * std::sin(seconds * (2.0f + 2.0f * Hash(gx, gz, 8U)) + 9.0f * Hash(gx, gz, 9U));
                const float fade = 1.0f - std::clamp((distance - Radius * 0.6f) / (Radius * 0.4f), 0.0f, 1.0f);
                // dust scatters forwards: brightest looking into the light
                const float into = std::max(0.0f, -Vector3::Dot(toCamera, toSun));
                const float b = std::clamp(0.9f * sunlit * glint * fade * forest * directSun * againstDark * (0.6f + 0.8f * into * into), 0.0f, 1.0f);
                if (b < 0.02f) continue;
                const auto c = [b](float v) { return static_cast<int>(std::lround(std::clamp(v * b, 0.0f, 1.0f) * 255.0f)); };
                const Color core(c(1.0f), c(0.95f), c(0.80f), 255), rim(0, 0, 0, 0);
                const float size = std::max(0.010f, distance * 0.0030f);
                for (int s = 0; s < Segments; ++s) {
                    const float a0 = 6.2831853f * static_cast<float>(s) / Segments, a1 = 6.2831853f * static_cast<float>(s + 1) / Segments;
                    staging_.emplace_back(at, core);
                    staging_.emplace_back(at + (right * std::cos(a0) + up * std::sin(a0)) * size, rim);
                    staging_.emplace_back(at + (right * std::cos(a1) + up * std::sin(a1)) * size, rim);
                }
                ++visible_;
            }
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
