#include "IslandSimulator/QuadBikeRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
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
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float Pi = std::numbers::pi_v<float>;
        enum Palette : int {
            Paint = 0, BlackPlastic, Rubber, Steel, SeatVinyl, Aluminium, Chrome, EngineMetal,
            HeadLens, TailLens, Spring, SkidPlate, Decal, Shirt, Skin, Trousers, PaletteCount
        };
        // albedo, then specular strength, shininess, emissive, metal tint
        constexpr std::array<std::array<float, 7>, PaletteCount> Materials{{
            {0.60f, 0.07f, 0.05f, 0.55f, 70.0f, 0.0f, 0.15f},   // deep red paint
            {0.055f, 0.055f, 0.06f, 0.18f, 24.0f, 0.0f, 0.0f},  // black plastic
            {0.075f, 0.068f, 0.062f, 0.03f, 6.0f, 0.0f, 0.0f},  // knobby rubber
            {0.16f, 0.16f, 0.17f, 0.35f, 40.0f, 0.0f, 0.6f},    // satin steel frame
            {0.05f, 0.05f, 0.055f, 0.12f, 18.0f, 0.0f, 0.0f},   // seat vinyl
            {0.62f, 0.63f, 0.64f, 0.60f, 60.0f, 0.0f, 0.8f},    // machined aluminium
            {0.56f, 0.56f, 0.57f, 0.85f, 120.0f, 0.0f, 0.9f},   // polished exhaust
            {0.24f, 0.24f, 0.25f, 0.30f, 30.0f, 0.0f, 0.5f},    // cast engine
            {0.95f, 0.95f, 0.90f, 0.90f, 200.0f, 1.0f, 0.0f},   // headlight lens
            {0.80f, 0.05f, 0.04f, 0.60f, 120.0f, 0.7f, 0.0f},   // taillight lens
            {0.85f, 0.62f, 0.08f, 0.50f, 60.0f, 0.0f, 0.2f},    // shock spring
            {0.48f, 0.49f, 0.50f, 0.45f, 40.0f, 0.0f, 0.7f},    // aluminium skid plate
            {0.85f, 0.85f, 0.85f, 0.30f, 30.0f, 0.0f, 0.0f},    // white decal
            {0.15f, 0.30f, 0.52f, 0.05f, 8.0f, 0.0f, 0.0f},     // rider's shirt
            {0.78f, 0.58f, 0.45f, 0.15f, 16.0f, 0.0f, 0.0f},    // rider's skin
            {0.20f, 0.22f, 0.24f, 0.05f, 8.0f, 0.0f, 0.0f},     // rider's trousers
        }};

        using Vertex = VertexPositionNormalTexture;
        [[nodiscard]] Vector3 V(const BikeVec& v) noexcept { return Vector3(v.x, v.y, v.z); }

        struct MeshBuilder
        {
            std::vector<Vertex>& out;
            void Tri(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& na, const Vector3& nb,
                     const Vector3& nc, const int palette, const float shade = 1.0f) const
            {
                out.emplace_back(a, na, Vector2(static_cast<float>(palette), shade));
                out.emplace_back(b, nb, Vector2(static_cast<float>(palette), shade));
                out.emplace_back(c, nc, Vector2(static_cast<float>(palette), shade));
            }
            void Flat(const Vector3& a, const Vector3& b, const Vector3& c, const int palette, const float shade = 1.0f) const
            {
                Vector3 n = Vector3::Cross(b - a, c - a);
                if (n.LengthSquared() < 1e-12f) return;
                n.Normalize();
                Tri(a, b, c, n, n, n, palette, shade);
            }
            void Quad(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const int palette,
                      const float shade = 1.0f) const
            {
                Flat(a, b, c, palette, shade);
                Flat(a, c, d, palette, shade);
            }
            void Box(const Vector3& centre, const Vector3& half, const int palette, const float shade = 1.0f) const
            {
                const auto p = [&](float sx, float sy, float sz) {
                    return centre + Vector3(half.X * sx, half.Y * sy, half.Z * sz);
                };
                Quad(p(-1, -1, 1), p(1, -1, 1), p(1, 1, 1), p(-1, 1, 1), palette, shade);
                Quad(p(1, -1, -1), p(-1, -1, -1), p(-1, 1, -1), p(1, 1, -1), palette, shade);
                Quad(p(1, -1, 1), p(1, -1, -1), p(1, 1, -1), p(1, 1, 1), palette, shade);
                Quad(p(-1, -1, -1), p(-1, -1, 1), p(-1, 1, 1), p(-1, 1, -1), palette, shade);
                Quad(p(-1, 1, 1), p(1, 1, 1), p(1, 1, -1), p(-1, 1, -1), palette, shade);
                Quad(p(-1, -1, -1), p(1, -1, -1), p(1, -1, 1), p(-1, -1, 1), palette, shade);
            }
            // A tube between two points, its ends capped.
            void Tube(const Vector3& a, const Vector3& b, const float radius, const int palette, const int segments = 8,
                      const bool caps = true) const
            {
                Vector3 axis = b - a;
                if (axis.LengthSquared() < 1e-10f) return;
                axis.Normalize();
                Vector3 u = std::abs(axis.Y) < 0.9f ? Vector3::Cross(axis, Vector3::Up) : Vector3::Cross(axis, Vector3::UnitX);
                u.Normalize();
                const Vector3 v = Vector3::Cross(axis, u);
                for (int i = 0; i < segments; ++i) {
                    const float a0 = 2.0f * Pi * static_cast<float>(i) / segments, a1 = 2.0f * Pi * static_cast<float>(i + 1) / segments;
                    const Vector3 n0 = u * std::cos(a0) + v * std::sin(a0), n1 = u * std::cos(a1) + v * std::sin(a1);
                    Tri(a + n0 * radius, b + n0 * radius, b + n1 * radius, n0, n0, n1, palette);
                    Tri(a + n0 * radius, b + n1 * radius, a + n1 * radius, n0, n1, n1, palette);
                    if (caps) {
                        Tri(b, b + n1 * radius, b + n0 * radius, axis, axis, axis, palette);
                        Tri(a, a + n0 * radius, a + n1 * radius, -axis, -axis, -axis, palette);
                    }
                }
            }
            void Path(const std::initializer_list<Vector3> points, const float radius, const int palette) const
            {
                const Vector3* previous = nullptr;
                for (const Vector3& p : points) {
                    if (previous) Tube(*previous, p, radius, palette);
                    previous = &p;
                }
            }
            // A surface of revolution about an axis through `origin`; profile: (along, radius).
            void Lathe(const Vector3& origin, const Vector3& axis, const std::vector<std::array<float, 2>>& profile,
                       const int segments, const int palette, const float shade = 1.0f) const
            {
                Vector3 u = std::abs(axis.Y) < 0.9f ? Vector3::Cross(axis, Vector3::Up) : Vector3::Cross(axis, Vector3::UnitX);
                u.Normalize();
                const Vector3 v = Vector3::Cross(axis, u);
                for (std::size_t k = 0; k + 1 < profile.size(); ++k) {
                    const float l0 = profile[k][0], r0 = profile[k][1], l1 = profile[k + 1][0], r1 = profile[k + 1][1];
                    // the profile's normal in (along, radius) space
                    const float dl = l1 - l0, dr = r1 - r0;
                    const float length = std::max(std::hypot(dl, dr), 1e-6f);
                    const float nl = -dr / length, nr = dl / length;
                    for (int i = 0; i < segments; ++i) {
                        const float a0 = 2.0f * Pi * static_cast<float>(i) / segments, a1 = 2.0f * Pi * static_cast<float>(i + 1) / segments;
                        const Vector3 d0 = u * std::cos(a0) + v * std::sin(a0), d1 = u * std::cos(a1) + v * std::sin(a1);
                        const Vector3 n0 = Vector3::Normalize(axis * nl + d0 * nr), n1 = Vector3::Normalize(axis * nl + d1 * nr);
                        const Vector3 p00 = origin + axis * l0 + d0 * r0, p01 = origin + axis * l0 + d1 * r0;
                        const Vector3 p10 = origin + axis * l1 + d0 * r1, p11 = origin + axis * l1 + d1 * r1;
                        Tri(p00, p10, p11, n0, n0, n1, palette, shade);
                        Tri(p00, p11, p01, n0, n1, n1, palette, shade);
                    }
                }
            }
            // A rounded shell (a superellipsoid): exponent near 0 boxy, 1 an ellipsoid.
            void Shell(const Vector3& centre, const Vector3& half, const float roundness, const int palette,
                       const int segments = 16, const int rings = 10) const
            {
                const auto signedPow = [](float v, float e) { return std::copysign(std::pow(std::abs(v), e), v); };
                const auto point = [&](float theta, float phi) {
                    const float ct = std::cos(theta), st = std::sin(theta), cp = std::cos(phi), sp = std::sin(phi);
                    return Vector3(half.X * signedPow(cp, roundness) * signedPow(ct, roundness),
                                   half.Y * signedPow(sp, roundness),
                                   half.Z * signedPow(cp, roundness) * signedPow(st, roundness));
                };
                const auto normal = [&](float theta, float phi) {
                    const float e = 2.0f - roundness;
                    const float ct = std::cos(theta), st = std::sin(theta), cp = std::cos(phi), sp = std::sin(phi);
                    Vector3 n(signedPow(cp, e) * signedPow(ct, e) / half.X, signedPow(sp, e) / half.Y,
                              signedPow(cp, e) * signedPow(st, e) / half.Z);
                    if (n.LengthSquared() < 1e-12f) return Vector3::Up;
                    n.Normalize();
                    return n;
                };
                for (int r = 0; r < rings; ++r) {
                    const float p0 = -Pi * 0.5f + Pi * static_cast<float>(r) / rings, p1 = -Pi * 0.5f + Pi * static_cast<float>(r + 1) / rings;
                    for (int s = 0; s < segments; ++s) {
                        const float t0 = 2.0f * Pi * static_cast<float>(s) / segments, t1 = 2.0f * Pi * static_cast<float>(s + 1) / segments;
                        Tri(centre + point(t0, p0), centre + point(t0, p1), centre + point(t1, p1),
                            normal(t0, p0), normal(t0, p1), normal(t1, p1), palette);
                        Tri(centre + point(t0, p0), centre + point(t1, p1), centre + point(t1, p0),
                            normal(t0, p0), normal(t1, p1), normal(t1, p0), palette);
                    }
                }
            }
            // A fender: a curved panel around a hub in the y-z plane, between x0 and x1, from
            // angle a0 to a1 (from +z toward +y), with a turned-down outer lip.
            void Fender(const Vector3& hub, const float x0, const float x1, const float radius, const float a0,
                        const float a1, const int palette) const
            {
                constexpr int steps = 14;
                for (int i = 0; i < steps; ++i) {
                    const float t0 = a0 + (a1 - a0) * static_cast<float>(i) / steps, t1 = a0 + (a1 - a0) * static_cast<float>(i + 1) / steps;
                    const auto at = [&](float x, float t, float r) { return Vector3(x, hub.Y + r * std::sin(t), hub.Z + r * std::cos(t)); };
                    const Vector3 n0(0.0f, std::sin(t0), std::cos(t0)), n1(0.0f, std::sin(t1), std::cos(t1));
                    Tri(at(x0, t0, radius), at(x1, t0, radius), at(x1, t1, radius), n0, n0, n1, palette);
                    Tri(at(x0, t0, radius), at(x1, t1, radius), at(x0, t1, radius), n0, n1, n1, palette);
                    // the outer lip, turned down
                    const float outer = std::abs(x1) > std::abs(x0) ? x1 : x0;
                    const float sideSign = outer > 0.0f ? 1.0f : -1.0f;
                    const Vector3 side(sideSign, 0.0f, 0.0f);
                    Tri(at(outer, t0, radius), at(outer, t1, radius), at(outer, t1, radius - 0.06f), side, side, side, palette);
                    Tri(at(outer, t0, radius), at(outer, t1, radius - 0.06f), at(outer, t0, radius - 0.06f), side, side, side, palette);
                }
            }
            // A smooth sheet through rows x cols points (row-major), its normals taken across
            // the grid so moulded panels shade without facets.
            void Grid(const std::vector<Vector3>& p, const int rows, const int cols, const int palette,
                      const float shade = 1.0f) const
            {
                const auto at = [&](int r, int c) {
                    return p[static_cast<std::size_t>(std::clamp(r, 0, rows - 1) * cols + std::clamp(c, 0, cols - 1))];
                };
                std::vector<Vector3> normals(p.size(), Vector3::Up);
                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        Vector3 n = Vector3::Cross(at(r, c + 1) - at(r, c - 1), at(r + 1, c) - at(r - 1, c));
                        if (n.LengthSquared() > 1e-12f) normals[static_cast<std::size_t>(r * cols + c)] = Vector3::Normalize(n);
                    }
                }
                const auto n = [&](int r, int c) { return normals[static_cast<std::size_t>(r * cols + c)]; };
                for (int r = 0; r + 1 < rows; ++r) {
                    for (int c = 0; c + 1 < cols; ++c) {
                        Tri(at(r, c), at(r + 1, c), at(r + 1, c + 1), n(r, c), n(r + 1, c), n(r + 1, c + 1), palette, shade);
                        Tri(at(r, c), at(r + 1, c + 1), at(r, c + 1), n(r, c), n(r + 1, c + 1), n(r, c + 1), palette, shade);
                    }
                }
            }
            // A moulded fender swept round a wheel: a crowned top between a turned-down outer
            // lip and a deeper inner skirt, from angle t0 to t1 about the hub (from +z toward
            // +y), carried on straight beyond both ends; `side` mirrors it to the left.
            void SweptFender(const Vector3& hub, const float side, const float xIn, const float xOut, const float radius,
                             const float t0, const float t1, const float extendStart, const float extendEnd,
                             const int palette) const
            {
                const float mid = 0.5f * (xIn + xOut);
                const std::array<std::array<float, 2>, 11> section{{
                    {xIn, -0.13f}, {xIn, -0.035f}, {xIn + 0.012f, -0.006f}, {xIn + 0.05f, 0.012f}, {mid, 0.024f},
                    {xOut - 0.05f, 0.012f}, {xOut - 0.016f, 0.0f}, {xOut, -0.012f}, {xOut + 0.010f, -0.035f},
                    {xOut + 0.018f, -0.075f}, {xOut + 0.020f, -0.095f}}};
                constexpr int arcSteps = 20;
                std::vector<std::array<float, 2>> stations;   // (angle, straight run beyond the end)
                stations.push_back({t0, -extendStart});
                stations.push_back({t0, -extendStart * 0.5f});
                for (int i = 0; i <= arcSteps; ++i) stations.push_back({t0 + (t1 - t0) * static_cast<float>(i) / arcSteps, 0.0f});
                stations.push_back({t1, extendEnd * 0.5f});
                stations.push_back({t1, extendEnd});
                std::vector<Vector3> points;
                for (const auto& station : stations) {
                    const float t = station[0];
                    const Vector3 normal(0.0f, std::sin(t), std::cos(t)), tangent(0.0f, std::cos(t), -std::sin(t));
                    const Vector3 centre = Vector3(0.0f, hub.Y, hub.Z) + normal * radius + tangent * station[1];
                    for (const auto& [u, dn] : section) points.push_back(centre + Vector3(side * u, 0.0f, 0.0f) + normal * dn);
                }
                Grid(points, static_cast<int>(stations.size()), static_cast<int>(section.size()), palette);
            }
            // A coil spring between two points.
            void Coil(const Vector3& a, const Vector3& b, const float radius, const float wire, const int turns, const int palette) const
            {
                Vector3 axis = b - a;
                const float length = axis.Length();
                if (length < 1e-4f) return;
                axis /= length;
                Vector3 u = std::abs(axis.Y) < 0.9f ? Vector3::Cross(axis, Vector3::Up) : Vector3::Cross(axis, Vector3::UnitX);
                u.Normalize();
                const Vector3 v = Vector3::Cross(axis, u);
                const int steps = turns * 8;
                Vector3 previous = a + u * radius;
                for (int i = 1; i <= steps; ++i) {
                    const float t = static_cast<float>(i) / steps;
                    const float angle = 2.0f * Pi * turns * t;
                    const Vector3 p = a + axis * (length * t) + (u * std::cos(angle) + v * std::sin(angle)) * radius;
                    Tube(previous, p, wire, palette, 5, false);
                    previous = p;
                }
            }
        };

        void BuildWheel(std::vector<Vertex>& out, const float width)
        {
            MeshBuilder m{out};
            const float half = width * 0.5f;
            const Vector3 axis = Vector3::UnitX;
            constexpr float R = QuadBike::WheelRadius;
            // the tyre's carcass
            m.Lathe(Vector3::Zero, axis, {{-half * 0.72f, 0.165f}, {-half, 0.205f}, {-half, 0.245f}, {-half * 0.88f, R - 0.010f},
                                          {-half * 0.55f, R - 0.012f}, {half * 0.55f, R - 0.012f}, {half * 0.88f, R - 0.010f},
                                          {half, 0.245f}, {half, 0.205f}, {half * 0.72f, 0.165f}}, 36, Rubber);
            // the knobs: two staggered rows on the tread and a row on each shoulder
            constexpr int knobs = 28;
            for (int k = 0; k < knobs; ++k) {
                const float angle = 2.0f * Pi * static_cast<float>(k) / knobs;
                const auto knob = [&](const float x0, const float x1, const float r0, const float r1, const float spread) {
                    const float c0 = std::cos(angle - spread), s0 = std::sin(angle - spread);
                    const float c1 = std::cos(angle + spread), s1 = std::sin(angle + spread);
                    const auto p = [&](float x, float r, float c, float s) { return Vector3(x, r * c, r * s); };
                    m.Quad(p(x0, r1, c0, s0), p(x1, r1, c0, s0), p(x1, r1, c1, s1), p(x0, r1, c1, s1), Rubber);
                    m.Quad(p(x0, r0, c0, s0), p(x1, r0, c0, s0), p(x1, r1, c0, s0), p(x0, r1, c0, s0), Rubber, 0.8f);
                    m.Quad(p(x1, r0, c1, s1), p(x0, r0, c1, s1), p(x0, r1, c1, s1), p(x1, r1, c1, s1), Rubber, 0.8f);
                    m.Quad(p(x0, r0, c1, s1), p(x0, r0, c0, s0), p(x0, r1, c0, s0), p(x0, r1, c1, s1), Rubber, 0.8f);
                    m.Quad(p(x1, r0, c0, s0), p(x1, r0, c1, s1), p(x1, r1, c1, s1), p(x1, r1, c0, s0), Rubber, 0.8f);
                };
                const float base = R - 0.013f, top = R + 0.008f;
                if (k % 2 == 0) knob(-half * 0.62f, -half * 0.08f, base, top, 0.050f);
                else knob(half * 0.08f, half * 0.62f, base, top, 0.050f);
                knob(-half * 1.0f, -half * 0.78f, R - 0.03f, R - 0.004f, 0.040f);
                knob(half * 0.78f, half * 1.0f, R - 0.03f, R - 0.004f, 0.040f);
            }
            // the rim: a barrel, a dished face, five spokes, a hub and its nuts
            m.Lathe(Vector3::Zero, axis, {{-half * 0.70f, 0.168f}, {-half * 0.66f, 0.155f}, {half * 0.66f, 0.155f},
                                          {half * 0.70f, 0.168f}}, 30, Aluminium);
            m.Lathe(Vector3::Zero, axis, {{half * 0.30f, 0.155f}, {half * 0.34f, 0.12f}, {half * 0.40f, 0.05f},
                                          {half * 0.42f, 0.0f}}, 24, Aluminium, 0.9f);
            for (int s = 0; s < 5; ++s) {
                const float a = 2.0f * Pi * static_cast<float>(s) / 5.0f;
                const Vector3 dir(0.0f, std::cos(a), std::sin(a));
                m.Tube(Vector3(half * 0.44f, 0.0f, 0.0f) + dir * 0.045f, Vector3(half * 0.36f, 0.0f, 0.0f) + dir * 0.150f,
                       0.016f, Aluminium, 6);
            }
            m.Lathe(Vector3(half * 0.40f, 0.0f, 0.0f), axis, {{0.0f, 0.048f}, {0.05f, 0.042f}, {0.07f, 0.02f}, {0.075f, 0.0f}}, 16, Steel);
            for (int n = 0; n < 4; ++n) {
                const float a = 2.0f * Pi * static_cast<float>(n) / 4.0f + 0.4f;
                const Vector3 at(half * 0.47f, 0.032f * std::cos(a), 0.032f * std::sin(a));
                m.Tube(at, at + Vector3(0.018f, 0.0f, 0.0f), 0.008f, Chrome, 6);
            }
        }

        void BuildBody(std::vector<Vertex>& out)
        {
            MeshBuilder m{out};
            const float hubY = -0.336f, frontZ = 0.5f * QuadBike::WheelBase, rearZ = -frontZ;
            // the steel frame: lower rails, upper rails, the uprights between them
            for (const float s : {-1.0f, 1.0f}) {
                m.Path({Vector3(s * 0.20f, -0.30f, 0.80f), Vector3(s * 0.20f, -0.30f, -0.72f)}, 0.019f, Steel);
                m.Path({Vector3(s * 0.16f, 0.06f, 0.48f), Vector3(s * 0.16f, 0.06f, -0.78f)}, 0.017f, Steel);
                m.Path({Vector3(s * 0.20f, -0.30f, 0.70f), Vector3(s * 0.16f, 0.06f, 0.48f)}, 0.017f, Steel);
                m.Path({Vector3(s * 0.20f, -0.30f, 0.10f), Vector3(s * 0.16f, 0.06f, 0.12f)}, 0.016f, Steel);
                m.Path({Vector3(s * 0.20f, -0.30f, -0.72f), Vector3(s * 0.16f, 0.06f, -0.78f)}, 0.016f, Steel);
                // the brush guard's sides and the rear grab bar
                m.Path({Vector3(s * 0.20f, -0.30f, 0.80f), Vector3(s * 0.22f, -0.21f, 1.08f), Vector3(s * 0.21f, 0.01f, 1.08f),
                        Vector3(s * 0.15f, 0.10f, 1.00f)}, 0.021f, BlackPlastic);
                m.Path({Vector3(s * 0.27f, 0.13f, -0.88f), Vector3(s * 0.27f, -0.08f, -1.07f), Vector3(s * 0.20f, -0.27f, -0.80f)},
                       0.020f, BlackPlastic);
                // the moulded fenders over the front and the rear wheels
                m.SweptFender(Vector3(0.0f, hubY, frontZ), s, 0.29f, 0.735f, 0.40f, 0.42f, 2.45f, 0.05f, 0.13f, Paint);
                m.SweptFender(Vector3(0.0f, hubY, rearZ), s, 0.29f, 0.765f, 0.425f, 0.68f, 2.78f, 0.10f, 0.04f, Paint);
                // the floorboards between them: a ribbed tread, a raised outer lip, toe and heel
                // guards rising into the fenders
                m.Box(Vector3(s * 0.40f, -0.262f, 0.01f), Vector3(0.175f, 0.012f, 0.30f), BlackPlastic);
                for (const float z : {-0.22f, -0.14f, -0.06f, 0.02f, 0.10f, 0.18f})
                    m.Box(Vector3(s * 0.40f, -0.246f, z), Vector3(0.15f, 0.005f, 0.012f), BlackPlastic, 0.9f);
                m.Quad(Vector3(s * 0.575f, -0.25f, -0.29f), Vector3(s * 0.575f, -0.25f, 0.31f), Vector3(s * 0.585f, -0.205f, 0.30f),
                       Vector3(s * 0.585f, -0.205f, -0.28f), BlackPlastic);
                m.Quad(Vector3(s * 0.23f, -0.25f, 0.30f), Vector3(s * 0.575f, -0.25f, 0.30f), Vector3(s * 0.60f, -0.10f, 0.25f),
                       Vector3(s * 0.25f, -0.10f, 0.25f), BlackPlastic);
                m.Quad(Vector3(s * 0.23f, -0.25f, -0.28f), Vector3(s * 0.575f, -0.25f, -0.28f), Vector3(s * 0.62f, -0.11f, -0.21f),
                       Vector3(s * 0.25f, -0.11f, -0.21f), BlackPlastic);
                // mud flaps behind the rear wheels
                m.Quad(Vector3(s * 0.49f, -0.19f, -1.035f), Vector3(s * 0.74f, -0.19f, -1.035f), Vector3(s * 0.74f, -0.37f, -1.025f),
                       Vector3(s * 0.49f, -0.37f, -1.025f), BlackPlastic);
                // the side panels under the seat
                m.Shell(Vector3(s * 0.205f, 0.03f, -0.05f), Vector3(0.022f, 0.10f, 0.25f), 0.25f, BlackPlastic, 14, 8);
                // the headlights in the fascia: a black housing, a wide lens
                const Vector3 lamp = V(s < 0.0f ? QuadBikeRenderer::HeadlightLeft : QuadBikeRenderer::HeadlightRight);
                m.Shell(lamp + Vector3(0.0f, 0.0f, -0.012f), Vector3(0.088f, 0.048f, 0.016f), 0.3f, BlackPlastic, 14, 6);
                m.Shell(lamp + Vector3(0.0f, 0.0f, -0.004f), Vector3(0.074f, 0.036f, 0.010f), 0.3f, HeadLens, 14, 6);
            }
            // the brush guard's cross bars
            m.Path({Vector3(-0.22f, -0.21f, 1.08f), Vector3(0.22f, -0.21f, 1.08f)}, 0.021f, BlackPlastic);
            m.Path({Vector3(-0.21f, 0.01f, 1.08f), Vector3(0.21f, 0.01f, 1.08f)}, 0.021f, BlackPlastic);
            m.Path({Vector3(-0.27f, -0.08f, -1.07f), Vector3(0.27f, -0.08f, -1.07f)}, 0.020f, BlackPlastic);
            m.Path({Vector3(-0.20f, -0.30f, 0.70f), Vector3(0.20f, -0.30f, 0.70f)}, 0.017f, Steel);
            m.Path({Vector3(-0.20f, -0.30f, -0.72f), Vector3(0.20f, -0.30f, -0.72f)}, 0.017f, Steel);
            // The hood between the front fenders, rolling over into the fascia; the rear deck
            // between the rear fenders, into its rear face.
            {
                constexpr int cols = 11;
                std::vector<Vector3> hood;
                const auto top = [](float x, float z) {
                    const float across = x / 0.315f;
                    return 0.092f + 0.03f * (1.0f - across * across)
                        - 0.05f * std::clamp((z - 0.72f) / 0.27f, 0.0f, 1.0f) * std::clamp((z - 0.72f) / 0.27f, 0.0f, 1.0f);
                };
                std::vector<float> rows;
                for (int r = 0; r <= 10; ++r) rows.push_back(0.28f + 0.71f * static_cast<float>(r) / 10.0f);
                for (const float z : rows) {
                    for (int c = 0; c < cols; ++c) {
                        const float u = -1.0f + 2.0f * static_cast<float>(c) / (cols - 1);
                        const float x = std::clamp(u * 1.1f, -1.0f, 1.0f) * 0.315f;
                        const float y = std::abs(u) > 0.95f ? top(x, z) - 0.11f : top(x, z);
                        hood.emplace_back(x, y, z);
                    }
                }
                // the fascia: over the nose's edge and down to the skid plate
                for (const auto& [dz, y] : std::array<std::array<float, 2>, 3>{{{0.012f, 0.0f}, {0.022f, -0.06f}, {0.0f, -0.22f}}}) {
                    for (int c = 0; c < cols; ++c) {
                        const float u = -1.0f + 2.0f * static_cast<float>(c) / (cols - 1);
                        const float x = std::clamp(u * 1.1f, -1.0f, 1.0f) * 0.315f;
                        const float t = top(x, 0.99f);
                        const float yy = y == 0.0f ? t - 0.015f : y;
                        hood.emplace_back(x, std::abs(u) > 0.95f ? std::min(yy, t - 0.11f) : yy, 0.99f + dz);
                    }
                }
                m.Grid(hood, static_cast<int>(rows.size()) + 3, cols, Paint);
                std::vector<Vector3> deck;
                const auto deckTop = [](float x, float z) {
                    const float across = x / 0.315f;
                    return 0.10f + 0.025f * (1.0f - across * across) - 0.025f * std::clamp((-z - 0.82f) / 0.18f, 0.0f, 1.0f);
                };
                std::vector<float> deckRows;
                for (int r = 0; r <= 9; ++r) deckRows.push_back(-0.26f - 0.74f * static_cast<float>(r) / 9.0f);
                for (const float z : deckRows) {
                    for (int c = 0; c < cols; ++c) {
                        const float u = -1.0f + 2.0f * static_cast<float>(c) / (cols - 1);
                        const float x = std::clamp(u * 1.1f, -1.0f, 1.0f) * 0.315f;
                        deck.emplace_back(x, std::abs(u) > 0.95f ? deckTop(x, z) - 0.12f : deckTop(x, z), z);
                    }
                }
                for (const auto& [dz, y] : std::array<std::array<float, 2>, 2>{{{-0.012f, 0.0f}, {-0.008f, -0.05f}}}) {
                    for (int c = 0; c < cols; ++c) {
                        const float u = -1.0f + 2.0f * static_cast<float>(c) / (cols - 1);
                        const float x = std::clamp(u * 1.1f, -1.0f, 1.0f) * 0.315f;
                        const float t = deckTop(x, -1.0f);
                        const float yy = y == 0.0f ? t - 0.015f : y;
                        deck.emplace_back(x, std::abs(u) > 0.95f ? std::min(yy, t - 0.12f) : yy, -1.0f + dz);
                    }
                }
                m.Grid(deck, static_cast<int>(deckRows.size()) + 2, cols, Paint);
            }
            // the fascia's grille between the lamps, the taillight in the rear face
            m.Box(Vector3(0.0f, -0.10f, 1.003f), Vector3(0.095f, 0.045f, 0.008f), BlackPlastic);
            for (int slot = 0; slot < 4; ++slot)
                m.Box(Vector3(0.0f, -0.135f + 0.022f * static_cast<float>(slot), 1.012f), Vector3(0.088f, 0.004f, 0.004f), Steel, 0.8f);
            m.Shell(V(QuadBikeRenderer::Taillight), Vector3(0.065f, 0.022f, 0.010f), 0.3f, TailLens, 12, 6);
            // the racks: tubes standing a hand above the hood and the deck
            const auto rack = [&](const float z0, const float z1, const float y, const float half, const float base) {
                for (const float s : {-1.0f, 1.0f}) {
                    m.Path({Vector3(s * half, y, z0), Vector3(s * half, y, z1)}, 0.013f, BlackPlastic);
                    m.Tube(Vector3(s * half, base, z0 + 0.04f), Vector3(s * half, y, z0 + 0.04f), 0.011f, BlackPlastic, 6);
                    m.Tube(Vector3(s * half, base, z1 - 0.04f), Vector3(s * half, y, z1 - 0.04f), 0.011f, BlackPlastic, 6);
                }
                for (int bar = 0; bar < 5; ++bar) {
                    const float z = z0 + (z1 - z0) * static_cast<float>(bar) / 4.0f;
                    m.Path({Vector3(-half, y, z), Vector3(half, y, z)}, 0.010f, BlackPlastic);
                }
            };
            rack(0.46f, 0.90f, 0.175f, 0.27f, 0.10f);
            rack(-0.58f, -0.98f, 0.205f, 0.29f, 0.10f);
            // skid plate, engine, finned head and side cover
            m.Box(Vector3(0.0f, -0.355f, 0.08f), Vector3(0.21f, 0.010f, 0.52f), SkidPlate);
            m.Shell(Vector3(0.0f, -0.14f, 0.02f), Vector3(0.14f, 0.15f, 0.19f), 0.3f, EngineMetal);
            m.Shell(Vector3(0.0f, 0.03f, 0.10f), Vector3(0.11f, 0.07f, 0.10f), 0.25f, EngineMetal);
            for (int fin = 0; fin < 4; ++fin)
                m.Box(Vector3(0.0f, -0.02f + 0.026f * static_cast<float>(fin), 0.10f), Vector3(0.128f, 0.005f, 0.118f), EngineMetal, 0.85f);
            m.Lathe(Vector3(-0.145f, -0.12f, 0.0f), -Vector3::UnitX, {{0.0f, 0.11f}, {0.02f, 0.10f}, {0.03f, 0.0f}}, 18, SkidPlate);
            // the exhaust: the header round the engine, the muffler under the right rear fender,
            // its tip out under the rear face
            const Vector3 tip = V(QuadBikeRenderer::ExhaustTip);
            m.Path({Vector3(0.06f, 0.04f, 0.20f), Vector3(0.15f, -0.02f, 0.10f), Vector3(0.21f, -0.06f, -0.20f),
                    Vector3(tip.X, tip.Y, -0.52f)}, 0.022f, Chrome);
            m.Lathe(Vector3(tip.X, tip.Y, -0.53f), -Vector3::UnitZ, {{0.0f, 0.0f}, {0.0f, 0.045f}, {0.03f, 0.062f},
                                                                    {0.40f, 0.062f}, {0.43f, 0.042f}, {0.43f, 0.0f}}, 20, Chrome);
            m.Tube(Vector3(tip.X, tip.Y, -0.95f), tip, 0.022f, Steel, 10);
            // the tank and console before the seat, its filler cap; the seat
            m.Shell(Vector3(0.0f, 0.16f, 0.27f), Vector3(0.17f, 0.085f, 0.16f), 0.45f, Paint, 16, 10);
            m.Lathe(Vector3(0.0f, 0.235f, 0.25f), Vector3::Up, {{0.0f, 0.032f}, {0.012f, 0.030f}, {0.016f, 0.0f}}, 14, Chrome);
            m.Shell(Vector3(0.0f, 0.15f, -0.25f), Vector3(0.175f, 0.04f, 0.40f), 0.30f, BlackPlastic, 16, 8);
            m.Shell(Vector3(0.0f, 0.215f, -0.25f), Vector3(0.16f, 0.055f, 0.39f), 0.35f, SeatVinyl, 18, 10);
            // the steering column
            m.Tube(Vector3(0.0f, 0.08f, 0.42f), Vector3(0.0f, 0.40f, 0.355f), 0.020f, Steel, 10);
        }

        void BuildBars(std::vector<Vertex>& out)
        {
            MeshBuilder m{out};
            m.Path({V(QuadBikeRenderer::GripLeft), Vector3(-0.16f, 0.44f, 0.35f), Vector3(0.16f, 0.44f, 0.35f),
                    V(QuadBikeRenderer::GripRight)}, 0.0145f, Aluminium);
            m.Box(Vector3(0.0f, 0.425f, 0.355f), Vector3(0.05f, 0.03f, 0.03f), BlackPlastic);
            m.Shell(Vector3(0.0f, 0.468f, 0.365f), Vector3(0.15f, 0.042f, 0.062f), 0.38f, Paint, 16, 8);
            m.Shell(Vector3(0.0f, 0.505f, 0.345f), Vector3(0.055f, 0.012f, 0.03f), 0.4f, BlackPlastic, 10, 6);
            for (const float s : {-1.0f, 1.0f}) {
                m.Tube(Vector3(s * 0.29f, 0.451f, 0.326f), Vector3(s * 0.415f, 0.456f, 0.307f), 0.021f, Rubber, 10);
                // the brake lever and the thumb throttle
                m.Path({Vector3(s * 0.24f, 0.455f, 0.35f), Vector3(s * 0.33f, 0.45f, 0.40f), Vector3(s * 0.39f, 0.445f, 0.395f)}, 0.006f, Steel);
            }
            m.Box(Vector3(0.26f, 0.44f, 0.30f), Vector3(0.012f, 0.015f, 0.022f), BlackPlastic);
        }
    }

    QuadBikeRenderer::QuadBikeRenderer(GraphicsDevice& device)
        : device_(device), glowEffect_(std::make_unique<BasicEffect>(device))
    {
        if (!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects))
            throw std::runtime_error("custom effects unavailable");
        constexpr const char* vertexBody = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aMaterial;
