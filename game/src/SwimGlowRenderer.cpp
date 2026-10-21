#include "IslandSimulator/SwimGlowRenderer.hpp"
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
        constexpr int Segments = 6;
        constexpr int Capacity = static_cast<int>(SwimGlowField::Capacity) * Segments * 3;
    }

    SwimGlowRenderer::SwimGlowRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly))
    {
        staging_.reserve(Capacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: on dark nights the plankton round a swimmer flashes blue with every stroke";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    SwimGlowRenderer::~SwimGlowRenderer() = default;

    void SwimGlowRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                const std::span<const GlowSpeck> specks, const float strength)
    {
        visible_ = 0;
        if (!(strength > 0.05f)) return;
        staging_.clear();
        for (const GlowSpeck& speck : specks) {
            const float b = std::min(1.0f, 1.4f * speck.Brightness() * std::clamp(strength, 0.0f, 1.0f));
            if (b < 0.02f) continue;
            const Vector3 at(speck.x, speck.y, speck.z);
            Vector3 toCamera = camera - at;
            const float distance = toCamera.Length();
            if (distance < 0.05f || distance > 60.0f) continue;
            toCamera /= distance;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            const float size = std::max(0.03f, distance * 0.0040f);
            const auto c = [b](float v) { return static_cast<int>(std::lround(std::clamp(v * b, 0.0f, 1.0f) * 255.0f)); };
            const Color core(c(0.25f), c(0.75f), c(1.0f), 255), rim(0, 0, 0, 0);
            for (int s = 0; s < Segments; ++s) {
                const float a0 = 6.2831853f * static_cast<float>(s) / Segments, a1 = 6.2831853f * static_cast<float>(s + 1) / Segments;
                staging_.emplace_back(at, core);
                staging_.emplace_back(at + (right * std::cos(a0) + up * std::sin(a0)) * size, rim);
                staging_.emplace_back(at + (right * std::cos(a1) + up * std::sin(a1)) * size, rim);
            }
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
