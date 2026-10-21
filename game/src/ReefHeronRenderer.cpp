#include "IslandSimulator/ReefHeronRenderer.hpp"

#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
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
        constexpr int MaximumVertices = static_cast<int>(ReefHeronGroup::Count)
            * ReefHeronRenderer::MaximumVerticesPerHeron;
        constexpr float NearDistance = 70.0f;

        struct Palette { Color body, belly, throat, bill, leg, eye; };
        // the dark slate morph with its white chin streak, and the white morph
        const std::array<Palette, 2> Palettes{{
            {Color(70, 76, 84), Color(78, 82, 88), Color(214, 214, 204), Color(122, 104, 58), Color(118, 120, 66), Color(212, 186, 70)},
            {Color(228, 228, 220), Color(236, 234, 226), Color(240, 238, 230), Color(196, 160, 74), Color(140, 138, 74), Color(214, 190, 72)},
        }};

        [[nodiscard]] Color Lit(const Color& base, const Vector3& normal)
        {
            const float light = 0.60f + 0.32f * std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection))
                + 0.10f * std::max(0.0f, normal.Y);
            return wildlife_render::ScaleColor(base, light);
        }

        struct Frame { Vector3 forward, right, up; };

        // A tube between two points whose ring radii vary; shaded at its corners.
        void Tube(std::vector<VertexPositionColor>& out, const Vector3& a, const Vector3& b,
                  const float radiusA, const float radiusB, const Color& color, const int sides = 6)
        {
            Vector3 axis = b - a;
            const float length = axis.Length();
            if (length < 1.0e-5f) return;
            axis /= length;
            Vector3 side = Vector3::Cross(axis, std::abs(axis.Y) < 0.9f ? Vector3::Up : Vector3::Right);
            side.Normalize();
            const Vector3 other = Vector3::Cross(axis, side);
            for (int index = 0; index < sides; ++index) {
                const float a0 = 2.0f * Pi * static_cast<float>(index) / static_cast<float>(sides);
                const float a1 = 2.0f * Pi * static_cast<float>(index + 1) / static_cast<float>(sides);
                const Vector3 r0 = side * std::cos(a0) + other * std::sin(a0);
                const Vector3 r1 = side * std::cos(a1) + other * std::sin(a1);
                const Color c0 = Lit(color, r0), c1 = Lit(color, r1);
                out.emplace_back(a + r0 * radiusA, c0); out.emplace_back(b + r0 * radiusB, c0); out.emplace_back(b + r1 * radiusB, c1);
                out.emplace_back(a + r0 * radiusA, c0); out.emplace_back(b + r1 * radiusB, c1); out.emplace_back(a + r1 * radiusA, c1);
            }
        }

        // An ellipsoid along `along`, shaded at its corners; `lower` takes over below its middle.
        void Ellipsoid(std::vector<VertexPositionColor>& out, const Vector3& centre, const Frame& frame,
                       const float alongRadius, const float sideRadius, const float upRadius,
                       const Color& upper, const Color& lower, const int rings = 5, const int sides = 10)
        {
            const auto point = [&](const int ring, const int side, Vector3& normal) {
                const float latitude = -0.5f * Pi + Pi * static_cast<float>(ring) / static_cast<float>(rings);
                const float longitude = 2.0f * Pi * static_cast<float>(side) / static_cast<float>(sides);
                const float c = std::cos(latitude);
                normal = frame.forward * (std::sin(latitude) / alongRadius)
                    + frame.right * (c * std::cos(longitude) / sideRadius)
                    + frame.up * (c * std::sin(longitude) / upRadius);
                if (normal.LengthSquared() > 1.0e-10f) normal.Normalize(); else normal = frame.up;
                return centre + frame.forward * (std::sin(latitude) * alongRadius)
                    + frame.right * (c * std::cos(longitude) * sideRadius)
                    + frame.up * (c * std::sin(longitude) * upRadius);
            };
            const auto corner = [&](const int ring, const int side) {
                Vector3 normal;
                const Vector3 position = point(ring, side, normal);
                const float underside = std::clamp(-Vector3::Dot(normal, frame.up) * 2.0f + 0.3f, 0.0f, 1.0f);
                return VertexPositionColor(position, Lit(Color::Lerp(upper, lower, underside), normal));
            };
            for (int ring = 0; ring < rings; ++ring) {
                for (int side = 0; side < sides; ++side) {
                    out.push_back(corner(ring, side)); out.push_back(corner(ring + 1, side)); out.push_back(corner(ring + 1, side + 1));
                    out.push_back(corner(ring, side)); out.push_back(corner(ring + 1, side + 1)); out.push_back(corner(ring, side + 1));
                }
            }
        }

        // A thin wing or tail sheet, lit on both faces.
        template <std::size_t Count>
        void Sheet(std::vector<VertexPositionColor>& out, const std::array<Vector3, Count>& outline,
                   const Color& top, const Color& under)
        {
            const Vector3 normal = Vector3::Normalize(Vector3::Cross(outline[1] - outline[0], outline[Count - 1] - outline[0]));
            const Color litTop = Lit(top, normal.Y >= 0.0f ? normal : -normal);
            const Color litUnder = Lit(under, normal.Y >= 0.0f ? -normal : normal);
            for (std::size_t i = 1; i + 1 < Count; ++i) {
                out.emplace_back(outline[0], litTop); out.emplace_back(outline[i], litTop); out.emplace_back(outline[i + 1], litTop);
                out.emplace_back(outline[0], litUnder); out.emplace_back(outline[i + 1], litUnder); out.emplace_back(outline[i], litUnder);
            }
        }

        [[nodiscard]] Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, const float t)
        {
            const float u = 1.0f - t;
            return p0 * (u * u * u) + p1 * (3.0f * u * u * t) + p2 * (3.0f * u * t * t) + p3 * (t * t * t);
        }

        void AppendHeron(std::vector<VertexPositionColor>& out, const ReefHeronState& heron, const bool near)
        {
            const Palette& palette = Palettes[heron.morph % Palettes.size()];
            const float s = heron.scale;
            const float flight = std::clamp(heron.flight, 0.0f, 1.0f);
            const float rest = std::clamp(heron.rest, 0.0f, 1.0f);
            const float neck = std::clamp(heron.neckStretch, 0.0f, 1.0f);
            const float strike = std::clamp(heron.strike, 0.0f, 1.0f);
            const Vector3 forward(std::cos(heron.headingRadians), 0.0f, std::sin(heron.headingRadians));
            const Vector3 right(-forward.Z, 0.0f, forward.X);
            const Vector3 up = Vector3::Up;
            const Vector3 feet(heron.x, heron.y, heron.z);
            const int ringSides = near ? 10 : 6;

            // The body: tilted up at the breast while standing, level in flight, hunched and
            // fluffed out at rest.
            const float tilt = (0.38f - 0.22f * rest) * (1.0f - flight);
            const Vector3 axis = Vector3::Normalize(forward * std::cos(tilt) + up * std::sin(tilt));
            const Vector3 bodyUp = Vector3::Normalize(up - axis * Vector3::Dot(up, axis));
            const Vector3 hip = feet + up * ((0.29f - 0.04f * rest) * s);
            const Vector3 body = hip + up * (0.07f * s);
            const float fluff = 1.0f + 0.16f * rest;
            Ellipsoid(out, body, Frame{axis, right, bodyUp}, 0.21f * s, 0.092f * s * fluff, 0.10f * s * fluff,
                      palette.body, palette.belly, near ? 6 : 4, ringSides);
            // the short tail, the wing tips crossing over it
            {
                const Vector3 root = body - axis * (0.15f * s) + bodyUp * (0.02f * s);
                const std::array<Vector3, 4> tail{root - right * (0.045f * s), root + right * (0.045f * s),
                    root - axis * (0.12f * s) + right * (0.035f * s) - bodyUp * (0.01f * s),
                    root - axis * (0.12f * s) - right * (0.035f * s) - bodyUp * (0.01f * s)};
                Sheet(out, tail, palette.body, palette.belly);
            }

            // The neck's S: drawn in onto the shoulders, half up on the wade, craned out to
            // watch; the strike throws the head forward and down.
            const Vector3 neckBase = body + axis * (0.13f * s) + bodyUp * (0.025f * s);
            // watching, the neck stands in an S with the head held well forward over the water
            Vector3 head = body + axis * (0.16f * s) + up * (0.12f * s)
                + (forward * (0.16f * s) + up * (0.13f * s)) * neck;
            head += (forward * (0.22f * s) - up * (0.30f * s)) * strike;
            head = Vector3::Lerp(head, body + forward * (0.17f * s) + up * (0.06f * s), flight);
            const Vector3 control1 = neckBase + up * ((0.06f + 0.14f * neck) * s) - forward * (0.07f * neck * s);
            const Vector3 control2 = head - forward * ((0.05f + 0.12f * neck) * s) + up * (0.02f * neck * s);
            // One loft along the curve: the neck bends in the bird's own forward-up plane, so
            // every ring shares the body's right as its side (separate tubes, each choosing
            // its own, met in light seams).
            constexpr int NeckRings = 8;
            const int neckSides = near ? 10 : 6;
            const Color neckColor = Color::Lerp(palette.body, palette.belly, 0.3f);
            std::array<Vector3, NeckRings + 1> centres{}, tangents{};
            for (int ring = 0; ring <= NeckRings; ++ring) {
                const float t = static_cast<float>(ring) / NeckRings;
                centres[static_cast<std::size_t>(ring)] = Bezier(neckBase, control1, control2, head, t);
            }
            for (int ring = 0; ring <= NeckRings; ++ring) {
                const auto r = static_cast<std::size_t>(ring);
                const Vector3 a = centres[ring == 0 ? 0U : r - 1U], b = centres[ring == NeckRings ? r : r + 1U];
                tangents[r] = b - a;
                if (tangents[r].LengthSquared() < 1.0e-10f) tangents[r] = up; else tangents[r].Normalize();
            }
            const auto neckCorner = [&](const int ring, const int sideIndex) {
                const auto r = static_cast<std::size_t>(ring);
                const float t = static_cast<float>(ring) / NeckRings;
                const float radius = (0.030f - 0.012f * t) * s;
                Vector3 other = Vector3::Cross(tangents[r], right);
                if (other.LengthSquared() < 1.0e-10f) other = up; else other.Normalize();
                const Vector3 sideAxis = Vector3::Normalize(Vector3::Cross(other, tangents[r]));
                const float angle = 2.0f * Pi * static_cast<float>(sideIndex) / static_cast<float>(neckSides);
                const Vector3 radial = sideAxis * std::cos(angle) + other * std::sin(angle);
                return VertexPositionColor(centres[r] + radial * radius, Lit(neckColor, radial));
            };
            for (int ring = 0; ring < NeckRings; ++ring) {
                for (int side = 0; side < neckSides; ++side) {
                    out.push_back(neckCorner(ring, side)); out.push_back(neckCorner(ring + 1, side));
                    out.push_back(neckCorner(ring + 1, side + 1));
                    out.push_back(neckCorner(ring, side)); out.push_back(neckCorner(ring + 1, side + 1));
                    out.push_back(neckCorner(ring, side + 1));
                }
            }

            // The head and the dagger bill, pitched down to watch the water, further in the strike.
            const float pitch = (0.30f + 0.28f * neck + 0.5f * strike) * (1.0f - flight) + 0.08f * flight;
            const Vector3 billAxis = Vector3::Normalize(forward * std::cos(pitch) - up * std::sin(pitch));
            const Vector3 headUp = Vector3::Normalize(up - billAxis * Vector3::Dot(up, billAxis));
            const Vector3 headRight = Vector3::Normalize(Vector3::Cross(headUp, billAxis));
            Ellipsoid(out, head, Frame{billAxis, headRight, headUp}, 0.046f * s, 0.026f * s, 0.031f * s,
                      palette.body, palette.throat, near ? 5 : 3, near ? 10 : 6);
            const Vector3 billRoot = head + billAxis * (0.036f * s) - headUp * (0.004f * s);
            Tube(out, billRoot, billRoot + billAxis * (0.105f * s), 0.011f * s, 0.0015f * s, palette.bill, near ? 6 : 4);
            if (near) {
                for (const float side : {-1.0f, 1.0f}) {
                    const Vector3 eye = head + billAxis * (0.014f * s) + headRight * (side * 0.026f * s) + headUp * (0.009f * s);
                    const Vector3 a = eye - billAxis * (0.006f * s), b = eye + billAxis * (0.006f * s);
                    const Vector3 c = eye + headUp * (0.006f * s), d = eye - headUp * (0.006f * s);
                    out.emplace_back(a, palette.eye); out.emplace_back(c, palette.eye); out.emplace_back(b, palette.eye);
                    out.emplace_back(a, palette.eye); out.emplace_back(b, palette.eye); out.emplace_back(d, palette.eye);
                }
            }

            // The long legs: wading in step, one drawn up into the belly at rest, trailing
            // behind in flight.
            for (const float side : {-1.0f, 1.0f}) {
                const float phase = heron.stepPhase + (side > 0.0f ? 0.0f : Pi);
                const float lift = heron.behavior == ReefHeronBehavior::Stalk ? std::max(0.0f, std::sin(phase)) * 0.05f * s : 0.0f;
                Vector3 foot = feet + right * (side * 0.045f * s) + forward * (std::cos(phase) * (heron.behavior == ReefHeronBehavior::Stalk ? 0.06f : 0.0f) * s)
                    + up * lift;
                if (side < 0.0f) foot = Vector3::Lerp(foot, hip - up * (0.10f * s) + forward * (0.02f * s), rest);
                const Vector3 legTop = hip + right * (side * 0.035f * s);
                foot = Vector3::Lerp(foot, body - axis * (0.40f * s) + right * (side * 0.03f * s), flight);
                // the ankle bends back, a heron's "knee"
                const Vector3 ankle = Vector3::Lerp(legTop, foot, 0.5f) - forward * (0.035f * s * (1.0f - flight));
                Tube(out, legTop, ankle, 0.011f * s, 0.008f * s, palette.leg, near ? 6 : 4);
                Tube(out, ankle, foot, 0.008f * s, 0.007f * s, palette.leg, near ? 6 : 4);
                if (near && flight < 0.5f && !(side < 0.0f && rest > 0.5f)) {
                    for (const float spread : {-0.45f, 0.0f, 0.45f}) {
                        const Vector3 toe = Vector3::Normalize(forward * std::cos(spread) + right * std::sin(spread));
                        Tube(out, foot, foot + toe * (0.06f * s), 0.004f * s, 0.002f * s, palette.leg, 4);
                    }
                    Tube(out, foot, foot - forward * (0.03f * s), 0.004f * s, 0.002f * s, palette.leg, 4);
                }
            }

            // In flight the broad, slow wings, bowed through each beat.
            if (flight > 0.02f) {
                const float flap = std::sin(heron.wingBeat) * 0.55f * flight;
                for (const float side : {-1.0f, 1.0f}) {
                    const Vector3 out1 = right * (side * std::cos(flap)) + up * std::sin(flap);
                    const Vector3 shoulder = body + axis * (0.05f * s) + right * (side * 0.05f * s) + bodyUp * (0.03f * s);
                    const auto wing = [&](const float span, const float along) {
                        return shoulder + out1 * (span * s * flight) + axis * (along * s);
                    };
                    const std::array<Vector3, 6> outline{wing(0.0f, 0.06f), wing(0.26f, 0.08f), wing(0.50f, 0.0f),
                        wing(0.47f, -0.15f), wing(0.16f, -0.19f), wing(0.0f, -0.12f)};
                    Sheet(out, outline, palette.body, palette.belly);
                }
            }
        }
    }

    ReefHeronRenderer::ReefHeronRenderer(GraphicsDevice& device, const float maximumDistance)
        : device_(device), effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          MaximumVertices, BufferUsage::WriteOnly)),
          maximumDistance_(maximumDistance)
    {
        if (!std::isfinite(maximumDistance_) || maximumDistance_ <= 0.0f)
            throw std::invalid_argument("Reef heron maximum distance must be finite and positive");
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        staging_.reserve(MaximumVertices);
        constexpr auto diagnostic = "island-simulator: four reef herons, dark slate and white, wade the lagoon's edge, "
            "freeze to watch the water, strike at fish, flush ahead of the walker and stand hunched on one leg at night";
        std::cout << diagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(diagnostic);
    }

    ReefHeronRenderer::~ReefHeronRenderer() = default;

    void ReefHeronRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& cameraPosition,
                                 const std::span<const ReefHeronState> herons)
    {
        staging_.clear();
        visibleCount_ = drawCallCount_ = 0;
        const BoundingFrustum frustum(view * projection);
        for (const ReefHeronState& heron : herons) {
            const Vector3 centre(heron.x, heron.y + 0.4f * heron.scale, heron.z);
            const float distanceSquared = Vector3::DistanceSquared(centre, cameraPosition);
            if (distanceSquared > maximumDistance_ * maximumDistance_
                || !frustum.Intersects(BoundingSphere(centre, 0.9f * heron.scale))) continue;
            const std::size_t first = staging_.size();
            AppendHeron(staging_, heron, distanceSquared < NearDistance * NearDistance);
            if (staging_.size() - first > static_cast<std::size_t>(MaximumVerticesPerHeron))
                throw std::length_error("Reef heron mesh exceeds its vertex budget");
            // Below the sea surface the legs and toes take the water's colour: the shallows are
            // nearly clear, so without it the wading legs showed no waterline at all.
            const Color water(52, 118, 112);
            for (std::size_t i = first; i < staging_.size(); ++i) {
                auto& vertex = staging_[i];
                if (vertex.Position.Y < 0.0f) {
                    const float under = std::clamp(-vertex.Position.Y / 0.25f, 0.0f, 1.0f);
                    vertex.Color = Color::Lerp(vertex.Color, water, 0.45f + 0.30f * under);
                }
            }
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
