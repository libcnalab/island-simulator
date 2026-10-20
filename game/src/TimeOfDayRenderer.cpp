#include "IslandSimulator/TimeOfDayRenderer.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SkyCloudDrift.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
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
        // The sky's eleven fixed cumulus masses (SkyRenderer's cumulusMass literals: azimuth,
        // elevation, angular half-width and half-height), for what stands in front of the moon.
        constexpr std::array<std::array<float, 4>, 11> CumulusMasses{{
            {0.35f, 0.075f, 0.27f, 0.060f}, {1.10f, 0.060f, 0.22f, 0.042f}, {1.95f, 0.090f, 0.30f, 0.075f},
            {2.60f, 0.055f, 0.19f, 0.036f}, {3.30f, 0.110f, 0.25f, 0.058f}, {3.95f, 0.070f, 0.31f, 0.068f},
            {4.70f, 0.085f, 0.24f, 0.048f}, {5.25f, 0.060f, 0.27f, 0.050f}, {5.90f, 0.100f, 0.26f, 0.052f},
            {0.80f, 0.230f, 0.17f, 0.040f}, {4.20f, 0.260f, 0.20f, 0.046f}}};
        // How much of a sky direction the cumulus cover (0..1): each mass as its body, an
        // ellipse over the flat base reaching up into the crown, with a soft rim.
        float CumulusCover(const Vector3& direction, const float driftRadians)
        {
            const float length = direction.Length();
            if (length < 1e-6f) return 0.0f;
            const float y = direction.Y / length;
            if (y < -0.02f || y > 0.45f) return 0.0f;
            const float azimuth = std::atan2(direction.Z, direction.X) - driftRadians;
            const float elevation = std::asin(std::clamp(y, -1.0f, 1.0f));
            float cover = 0.0f;
            for (const auto& cloud : CumulusMasses) {
                float dAzimuth = azimuth - cloud[0];
                dAzimuth -= 6.2831853f * std::floor(dAzimuth / 6.2831853f + 0.5f);
                const float lx = dAzimuth / cloud[2], ly = (elevation - cloud[1]) / cloud[3];
                const float r = std::hypot(lx / 0.95f, (ly - 0.22f) / 0.85f);
                const float t = std::clamp((r - 0.70f) / 0.35f, 0.0f, 1.0f);
                cover = std::max(cover, 1.0f - t * t * (3.0f - 2.0f * t));
            }
            return cover;
        }
        constexpr int RainbowSegments = 72, RainbowBands = 7;
        constexpr int RainbowVertexCapacity = RainbowSegments * RainbowBands * 6 * 2;
        constexpr int BoltSegments = 14, BoltBranches = 2, BoltBranchSegments = 6;
        constexpr int BoltVertexCapacity = (BoltSegments + BoltBranches * BoltBranchSegments) * 6 * 2;
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
        meteor_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), 12,
                                                        BufferUsage::WriteOnly);
        rainbow_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                         RainbowVertexCapacity, BufferUsage::WriteOnly);
        rainbowStaging_.reserve(static_cast<std::size_t>(RainbowVertexCapacity));
        bolt_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), BoltVertexCapacity,
                                                      BufferUsage::WriteOnly);
        boltStaging_.reserve(static_cast<std::size_t>(BoltVertexCapacity));
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

    float TimeOfDayRenderer::RainbowStrength(const DayLighting& light, const float rain) noexcept
    {
        if (!std::isfinite(rain)) return 0.0f;
        const auto smooth = [](float a, float b, float v) {
            const float t = std::clamp((v - a) / (b - a), 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        };
        // light rain in the air, not a downpour; the sun breaking through; low in the sky
        const float drops = smooth(0.02f, 0.12f, rain) * (1.0f - smooth(0.35f, 0.6f, rain));
        const float sun = smooth(0.25f, 0.6f, light.sunVisibility);
        const float low = smooth(4.0f, 9.0f, light.sunElevationDegrees) * (1.0f - smooth(34.0f, 41.0f, light.sunElevationDegrees));
        return drops * sun * low;
    }

    void TimeOfDayRenderer::DrawRainbow(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                        const DayLighting& light, const float strength)
    {
        if (!(strength > 0.01f)) return;
        Vector3 anti(-light.toSun.x, -light.toSun.y, -light.toSun.z);   // the point opposite the sun
        if (anti.LengthSquared() < 1e-6f) return;
        anti.Normalize();
        Vector3 u = Vector3::Cross(anti, Vector3::Up);
        if (u.LengthSquared() < 1e-6f) u = Vector3::UnitX;
        u.Normalize();
        const Vector3 v = Vector3::Cross(u, anti);
        constexpr float Degrees = std::numbers::pi_v<float> / 180.0f;
        // the bands across a bow, outside to inside: red to violet
        constexpr std::array<std::array<float, 3>, RainbowBands> Colours{{
            {0.95f, 0.18f, 0.12f}, {1.0f, 0.55f, 0.10f}, {1.0f, 0.92f, 0.20f}, {0.30f, 0.90f, 0.30f},
            {0.20f, 0.55f, 1.0f}, {0.30f, 0.25f, 0.95f}, {0.55f, 0.20f, 0.85f}}};
        rainbowStaging_.clear();
        const auto c = [](float x) { return static_cast<int>(std::lround(std::clamp(x, 0.0f, 1.0f) * 255.0f)); };
        const auto bow = [&](const float radiusDegrees, const float widthDegrees, const float alpha, const bool reversed) {
            for (int band = 0; band < RainbowBands; ++band) {
                const int colourIndex = reversed ? RainbowBands - 1 - band : band;
                const auto& rgb = Colours[static_cast<std::size_t>(colourIndex)];
                const float outer = (radiusDegrees + widthDegrees * (0.5f - static_cast<float>(band) / RainbowBands)) * Degrees;
                const float inner = outer - widthDegrees / RainbowBands * Degrees;
                // the edge bands softer
                const float edge = band == 0 || band == RainbowBands - 1 ? 0.55f : 1.0f;
                for (int s = 0; s < RainbowSegments; ++s) {
                    const float a0 = 6.2831853f * static_cast<float>(s) / RainbowSegments;
                    const float a1 = 6.2831853f * static_cast<float>(s + 1) / RainbowSegments;
                    const auto at = [&](float angle, float radius) {
                        const Vector3 d = anti * std::cos(radius) + (u * std::cos(angle) + v * std::sin(angle)) * std::sin(radius);
                        return d * MoonDistance;
                    };
                    const auto colour = [&](const Vector3& p) {
                        // fades out toward the horizon and is gone below it
                        const float above = std::clamp(p.Y / MoonDistance / 0.08f, 0.0f, 1.0f);
                        const float a = alpha * edge * above * strength;
                        return Color(c(rgb[0] * a), c(rgb[1] * a), c(rgb[2] * a), 255);
                    };
                    const Vector3 o0 = at(a0, outer), o1 = at(a1, outer), i0 = at(a0, inner), i1 = at(a1, inner);
                    rainbowStaging_.emplace_back(o0, colour(o0)); rainbowStaging_.emplace_back(o1, colour(o1));
                    rainbowStaging_.emplace_back(i1, colour(i1));
                    rainbowStaging_.emplace_back(o0, colour(o0)); rainbowStaging_.emplace_back(i1, colour(i1));
                    rainbowStaging_.emplace_back(i0, colour(i0));
                }
            }
        };
        bow(42.0f, 2.2f, 0.32f, false);
        bow(51.0f, 3.2f, 0.12f, true);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        rainbow_->SetData(rainbowStaging_.data(), 0, static_cast<int>(rainbowStaging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(rainbow_.get());
        device_.setIndicesProperty(nullptr);
        skyEffect_->setWorldProperty(Matrix::CreateTranslation(camera));
        skyEffect_->setViewProperty(view);
        skyEffect_->setProjectionProperty(projection);
        skyEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        skyEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(rainbowStaging_.size() / 3));
        ++drawCalls_;
    }

    void TimeOfDayRenderer::DrawBolt(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                     const std::uint32_t strike, const float distance, const float flash)
    {
        if (!(flash > 0.05f) || !std::isfinite(distance)) return;
        const auto hash = [strike](const std::uint32_t salt) {
            std::uint32_t h = strike * 0x9e3779b9U ^ salt * 0x85ebca6bU;
            h ^= h >> 16; h *= 0x7feb352dU; h ^= h >> 15; h *= 0x846ca68bU; h ^= h >> 16;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        };
        constexpr float SkyDistance = MoonDistance;
        const float bearing = hash(1U) * 6.2831853f;
        // the cloud base about 1.4 km up at the strike's distance, seen from here
        const float top = std::atan2(1400.0f, std::max(distance, 300.0f));
        const auto direction = [&](const float azimuth, const float elevation) {
            return Vector3(std::cos(elevation) * std::cos(azimuth), std::sin(elevation), std::cos(elevation) * std::sin(azimuth))
                * SkyDistance;
        };
        // a nearer bolt is thicker on the sky
        const float width = SkyDistance * std::clamp(3.0f / std::max(distance, 300.0f), 0.0012f, 0.006f);
        const float brightness = std::clamp(flash * (1.3f - distance / 5200.0f), 0.0f, 1.0f);
        boltStaging_.clear();
        const auto segment = [&](const Vector3& a, const Vector3& b, const float thickness, const float glow) {
            Vector3 across = Vector3::Cross(Vector3::Normalize(a), b - a);
            if (across.LengthSquared() < 1e-6f) return;
            across = Vector3::Normalize(across) * thickness;
            const auto c = [](float x) { return static_cast<int>(std::lround(std::clamp(x, 0.0f, 1.0f) * 255.0f)); };
            // a white core in a violet halo three times as wide
            const Color core(c(glow), c(glow), c(glow), 255), halo(c(glow * 0.30f), c(glow * 0.28f), c(glow * 0.55f), 255),
                rim(0, 0, 0, 0);
            const std::array<std::pair<float, std::pair<Color, Color>>, 2> layers{{{3.0f, {halo, rim}}, {1.0f, {core, core}}}};
            for (const auto& [scale, colours] : layers) {
                const Vector3 w = across * scale;
                boltStaging_.emplace_back(a - w, colours.second); boltStaging_.emplace_back(a, colours.first);
                boltStaging_.emplace_back(b, colours.first);
                boltStaging_.emplace_back(a + w, colours.second); boltStaging_.emplace_back(b + w, colours.second);
                boltStaging_.emplace_back(b, colours.first);
                (void)rim;
            }
        };
        // the main channel: down from the cloud base in jagged steps to the horizon
        std::array<Vector3, BoltSegments + 1> path{};
        float azimuth = bearing;
        for (int i = 0; i <= BoltSegments; ++i) {
            const float t = static_cast<float>(i) / BoltSegments;
            if (i > 0) azimuth += (hash(10U + static_cast<std::uint32_t>(i)) - 0.5f) * top * 0.16f;
            path[static_cast<std::size_t>(i)] = direction(azimuth, top * (1.0f - t) - 0.002f);
        }
        for (int i = 0; i < BoltSegments; ++i)
            segment(path[static_cast<std::size_t>(i)], path[static_cast<std::size_t>(i + 1)], width, brightness);
        // a branch or two forking off the upper half, thinner and dimmer
        for (int b = 0; b < BoltBranches; ++b) {
            const int from = 2 + static_cast<int>(hash(40U + static_cast<std::uint32_t>(b)) * (BoltSegments / 2));
            const float side = hash(50U + static_cast<std::uint32_t>(b)) < 0.5f ? -1.0f : 1.0f;
            Vector3 previous = path[static_cast<std::size_t>(from)];
            float branchAzimuth = std::atan2(previous.Z, previous.X);
            float elevation = std::asin(std::clamp(previous.Y / SkyDistance, -1.0f, 1.0f));
            for (int i = 0; i < BoltBranchSegments; ++i) {
                branchAzimuth += side * top * (0.035f + 0.03f * hash(60U + static_cast<std::uint32_t>(b * 10 + i)));
                elevation -= top * (0.035f + 0.03f * hash(80U + static_cast<std::uint32_t>(b * 10 + i)));
                const Vector3 next = direction(branchAzimuth, std::max(elevation, 0.0f));
                segment(previous, next, width * 0.5f, brightness * (0.6f - 0.07f * static_cast<float>(i)));
                previous = next;
            }
        }
        if (boltStaging_.empty()) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        bolt_->SetData(boltStaging_.data(), 0, static_cast<int>(boltStaging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(bolt_.get());
        device_.setIndicesProperty(nullptr);
        skyEffect_->setWorldProperty(Matrix::CreateTranslation(camera));
        skyEffect_->setViewProperty(view);
        skyEffect_->setProjectionProperty(projection);
        skyEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
        skyEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(boltStaging_.size() / 3));
        ++drawCalls_;
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
                                         const DayLighting& light, const double seconds)
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
            // A shooting star: a bright head drawing a fading trail as it slides down the sky.
            const shooting_stars::Meteor meteor = shooting_stars::At(seconds);
            if (meteor.visible && v > 0.25f) {
                const auto along = [&](const float t) {
                    const float elevation = meteor.elevation - 0.30f * t;
                    const float azimuth = meteor.azimuth + meteor.slant * 0.30f * t;
                    return Vector3(std::cos(elevation) * std::cos(azimuth), std::sin(elevation),
                                   std::cos(elevation) * std::sin(azimuth)) * StarDistance;
                };
                const float head = meteor.progress, tail = std::max(0.0f, head - 0.28f);
                const Vector3 h = along(head), t = along(tail);
                Vector3 width = Vector3::Cross(Vector3::Normalize(h), Vector3::Normalize(h - t));
                if (width.LengthSquared() < 1e-6f) width = Vector3::UnitX;
                width = Vector3::Normalize(width) * (StarDistance * 0.0016f);
                // bright as it streaks, gone at the end
                const float bright = v * std::sin(std::numbers::pi_v<float> * std::min(meteor.progress * 1.4f, 1.0f));
                const auto c = [](float x) { return static_cast<int>(std::lround(std::clamp(x, 0.0f, 1.0f) * 255.0f)); };
                const Color headColour(c(bright), c(bright), c(bright * 0.92f), 255), tailColour(0, 0, 0, 0);
                const std::array<VertexPositionColor, 6> streak{
                    VertexPositionColor(t, tailColour), VertexPositionColor(h + width, headColour), VertexPositionColor(h - width, headColour),
                    VertexPositionColor(h + width, headColour), VertexPositionColor(h + width * 0.2f + (h - t) * 0.04f, headColour),
                    VertexPositionColor(h - width, headColour)};
                meteor_->SetData(streak.data(), 0, static_cast<int>(streak.size()), SetDataOptions::Discard);
                device_.SetVertexBuffer(meteor_.get());
                skyEffect_->setDiffuseColorProperty(Vector3(1.0f, 1.0f, 1.0f));
                skyEffect_->Apply();
                device_.DrawPrimitives(PrimitiveType::TriangleList, 0, 2);
                ++drawCalls_;
            }
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
        // Clouds stand in front of the moon: the drifting cumulus by the part of the disc they
        // cover, an overcast deck over all of it.
        const float drift = sky_cloud::DriftAngle(static_cast<float>(seconds));
        const float deck = 1.0f - 0.95f * std::clamp((weather_now::Current().cloudCover - 0.25f) / 0.55f, 0.0f, 1.0f);
        const auto vertex = [&](const float u, const float v) {
            const float lit = std::clamp((u - std::sqrt(std::max(0.0f, 1.0f - v * v)) * cosine) * 8.0f + 0.5f, 0.0f, 1.0f);
            const float mare = 0.92f - 0.10f * std::max(0.0f, std::sin(u * 5.1f + 0.7f) * std::sin(v * 4.3f - 0.4f));
            const Vector3 point = centre + sunAxis * (u * radius) + across * (v * radius);
            const float clear = deck * (1.0f - 0.92f * CumulusCover(point, drift));
            const float litAlpha = (0.45f + 0.55f * nightness) * rise * clear, darkAlpha = nightness * rise * clear;
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
