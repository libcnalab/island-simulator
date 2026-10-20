#include "IslandSimulator/TimeOfDayRenderer.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int MoonSegments = 40;
        constexpr std::array<float, 6> MoonRings{0.0f, 0.3f, 0.55f, 0.75f, 0.9f, 1.0f};
        constexpr int GlowSegments = 32;
        constexpr int MoonVertexCapacity = (static_cast<int>(MoonRings.size()) - 1) * MoonSegments * 6
            + GlowSegments * 3;

        [[nodiscard]] std::uint32_t Mix(std::uint32_t v) noexcept
        {
            v ^= v >> 16U; v *= 0x7feb352dU; v ^= v >> 15U; v *= 0x846ca68bU; return v ^ (v >> 16U);
        }
        [[nodiscard]] float Unit(const std::uint32_t i, const std::uint32_t salt) noexcept
        {
            return static_cast<float>(Mix(i * 0x9e3779b9U ^ salt) >> 8U) * (1.0f / 16777216.0f);
        }
        [[nodiscard]] Color Premultiplied(const float r, const float g, const float b, const float a) noexcept
        {
            const auto c = [](const float v) { return static_cast<int>(std::lround(std::clamp(v, 0.0f, 1.0f) * 255.0f)); };
            return Color(c(r * a), c(g * a), c(b * a), c(a));
        }
    }

    TimeOfDayRenderer::TimeOfDayRenderer(GraphicsDevice& device)
        : device_(device), gradeEffect_(std::make_unique<BasicEffect>(device)),
          skyEffect_(std::make_unique<BasicEffect>(device))
    {
        for (auto* effect : {gradeEffect_.get(), skyEffect_.get()}) {
            effect->setVertexColorEnabledProperty(true);
            effect->setLightingEnabledProperty(false);
            effect->setFogEnabledProperty(false);
        }
        // The grade: the frame times the blend factor (the hour's colour), no source colour.
        gradeBlend_ = std::make_unique<BlendState>();
        gradeBlend_->setColorSourceBlendProperty(Blend::Zero);
        gradeBlend_->setColorDestinationBlendProperty(Blend::BlendFactor);
        gradeBlend_->setAlphaSourceBlendProperty(Blend::Zero);
        gradeBlend_->setAlphaDestinationBlendProperty(Blend::One);
        // With a local light: the frame times (the hour's colour, the source, plus one minus
        // the frame's alpha, where the light wrote its strength); the alpha back to one.
        litGradeBlend_ = std::make_unique<BlendState>();
        litGradeBlend_->setColorSourceBlendProperty(Blend::DestinationColor);
        litGradeBlend_->setColorDestinationBlendProperty(Blend::InverseDestinationAlpha);
        litGradeBlend_->setAlphaSourceBlendProperty(Blend::One);
        litGradeBlend_->setAlphaDestinationBlendProperty(Blend::Zero);
        alphaResetBlend_ = std::make_unique<BlendState>();
        alphaResetBlend_->setColorSourceBlendProperty(Blend::Zero);
        alphaResetBlend_->setColorDestinationBlendProperty(Blend::One);
        alphaResetBlend_->setAlphaSourceBlendProperty(Blend::One);
        alphaResetBlend_->setAlphaDestinationBlendProperty(Blend::Zero);
        const std::array<VertexPositionColor, 3> triangle{
            VertexPositionColor(Vector3(-1, -1, 0), Color(255, 255, 255, 255)),
            VertexPositionColor(Vector3(3, -1, 0), Color(255, 255, 255, 255)),
            VertexPositionColor(Vector3(-1, 3, 0), Color(255, 255, 255, 255))};
        gradeTriangle_ = std::make_unique<VertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), 3,
                                                        BufferUsage::WriteOnly);
        gradeTriangle_->SetData(triangle.data(), 3);

        // The stars: fixed directions above the horizon, a few bright and many faint, each a
        // small square facing its own direction; translated with the camera each frame.
        std::vector<VertexPositionColor> stars;
        stars.reserve(static_cast<std::size_t>(StarCount) * 6U);
        for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(StarCount); ++i) {
            const float y = 0.04f + 0.96f * std::sqrt(Unit(i, 0x5a17u));
            const float azimuth = 2.0f * std::numbers::pi_v<float> * Unit(i, 0x5a18u);
            const float horizontal = std::sqrt(std::max(0.0f, 1.0f - y * y));
            const Vector3 direction(horizontal * std::cos(azimuth), y, horizontal * std::sin(azimuth));
            const float magnitude = std::pow(Unit(i, 0x5a19u), 3.2f);          // most faint
            const float brightness = 0.25f + 0.75f * magnitude;
            // the low sky is behind the cumulus band and the haze
            const float low = 0.35f + 0.65f * std::clamp((y - 0.10f) / 0.35f, 0.0f, 1.0f);
            const float tint = Unit(i, 0x5a1au);
            const float r = (tint < 0.2f ? 0.82f : tint > 0.85f ? 1.0f : 0.95f) * brightness * low;
            const float g = (tint < 0.2f ? 0.88f : tint > 0.85f ? 0.90f : 0.96f) * brightness * low;
            const float b = (tint < 0.2f ? 1.0f : tint > 0.85f ? 0.78f : 1.0f) * brightness * low;
            const float half = StarDistance * std::tan((0.035f + 0.05f * magnitude) * std::numbers::pi_v<float> / 180.0f);
            Vector3 right = Vector3::Cross(direction, Vector3::Up);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Normalize(Vector3::Cross(right, direction));
            const Vector3 centre = direction * StarDistance;
            const Color color(static_cast<int>(r * 255.0f), static_cast<int>(g * 255.0f), static_cast<int>(b * 255.0f), 255);
            const Vector3 a = centre - right * half - up * half, bq = centre + right * half - up * half;
            const Vector3 c = centre + right * half + up * half, d = centre - right * half + up * half;
            stars.emplace_back(a, color); stars.emplace_back(bq, color); stars.emplace_back(c, color);
            stars.emplace_back(a, color); stars.emplace_back(c, color); stars.emplace_back(d, color);
        }
        starVertexCount_ = static_cast<int>(stars.size());
        stars_ = std::make_unique<VertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                starVertexCount_, BufferUsage::WriteOnly);
        stars_->SetData(stars.data(), starVertexCount_);
        moon_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                      MoonVertexCapacity, BufferUsage::WriteOnly);
        moonStaging_.reserve(static_cast<std::size_t>(MoonVertexCapacity));
        const std::string message = "island-simulator: night sky: " + std::to_string(StarCount)
            + " stars and a phased moon over the graded sky; the scene is graded by the hour (golden hours, a dark blue night)";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    TimeOfDayRenderer::~TimeOfDayRenderer() = default;

    bool TimeOfDayRenderer::GradeActive(const DayLighting& light) noexcept
    {
        return !(light.grade.x >= 0.999f && light.grade.y >= 0.999f && light.grade.z >= 0.999f);
    }

    void TimeOfDayRenderer::ResetAlpha()
    {
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(*alphaResetBlend_);
        device_.setDepthStencilStateProperty(DepthStencilState::None);
        device_.SetVertexBuffer(gradeTriangle_.get());
        device_.setIndicesProperty(nullptr);
        gradeEffect_->setWorldProperty(Matrix::getIdentityProperty());
        gradeEffect_->setViewProperty(Matrix::getIdentityProperty());
        gradeEffect_->setProjectionProperty(Matrix::getIdentityProperty());
        gradeEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, 1);
    }

    void TimeOfDayRenderer::DrawGrade(const DayLighting& light, const bool localLight)
    {
        drawCalls_ = 0;
        gradeApplied_ = false;
        if (!GradeActive(light)) return;   // full day
        const auto c = [](const float v) { return static_cast<int>(std::lround(std::clamp(v, 0.0f, 1.0f) * 255.0f)); };
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        if (localLight) {
            device_.setBlendStateProperty(*litGradeBlend_);
            gradeEffect_->setDiffuseColorProperty(Vector3(std::clamp(light.grade.x, 0.0f, 1.0f), std::clamp(light.grade.y, 0.0f, 1.0f),
                                                          std::clamp(light.grade.z, 0.0f, 1.0f)));
        } else {
            device_.setBlendStateProperty(*gradeBlend_);
            device_.setBlendFactorProperty(Color(c(light.grade.x), c(light.grade.y), c(light.grade.z), 255));
        }
        device_.setDepthStencilStateProperty(DepthStencilState::None);
        device_.SetVertexBuffer(gradeTriangle_.get());
        device_.setIndicesProperty(nullptr);
        gradeEffect_->setWorldProperty(Matrix::getIdentityProperty());
        gradeEffect_->setViewProperty(Matrix::getIdentityProperty());
        gradeEffect_->setProjectionProperty(Matrix::getIdentityProperty());
        gradeEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, 1);
        device_.setBlendFactorProperty(Color(255, 255, 255, 255));
        gradeEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        ++drawCalls_;
        gradeApplied_ = true;
    }

    void TimeOfDayRenderer::DrawFlash(const float flash)
    {
        if (!(flash > 0.01f)) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::None);
        device_.SetVertexBuffer(gradeTriangle_.get());
        device_.setIndicesProperty(nullptr);
        const float f = std::clamp(flash, 0.0f, 1.0f) * 0.42f;
        gradeEffect_->setDiffuseColorProperty(Vector3(0.78f * f, 0.82f * f, 0.95f * f));
        gradeEffect_->setWorldProperty(Matrix::getIdentityProperty());
        gradeEffect_->setViewProperty(Matrix::getIdentityProperty());
        gradeEffect_->setProjectionProperty(Matrix::getIdentityProperty());
        gradeEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, 1);
        gradeEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        ++drawCalls_;
    }

    void TimeOfDayRenderer::DrawNightSky(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                         const DayLighting& light)
    {
        const float nightness = 1.0f - light.sunVisibility;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        device_.setIndicesProperty(nullptr);
        skyEffect_->setViewProperty(view);
        skyEffect_->setProjectionProperty(projection);
        // Stars: additive, faded in with the dark.
        if (light.starVisibility > 0.01f) {
            device_.setBlendStateProperty(BlendState::Additive);
            device_.SetVertexBuffer(stars_.get());
            skyEffect_->setWorldProperty(Matrix::CreateTranslation(camera));
            const float v = std::clamp(light.starVisibility * (1.0f - 0.55f * light.moonLight), 0.0f, 1.0f);
            skyEffect_->setDiffuseColorProperty(Vector3(v, v, v));
            skyEffect_->Apply();
            device_.DrawPrimitives(PrimitiveType::TriangleList, 0, starVertexCount_ / 3);
            ++drawCalls_;
        }
        // The moon: a disc lit on the side facing the sun, pale by day, with a faint glow at night.
        // It rises and sets through the haze: hidden below half a degree (the distant sea does
        // not write depth) and faded in by three.
        const float rise = std::clamp((light.moonElevationDegrees - 0.5f) / 2.5f, 0.0f, 1.0f);
        if (rise <= 0.0f) return;
        Vector3 forward(light.toMoon.x, light.toMoon.y, light.toMoon.z);
        forward.Normalize();
        Vector3 toSun(light.toSun.x, light.toSun.y, light.toSun.z);
        Vector3 sunAxis = toSun - forward * Vector3::Dot(toSun, forward);
        Vector3 helper = Vector3::Cross(forward, Vector3::Up);
        if (helper.LengthSquared() < 1e-6f) helper = Vector3::UnitX;
        if (sunAxis.LengthSquared() < 1e-6f) sunAxis = helper;
        sunAxis.Normalize();
        const Vector3 across = Vector3::Normalize(Vector3::Cross(forward, sunAxis));
        const float radius = MoonDistance * std::tan(MoonRadiusDegrees * std::numbers::pi_v<float> / 180.0f);
        const Vector3 centre = forward * MoonDistance;
        const float cosine = 1.0f - 2.0f * light.moonIllumination;   // the terminator's place across the disc
        const auto vertex = [&](const float u, const float v) {
            const float lit = std::clamp((u - std::sqrt(std::max(0.0f, 1.0f - v * v)) * cosine) * 8.0f + 0.5f, 0.0f, 1.0f);
            const float mare = 0.92f - 0.10f * std::max(0.0f, std::sin(u * 5.1f + 0.7f) * std::sin(v * 4.3f - 0.4f));
            const float litAlpha = (0.45f + 0.55f * nightness) * rise, darkAlpha = nightness * rise;
            const float alpha = darkAlpha + (litAlpha - darkAlpha) * lit;
            const float r = 0.025f + (0.95f * mare - 0.025f) * lit;
            const float g = 0.030f + (0.95f * mare - 0.030f) * lit;
            const float b = 0.045f + (0.90f * mare - 0.045f) * lit;
            return VertexPositionColor(centre + sunAxis * (u * radius) + across * (v * radius), Premultiplied(r, g, b, alpha));
        };
        moonStaging_.clear();
        const float tau = 2.0f * std::numbers::pi_v<float>;
        for (std::size_t ring = 0; ring + 1 < MoonRings.size(); ++ring) {
            for (int s = 0; s < MoonSegments; ++s) {
                const float a0 = tau * static_cast<float>(s) / MoonSegments, a1 = tau * static_cast<float>(s + 1) / MoonSegments;
                const float r0 = MoonRings[ring], r1 = MoonRings[ring + 1];
                const auto p = [&](const float r, const float a) { return vertex(r * std::cos(a), r * std::sin(a)); };
                moonStaging_.push_back(p(r0, a0)); moonStaging_.push_back(p(r1, a0)); moonStaging_.push_back(p(r1, a1));
                moonStaging_.push_back(p(r0, a0)); moonStaging_.push_back(p(r1, a1)); moonStaging_.push_back(p(r0, a1));
            }
        }
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        moon_->SetData(moonStaging_.data(), 0, static_cast<int>(moonStaging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(moon_.get());
        skyEffect_->setWorldProperty(Matrix::CreateTranslation(camera));
        skyEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        skyEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(moonStaging_.size() / 3));
        ++drawCalls_;
        // A soft halo around a bright moon at night.
        const float glow = 0.10f * light.moonIllumination * nightness * rise;
        if (glow > 0.005f) {
            moonStaging_.clear();
            const Vector3 glowCentre = forward * (MoonDistance + 5.0f);
            const Color inner = Premultiplied(0.55f, 0.62f, 0.75f, glow), outer(0, 0, 0, 0);
            for (int s = 0; s < GlowSegments; ++s) {
                const float a0 = tau * static_cast<float>(s) / GlowSegments, a1 = tau * static_cast<float>(s + 1) / GlowSegments;
                const float r = radius * 4.5f;
                moonStaging_.emplace_back(glowCentre, inner);
                moonStaging_.emplace_back(glowCentre + sunAxis * (std::cos(a0) * r) + across * (std::sin(a0) * r), outer);
                moonStaging_.emplace_back(glowCentre + sunAxis * (std::cos(a1) * r) + across * (std::sin(a1) * r), outer);
            }
            device_.setBlendStateProperty(BlendState::Additive);
            moon_->SetData(moonStaging_.data(), 0, static_cast<int>(moonStaging_.size()), SetDataOptions::Discard);
            skyEffect_->Apply();
            device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(moonStaging_.size() / 3));
            ++drawCalls_;
        }
    }
}
