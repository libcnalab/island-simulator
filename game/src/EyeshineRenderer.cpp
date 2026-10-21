#include "IslandSimulator/EyeshineRenderer.hpp"
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
        constexpr int Segments = 8, MaximumEyes = 64;
        constexpr int Capacity = MaximumEyes * Segments * 3;
    }

    EyeshineRenderer::EyeshineRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), Capacity,
                                                          BufferUsage::WriteOnly))
    {
        staging_.reserve(Capacity);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const char* message = "island-simulator: at night the eyes of deer, goats and boars shine back in the quad's headlights";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    EyeshineRenderer::~EyeshineRenderer() = default;

    void EyeshineRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                const std::span<const EyeGlint> eyes, const Vector3& lamp, const Vector3& axis,
                                const float cosCone, const float reach, const float strength)
    {
        shining_ = 0;
        if (!(strength > 0.05f) || eyes.empty()) return;
        staging_.clear();
        const Vector3 beam = Vector3::Normalize(axis);
        for (const EyeGlint& eye : eyes) {
            if (shining_ >= MaximumEyes) break;
            const Vector3 fromLamp = eye.at - lamp;
            const float distance = fromLamp.Length();
            if (distance < 1.0f || distance > reach) continue;
            // in the beam
            const float axial = Vector3::Dot(fromLamp / distance, beam);
            const float inBeam = std::clamp((axial - cosCone) / (1.0f - cosCone) * 3.0f, 0.0f, 1.0f);
            if (inBeam <= 0.0f) continue;
            // the viewer near the line back to the lamp (retro-reflection)
            Vector3 toCamera = camera - eye.at;
            const float range = toCamera.Length();
            if (range < 0.5f) continue;
            toCamera /= range;
            const float aligned = Vector3::Dot(toCamera, -fromLamp / distance);
            const float retro = std::clamp((aligned - 0.87f) / (0.995f - 0.87f), 0.0f, 1.0f);
            const float falloff = 1.0f - (distance / reach) * (distance / reach);
            const float b = std::clamp(strength * inBeam * retro * falloff * 1.3f, 0.0f, 1.0f);
            if (b < 0.03f) continue;
            // pulled a little toward the eye of the viewer so the head's own surface does not
            // hide the eye's front
            const Vector3 at = eye.at + toCamera * 0.03f;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            const float size = std::max(0.025f, range * 0.0028f);
            const auto c = [b](float v) { return static_cast<int>(std::lround(std::clamp(v * b, 0.0f, 1.0f) * 255.0f)); };
            const Color core(c(eye.tint.X), c(eye.tint.Y), c(eye.tint.Z), 255), rim(0, 0, 0, 0);
            for (int s = 0; s < Segments; ++s) {
                const float a0 = 6.2831853f * static_cast<float>(s) / Segments, a1 = 6.2831853f * static_cast<float>(s + 1) / Segments;
                staging_.emplace_back(at, core);
                staging_.emplace_back(at + (right * std::cos(a0) + up * std::sin(a0)) * size, rim);
                staging_.emplace_back(at + (right * std::cos(a1) + up * std::sin(a1)) * size, rim);
            }
            ++shining_;
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
