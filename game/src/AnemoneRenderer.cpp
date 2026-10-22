#include "IslandSimulator/AnemoneRenderer.hpp"
#include "IslandSimulator/AquaticLightingModel.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
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
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float Pi = std::numbers::pi_v<float>;
        constexpr int Tentacles = AnemoneRenderer::Tentacles;

        [[nodiscard]] Color Lit(const Color& base, const Vector3& normal, const AquaticLightProfile& water)
        {
            const float light = 0.62f + 0.30f * std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection))
                + 0.10f * std::max(0.0f, normal.Y);
            return wildlife_render::ApplyUnderwaterLight(wildlife_render::ScaleColor(base, light), water);
        }

        // A tube between two points, `sides` around, radii at its ends; shaded at its corners.
        void Tube(std::vector<VertexPositionColor>& out, const Vector3& a, const Vector3& b, const float radiusA,
                  const float radiusB, const Color& colorA, const Color& colorB, const AquaticLightProfile& water,
                  const int sides)
        {
            Vector3 axis = b - a;
            const float length = axis.Length();
            if (length < 1e-5f) return;
            axis /= length;
            Vector3 side = Vector3::Cross(axis, std::abs(axis.Y) < 0.9f ? Vector3::Up : Vector3::Right);
            side.Normalize();
            const Vector3 other = Vector3::Cross(axis, side);
            for (int i = 0; i < sides; ++i) {
                const float a0 = 2.0f * Pi * static_cast<float>(i) / static_cast<float>(sides);
                const float a1 = 2.0f * Pi * static_cast<float>(i + 1) / static_cast<float>(sides);
                const Vector3 r0 = side * std::cos(a0) + other * std::sin(a0);
                const Vector3 r1 = side * std::cos(a1) + other * std::sin(a1);
                const VertexPositionColor p0(a + r0 * radiusA, Lit(colorA, r0, water));
                const VertexPositionColor p1(b + r0 * radiusB, Lit(colorB, r0, water));
                const VertexPositionColor p2(b + r1 * radiusB, Lit(colorB, r1, water));
                const VertexPositionColor p3(a + r1 * radiusA, Lit(colorA, r1, water));
                out.insert(out.end(), {p0, p1, p2, p0, p2, p3});
            }
        }

        void AppendAnemone(std::vector<VertexPositionColor>& out, const ReefAnemone& anemone, const float seconds)
        {
            const AquaticLightProfile water = AquaticLightAt(anemone.x, anemone.y + 0.1f, anemone.z, seconds);
            const float s = anemone.radius;
            const Color column(118, 58, 104), disc(150, 112, 96), tentacle(176, 160, 112), tip(214, 132, 170);
            // the column: a short purple stalk flaring to the oral disc
            const Vector3 foot(anemone.x, anemone.y, anemone.z);
            const Vector3 disk = foot + Vector3(0.0f, 0.45f * s, 0.0f);
            Tube(out, foot, disk, 0.55f * s, 0.80f * s, column, column, water, 12);
            // the surge rocks the tentacles to and fro together, each a little out of step
            const Vector3 surge(std::cos(anemone.surgeHeading), 0.0f, std::sin(anemone.surgeHeading));
            for (int t = 0; t < Tentacles; ++t) {
                const float ring = t < Tentacles / 2 ? 0.78f : 0.48f;
                const float angle = 2.0f * Pi * (static_cast<float>(t) + (t < Tentacles / 2 ? 0.0f : 0.5f))
                    / static_cast<float>(Tentacles / 2) + anemone.phase;
                const Vector3 outward(std::cos(angle), 0.0f, std::sin(angle));
                const Vector3 root = disk + outward * (ring * s);
                const float sway = std::sin(seconds * 1.25f + anemone.phase + 0.35f * static_cast<float>(t % 7));
                const float length = (0.9f + 0.25f * std::sin(static_cast<float>(t) * 2.3f)) * s;
                // up and out, bending over toward the tip with the surge
                const Vector3 middle = root + outward * (0.30f * length) + Vector3::Up * (0.50f * length)
                    + surge * (0.10f * length * sway);
                const Vector3 end = middle + outward * (0.30f * length) + Vector3::Up * (0.32f * length)
                    + surge * (0.28f * length * sway);
                const Color mid = Color::Lerp(tentacle, tip, 0.25f);
                Tube(out, root, middle, 0.10f * s, 0.085f * s, tentacle, mid, water, 3);
                Tube(out, middle, end, 0.085f * s, 0.045f * s, mid, tip, water, 3);
            }
            // the oral disc's face, under the tentacles
            for (int i = 0; i < 12; ++i) {
                const float a0 = 2.0f * Pi * static_cast<float>(i) / 12.0f, a1 = 2.0f * Pi * static_cast<float>(i + 1) / 12.0f;
                const Color face = Lit(disc, Vector3::Up, water);
                out.emplace_back(disk + Vector3(0.0f, 0.01f * s, 0.0f), face);
                out.emplace_back(disk + Vector3(std::cos(a0) * 0.80f * s, 0.0f, std::sin(a0) * 0.80f * s), face);
                out.emplace_back(disk + Vector3(std::cos(a1) * 0.80f * s, 0.0f, std::sin(a1) * 0.80f * s), face);
            }
        }
    }

    AnemoneRenderer::AnemoneRenderer(GraphicsDevice& device)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          VerticesPerAnemone * MaximumVisible, BufferUsage::WriteOnly))
    {
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        staging_.reserve(static_cast<std::size_t>(VerticesPerAnemone) * MaximumVisible);
        constexpr auto diagnostic = "island-simulator: sea anemones on the reef's coral heads: a purple column under a mop of "
            "pink-tipped tentacles swaying in the surge, each the home of a pair of clownfish";
        std::cout << diagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
    }

    AnemoneRenderer::~AnemoneRenderer() = default;

    void AnemoneRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& cameraPosition,
                               const std::span<const ReefAnemone> anemones, const float timeSeconds)
    {
        staging_.clear();
        visibleCount_ = drawCallCount_ = 0;
        const BoundingFrustum frustum(view * projection);
        for (const ReefAnemone& anemone : anemones) {
            if (visibleCount_ >= MaximumVisible) break;
            const Vector3 centre(anemone.x, anemone.y + 0.3f * anemone.radius, anemone.z);
            if (Vector3::DistanceSquared(centre, cameraPosition) > DrawDistance * DrawDistance
                || !frustum.Intersects(BoundingSphere(centre, 2.2f * anemone.radius))) continue;
            const std::size_t first = staging_.size();
            AppendAnemone(staging_, anemone, timeSeconds);
            if (staging_.size() - first != static_cast<std::size_t>(VerticesPerAnemone))
                throw std::logic_error("Anemone mesh violated its exact vertex budget");
            ++visibleCount_;
        }
        if (staging_.empty()) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        underwater_fog::Apply(*effect_, 150.0f, 420.0f);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3U));
        drawCallCount_ = 1;
    }
}
