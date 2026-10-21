#include "IslandSimulator/TurtleTrackRenderer.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
    // a deterministic 0..1 value per stroke, for its irregularity
    float Jitter(std::uint32_t seed) noexcept
    {
        seed ^= seed >> 16U; seed *= 0x7feb352dU;
        seed ^= seed >> 15U; seed *= 0x846ca68bU;
        seed ^= seed >> 16U;
        return static_cast<float>(seed & 0xffffU) / 65535.0f;
    }

    // A polyline through the trace pieces' centres, walked by distance along it.
    struct Path {
        std::vector<Vector3> points;
        std::vector<float> at;   // distance along the path to each point
        [[nodiscard]] float Length() const noexcept { return at.back(); }
        [[nodiscard]] std::size_t Segment(const float s) const noexcept
        {
            std::size_t index = 0;
            while (index + 2U < points.size() && s > at[index + 1U]) ++index;
            return index;
        }
        [[nodiscard]] Vector3 Point(const float s) const noexcept
        {
            const std::size_t i = Segment(s);
            const float span = std::max(at[i + 1U] - at[i], 1.0e-4f);
            return Vector3::Lerp(points[i], points[i + 1U], std::clamp((s - at[i]) / span, 0.0f, 1.0f));
        }
        [[nodiscard]] Vector3 Forward(const float s) const noexcept
        {
            const std::size_t i = Segment(s);
            Vector3 d = points[i + 1U] - points[i];
            d.Y = 0.0f;
            return d.LengthSquared() > 1.0e-8f ? Vector3::Normalize(d) : Vector3::UnitZ;
        }
    };
}