uniform mat4 uWorld;
uniform mat4 uViewProjection;
out vec3 vWorld;
out vec3 vNormal;
out vec2 vMaterial;
out vec3 vLocal;
void main()
{
    vec4 world = uWorld * vec4(aPosition, 1.0);
    vWorld = world.xyz;
    vLocal = aPosition;
    vNormal = mat3(uWorld) * aNormal;
    vMaterial = aMaterial;
    gl_Position = uViewProjection * world;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";
        constexpr const char* fragmentBody = R"(
in vec3 vWorld;
in vec3 vNormal;
in vec2 vMaterial;
in vec3 vLocal;
uniform vec3 uPalette[16];
uniform vec3 uMaterial[16];   // specular, shininess, emissive
uniform float uMetal[16];
uniform vec3 uToSun;
uniform vec3 uCameraPosition;
uniform vec2 uEmissive;       // x: one over the night grade, y: the lamps lit
uniform float uWetness;       // 0..1 rain-wet (WeatherSystem)
uniform vec3 uDirt;           // x: how dirty 0..1, y: how wet the dirt, z: 1 drawing a wheel
out vec4 fragColor;
void main()
{
    int index = int(vMaterial.x + 0.5);
    vec3 albedo = uPalette[index];
    vec3 material = uMaterial[index];
    // Rain-wet, the rough parts (tyres, plastic, the seat, cloth) darken and every surface
    // carries a film of water that mirrors the sky at a glancing view.
    float wet = clamp(uWetness, 0.0, 1.0);
    albedo *= 1.0 - wet * 0.25 * (1.0 - uMetal[index]) * (1.0 - smoothstep(20.0, 60.0, material.y));
    // Dirt thrown up by the wheels: mud, dark while wet and drying pale, thickest low on the
    // body and all over the tyres, in splotches fixed to the parts (not on the lamps' glass).
    float splotch = 0.5 + 0.22 * sin(vLocal.x * 41.0 + sin(vLocal.z * 29.0) * 1.9) * sin(vLocal.z * 47.0 + vLocal.y * 33.0)
        + 0.18 * sin(vLocal.y * 67.0 + vLocal.x * 23.0) * sin(vLocal.z * 59.0 - vLocal.x * 31.0)
        + 0.10 * sin(vLocal.x * 113.0 + vLocal.z * 97.0 + vLocal.y * 71.0);
    float low = uDirt.z > 0.5 ? 1.0 : smoothstep(0.22, -0.25, vLocal.y);
    float threshold = 1.0 - clamp(uDirt.x, 0.0, 1.0) * (0.08 + 1.0 * low);
    float dirt = smoothstep(threshold, threshold + 0.22, splotch) * step(0.001, uDirt.x) * (1.0 - material.z);
    albedo = mix(albedo, mix(vec3(0.47, 0.40, 0.30), vec3(0.17, 0.12, 0.08), clamp(uDirt.y, 0.0, 1.0)), dirt);
    material.x *= 1.0 - 0.8 * dirt;
    float metal = uMetal[index] * (1.0 - dirt);
    vec3 n = normalize(vNormal);
    vec3 toEye = normalize(uCameraPosition - vWorld);
    if (dot(n, toEye) < 0.0) n = -n;
    vec3 toSun = normalize(uToSun);
    float shadow = sunShadowVisibility(vWorld);
    float diffuse = max(dot(n, toSun), 0.0) * shadow;
    vec3 ambient = mix(vec3(0.19, 0.18, 0.16), vec3(0.34, 0.40, 0.48), 0.5 + 0.5 * n.y) * vMaterial.y;
    vec3 color = albedo * (ambient + diffuse * vec3(1.0, 0.97, 0.92));
    vec3 halfway = normalize(toSun + toEye);
    float fresnel = 0.04 + 0.96 * pow(1.0 - max(dot(n, toEye), 0.0), 5.0);
    float specular = pow(max(dot(n, halfway), 0.0), material.y) * material.x * shadow * max(uDirectSun, 0.12);
    color += mix(vec3(1.0), albedo, metal) * specular;
    color += mix(vec3(0.30, 0.40, 0.50), albedo, metal) * fresnel * max(material.x, 0.55 * wet)
        * (0.18 + 0.30 * wet) * vMaterial.y;
    color = mix(color, albedo * uEmissive.x, material.z * uEmissive.y);
    float fog = smoothstep(260.0, 720.0, length(vWorld - uCameraPosition));
    fragColor = vec4(mix(color, islandAtmosphereFog, fog), 1.0);
}
)";
        using CNA::Graphics::ShaderCodeEXT;
        const auto source = [&](const bool embedded, const bool fragment) {
            return std::string(embedded ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + environment::FogShaderDefinition + (fragment ? SunShadowMap::ReceiverShader : std::string())
                + (fragment ? fragmentBody : vertexBody);
        };
        const CNA::Graphics::ShaderPackageEXT package(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "quad/es.vert", source(true, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "quad/es.frag", source(true, true)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "quad/gl.vert", source(false, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "quad/gl.frag", source(false, true))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        material_ = std::make_unique<ShaderEffect>(device, package);
        if (!material_->IsEffectValid()) throw std::runtime_error("quad bike material failed: " + material_->GetCompileErrorEXT());
        // The cast shadow: every vertex slid along the sun onto the ground plane, a flat dark
        // film there. The depth test is strict and the film writes its depth rounded down to
        // a coarse step, so where the silhouette overlaps itself the second layer meets the
        // very same depth, fails, and the shadow stays one even tone (unrounded, two
        // triangles in one plane differ in the last bits and half the overlaps darken twice).
        constexpr const char* shadowVertex = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aMaterial;
uniform mat4 uWorld;
uniform mat4 uViewProjection;
uniform vec4 uPlane;
uniform vec3 uToSun;
void main()
{
    vec3 world = (uWorld * vec4(aPosition, 1.0)).xyz;
    float height = max(dot(uPlane.xyz, world) + uPlane.w, 0.0);
    vec3 onGround = world - uToSun * (height / max(dot(uPlane.xyz, uToSun), 0.25)) + uPlane.xyz * 0.03;
    gl_Position = uViewProjection * vec4(onGround, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";
        constexpr const char* shadowFragment = R"(
uniform float uShadowAlpha;
out vec4 fragColor;
void main()
{
    fragColor = vec4(0.0, 0.0, 0.0, uShadowAlpha);
    gl_FragDepth = floor(gl_FragCoord.z * 65536.0) / 65536.0;
}
)";
        const auto shadowSource = [&](const bool embedded, const bool fragment) {
            return std::string(embedded ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + (fragment ? shadowFragment : shadowVertex);
        };
        const CNA::Graphics::ShaderPackageEXT shadowPackage(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "quad-shadow/es.vert", shadowSource(true, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "quad-shadow/es.frag", shadowSource(true, true)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "quad-shadow/gl.vert", shadowSource(false, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "quad-shadow/gl.frag", shadowSource(false, true))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        shadow_ = std::make_unique<ShaderEffect>(device, shadowPackage);
        if (!shadow_->IsEffectValid()) throw std::runtime_error("quad bike shadow failed: " + shadow_->GetCompileErrorEXT());
        shadowDepth_ = std::make_unique<DepthStencilState>();
        shadowDepth_->setDepthBufferEnableProperty(true);
        shadowDepth_->setDepthBufferWriteEnableProperty(true);
        shadowDepth_->setDepthBufferFunctionProperty(CompareFunction::Less);

        std::vector<Vertex> vertices;
        BuildBody(vertices);
        Upload(body_, bodyCount_, vertices);
        vertices.clear();
        BuildBars(vertices);
        Upload(bars_, barsCount_, vertices);
        vertices.clear();
        BuildWheel(vertices, 0.20f);
        Upload(frontWheel_, frontWheelCount_, vertices);
        vertices.clear();
        BuildWheel(vertices, 0.245f);
        Upload(rearWheel_, rearWheelCount_, vertices);
        dynamicStaging_.reserve(20000U);
        for (auto& buffer : dynamic_)
            buffer = std::make_unique<DynamicVertexBuffer>(device, Vertex::getVertexDeclarationStatic(), 20000, BufferUsage::WriteOnly);
        glowStaging_.reserve(512U);
        glow_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(), 512, BufferUsage::WriteOnly);
        glowEffect_->setVertexColorEnabledProperty(true);
        glowEffect_->setLightingEnabledProperty(false);
        glowEffect_->setFogEnabledProperty(false);
        triangles_ = (bodyCount_ + barsCount_ + 2 * frontWheelCount_ + 2 * rearWheelCount_) / 3;
        const std::string message = "island-simulator: quad bike: a utility quad of " + std::to_string(triangles_)
            + " triangles (steel frame, moulded fenders, hood and rear deck, floorboards, seat, tank, finned engine, "
              "exhaust and muffler, racks, brush guard, skid plate, lamps), turning bars under a pod, knobby tyres on "
              "spoked rims, A-arms, coil-over shocks and a swingarm";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    QuadBikeRenderer::~QuadBikeRenderer() = default;

    void QuadBikeRenderer::Upload(std::unique_ptr<VertexBuffer>& buffer, int& count, const std::vector<Vertex>& vertices)
    {
        count = static_cast<int>(vertices.size());
        buffer = std::make_unique<VertexBuffer>(device_, Vertex::getVertexDeclarationStatic(), count, BufferUsage::WriteOnly);
        buffer->SetData(vertices.data(), count);
    }

    void QuadBikeRenderer::DrawPart(ShaderEffect& effect, VertexBuffer& buffer, const int count, const Matrix& world)
    {
        if (count <= 0) return;
        float matrix[16];
        world.ToColumnMajor(matrix);
        effect.SetUniformMat4("uWorld", matrix);
        device_.SetVertexBuffer(&buffer);
        device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, count / 3);
        ++drawCalls_;
    }

    QuadBikeRenderer::Pose QuadBikeRenderer::PoseOf(const QuadBike& bike)
    {
        const BikeVec p = bike.Position(), x = bike.Right(), y = bike.Up(), z = bike.Forward();
        // the body frame is mirrored in the world (x right of z forward, y up), so this reflects
        Matrix rotation = Matrix::getIdentityProperty();
        rotation.M11 = x.x; rotation.M12 = x.y; rotation.M13 = x.z;
        rotation.M21 = y.x; rotation.M22 = y.y; rotation.M23 = y.z;
        rotation.M31 = z.x; rotation.M32 = z.y; rotation.M33 = z.z;
        Pose pose;
        pose.body = rotation * Matrix::CreateTranslation(p.x, p.y, p.z);
        // the bars swing about the steering column
        const Vector3 pivot(0.0f, 0.40f, 0.355f);
        const Vector3 columnAxis = Vector3::Normalize(Vector3(0.0f, 1.0f, -0.20f));
        pose.bars = Matrix::CreateTranslation(-pivot) * Matrix::CreateFromAxisAngle(columnAxis, bike.SteerRadians())
            * Matrix::CreateTranslation(pivot) * pose.body;
        // the wheels at their hubs: spinning, the front ones steered, the left ones turned outward
        for (int wheel = 0; wheel < 4; ++wheel) {
            const BikeVec hub = bike.WheelHub(wheel);
            const bool left = wheel % 2 == 0, front = wheel < 2;
            const float spin = bike.WheelSpin(wheel);
            Matrix local = Matrix::CreateRotationX(left ? -spin : spin);
            if (left) local = local * Matrix::CreateRotationY(Pi);
            if (front) local = local * Matrix::CreateRotationY(bike.SteerRadians());
            pose.wheels[static_cast<std::size_t>(wheel)] = local * rotation * Matrix::CreateTranslation(hub.x, hub.y, hub.z);
        }
        return pose;
    }

    int QuadBikeRenderer::BuildDynamic(const QuadBike& bike, const bool riderFallback, const bool detailed)
    {
        const BikeVec p = bike.Position(), x = bike.Right(), y = bike.Up(), z = bike.Forward();
        const auto local = [&](const BikeVec& world) {
            const BikeVec d{world.x - p.x, world.y - p.y, world.z - p.z};
            return Vector3(d.x * x.x + d.y * x.y + d.z * x.z, d.x * y.x + d.y * y.y + d.z * y.z, d.x * z.x + d.y * z.y + d.z * z.z);
        };
        if (riderFallback) {
            // leaning into a corner as the CNA rider does: from the cornering pull, eased, up
            // to 12 degrees, the upper body turned about the hips (the hands stay on the grips)
            const float speed = bike.Speed();
            const float pull = speed * speed * std::tan(bike.SteerRadians()) / QuadBike::WheelBase;
            riderLean_ += (std::clamp(pull / 9.81f, -1.0f, 1.0f) * 0.2094f - riderLean_) * 0.12f;
        }
        // Only the wheels' travel and the rider's lean move these parts: a parked quad (and a
        // second call in the same frame) keeps the last upload instead of rebuilding it.
        std::array<Vector3, 4> hubs;
        for (int wheel = 0; wheel < 4; ++wheel) hubs[static_cast<std::size_t>(wheel)] = local(bike.WheelHub(wheel));
        auto& key = dynamicKeys_[riderFallback ? 1U : 0U];
        if (key.valid && key.hubs == hubs && key.detailed == detailed && (!riderFallback || key.lean == riderLean_))
            return key.count;
        dynamicStaging_.clear();
        MeshBuilder m{dynamicStaging_};
        // The struts, springs and links are a few centimetres thick: sub-pixel past 30 m.
        if (detailed) {
            for (const int wheel : {0, 1}) {
                const float s = wheel == 0 ? -1.0f : 1.0f;
                const Vector3 hub = local(bike.WheelHub(wheel));
                const Vector3 lowerBall(hub.X - s * 0.07f, hub.Y - 0.07f, hub.Z), upperBall(hub.X - s * 0.07f, hub.Y + 0.09f, hub.Z);
                m.Box(Vector3(hub.X - s * 0.06f, hub.Y, hub.Z), Vector3(0.025f, 0.09f, 0.035f), EngineMetal);
                m.Path({Vector3(s * 0.20f, -0.32f, 0.54f), lowerBall, Vector3(s * 0.20f, -0.32f, 0.72f)}, 0.016f, Steel);
                m.Path({Vector3(s * 0.17f, -0.13f, 0.57f), upperBall, Vector3(s * 0.17f, -0.13f, 0.70f)}, 0.014f, Steel);
                const Vector3 top(s * 0.23f, 0.03f, 0.63f);
                const Vector3 bottom = Vector3::Lerp(Vector3(s * 0.20f, -0.32f, 0.63f), lowerBall, 0.70f);
                m.Tube(top, Vector3::Lerp(top, bottom, 0.55f), 0.022f, Steel, 10);
                m.Tube(Vector3::Lerp(top, bottom, 0.5f), bottom, 0.010f, Chrome, 8);
                m.Coil(Vector3::Lerp(top, bottom, 0.08f), Vector3::Lerp(top, bottom, 0.92f), 0.038f, 0.007f, 7, Spring);
            }
            {
                const Vector3 rl = local(bike.WheelHub(2)), rr = local(bike.WheelHub(3));
                const Vector3 axle = (rl + rr) * 0.5f;
                m.Tube(rl, rr, 0.026f, Steel, 10);
                m.Lathe(rl + Vector3(0.04f, 0.0f, 0.0f), Vector3::UnitX, {{0.0f, 0.10f}, {0.012f, 0.10f}, {0.014f, 0.0f}}, 14, SkidPlate);
                for (const float s : {-1.0f, 1.0f})
                    m.Tube(Vector3(s * 0.12f, -0.24f, -0.08f), Vector3(s * 0.12f, axle.Y + 0.02f, axle.Z + 0.06f), 0.024f, Steel, 8);
                m.Tube(Vector3(-0.12f, -0.22f, -0.30f), Vector3(0.12f, -0.22f, -0.30f), 0.018f, Steel, 8);
                const Vector3 shockTop(0.0f, 0.06f, -0.26f);
                const Vector3 shockBottom = Vector3::Lerp(Vector3(0.0f, -0.24f, -0.08f), Vector3(0.0f, axle.Y + 0.02f, axle.Z + 0.06f), 0.45f);
                m.Tube(shockTop, Vector3::Lerp(shockTop, shockBottom, 0.55f), 0.026f, Steel, 10);
                m.Tube(Vector3::Lerp(shockTop, shockBottom, 0.5f), shockBottom, 0.012f, Chrome, 8);
                m.Coil(Vector3::Lerp(shockTop, shockBottom, 0.08f), Vector3::Lerp(shockTop, shockBottom, 0.92f), 0.045f, 0.008f, 7, Spring);
            }
        }
        if (riderFallback) {
            // a simple seated rider: torso over the seat, arms to the grips, legs to the footwells
            const Vector3 seat = V(Seat);
            const float leanCos = std::cos(riderLean_), leanSin = std::sin(riderLean_);
            const Vector3 hips = seat + Vector3(0.0f, 0.06f, 0.0f);
            const auto leaned = [&](const Vector3& point) {
                const Vector3 d = point - hips;
                // (the body frame's x runs to the rider's left)
                return hips + Vector3(d.X * leanCos - d.Y * leanSin, d.Y * leanCos + d.X * leanSin, d.Z);
            };
            const Vector3 chest = leaned(seat + Vector3(0.0f, 0.42f, 0.10f));
            const Vector3 head = leaned(seat + Vector3(0.0f, 0.66f, 0.14f));
            m.Shell((hips + chest) * 0.5f, Vector3(0.17f, 0.24f, 0.11f), 0.6f, Shirt, 14, 8);
            m.Shell(head, Vector3(0.11f, 0.13f, 0.12f), 0.85f, BlackPlastic, 14, 10);   // a helmet
            m.Shell(head + Vector3(0.0f, -0.02f, 0.09f), Vector3(0.07f, 0.05f, 0.03f), 0.8f, Skin, 10, 6);
            for (const float s : {-1.0f, 1.0f}) {
                const Vector3 shoulder = chest + Vector3(s * 0.19f, 0.06f, 0.0f);
                const Vector3 grip = s < 0.0f ? V(GripLeft) : V(GripRight);
                const Vector3 elbow = (shoulder + grip) * 0.5f + Vector3(s * 0.08f, -0.06f, -0.04f);
                m.Path({shoulder, elbow, grip}, 0.042f, Shirt);
                const Vector3 hip = hips + Vector3(s * 0.11f, 0.0f, 0.0f);
                const Vector3 foot = s < 0.0f ? V(FootLeft) : V(FootRight);
                const Vector3 knee = Vector3(s * 0.25f, hip.Y + 0.10f, 0.28f);
                m.Path({hip, knee, foot + Vector3(0.0f, 0.06f, 0.0f)}, 0.062f, Trousers);
                m.Box(foot + Vector3(0.0f, 0.03f, 0.04f), Vector3(0.05f, 0.04f, 0.12f), BlackPlastic);
            }
        }
        const int count = static_cast<int>(std::min<std::size_t>(dynamicStaging_.size(), 20000U) / 3U * 3U);
        if (count > 0) dynamic_[riderFallback ? 1U : 0U]->SetData(dynamicStaging_.data(), 0, count, SetDataOptions::Discard);
        key = {hubs, riderLean_, detailed, true, count};
        return count;
    }

    void QuadBikeRenderer::DrawAll(ShaderEffect& effect, const Pose& pose, const int dynamicCount, const bool rider)
    {
        effect.SetUniformVec3("uDirt", dirt_, dirtWet_, 0.0f);
        DrawPart(effect, *body_, bodyCount_, pose.body);
        DrawPart(effect, *bars_, barsCount_, pose.bars);
        if (dynamicCount > 0) DrawPart(effect, *dynamic_[rider ? 1U : 0U], dynamicCount, pose.body);
        effect.SetUniformVec3("uDirt", dirt_, dirtWet_, 1.0f);
        for (std::size_t wheel = 0; wheel < 4; ++wheel) {
            if (wheel < 2) DrawPart(effect, *frontWheel_, frontWheelCount_, pose.wheels[wheel]);
            else DrawPart(effect, *rearWheel_, rearWheelCount_, pose.wheels[wheel]);
        }
    }

    void QuadBikeRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                const QuadBike& bike, const bool lightsOn, const bool riderFallback)
    {
        drawCalls_ = 0;
        const BikeVec p = bike.Position();
        const float distanceSquared = (Vector3(p.x, p.y, p.z) - camera).LengthSquared();
        if (distanceSquared > 300.0f * 300.0f) return;
        const Pose pose = PoseOf(bike);
        const int dynamicCount = BuildDynamic(bike, riderFallback, distanceSquared < 30.0f * 30.0f);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        float viewProjection[16];
        (view * projection).ToColumnMajor(viewProjection);
        material_->Apply();
        material_->SetUniformMat4("uViewProjection", viewProjection);
        material_->SetUniformVec3("uCameraPosition", camera.X, camera.Y, camera.Z);
        const auto toSun = sun_light::UniformDirection();
        material_->SetUniformVec3("uToSun", toSun.x, toSun.y, toSun.z);
        std::array<float, PaletteCount * 3> palette{}, material{};
        std::array<float, PaletteCount> metal{};
        for (std::size_t i = 0; i < Materials.size(); ++i) {
            for (std::size_t c = 0; c < 3; ++c) {
                palette[i * 3 + c] = Materials[i][c];
                material[i * 3 + c] = Materials[i][3 + c];
            }
            metal[i] = Materials[i][6];
        }
        material_->SetUniformVec3Array("uPalette", palette.data(), PaletteCount);
        material_->SetUniformVec3Array("uMaterial", material.data(), PaletteCount);
        material_->SetUniformFloatArray("uMetal", metal.data(), PaletteCount);
        const auto& light = sun_light::Current().lighting;
        const float grade = std::max(0.08f, std::min({light.grade.x, light.grade.y, light.grade.z}));
        material_->SetUniformVec2("uEmissive", 1.0f / grade, lightsOn ? 1.0f : 0.0f);
        material_->SetUniformFloat("uWetness", weather_now::Current().wetness);
        if (sunShadow_) sunShadow_->Bind(*material_);
        else SunShadowMap::BindDisabled(*material_);
        DrawAll(*material_, pose, dynamicCount, riderFallback);
    }

    void QuadBikeRenderer::DrawShadow(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                      const QuadBike& bike, const bool rider, const BikeVec& groundPoint,
                                      const BikeVec& groundNormal, const float sunStrength)
    {
        const BikeVec p = bike.Position();
        const float distance = (Vector3(p.x, p.y, p.z) - camera).Length();
        if (distance > 140.0f) return;
        const float fade = 1.0f - std::clamp((distance - 90.0f) / 50.0f, 0.0f, 1.0f);
        Vector3 n(groundNormal.x, groundNormal.y, groundNormal.z);
        if (n.LengthSquared() < 1e-6f) n = Vector3::Up;
        n.Normalize();
        const Vector3 g(groundPoint.x, groundPoint.y, groundPoint.z);
        // Soft occlusion first: a broad dark pool under the body and a tighter one under each
        // tyre where it meets the ground (they stay at night and under cloud).
        glowStaging_.clear();
        Vector3 across = Vector3::Cross(n, Vector3(bike.Forward().x, bike.Forward().y, bike.Forward().z));
        if (across.LengthSquared() < 1e-6f) across = Vector3::UnitX;
        across.Normalize();
        const Vector3 along = Vector3::Cross(across, n);
        const auto pool = [&](const Vector3& centre, const float halfAcross, const float halfAlong, const int alpha) {
            const Vector3 c = centre + n * 0.02f;
            const Color middle(0, 0, 0, static_cast<int>(std::lround(alpha * fade))), edge(0, 0, 0, 0);
            constexpr int segments = 20;
            for (int i = 0; i < segments; ++i) {
                const float a0 = 2.0f * Pi * static_cast<float>(i) / segments, a1 = 2.0f * Pi * static_cast<float>(i + 1) / segments;
                glowStaging_.emplace_back(c, middle);
                glowStaging_.emplace_back(c + across * (std::cos(a0) * halfAcross) + along * (std::sin(a0) * halfAlong), edge);
                glowStaging_.emplace_back(c + across * (std::cos(a1) * halfAcross) + along * (std::sin(a1) * halfAlong), edge);
            }
        };
        const auto onPlane = [&](const BikeVec& v) {
            const Vector3 w(v.x, v.y, v.z);
            return w - n * Vector3::Dot(w - g, n);
        };
        const bool upright = !bike.Overturned();
        if (upright) pool(onPlane(p), 0.72f, 1.10f, 105);
        for (int wheel = 0; wheel < 4; ++wheel) {
            const BikeVec hub = bike.WheelHub(wheel);
            const Vector3 w(hub.x, hub.y, hub.z);
            const float height = Vector3::Dot(w - g, n);
            if (height > QuadBike::WheelRadius + 0.25f) continue;   // in the air
            const float near = 1.0f - std::clamp((height - QuadBike::WheelRadius) / 0.25f, 0.0f, 1.0f);
            pool(onPlane(hub), wheel < 2 ? 0.16f : 0.19f, 0.30f, static_cast<int>(std::lround(120.0f * near)));
        }
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        if (!glowStaging_.empty()) {
            glow_->SetData(glowStaging_.data(), 0, static_cast<int>(glowStaging_.size()), SetDataOptions::Discard);
            device_.SetVertexBuffer(glow_.get());
            device_.setIndicesProperty(nullptr);
            glowEffect_->setWorldProperty(Matrix::getIdentityProperty());
            glowEffect_->setViewProperty(view);
            glowEffect_->setProjectionProperty(projection);
            glowEffect_->Apply();
            device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(glowStaging_.size() / 3));
            ++drawCalls_;
        }
        // Then the sun's: the silhouette slid along the light onto the ground plane.
        const auto toSun = sun_light::Current().shading;
        Vector3 l(toSun.x, toSun.y, toSun.z);
        if (sunStrength * fade < 0.01f || l.LengthSquared() < 1e-6f) return;
        l.Normalize();
        if (Vector3::Dot(l, n) < 0.2f) return;
        const Pose pose = PoseOf(bike);
        const int dynamicCount = BuildDynamic(bike, rider, distance < 30.0f);
        device_.setDepthStencilStateProperty(*shadowDepth_);
        float viewProjection[16];
        (view * projection).ToColumnMajor(viewProjection);
        shadow_->Apply();
        shadow_->SetUniformMat4("uViewProjection", viewProjection);
        shadow_->SetUniformVec4("uPlane", n.X, n.Y, n.Z, -Vector3::Dot(n, g));
        shadow_->SetUniformVec3("uToSun", l.X, l.Y, l.Z);
        shadow_->SetUniformFloat("uShadowAlpha", std::clamp(sunStrength, 0.0f, 1.0f) * 0.42f * fade);
        DrawAll(*shadow_, pose, dynamicCount, rider);
    }

    void QuadBikeRenderer::DrawLights(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                      const QuadBike& bike, const float nightness)
    {
        glowStaging_.clear();
        const auto add = [&](const BikeVec& lamp, const float size, const float r, const float g, const float b, const float a) {
            const BikeVec w = bike.ToWorld(lamp);
            const Vector3 centre(w.x, w.y, w.z);
            Vector3 toCamera = camera - centre;
            const float distance = toCamera.Length();
            if (distance < 0.2f) return;
            toCamera /= distance;
            Vector3 right = Vector3::Cross(Vector3::Up, toCamera);
            if (right.LengthSquared() < 1e-6f) right = Vector3::UnitX;
            right.Normalize();
            const Vector3 up = Vector3::Cross(toCamera, right);
            // facing the camera more, the lamp glares brighter
            const BikeVec f = bike.Forward();
            const float facing = std::clamp(-(toCamera.X * -f.x + toCamera.Y * -f.y + toCamera.Z * -f.z), 0.0f, 1.0f);
            const float k = a * (0.35f + 0.65f * facing);
            const auto c = [k](float v) { return static_cast<int>(std::lround(std::clamp(v * k, 0.0f, 1.0f) * 255.0f)); };
            const Color centreColour(c(r), c(g), c(b), 255), edge(0, 0, 0, 0);
            constexpr int segments = 16;
            for (int i = 0; i < segments; ++i) {
                const float a0 = 2.0f * Pi * static_cast<float>(i) / segments, a1 = 2.0f * Pi * static_cast<float>(i + 1) / segments;
                glowStaging_.emplace_back(centre + toCamera * 0.05f, centreColour);
                glowStaging_.emplace_back(centre + toCamera * 0.05f + (right * std::cos(a0) + up * std::sin(a0)) * size, edge);
                glowStaging_.emplace_back(centre + toCamera * 0.05f + (right * std::cos(a1) + up * std::sin(a1)) * size, edge);
            }
        };
        const float day = 1.0f - std::clamp(nightness, 0.0f, 1.0f);
        add(HeadlightLeft, 0.11f + 0.20f * nightness, 1.0f, 0.95f, 0.85f, 0.45f + 0.55f * nightness - 0.2f * day);
        add(HeadlightRight, 0.11f + 0.20f * nightness, 1.0f, 0.95f, 0.85f, 0.45f + 0.55f * nightness - 0.2f * day);
        // the tail lamp at night; braking, the brake light flares red behind by day and night
        if (braking_) add(Taillight, 0.27f + 0.05f * nightness, 1.0f, 0.10f, 0.06f, 0.9f + 0.1f * nightness);
        else if (nightness > 0.05f) add(Taillight, 0.16f, 1.0f, 0.10f, 0.06f, 0.7f * nightness);
        if (glowStaging_.empty()) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        glow_->SetData(glowStaging_.data(), 0, static_cast<int>(glowStaging_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(glow_.get());
        device_.setIndicesProperty(nullptr);
        glowEffect_->setWorldProperty(Matrix::getIdentityProperty());
        glowEffect_->setViewProperty(view);
        glowEffect_->setProjectionProperty(projection);
        glowEffect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(glowStaging_.size() / 3));
        ++drawCalls_;
    }
}
