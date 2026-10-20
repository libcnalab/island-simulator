#include "IslandSimulator/MistBankRenderer.hpp"
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

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int Segments = 14;
        // a centre, an inner ring at half the radius and the rim: a long soft falloff
        constexpr int VerticesPerPuff = Segments * 3 * 3;
    }

    MistBankRenderer::MistBankRenderer(GraphicsDevice& device, const std::size_t capacity)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          VerticesPerPuff * static_cast<int>(std::max<std::size_t>(capacity, 1)),
                                                          BufferUsage::WriteOnly)),
          capacity_(std::max<std::size_t>(capacity, 1))
    {
        staging_.reserve(static_cast<std::size_t>(VerticesPerPuff) * capacity_);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: morning mist lies in banks over the stream's valley, the wetland and the coastal flats at first light and burns off as the sun climbs";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    MistBankRenderer::~MistBankRenderer() = default;

    void MistBankRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                const std::span<const MistPuff> puffs)
    {
        visible_ = 0;
        staging_.clear();
        std::array<float, Segments + 1> cosines{}, sines{};
        for (int s = 0; s <= Segments; ++s) {
            cosines[static_cast<std::size_t>(s)] = std::cos(6.2831853f * static_cast<float>(s) / Segments);
            sines[static_cast<std::size_t>(s)] = std::sin(6.2831853f * static_cast<float>(s) / Segments);
        }
        for (const MistPuff& puff : puffs) {
            if (static_cast<std::size_t>(visible_) >= capacity_) break;
            if (!puff.active || puff.density <= 0.01f || !std::isfinite(puff.x)) continue;
            const Vector3 centre(puff.x, puff.y, puff.z);
            Vector3 toCamera = camera - centre;
            const float distance = toCamera.Length();
            if (distance > DrawDistance || distance < 0.5f) continue;
            toCamera /= distance;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            // faint up close (a walker inside the bank sees through it), fullest at a distance
            const float near = std::clamp((distance - puff.size * 0.6f) / (puff.size * 2.5f), 0.15f, 1.0f);
            const float far = 1.0f - std::clamp((distance - DrawDistance * 0.7f) / (DrawDistance * 0.3f), 0.0f, 1.0f);
            const int alpha = static_cast<int>(std::lround(125.0f * std::clamp(puff.density, 0.0f, 1.0f) * near * far));
            if (alpha < 2) continue;
            const Color core(222 * alpha / 255, 228 * alpha / 255, 232 * alpha / 255, alpha);
            const int mid = alpha * 11 / 20;
            const Color inner(222 * mid / 255, 228 * mid / 255, 232 * mid / 255, mid), rim(0, 0, 0, 0);
            // wider than tall: mist lies in banks
            const auto at = [&](int s, float r) {
                return centre + right * (cosines[static_cast<std::size_t>(s)] * puff.size * r)
                    + up * (sines[static_cast<std::size_t>(s)] * puff.size * r * 0.45f);
            };
            for (int s = 0; s < Segments; ++s) {
                staging_.emplace_back(centre, core);
                staging_.emplace_back(at(s, 0.5f), inner);
                staging_.emplace_back(at(s + 1, 0.5f), inner);
                staging_.emplace_back(at(s, 0.5f), inner);
                staging_.emplace_back(at(s, 1.0f), rim);
                staging_.emplace_back(at(s + 1, 1.0f), rim);
                staging_.emplace_back(at(s, 0.5f), inner);
                staging_.emplace_back(at(s + 1, 1.0f), rim);
                staging_.emplace_back(at(s + 1, 0.5f), inner);
            }
            ++visible_;
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