TurtleTrackRenderer::TurtleTrackRenderer(GraphicsDevice& device,const std::span<const BeachDetailPlacement> details,
                                         const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    const auto onGround = [&ground](const Vector3& point) {
        return Vector3(point.X, ground.Height(point.X, point.Z) + 0.014f, point.Z);
    };
    // The layout lays each nest's trace pieces in order, seaward first, 1.34 m apart.
    std::vector<std::vector<Vector3>> groups;
    for (const BeachDetailPlacement& detail : details) {
        if (!detail.turtleTrace || detail.turtleNest) continue;   // the nests are drawn apart
        const Vector3 piece(detail.x, detail.y + 0.004f, detail.z);
        if (groups.empty() || Vector3::DistanceSquared(groups.back().back(), piece) > 2.5f * 2.5f)
            groups.emplace_back();
        groups.back().push_back(piece);
    }
    std::size_t total = 0U;
    // The nests: a churned patch of dry sand, darker in the body pit's rim, with clods of thrown
    // sand scattered round it, all soft-edged (the opaque fourteen-sided stain had a hard rim).
    std::uint32_t nestIndex = 0U;
    for (const BeachDetailPlacement& detail : details) {
        if (!detail.turtleNest) continue;
        const auto seed = 4099U + 31U * nestIndex++;
        Track nest;
        const Vector3 centre(detail.x, detail.y + 0.004f, detail.z);
        const float size = detail.scale * 0.85f;
        nest.centre = centre;
        nest.radius = size * 1.8f;
        const auto shade = [](const float alpha) {
            return Color(0, 0, 0, static_cast<int>(std::lround(std::clamp(alpha, 0.0f, 255.0f))));
        };
        const auto at = [&](const float angle, const float radius) {
            return onGround(centre + Vector3(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius * 0.82f));
        };
        constexpr int Segments = 28;
        // rings out from the middle: the pit's floor, its darker rim, the churned apron, the edge
        constexpr std::array<float, 4> RingRadius{0.0f, 0.42f, 0.62f, 1.0f};
        constexpr std::array<float, 4> RingAlpha{30.0f, 56.0f, 38.0f, 0.0f};
        for (int segment = 0; segment < Segments; ++segment) {
            const float a0 = 6.2831853f * static_cast<float>(segment) / Segments;
            const float a1 = 6.2831853f * static_cast<float>(segment + 1) / Segments;
            const auto edge = [&](const float angle) {
                return 1.0f + 0.16f * std::sin(angle * 3.0f + static_cast<float>(seed % 11U))
                    + 0.08f * std::sin(angle * 7.0f + static_cast<float>(seed % 5U));
            };
            for (std::size_t ring = 0; ring + 1U < RingRadius.size(); ++ring) {
                const float r0 = RingRadius[ring] * size, r1 = RingRadius[ring + 1U] * size;
                const float e00 = ring == 0U ? 1.0f : edge(a0), e01 = ring == 0U ? 1.0f : edge(a1);
                const float e10 = edge(a0), e11 = edge(a1);
                nest.vertices.insert(nest.vertices.end(), {
                    VertexPositionColor(at(a0, r0 * e00), shade(RingAlpha[ring])),
                    VertexPositionColor(at(a0, r1 * e10), shade(RingAlpha[ring + 1U])),
                    VertexPositionColor(at(a1, r1 * e11), shade(RingAlpha[ring + 1U])),
                    VertexPositionColor(at(a0, r0 * e00), shade(RingAlpha[ring])),
                    VertexPositionColor(at(a1, r1 * e11), shade(RingAlpha[ring + 1U])),
                    VertexPositionColor(at(a1, r0 * e01), shade(RingAlpha[ring]))});
            }
        }
        // thrown clods, irregular little shadows out past the apron
        for (int clod = 0; clod < 22; ++clod) {
            const auto clodSeed = seed * 53U + static_cast<std::uint32_t>(clod) * 389U;
            const float angle = 6.2831853f * Jitter(clodSeed);
            const float reach = size * (0.85f + 0.65f * Jitter(clodSeed * 3U));
            const float radius = 0.035f + 0.05f * Jitter(clodSeed * 5U);
            const float strength = 34.0f + 26.0f * Jitter(clodSeed * 7U);
            const Vector3 middle = at(angle, reach);
            constexpr int Sides = 6;
            for (int side = 0; side < Sides; ++side) {
                const float b0 = 6.2831853f * static_cast<float>(side) / Sides + angle;
                const float b1 = 6.2831853f * static_cast<float>(side + 1) / Sides + angle;
                const float stretch0 = 1.0f + 0.4f * std::cos(b0 - angle), stretch1 = 1.0f + 0.4f * std::cos(b1 - angle);
                nest.vertices.insert(nest.vertices.end(), {
                    VertexPositionColor(onGround(middle), shade(strength)),
                    VertexPositionColor(onGround(middle + Vector3(std::cos(b0), 0.0f, std::sin(b0)) * (radius * stretch0)), shade(0.0f)),
                    VertexPositionColor(onGround(middle + Vector3(std::cos(b1), 0.0f, std::sin(b1)) * (radius * stretch1)), shade(0.0f))});
            }
        }
        total += nest.vertices.size();
        tracks_.push_back(std::move(nest));
    }
    for (std::size_t group = 0; group < groups.size(); ++group) {
        const auto& pieces = groups[group];
        if (pieces.size() < 2U) continue;
        // half a piece's spacing beyond either end, as the pieces once covered
        Path path;
        const Vector3 firstStep = pieces[1] - pieces[0];
        const Vector3 lastStep = pieces[pieces.size() - 1U] - pieces[pieces.size() - 2U];
        path.points.push_back(pieces.front() - firstStep * 0.5f);
        path.points.insert(path.points.end(), pieces.begin(), pieces.end());
        path.points.push_back(pieces.back() + lastStep * 0.5f);
        path.at.push_back(0.0f);
        for (std::size_t i = 1; i < path.points.size(); ++i) {
            Vector3 d = path.points[i] - path.points[i - 1U];
            d.Y = 0.0f;
            path.at.push_back(path.at.back() + d.Length());
        }
        const float length = path.Length();
        const auto seed = static_cast<std::uint32_t>(group * 7919U + 17U);
        Track track;
        track.centre = path.Point(length * 0.5f);
        track.radius = length * 0.5f + 1.0f;
        const auto point = [&](const float s, const float across) {
            const float clamped = std::clamp(s, 0.0f, length);
            const Vector3 forward = path.Forward(clamped);
            return onGround(path.Point(clamped) + Vector3(-forward.Z, 0.0f, forward.X) * across);
        };
        // the crawl fades into the wet sand seaward and into the churned nest inland
        const auto endFade = [length](const float s) {
            return std::clamp(s / 0.9f, 0.0f, 1.0f) * std::clamp((length - s) / 0.45f, 0.0f, 1.0f);
        };
        const auto quad = [&track](const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d,
                                   const float alphaA, const float alphaB, const float alphaC, const float alphaD) {
            const auto shade = [](const float alpha) {
                return Color(0, 0, 0, static_cast<int>(std::lround(std::clamp(alpha, 0.0f, 255.0f))));
            };
            track.vertices.insert(track.vertices.end(), {
                VertexPositionColor(a, shade(alphaA)), VertexPositionColor(b, shade(alphaB)),
                VertexPositionColor(c, shade(alphaC)), VertexPositionColor(a, shade(alphaA)),
                VertexPositionColor(c, shade(alphaC)), VertexPositionColor(d, shade(alphaD))});
        };
        // The plastron's trough: darker within, soft at either edge, its width wandering a little.
        constexpr std::array<float, 6> TroughAcross{-0.62f, -0.44f, -0.18f, 0.18f, 0.44f, 0.62f};
        constexpr std::array<float, 6> TroughAlpha{0.0f, 24.0f, 34.0f, 34.0f, 24.0f, 0.0f};
        const int rows = std::max(2, static_cast<int>(std::ceil(length / 0.4f)));
        for (int row = 0; row < rows; ++row) {
            const float s0 = length * static_cast<float>(row) / static_cast<float>(rows);
            const float s1 = length * static_cast<float>(row + 1) / static_cast<float>(rows);
            const float w0 = 1.0f + 0.06f * std::sin(s0 * 2.3f + static_cast<float>(seed % 7U));
            const float w1 = 1.0f + 0.06f * std::sin(s1 * 2.3f + static_cast<float>(seed % 7U));
            for (std::size_t column = 0; column + 1U < TroughAcross.size(); ++column) {
                quad(point(s0, TroughAcross[column] * w0), point(s1, TroughAcross[column] * w1),
                     point(s1, TroughAcross[column + 1U] * w1), point(s0, TroughAcross[column + 1U] * w0),
                     TroughAlpha[column] * endFade(s0), TroughAlpha[column] * endFade(s1),
                     TroughAlpha[column + 1U] * endFade(s1), TroughAlpha[column + 1U] * endFade(s0));
            }
            // the tail's faint drag, wavering down the middle
            const float x0 = 0.03f * std::sin(s0 * 3.1f + static_cast<float>(seed % 5U));
            const float x1 = 0.03f * std::sin(s1 * 3.1f + static_cast<float>(seed % 5U));
            for (const float side : {-1.0f, 1.0f}) {
                quad(point(s0, x0), point(s1, x1), point(s1, x1 + side * 0.035f), point(s0, x0 + side * 0.035f),
                     20.0f * endFade(s0), 20.0f * endFade(s1), 0.0f, 0.0f);
            }
        }
        // The flippers' gouges, paired (a green turtle heaves with both at once), every stroke a
        // little different, the outer end swept back toward the sea; now and then one is lost.
        int stroke = 0;
        for (float s = 0.35f; s < length - 0.3f; ++stroke) {
            const auto strokeSeed = seed * 131U + static_cast<std::uint32_t>(stroke) * 977U;
            for (const float side : {-1.0f, 1.0f}) {
                const auto sideSeed = strokeSeed + (side > 0.0f ? 1U : 2U);
                if (Jitter(sideSeed * 3U) < 0.08f) continue;
                const float inner = 0.17f + 0.03f * Jitter(sideSeed * 5U);
                const float outer = 0.53f + 0.07f * Jitter(sideSeed * 7U);
                const float sweep = 0.08f + 0.06f * Jitter(sideSeed * 11U);
                const float width = 0.045f + 0.025f * Jitter(sideSeed * 13U);
                const float strength = (72.0f + 22.0f * Jitter(sideSeed * 17U)) * endFade(s);
                const std::array<float, 3> along{0.0f, 0.5f, 1.0f};
                const std::array<float, 3> taper{0.55f, 1.0f, 0.35f};
                std::array<std::array<Vector3, 3>, 3> grid{};
                for (std::size_t k = 0; k < along.size(); ++k) {
                    const float t = along[k];
                    const float across = side * (inner + (outer - inner) * t);
                    // bowed: the stroke's middle a little ahead of the straight line
                    const float back = sweep * t - 0.025f * std::sin(t * 3.14159265f);
                    const float half = width * (0.75f + 0.25f * std::sin(t * 3.14159265f));
                    grid[k][0] = point(s - back - half, across);
                    grid[k][1] = point(s - back, across);
                    grid[k][2] = point(s - back + half, across);
                }
                for (std::size_t k = 0; k + 1U < along.size(); ++k) {
                    for (std::size_t c = 0; c < 2U; ++c) {
                        const float a0 = c == 0U ? 0.0f : strength * taper[k];
                        const float a1 = c == 0U ? strength * taper[k] : 0.0f;
                        const float b0 = c == 0U ? 0.0f : strength * taper[k + 1U];
                        const float b1 = c == 0U ? strength * taper[k + 1U] : 0.0f;
                        quad(grid[k][c], grid[k + 1U][c], grid[k + 1U][c + 1U], grid[k][c + 1U],
                             a0, b0, b1, a1);
                    }
                }
            }
            s += 0.25f + 0.07f * Jitter(strokeSeed * 19U);
        }
        total += track.vertices.size();
        tracks_.push_back(std::move(track));
    }
    vertices_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(std::max<std::size_t>(total, 6U)), BufferUsage::WriteOnly);
    staging_.reserve(total);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    const std::string message = "island-simulator: " + std::to_string(tracks_.size())
        + " nesting turtles' crawls and nests: soft plastron troughs with a tail drag and irregular paired flipper gouges, churned body pits with thrown clods";
    std::cout << message << '\n';
    MirrorRuntimeDiagnosticToPlatformLog(message.c_str());
}

void TurtleTrackRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera)
{
    drawCalls_ = 0;
    visible_ = 0;
    staging_.clear();
    for (const Track& track : tracks_) {
        const float distance = std::max(0.0f, Vector3::Distance(track.centre, camera) - track.radius);
        const float fade = 1.0f - std::clamp((distance - FadeStart) / (DrawDistance - FadeStart), 0.0f, 1.0f);
        if (fade <= 0.0f) continue;
        for (const VertexPositionColor& vertex : track.vertices) {
            staging_.emplace_back(vertex.Position, Color(0, 0, 0, static_cast<int>(std::lround(
                static_cast<float>(vertex.Color.getAProperty()) * fade))));
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
    device_.DrawPrimitives(PrimitiveType::TriangleList, 0, static_cast<int>(staging_.size() / 3U));
    ++drawCalls_;
}
}
