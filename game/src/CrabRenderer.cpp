#include "IslandSimulator/VegetationFade.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/CrabRenderer.hpp"

#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string_view>

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

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float NearLodDistance = 38.0f;
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        constexpr int CarapaceSegments = 18;
        constexpr int NearCrabVertexCount = 2112;
        constexpr int FarCrabVertexCount = 12;
        constexpr int MaximumVerticesPerCrab = NearCrabVertexCount;
        constexpr int MaximumBurrowVertices = 1024;
        constexpr int HermitCrabVertexCount = 3120 - 732 + 30 * 9 * 6;   // the near shell a whorl tube
        // Past this a hermit crab is its shell, a few pixels across: a six-sided shell alone
        // (out to the crabs' 92 m every one was built whole, legs, eyes and spiral marks).
        constexpr float HermitDetailDistance = 18.0f;
        constexpr int FarHermitCrabVertexCount = 5 * 6 * 6 + 2 * 6 * 3;
        constexpr int MaximumHermitCrabVertices =
            static_cast<int>(HermitCrabGroup::Count) * HermitCrabVertexCount;
        constexpr int CoconutCrabVertexCount = 3000;
        constexpr int MaximumCoconutCrabVertices =
            static_cast<int>(CoconutCrabGroup::Count) * CoconutCrabVertexCount;
        constexpr int MaximumVertexCount = static_cast<int>(CrabColony::CrabCount)
            * MaximumVerticesPerCrab + MaximumBurrowVertices
            + MaximumHermitCrabVertices + MaximumCoconutCrabVertices;
        constexpr std::string_view CrabDiagnostic =
            "island-simulator: beach crabs use rounded carapaces, volumetric jointed legs, eye stalks, and one culled batch";
        constexpr std::string_view CrabDetailDiagnostic =
            "island-simulator: near crabs use tapered toes, socketed eyes, and solid pincer fingers";
        constexpr std::string_view CrabSilhouetteDiagnostic =
            "island-simulator: near crabs use flattened lateral carapaces, sturdy legs, and enlarged claw palms";
        constexpr std::string_view CrabSurfaceDiagnostic =
            "island-simulator: near crabs use eighteen-sided smooth-lit mottled carapaces and solid tapered legs";
        constexpr std::string_view CrabAnatomyDiagnostic =
            "island-simulator: near crabs use low broad shells with retained lateral carapace corners";
        constexpr std::string_view CrabUndersideDiagnostic =
            "island-simulator: near crab carapaces close around a shallow dark underside instead of ending at an open rim";
        constexpr std::string_view CrabClawDiagnostic =
            "island-simulator: near beach crabs use rounded claw palms and solid tapered forearms";
        constexpr std::string_view CrabHabitatDiagnostic =
            "island-simulator: six debris-linked crab colonies share visible sand burrows";
        constexpr std::string_view CrabSpacingDiagnostic =
            "island-simulator: live beach crabs keep 0.72m centre separation while foraging";
        constexpr std::string_view HermitCrabDiagnostic =
            "island-simulator: ten sheltered-beach hermit crabs pause, forage, and retreat in the shared crab batch";
        constexpr std::string_view HermitCrabAnatomyDiagnostic =
            "island-simulator: hermit crabs use rounded backswept spiral shells with fitted apertures, rounded bodies, projecting eyes, bent antennae, six solid three-segment legs, and unequal claws";
        constexpr std::string_view CoconutCrabDiagnostic =
            "island-simulator: six coastal palm-zone coconut crabs rest, forage, and retreat in the shared crab batch";
        constexpr std::string_view CoconutCrabAnatomyDiagnostic =
            "island-simulator: coconut crabs use deep closed cephalothoraxes, tucked segmented abdomens, four-joint walking legs, and massive unequal claws";
        constexpr std::string_view CoconutCrabProfileDiagnostic =
            "island-simulator: coconut crabs use elevated compact bodies, planted articulated legs, and fitted dorsal armour markings";
        constexpr std::string_view CoconutCrabClawDiagnostic =
            "island-simulator: near coconut crabs use rounded unequal claw palms over separated solid pincer fingers";
        constexpr std::string_view CoconutCrabColourDiagnostic =
            "island-simulator: coconut crabs use cool blue-grey, violet-brown, slate, and warm umber limb armour with a front-raised terrestrial stance";

        struct CrabPalette
        {
            Color shellTop;
            Color shellHighlight;
            Color shellSide;
            Color limb;
            Color joint;
            Color claw;
        };

        const std::array<CrabPalette, 4> Palettes = {{
            {Color(139, 61, 39, 255), Color(167, 79, 50, 255),
             Color(91, 43, 33, 255), Color(116, 50, 35, 255),
             Color(72, 35, 30, 255), Color(171, 80, 50, 255)},
            {Color(177, 91, 46, 255), Color(208, 119, 63, 255),
             Color(111, 57, 38, 255), Color(145, 70, 42, 255),
             Color(91, 49, 36, 255), Color(211, 119, 61, 255)},
            {Color(111, 72, 52, 255), Color(143, 96, 67, 255),
             Color(75, 53, 42, 255), Color(92, 61, 45, 255),
             Color(61, 43, 36, 255), Color(148, 93, 60, 255)},
            {Color(187, 126, 80, 255), Color(218, 158, 102, 255),
             Color(117, 79, 58, 255), Color(153, 98, 65, 255),
             Color(96, 65, 51, 255), Color(220, 154, 96, 255)}
        }};

        const std::array<Color, 4> CoconutBodyColours = {{
            Color(112, 112, 142, 255), Color(135, 91, 106, 255),
            Color(84, 116, 126, 255), Color(139, 111, 82, 255)
        }};

        void AddTriangle(std::vector<VertexPositionColor>& vertices,
                         const Vector3& a, const Vector3& b, const Vector3& c,
                         const Color& color)
        {
            const Color lit = wildlife_render::FacetColor(a, b, c, color, true);
            vertices.emplace_back(a, lit);
            vertices.emplace_back(b, lit);
            vertices.emplace_back(c, lit);
        }

        void AddDoubleSidedTriangle(std::vector<VertexPositionColor>& vertices,
                                    const Vector3& a, const Vector3& b, const Vector3& c,
                                    const Color& color)
        {
            AddTriangle(vertices, a, b, c, color);
            AddTriangle(vertices, c, b, a, color);
        }

        void AddQuad(std::vector<VertexPositionColor>& vertices,
                     const Vector3& a, const Vector3& b,
                     const Vector3& c, const Vector3& d, const Color& color)
        {
            AddTriangle(vertices, a, b, c, color);
            AddTriangle(vertices, a, c, d, color);
        }

        // One-sided light from a surface's own normal, with FacetColor's brightness terms.
        [[nodiscard]] VertexPositionColor SmoothLit(const Vector3& point, const Vector3& normal,
                                                    const Color& base)
        {
            const float direct = std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
            return VertexPositionColor(point, wildlife_render::ScaleColor(
                base, .72f + .30f * direct + .06f * std::max(0.0f, normal.Y)));
        }

        void AddGradientTriangle(std::vector<VertexPositionColor>& vertices,
                                 const Vector3& a, const Vector3& b, const Vector3& c,
                                 const Color& colorA, const Color& colorB,
                                 const Color& colorC)
        {
            vertices.emplace_back(a, colorA);
            vertices.emplace_back(b, colorB);
            vertices.emplace_back(c, colorC);
        }

        void AddGradientQuad(std::vector<VertexPositionColor>& vertices,
                             const Vector3& a, const Vector3& b,
                             const Vector3& c, const Vector3& d,
                             const Color& colorA, const Color& colorB,
                             const Color& colorC, const Color& colorD)
        {
            AddGradientTriangle(vertices, a, b, c, colorA, colorB, colorC);
            AddGradientTriangle(vertices, a, c, d, colorA, colorC, colorD);
        }

        void AppendCrossedSegment(std::vector<VertexPositionColor>& vertices,
                                  const Vector3& a, const Vector3& b,
                                  const float halfWidth, const Color& color)
        {
            const Vector3 delta = b - a;
            const float horizontalLength = std::hypot(delta.X, delta.Z);
            const Vector3 side = horizontalLength > 0.0001f
                ? Vector3(-delta.Z * halfWidth / horizontalLength, 0.0f,
                          delta.X * halfWidth / horizontalLength)
                : Vector3(halfWidth, 0.0f, 0.0f);
            const Vector3 vertical(0.0f, halfWidth * 0.62f, 0.0f);
            AddQuad(vertices, a + side, a - side, b - side, b + side, color);
            AddQuad(vertices, a + vertical, a - vertical,
                    b - vertical, b + vertical, color);
        }

        void AppendTaperedLimbSegment(std::vector<VertexPositionColor>& vertices,
                                      const Vector3& a, const Vector3& b,
                                      const float startHalfWidth,
                                      const float endHalfWidth, const Color& color)
        {
            const Vector3 delta = b - a;
            const float horizontalLength = std::hypot(delta.X, delta.Z);
            const Vector3 side = horizontalLength > .0001f
                ? Vector3(-delta.Z / horizontalLength, 0.0f, delta.X / horizontalLength)
                : Vector3::Right;
            // A six-sided elliptical tube (about the old square prism's girth), lit per vertex
            // from its own round normals: the prism, one flat colour a face, made every leg and
            // arm a faceted stick.
            constexpr int Sides = 6;
            constexpr float Across = 1.10f, Upright = .77f;
            const auto ring = [&](const Vector3& centre, const float halfWidth, const int index) {
                const float angle = TwoPi * static_cast<float>(index) / static_cast<float>(Sides);
                const float c = std::cos(angle), s = std::sin(angle);
                const Vector3 normal = Vector3::Normalize(side * (c / Across) + Vector3::Up * (s / Upright));
                const float direct = std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
                return VertexPositionColor(
                    centre + side * (c * Across * halfWidth) + Vector3::Up * (s * Upright * halfWidth),
                    wildlife_render::ScaleColor(color, .72f + .30f * direct + .06f * std::max(0.0f, normal.Y)));
            };
            for (int index = 0; index < Sides; ++index) {
                const VertexPositionColor a0 = ring(a, startHalfWidth, index);
                const VertexPositionColor a1 = ring(a, startHalfWidth, index + 1);
                const VertexPositionColor b0 = ring(b, endHalfWidth, index);
                const VertexPositionColor b1 = ring(b, endHalfWidth, index + 1);
                vertices.insert(vertices.end(), {a0, b0, b1, a0, b1, a1});
            }
        }

        [[nodiscard]] Vector3 LocalPoint(const Vector3& centre, const Vector3& right,
                                         const Vector3& up, const Vector3& forward,
                                         const float x, const float y, const float z,
                                         const float scale)
        {
            return centre + right * (x * scale) + up * (y * scale)
                + forward * (z * scale);
        }

        [[nodiscard]] Color SmoothShellColor(const CrabPalette& palette,
                                             const Vector3& right, const Vector3& up,
                                             const Vector3& forward, const float angle,
                                             const float vertical)
        {
            const float horizontal = std::sqrt(std::max(0.0f, 1.0f - vertical * vertical));
            const Vector3 normal = Vector3::Normalize(
                right * (std::cos(angle) * horizontal)
                + forward * (std::sin(angle) * horizontal) + up * vertical);
            const float direct = std::max(
                0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
            const Color base = Color::Lerp(palette.shellSide, palette.shellTop,
                                           std::clamp(vertical * 1.08f, 0.0f, 1.0f));
            return wildlife_render::ScaleColor(base, .78f + .25f * direct + .05f * vertical);
        }

        [[nodiscard]] float CarapaceSurfaceHeight(const float x, const float z)
        {
            const float ellipse = std::clamp(
                1.0f - (x * x) / (.36f * .36f) - (z * z) / (.33f * .33f),
                0.0f, 1.0f);
            return .070f + .058f * std::sqrt(ellipse);
        }

        void AppendOctahedron(std::vector<VertexPositionColor>& vertices,
                              const Vector3& centre, const Vector3& right,
                              const Vector3& up, const Vector3& forward,
                              const float halfX, const float halfY, const float halfZ,
                              const Color& topColor, const Color& sideColor)
        {
            const Vector3 top = centre + up * halfY;
            const Vector3 bottom = centre - up * halfY;
            const Vector3 left = centre - right * halfX;
            const Vector3 rightPoint = centre + right * halfX;
            const Vector3 front = centre + forward * halfZ;
            const Vector3 back = centre - forward * halfZ;
            // Each corner lit from its own axis, so a knee or an eye shades round rather than
            // as eight flat facets.
            const auto lit = [&](const Vector3& point, const Vector3& normal, const Color& base) {
                return SmoothLit(point, normal, base);
            };
            const std::array<std::array<Vector3, 2>, 4> ring{{
                {front, forward}, {rightPoint, right}, {back, -forward}, {left, -right}}};
            for (std::size_t corner = 0; corner < ring.size(); ++corner) {
                const auto& a = ring[corner];
                const auto& b = ring[(corner + 1U) % ring.size()];
                vertices.insert(vertices.end(), {lit(top, up, topColor), lit(a[0], a[1], topColor),
                                                 lit(b[0], b[1], topColor)});
            }
            for (std::size_t corner = 0; corner < ring.size(); ++corner) {
                const auto& a = ring[corner];
                const auto& b = ring[(corner + 1U) % ring.size()];
                vertices.insert(vertices.end(), {lit(bottom, -up, sideColor), lit(b[0], b[1], sideColor),
                                                 lit(a[0], a[1], sideColor)});
            }
        }

        void AppendRoundedCrabPalm(std::vector<VertexPositionColor>& vertices,
                                   const Vector3& centre, const Vector3& right,
                                   const Vector3& up, const Vector3& forward,
                                   const float halfX, const float halfY,
                                   const float halfZ, const Color& topColor,
                                   const Color& sideColor)
        {
            constexpr int Segments = 12;
            const Vector3 top = centre + up * halfY;
            const Vector3 bottom = centre - up * halfY;
            // Three rings of the inscribed ellipsoid (two, at +-0.45, made a drum with conical
            // lids: a claw read as a round cap on a stalk).
            constexpr std::array<std::array<float, 2>, 3> Rings{{{.6f, .8f}, {0.0f, 1.0f}, {-.6f, .8f}}};
            const auto ringPoint = [&](const std::size_t ring, const int segment) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(Segments);
                return centre + right * (std::cos(angle) * halfX * Rings[ring][1])
                    + forward * (std::sin(angle) * halfZ * Rings[ring][1])
                    + up * (Rings[ring][0] * halfY);
            };
            // Lit per vertex from the ellipsoid's own normals (one-sided), not one flat colour a
            // facet: the claw's palm and the body read rounded.
            const auto lit = [&](const Vector3& point, const Color& base) {
                const Vector3 d = point - centre;
                const Vector3 normal = Vector3::Normalize(
                    right * (Vector3::Dot(d, right) / (halfX * halfX))
                    + up * (Vector3::Dot(d, up) / (halfY * halfY))
                    + forward * (Vector3::Dot(d, forward) / (halfZ * halfZ)));
                return SmoothLit(point, normal, base);
            };
            // the upper half in the top colour, the lower in the side colour
            for (int segment = 0; segment < Segments; ++segment) {
                const int next = (segment + 1) % Segments;
                const Vector3 upper = ringPoint(0U, segment), upperNext = ringPoint(0U, next);
                const Vector3 middle = ringPoint(1U, segment), middleNext = ringPoint(1U, next);
                const Vector3 lower = ringPoint(2U, segment), lowerNext = ringPoint(2U, next);
                vertices.insert(vertices.end(), {lit(top, topColor), lit(upperNext, topColor), lit(upper, topColor),
                    lit(upper, topColor), lit(upperNext, topColor), lit(middleNext, topColor),
                    lit(upper, topColor), lit(middleNext, topColor), lit(middle, topColor),
                    lit(middle, sideColor), lit(middleNext, sideColor), lit(lowerNext, sideColor),
                    lit(middle, sideColor), lit(lowerNext, sideColor), lit(lower, sideColor),
                    lit(bottom, sideColor), lit(lower, sideColor), lit(lowerNext, sideColor)});
            }
        }

        void AppendRoundedCoconutPalm(std::vector<VertexPositionColor>& vertices,
                                      const Vector3& centre, const Vector3& right,
                                      const Vector3& up, const Vector3& forward,
                                      const float halfX, const float halfY,
                                      const float halfZ, const Color& topColor,
                                      const Color& sideColor)
        {
            constexpr int Segments = 10;
            constexpr std::array<float, 5> RingHeights = {{-.75f, -.38f, 0.0f, .38f, .75f}};
            constexpr std::array<float, 5> RingRadii = {{.66f, .92f, 1.0f, .92f, .66f}};
            const auto point = [&](const std::size_t ring, const int segment) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(Segments);
                return centre + right * (std::cos(angle) * RingRadii[ring] * halfX)
                    + forward * (std::sin(angle) * RingRadii[ring] * halfZ)
                    + up * (RingHeights[ring] * halfY);
            };
            const auto color = [&](const std::size_t ring, const int segment) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(Segments);
                const Vector3 normal = Vector3::Normalize(
                    right * (std::cos(angle) * RingRadii[ring] / halfX)
                    + forward * (std::sin(angle) * RingRadii[ring] / halfZ)
                    + up * (RingHeights[ring] / halfY));
                const float direct = std::max(
                    0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
                const Color base = Color::Lerp(
                    sideColor, topColor, std::clamp(.50f + RingHeights[ring] * .52f,
                                                     0.0f, 1.0f));
                return wildlife_render::ScaleColor(base, .76f + .28f * direct);
            };

            const Vector3 bottom = centre - up * halfY;
            const Vector3 top = centre + up * halfY;
            const Color bottomColor = wildlife_render::ScaleColor(sideColor, .72f);
            const Color topCapColor = wildlife_render::ScaleColor(topColor, 1.03f);
            for (int segment = 0; segment < Segments; ++segment) {
                const int next = (segment + 1) % Segments;
                AddGradientTriangle(vertices, bottom, point(0U, segment),
                                    point(0U, next), bottomColor,
                                    color(0U, segment), color(0U, next));
                for (std::size_t ring = 0; ring + 1U < RingHeights.size(); ++ring) {
                    AddGradientQuad(vertices, point(ring, segment),
                                    point(ring + 1U, segment),
                                    point(ring + 1U, next), point(ring, next),
                                    color(ring, segment), color(ring + 1U, segment),
                                    color(ring + 1U, next), color(ring, next));
                }
                const std::size_t last=RingHeights.size()-1U;
                AddGradientTriangle(vertices, top, point(last, next), point(last, segment),
                                    topCapColor, color(last, next), color(last, segment));
            }
        }

        void AppendPrismaticFinger(std::vector<VertexPositionColor>& vertices,
                                   const Vector3& a, const Vector3& b,
                                   const Vector3& tip, float halfThickness,
                                   const Color& faceColor, const Color& edgeColor);

        void AppendHermitCrab(std::vector<VertexPositionColor>& vertices,
                              const HermitCrabState& crab, const bool far = false)
        {
            const std::array<Color, 4> ShellColors = {{
                Color(150, 111, 70, 255), Color(178, 139, 92, 255),
                Color(118, 99, 72, 255), Color(190, 158, 112, 255)}};
            const std::array<Color, 4> LimbColors = {{
                Color(132, 67, 45, 255), Color(166, 89, 55, 255),
                Color(103, 71, 52, 255), Color(176, 112, 75, 255)}};
            const Color shell = ShellColors[crab.shellVariant % ShellColors.size()];
            const Color shellShade = Color::Lerp(shell, Color(49, 39, 31, 255), .38f);
            const Color shellRidge = Color::Lerp(shell, Color(226, 205, 160, 255), .28f);
            const Color limb = LimbColors[crab.shellVariant % LimbColors.size()];
            const Color joint = Color::Lerp(limb, Color(40, 29, 25, 255), .42f);
            const Color eye(16, 18, 15, 255);
            const float scale = crab.scale;
            const Vector3 forward(std::cos(crab.headingRadians), 0.0f,
                                  std::sin(crab.headingRadians));
            const Vector3 right(-forward.Z, 0.0f, forward.X);
            const Vector3 up = Vector3::Up;
            // Withdrawn, the body and its limbs draw back through the aperture into the
            // shell (which stays where it is) and the legs tuck in; the claws, carried with
            // the body, come to close the opening.
            const float withdraw = std::clamp(crab.withdrawAmount, 0.0f, 1.0f);
            const Vector3 shellBase(crab.x, crab.y, crab.z);
            const Vector3 centre = shellBase - forward * (withdraw * .17f * scale);
            const float limbScale = scale * (1.0f - .55f * withdraw);

            // A tilted six-station shell replaces the former vertical cone. Its
            // broad lower whorl encloses the body and its restrained apex sweeps
            // rearward, matching the silhouette of a carried gastropod shell.
            const int RingSegments = far ? 6 : 16;
            constexpr std::array<float, 6> ShellStations =
                {{-.145f, -.090f, -.015f, .070f, .145f, .215f}};
            constexpr std::array<float, 6> ShellRadii =
                {{.050f, .125f, .158f, .150f, .095f, .018f}};
            // Carried, the shell rides a little higher and rocks with the steps; set down
            // at a pause it rests on the sand again.
            const float carry = std::clamp(crab.shellCarry, 0.0f, 1.0f);
            const float rock = carry * crab.legPhase * .07f;
            const Vector3 shellAxis = Vector3::Normalize(up * .84f - forward * (.54f + rock) + right * (rock * .6f));
            const Vector3 shellOther = Vector3::Normalize(Vector3::Cross(shellAxis, right));
            const Vector3 shellCentre = shellBase - forward * (.070f * scale)
                + up * ((.145f + .030f * carry) * scale);
            const auto shellPoint = [&](const std::size_t station, const int segment) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(RingSegments);
                return shellCentre + shellAxis * (ShellStations[station] * scale)
                    + right * (std::cos(angle) * ShellRadii[station] * scale)
                    + shellOther * (std::sin(angle) * ShellRadii[station] * .92f * scale);
            };
            // Lit from the whorl's own normals (the radius's slope along the axis tilts them),
            // one-sided, instead of one brightness per facet: the shell rounds off with a
            // shadowed flank, while each whorl keeps its own band colour edge to edge.
            const auto shellVertex = [&](const std::size_t station, const int segment,
                                         const Color& base) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(RingSegments);
                const std::size_t before = station == 0U ? 0U : station - 1U;
                const std::size_t after = std::min<std::size_t>(station + 1U, ShellStations.size() - 1U);
                const float slope = (ShellRadii[after] - ShellRadii[before])
                    / (ShellStations[after] - ShellStations[before]);
                const Vector3 normal = Vector3::Normalize(right * std::cos(angle)
                    + shellOther * (std::sin(angle) / .92f) - shellAxis * slope);
                const float direct = std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
                return VertexPositionColor(shellPoint(station, segment), wildlife_render::ScaleColor(
                    base, .72f + .30f * direct + .06f * std::max(0.0f, normal.Y)));
            };
            if (far) {
                for (std::size_t station = 0; station + 1U < ShellStations.size(); ++station) {
                    for (int segment = 0; segment < RingSegments; ++segment) {
                        const int next = (segment + 1) % RingSegments;
                        const Color band = station % 2U == 0U ? shell : shellShade;
                        const Color base = segment < RingSegments / 2
                            ? Color::Lerp(band, shellRidge, .10f) : band;
                        const VertexPositionColor a = shellVertex(station, segment, base);
                        const VertexPositionColor b = shellVertex(station + 1U, segment, base);
                        const VertexPositionColor c = shellVertex(station + 1U, next, base);
                        const VertexPositionColor d = shellVertex(station, next, base);
                        vertices.insert(vertices.end(), {a, b, c, a, c, d});
                    }
                }
                const Vector3 shellRear = shellCentre
                    + shellAxis * (ShellStations.front() * scale);
                const Vector3 shellApex = shellCentre
                    + shellAxis * (ShellStations.back() * scale);
                for (int segment = 0; segment < RingSegments; ++segment) {
                    const int next = (segment + 1) % RingSegments;
                    AddTriangle(vertices, shellRear, shellPoint(0U, next),
                                shellPoint(0U, segment), shellShade);
                    AddTriangle(vertices, shellApex, shellPoint(5U, segment),
                                shellPoint(5U, next), shellRidge);
                }
                return;
            }
            // Near, a gastropod's shell: a whorl tube winding three turns up to the spire, each
            // whorl 2.1 times the one before it and lapping over it, the aperture at the front
            // where the crab comes out; colour bands spiral along the whorls and the sutures
            // between them lie in shade. (A body of revolution banded in rings, with a wire
            // spiral laid on its side, read as a beehive.)
            {
                constexpr int Turns = 3, RingsPerTurn = 10, TubeSides = 9;
                constexpr int Rings = Turns * RingsPerTurn;
                const float growth = std::log(2.1f) / TwoPi;
                const float thetaEnd = TwoPi * static_cast<float>(Turns);
                const Vector3 opening = Vector3::Normalize(forward - shellAxis * Vector3::Dot(forward, shellAxis));
                const Vector3 aside = Vector3::Normalize(Vector3::Cross(shellAxis, opening));
                constexpr float ApertureRadius = .108f, ApertureOffset = .072f, SpireHeight = .25f;
                const Vector3 apertureTarget = shellBase + forward * (.060f * scale)
                    + up * ((.105f + .030f * carry) * scale);
                const Vector3 origin = apertureTarget - aside * (ApertureOffset * scale);
                const auto whorlPoint = [&](const int ring, const int side, Vector3& normal, float& band) {
                    const float theta = thetaEnd * static_cast<float>(ring) / static_cast<float>(Rings);
                    const float g = std::exp(growth * (theta - thetaEnd));
                    const float phi = TwoPi * static_cast<float>(side) / static_cast<float>(TubeSides);
                    const float turn = theta - thetaEnd;
                    const Vector3 radial = aside * std::cos(turn) + opening * std::sin(turn);
                    const Vector3 tubeCentre = origin + radial * (ApertureOffset * g * scale)
                        + shellAxis * (SpireHeight * (1.0f - g) * scale);
                    normal = radial * std::cos(phi) + shellAxis * std::sin(phi);
                    band = phi;
                    return tubeCentre + normal * (ApertureRadius * g * scale);
                };
                const auto whorlVertex = [&](const int ring, const int side) {
                    Vector3 normal;
                    float phi = 0.0f;
                    const Vector3 position = whorlPoint(ring, side, normal, phi);
                    // spiral bands along the whorl; the inner side, toward the axis and under
                    // the next whorl, darkens into the suture
                    const float bandWave = std::sin(phi * 3.0f + .6f);
                    Color base = bandWave > .25f ? shellShade : (bandWave < -.55f ? shellRidge : shell);
                    const float inward = std::max(0.0f, -std::cos(phi));
                    base = Color::Lerp(base, Color(49, 39, 31, 255), .45f * inward * inward);
                    const float direct = std::max(0.0f, Vector3::Dot(normal, wildlife_render::FixedSunDirection));
                    return VertexPositionColor(position, wildlife_render::ScaleColor(
                        base, .72f + .30f * direct + .06f * std::max(0.0f, normal.Y)));
                };
                for (int ring = 0; ring < Rings; ++ring) {
                    for (int side = 0; side < TubeSides; ++side) {
                        const int next = (side + 1) % TubeSides;
                        const VertexPositionColor a = whorlVertex(ring, side);
                        const VertexPositionColor b = whorlVertex(ring + 1, side);
                        const VertexPositionColor c = whorlVertex(ring + 1, next);
                        const VertexPositionColor d = whorlVertex(ring, next);
                        vertices.insert(vertices.end(), {a, c, b, a, d, c});
                    }
                }
            }

            const Vector3 aperture = shellBase + forward * (.105f * scale)
                + up * ((.090f + .030f * carry) * scale);
            AppendRoundedCrabPalm(vertices, aperture, right, up, forward,
                .070f * scale, .058f * scale, .026f * scale,
                Color(54, 38, 29, 255), Color(28, 23, 19, 255));

            const Vector3 exposedBody = centre + forward * (.020f * scale)
                + up * (.055f * scale);
            const Vector3 head = centre + forward * (.125f * scale)
                + up * (.070f * scale);
            AppendRoundedCrabPalm(vertices, exposedBody, right, up, forward,
                .076f * scale, .050f * scale, .105f * scale, limb, joint);
            AppendRoundedCrabPalm(vertices, head, right, up, forward,
                .067f * scale, .052f * scale, .068f * scale, limb, joint);

            for (int leg = 0; leg < 3; ++leg) {
                for (const float side : {-1.0f, 1.0f}) {
                    const float stride = crab.legPhase
                        * (leg % 2 == 0 ? .020f : -.020f) * limbScale;
                    const Vector3 hip = centre + right * (side * .050f * limbScale)
                        + forward * ((.060f - leg * .058f) * limbScale)
                        + up * (.050f * limbScale);
                    const Vector3 knee = hip
                        + right * (side * (.080f + leg * .010f) * limbScale)
                        + forward * stride + up * (.012f * limbScale);
                    const Vector3 ankle = knee
                        + right * (side * (.072f + leg * .008f) * limbScale)
                        + forward * ((-.028f + leg * .018f) * limbScale)
                        - up * (.048f * limbScale);
                    const Vector3 toe = ankle + right * (side * .060f * limbScale)
                        + forward * ((-.020f + leg * .016f) * limbScale)
                        - up * (.010f * limbScale);
                    AppendTaperedLimbSegment(vertices, hip, knee,
                                             .019f * scale, .015f * scale, limb);
                    AppendTaperedLimbSegment(vertices, knee, ankle,
                                             .015f * scale, .010f * scale, joint);
                    AppendTaperedLimbSegment(vertices, ankle, toe,
                                             .010f * scale, .0035f * scale, limb);
                    AppendOctahedron(vertices, knee, right, up, forward,
                        .021f * scale, .018f * scale, .021f * scale, limb, joint);
                }
            }

            for (const float side : {-1.0f, 1.0f}) {
                const bool major = side > 0.0f;
                // Feeding, the small claw dips to the sand and lifts to the mouth in turn,
                // the forearm following it halfway; the big claw is held as it was.
                const float feed = major ? 0.0f : std::clamp(crab.feedAmount, 0.0f, 1.0f);
                const float pick = std::sin(crab.feedPhase);
                const Vector3 feedOffset = (pick > 0.0f
                    ? (forward * .020f - up * .030f) * pick
                    : (up * .028f - forward * .075f - right * (side * .045f)) * (-pick)) * (feed * scale);
                const Vector3 root = head + right * (side * .045f * scale)
                    + forward * (.010f * scale);
                const Vector3 elbow = head + right * (side * .105f * scale)
                    + forward * (.080f * scale) + up * (.015f * scale) + feedOffset * .5f;
                const Vector3 wrist = head + right * (side * .132f * scale)
                    + forward * (.145f * scale) - up * (.005f * scale) + feedOffset;
                const Vector3 palm = wrist + forward * (.030f * scale);
                AppendTaperedLimbSegment(vertices, root, elbow,
                    .020f * scale, .016f * scale, limb);
                AppendTaperedLimbSegment(vertices, elbow, wrist,
                    .016f * scale, .012f * scale, joint);
                AppendOctahedron(vertices, elbow, right, up, forward,
                    .022f * scale, .019f * scale, .023f * scale, limb, joint);
                AppendRoundedCrabPalm(vertices, palm, right, up, forward,
                    (major ? .046f : .039f) * scale,
                    (major ? .032f : .028f) * scale,
                    (major ? .052f : .045f) * scale, limb, joint);
                const Vector3 outerBase = palm + right * (side * .020f * scale)
                    + forward * (.030f * scale);
                const Vector3 innerBase = palm - right * (side * .016f * scale)
                    + forward * (.030f * scale);
                const Vector3 outerMid = palm + right * (side * .032f * scale)
                    + forward * (.070f * scale);
                const Vector3 innerMid = palm - right * (side * .026f * scale)
                    + forward * (.068f * scale);
                const Vector3 outerTip = palm + right * (side * .010f * scale)
                    + forward * ((major ? .112f : .098f) * scale);
                const Vector3 innerTip = palm - right * (side * .006f * scale)
                    + forward * ((major ? .108f : .094f) * scale);
                AppendPrismaticFinger(vertices, outerBase, outerMid, outerTip,
                    .010f * scale, limb, joint);
                AppendPrismaticFinger(vertices, innerBase, innerMid, innerTip,
                    .008f * scale, shellRidge, joint);
            }

            for (const float side : {-1.0f, 1.0f}) {
                const Vector3 eyeRoot = head + right * (side * .036f * scale)
                    + forward * (.040f * scale) + up * (.030f * scale);
                const Vector3 eyeTip = eyeRoot + forward * (.055f * scale)
                    + up * (.035f * scale);
                AppendTaperedLimbSegment(vertices, eyeRoot, eyeTip,
                    .009f * scale, .006f * scale, limb);
                AppendOctahedron(vertices, eyeTip, right, up, forward,
                    .013f * scale, .013f * scale, .011f * scale, eye, eye);
                AppendOctahedron(vertices,
                    eyeTip + forward * (.010f * scale) + up * (.004f * scale),
                    right, up, forward, .004f * scale, .004f * scale,
                    .003f * scale, Color(213, 190, 144, 255), eye);
                const float antennaSwing = std::sin(crab.antennaPhase + side * .8f)
                    * .012f * scale;
                const Vector3 antennaMid = head + forward * (.095f * scale)
                    + right * (side * .050f * scale) + up * (.040f * scale);
                const Vector3 antennaTip = head + forward * (.185f * scale)
                    + right * (side * (.075f * scale + antennaSwing))
                    + up * (.085f * scale);
                AppendCrossedSegment(vertices, eyeRoot, antennaMid,
                                     .0032f * scale, joint);
                AppendCrossedSegment(vertices, antennaMid, antennaTip,
                                     .0020f * scale, joint);
            }

            const Vector3 mouth = head + forward * (.070f * scale)
                - up * (.032f * scale);
            AddQuad(vertices, mouth - right * (.032f * scale),
                    mouth + right * (.032f * scale),
                    mouth + right * (.026f * scale) - up * (.010f * scale),
                    mouth - right * (.026f * scale) - up * (.010f * scale), joint);
            AddDoubleSidedTriangle(vertices,
                mouth - right * (.018f * scale) - up * (.012f * scale),
                mouth + right * (.018f * scale) - up * (.012f * scale),
                mouth - up * (.028f * scale), shellRidge);
        }

        void AppendPrismaticFinger(std::vector<VertexPositionColor>& vertices,
                                   const Vector3& a, const Vector3& b, const Vector3& tip,
                                   const float halfThickness,
                                   const Color& faceColor, const Color& edgeColor)
        {
            const Vector3 lift(0.0f, halfThickness, 0.0f);
            const Vector3 aTop = a + lift, bTop = b + lift, tipTop = tip + lift * 0.45f;
            const Vector3 aBottom = a - lift, bBottom = b - lift, tipBottom = tip - lift * 0.45f;
            AddTriangle(vertices, aTop, bTop, tipTop, faceColor);
            AddTriangle(vertices, aBottom, tipBottom, bBottom, faceColor);
            AddQuad(vertices, aTop, aBottom, bBottom, bTop, edgeColor);
            AddQuad(vertices, bTop, bBottom, tipBottom, tipTop, edgeColor);
            AddQuad(vertices, tipTop, tipBottom, aBottom, aTop, edgeColor);
        }

        void AppendNearCrab(std::vector<VertexPositionColor>& vertices,
                            const CrabState& crab,
                            const bool useRoundedUnequalPalms = false)
        {
            const CrabPalette& palette = Palettes[crab.colorVariant % Palettes.size()];
            const Vector3 centre(crab.x, crab.y, crab.z);
            const Vector3 forward(std::cos(crab.bodyYawRadians), 0.0f,
                                  std::sin(crab.bodyYawRadians));
            const Vector3 right(-forward.Z, 0.0f, forward.X);
            const Vector3 up = Vector3::Up;
            const float scale = crab.scale;
            const Vector3 top = LocalPoint(
                centre, right, up, forward, 0.0f, 0.134f, -0.01f, scale);
            std::array<Vector3, CarapaceSegments> crown{};
            std::array<Vector3, CarapaceSegments> shoulder{};
            std::array<Vector3, CarapaceSegments> rim{};
            std::array<Vector3, CarapaceSegments> lower{};
            for (std::size_t index = 0; index < crown.size(); ++index) {
                const float angle = static_cast<float>(index) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(crown.size());
                const float cosine = std::cos(angle);
                const float sine = std::sin(angle);
                const float frontTaper = sine > 0.0f ? 0.94f : 1.0f;
                // A beach crab's carapace is near square, a third again as wide as long (at
                // 2.6 to 1 a crab seen from its side or its front read as a sausage).
                crown[index] = LocalPoint(centre, right, up, forward,
                    cosine * 0.225f, 0.130f, sine * 0.135f - 0.01f, scale);
                shoulder[index] = LocalPoint(centre, right, up, forward,
                    cosine * 0.355f, 0.112f,
                    sine * 0.243f * frontTaper - 0.005f, scale);
                rim[index] = LocalPoint(centre, right, up, forward,
                    cosine * 0.420f, 0.078f, sine * 0.310f * frontTaper, scale);
                lower[index] = LocalPoint(
                    centre, right, up, forward, cosine * 0.390f, 0.040f,
                    sine * 0.279f * frontTaper, scale);
            }
            for (std::size_t index = 0; index < crown.size(); ++index) {
                const std::size_t next = (index + 1U) % crown.size();
                const float angle = static_cast<float>(index) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(crown.size());
                const float nextAngle = static_cast<float>(next) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(crown.size());
                const Color crownColor = SmoothShellColor(
                    palette, right, up, forward, angle, .91f);
                const Color nextCrownColor = SmoothShellColor(
                    palette, right, up, forward, nextAngle, .91f);
                const Color shoulderColor = SmoothShellColor(
                    palette, right, up, forward, angle, .68f);
                const Color nextShoulderColor = SmoothShellColor(
                    palette, right, up, forward, nextAngle, .68f);
                const Color rimColor = SmoothShellColor(
                    palette, right, up, forward, angle, .34f);
                const Color nextRimColor = SmoothShellColor(
                    palette, right, up, forward, nextAngle, .34f);
                const Color lowerColor = SmoothShellColor(
                    palette, right, up, forward, angle, .10f);
                const Color nextLowerColor = SmoothShellColor(
                    palette, right, up, forward, nextAngle, .10f);
                AddGradientTriangle(vertices, top, crown[index], crown[next],
                                    palette.shellHighlight, crownColor, nextCrownColor);
                AddGradientQuad(vertices, crown[index], shoulder[index], shoulder[next],
                                crown[next], crownColor, shoulderColor,
                                nextShoulderColor, nextCrownColor);
                AddGradientQuad(vertices, shoulder[index], rim[index], rim[next],
                                shoulder[next], shoulderColor, rimColor,
                                nextRimColor, nextShoulderColor);
                AddGradientQuad(vertices, rim[index], lower[index], lower[next], rim[next],
                                rimColor, lowerColor, nextLowerColor, nextRimColor);
            }
            // Close the shell below the lateral rim. The old open ring was hidden
            // from above but collapsed into a paper-thin disc whenever a moving crab
            // turned broadside. One shallow concave fan adds a darker abdominal plane
            // without increasing the number of agents, materials or draw calls.
            const Vector3 underside = LocalPoint(
                centre, right, up, forward, 0.0f, 0.018f, -0.010f, scale);
            const Color undersideColor = wildlife_render::ScaleColor(
                palette.shellSide, .72f);
            for (std::size_t index = 0; index < lower.size(); ++index) {
                const std::size_t next = (index + 1U) % lower.size();
                AddTriangle(vertices, lower[next], lower[index], underside, undersideColor);
            }

            struct ShellMark
            {
                float x;
                float z;
                float halfX;
                float halfZ;
            };
            constexpr std::array<ShellMark, 10> ShellMarks = {{
                {-.22f, -.075f, .030f, .018f}, {-.10f, -.115f, .024f, .014f},
                { .04f, -.105f, .032f, .017f}, { .19f, -.060f, .026f, .015f},
                {-.27f,  .015f, .022f, .014f}, {-.13f,  .035f, .035f, .019f},
                { .09f,  .020f, .027f, .016f}, { .25f,  .030f, .021f, .013f},
                {-.16f,  .120f, .023f, .013f}, { .14f,  .105f, .030f, .015f},
            }};
            const Color markColor = Color::Lerp(palette.shellSide, palette.shellTop, .28f);
            for (const ShellMark& mark : ShellMarks) {
                const auto point = [&](const float x, const float z) {
                    return LocalPoint(centre, right, up, forward, x,
                                      CarapaceSurfaceHeight(x, z) + .004f, z, scale);
                };
                // (the marks were laid out on the old long-and-narrow shell)
                const float mx = mark.x * .82f, mz = mark.z * 1.5f;
                const float hx = mark.halfX * .82f, hz = mark.halfZ * 1.5f;
                AddQuad(vertices,
                        point(mx - hx, mz),
                        point(mx, mz + hz),
                        point(mx + hx, mz),
                        point(mx, mz - hz), markColor);
            }

            // A crab's lateral shell corners remain characteristic even when its broad axis is
            // foreshortened by the camera. Keep these low and integrated with the rim rather than
            // turning them into decorative spikes.
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                AddDoubleSidedTriangle(
                    vertices,
                    LocalPoint(centre, right, up, forward, side * .31f, .088f,
                               .21f, scale),
                    LocalPoint(centre, right, up, forward, side * .47f, .072f,
                               .04f, scale),
                    LocalPoint(centre, right, up, forward, side * .32f, .080f,
                               -.17f, scale),
                    palette.shellSide);
            }

            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                for (int leg = 0; leg < 4; ++leg) {
                    const float z = -0.20f + static_cast<float>(leg) * 0.115f;
                    const float stride = crab.legPhase * (leg % 2 == 0 ? 0.045f : -0.045f);
                    const float bend = (leg < 2 ? -0.045f : 0.045f)
                        + (leg % 2 == 0 ? 0.018f : -0.018f);
                    const Vector3 root = LocalPoint(
                        centre, right, up, forward, side * 0.30f, 0.075f, z, scale);
                    const Vector3 knee = LocalPoint(
                        centre, right, up, forward, side * 0.50f, 0.035f,
                        z + bend + stride, scale);
                    const Vector3 foot = LocalPoint(
                        centre, right, up, forward, side * 0.66f, -0.015f,
                        z + bend * 1.35f + stride * 1.55f, scale);
                    const Vector3 toe = LocalPoint(
                        centre, right, up, forward, side * 0.77f, -0.020f,
                        z + bend * 1.55f + stride * 1.72f
                            + (leg % 2 == 0 ? -0.018f : 0.018f), scale);
                    AppendTaperedLimbSegment(vertices, root, knee, 0.026f * scale,
                                             0.021f * scale, palette.limb);
                    AppendTaperedLimbSegment(vertices, knee, foot, 0.020f * scale,
                                             0.013f * scale, palette.joint);
                    AppendTaperedLimbSegment(vertices, foot, toe, 0.013f * scale,
                                             0.006f * scale, palette.limb);
                }

                const float clawSpread = 0.025f + crab.clawPose * 0.035f;
                // The waving claw: raised and swept up and down from the shoulder, the
                // other claw held as it was.
                const float wave = crab.waveSide * side > 0.0f ? std::clamp(crab.waveAmount, 0.0f, 1.0f) : 0.0f;
                const float lift = wave * (0.16f + 0.09f * std::sin(crab.wavePhase));
                const Vector3 clawRoot = LocalPoint(
                    centre, right, up, forward, side * 0.22f, 0.11f + lift * 0.25f, 0.22f, scale);
                const Vector3 clawJoint = LocalPoint(
                    centre, right, up, forward, side * 0.36f, 0.13f + lift * 0.7f, 0.36f - lift * 0.1f, scale);
                AppendTaperedLimbSegment(vertices, clawRoot, clawJoint,
                                         0.032f * scale, 0.024f * scale,
                                         palette.limb);
                const Vector3 palm = LocalPoint(
                    centre, right, up, forward, side * 0.44f, 0.15f + lift, 0.440f - lift * 0.2f, scale);
                if (useRoundedUnequalPalms) {
                    AppendRoundedCoconutPalm(
                        vertices, palm, right, up, forward,
                        (side > 0.0f ? .095f : .080f) * scale,
                        (side > 0.0f ? .058f : .052f) * scale,
                        (side > 0.0f ? .140f : .122f) * scale,
                        palette.claw, palette.shellSide);
                } else {
                    // A crab's palm is flattened side to side, taller than wide and long along
                    // the arm; the old squat lozenge read from above as a round cap on a stalk.
                    AppendRoundedCrabPalm(vertices, palm, right, up, forward,
                                          .062f * scale, .085f * scale,
                                          .140f * scale, palette.claw,
                                          palette.shellSide);
                }
                const Vector3 outerTip = LocalPoint(
                    centre, right, up, forward, side * (0.41f + clawSpread),
                    0.165f + lift * 1.15f, 0.575f - lift * 0.3f, scale);
                const Vector3 innerTip = LocalPoint(
                    centre, right, up, forward, side * (0.39f - clawSpread * 0.45f),
                    0.125f + lift * 1.15f, 0.58f - lift * 0.3f, scale);
                const Vector3 outerBase = palm + right * (side * 0.035f * scale)
                    + forward * (0.035f * scale);
                const Vector3 innerBase = palm - right * (side * 0.025f * scale)
                    + forward * (0.035f * scale);
                if (useRoundedUnequalPalms) {
                    const Vector3 outerMid = palm + forward * (.105f * scale)
                        + right * (side * .058f * scale) + up * (.013f * scale);
                    const Vector3 innerMid = palm + forward * (.105f * scale)
                        - right * (side * .050f * scale) - up * (.006f * scale);
                    AppendPrismaticFinger(vertices, outerBase, outerMid, outerTip,
                                          .020f * scale, palette.claw,
                                          palette.shellSide);
                    AppendPrismaticFinger(vertices, innerBase, innerMid, innerTip,
                                          .017f * scale, palette.shellHighlight,
                                          palette.shellSide);
                } else {
                    AppendPrismaticFinger(vertices, outerBase, palm, outerTip,
                                          .012f * scale, palette.claw,
                                          palette.shellSide);
                    AppendPrismaticFinger(vertices, palm, innerBase, innerTip,
                                          .010f * scale, palette.shellHighlight,
                                          palette.shellSide);
                }
            }

            // Folded, each eyestalk lies down along the carapace front by the fold amount.
            const float fold = std::clamp(crab.eyeFold, 0.0f, 1.0f);
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                const Vector3 eyeRoot = LocalPoint(
                    centre, right, up, forward, side * 0.12f, 0.150f, 0.26f, scale);
                const Vector3 eyeCentre = LocalPoint(
                    centre, right, up, forward, side * (0.12f + 0.02f * fold),
                    0.220f - 0.062f * fold, 0.295f + 0.045f * fold, scale);
                AppendCrossedSegment(vertices, eyeRoot, eyeCentre,
                                     0.012f * scale, palette.shellSide);
                AppendOctahedron(vertices, eyeCentre, right, up, forward,
                                 0.027f * scale, 0.024f * scale, 0.025f * scale,
                                 palette.shellHighlight, palette.shellSide);
                const Vector3 pupilCentre = eyeCentre + forward * (0.014f * scale)
                    + up * (0.006f * scale);
                AppendOctahedron(vertices, pupilCentre, right, up, forward,
                                 0.012f * scale, 0.012f * scale, 0.011f * scale,
                                 Color(38, 30, 24, 255), Color(20, 18, 17, 255));
            }

            const Vector3 mouthCentre = LocalPoint(
                centre, right, up, forward, 0.0f, 0.072f, 0.300f, scale);
            const Vector3 mouthLeft = LocalPoint(
                centre, right, up, forward, -0.07f, 0.068f, 0.306f, scale);
            const Vector3 mouthRight = LocalPoint(
                centre, right, up, forward, 0.07f, 0.068f, 0.306f, scale);
            const Vector3 mouthLow = LocalPoint(
                centre, right, up, forward, 0.0f, 0.035f, 0.318f, scale);
            AddDoubleSidedTriangle(vertices, mouthLeft, mouthCentre, mouthLow,
                                   palette.joint);
            AddDoubleSidedTriangle(vertices, mouthCentre, mouthRight, mouthLow,
                                   palette.joint);

            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                AddDoubleSidedTriangle(
                    vertices,
                    LocalPoint(centre, right, up, forward, side * 0.25f, 0.11f,
                               -0.24f, scale),
                    LocalPoint(centre, right, up, forward, side * 0.35f, 0.085f,
                               -0.17f, scale),
                    LocalPoint(centre, right, up, forward, side * 0.30f, 0.10f,
                               -0.07f, scale),
                    palette.shellSide);
            }
        }

        void AppendCoconutCrab(std::vector<VertexPositionColor>& vertices,
                               const CoconutCrabState& crab)
        {
            const Vector3 centre(crab.x, crab.y, crab.z);
            const Vector3 flatForward(std::cos(crab.headingRadians), 0.0f,
                                      std::sin(crab.headingRadians));
            // On a trunk the body turns up it, head up, its underside to the bark: the frame
            // blends from the ground's to one whose forward is up and whose up points away
            // from the trunk's axis.
            const float climb = std::clamp(crab.climbAmount, 0.0f, 1.0f);
            Vector3 radial(crab.x - crab.trunkX, 0.0f, crab.z - crab.trunkZ);
            if (radial.LengthSquared() < 1.0e-6f) radial = -flatForward;
            radial = Vector3::Normalize(radial);
            Vector3 up = Vector3::Normalize(Vector3::Lerp(Vector3::Up, radial, climb));
            Vector3 forward = Vector3::Normalize(Vector3::Lerp(flatForward, Vector3::Up, climb));
            const Vector3 right = Vector3::Normalize(Vector3::Cross(up, forward));
            forward = Vector3::Normalize(Vector3::Cross(right, up));
            const float scale = crab.scale;
            const std::array<Color, 4> plates = {{
                Color(108, 116, 95, 255), Color(128, 91, 82, 255),
                Color(94, 91, 119, 255), Color(145, 112, 72, 255)}};
            const Color plate = plates[crab.colorVariant % plates.size()];
            const Color plateShade = Color::Lerp(plate, Color(25, 28, 25, 255), .42f);
            const Color body = CoconutBodyColours[
                crab.colorVariant % CoconutBodyColours.size()];
            const Color bodyLight = wildlife_render::ScaleColor(body, 1.16f);
            const Color bodyShade = wildlife_render::ScaleColor(body, .72f);
            const Color limb = Color::Lerp(body, plateShade, .36f);
            const Color joint = wildlife_render::ScaleColor(limb, .69f);
            const Color claw = Color::Lerp(bodyLight, plate, .38f);

            // Build the terrestrial species directly. Reusing and stretching the
            // beach-crab mesh made the animal read as a flat disc with wheel-shaped
            // claws. This deep, closed volume keeps a high shoulder and a visible
            // ventral edge from frontal, lateral and elevated views.
            AppendRoundedCoconutPalm(vertices,
                centre + forward * (.025f * scale) + up * (.245f * scale),
                right, up, forward, .340f * scale, .185f * scale,
                .290f * scale, bodyLight, bodyShade);

            struct ArmourMark { float x; float z; float halfX; float halfZ; };
            constexpr std::array<ArmourMark, 10> ArmourMarks = {{
                {-.20f, -.10f, .040f, .026f}, {-.07f, -.15f, .035f, .022f},
                { .09f, -.14f, .042f, .025f}, { .21f, -.08f, .032f, .021f},
                {-.24f,  .00f, .034f, .024f}, {-.09f, -.02f, .042f, .028f},
                { .08f,  .00f, .037f, .025f}, { .23f,  .020f, .030f, .020f},
                {-.14f,  .07f, .034f, .021f}, { .14f,  .08f, .038f, .022f},
            }};
            for (const ArmourMark& mark : ArmourMarks) {
                const auto shellPoint = [&](const float x, const float z) {
                    const float dome = std::sqrt(std::max(0.0f, 1.0f
                        - x * x / (.340f * .340f) - z * z / (.290f * .290f)));
                    return centre + right * (x * scale)
                        + forward * ((z + .025f) * scale)
                        + up * ((.245f + .185f * dome + .004f) * scale);
                };
                AddQuad(vertices,
                        shellPoint(mark.x - mark.halfX, mark.z),
                        shellPoint(mark.x, mark.z + mark.halfZ),
                        shellPoint(mark.x + mark.halfX, mark.z),
                        shellPoint(mark.x, mark.z - mark.halfZ),
                        Color::Lerp(bodyShade, plate, .22f));
            }

            // Coconut crabs have a reduced, tucked abdomen. It remains readable
            // behind the main shield without turning into a second flat shell.
            constexpr int Segments = 12;
            constexpr std::array<float, 4> Stations = {{-.335f, -.275f, -.205f, -.125f}};
            constexpr std::array<float, 4> Widths = {{.120f, .190f, .250f, .230f}};
            constexpr std::array<float, 4> Heights = {{.070f, .105f, .140f, .120f}};
            const auto ringPoint = [&](std::size_t station, int segment) {
                const float angle = TwoPi * static_cast<float>(segment)
                    / static_cast<float>(Segments);
                return centre + forward * (Stations[station] * scale)
                    + right * (std::cos(angle) * Widths[station] * scale)
                    + up * ((.155f + std::sin(angle) * Heights[station]) * scale);
            };
            for (std::size_t station = 0; station + 1U < Stations.size(); ++station) {
                for (int segment = 0; segment < Segments; ++segment) {
                    const int next = (segment + 1) % Segments;
                    AddQuad(vertices, ringPoint(station, segment),
                            ringPoint(station + 1U, segment),
                            ringPoint(station + 1U, next),
                            ringPoint(station, next),
                            segment < Segments / 2 ? plate : plateShade);
                }
            }
            const Vector3 rear = centre + forward * (Stations.front() * scale)
                + up * (.155f * scale);
            for (int segment = 0; segment < Segments; ++segment) {
                const int next = (segment + 1) % Segments;
                AddTriangle(vertices, rear, ringPoint(0U, next),
                            ringPoint(0U, segment), plateShade);
            }

            // Four fitted dorsal plates articulate the tucked abdomen.
            for (int band = 0; band < 4; ++band) {
                const float z = -.305f + static_cast<float>(band) * .055f;
                const float width = .115f + static_cast<float>(band) * .030f;
                const float y = .236f + static_cast<float>(band) * .018f;
                AddQuad(vertices,
                    LocalPoint(centre, right, up, forward, -width, y, z-.010f, scale),
                    LocalPoint(centre, right, up, forward,  width, y, z-.010f, scale),
                    LocalPoint(centre, right, up, forward,  width, y, z+.010f, scale),
                    LocalPoint(centre, right, up, forward, -width, y, z+.010f, scale),
                    wildlife_render::ScaleColor(plate, 1.04f));
            }

            // Four strongly articulated walking legs per side provide the high,
            // planted coconut-crab stance while every toe remains on the terrain.
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                for (int leg = 0; leg < 4; ++leg) {
                    const float z = -.145f + static_cast<float>(leg) * .112f;
                    const float stride = crab.legPhase
                        * (leg % 2 == 0 ? .034f : -.034f);
                    const Vector3 hip = LocalPoint(centre, right, up, forward,
                        side * .245f, .175f, z, scale);
                    const Vector3 knee = LocalPoint(centre, right, up, forward,
                        side * (.455f + leg * .014f), .195f,
                        z + stride, scale);
                    const Vector3 ankle = LocalPoint(centre, right, up, forward,
                        side * (.635f + leg * .012f), .080f,
                        z + (leg < 2 ? -.038f : .038f) + stride * 1.35f, scale);
                    const Vector3 toe = LocalPoint(centre, right, up, forward,
                        side * (.790f + leg * .008f), .006f,
                        z + (leg < 2 ? -.060f : .060f) + stride * 1.65f, scale);
                    AppendTaperedLimbSegment(vertices, hip, knee,
                        .041f * scale, .034f * scale, limb);
                    AppendTaperedLimbSegment(vertices, knee, ankle,
                        .035f * scale, .022f * scale, joint);
                    AppendTaperedLimbSegment(vertices, ankle, toe,
                        .023f * scale, .008f * scale, limb);
                    AppendOctahedron(vertices, knee, right, up, forward,
                        .045f * scale, .038f * scale, .045f * scale,
                        limb, joint);
                    AppendOctahedron(vertices, ankle, right, up, forward,
                        .030f * scale, .025f * scale, .032f * scale,
                        joint, bodyShade);
                }
            }

            // The chelipeds bend forward from the shoulder. Their palms are
            // unequal, integrated into the arms, and the two fingers converge
            // toward the midline instead of presenting as circular side plates.
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                const bool major = sideIndex > 0;
                const Vector3 root = LocalPoint(centre, right, up, forward,
                    side * .205f, .230f, .180f, scale);
                const Vector3 elbow = LocalPoint(centre, right, up, forward,
                    side * .355f, .250f, .315f, scale);
                const Vector3 wrist = LocalPoint(centre, right, up, forward,
                    side * .405f, .215f, .430f, scale);
                const Vector3 palm = LocalPoint(centre, right, up, forward,
                    side * .410f, .225f, .510f, scale);
                AppendTaperedLimbSegment(vertices, root, elbow,
                    .050f * scale, .044f * scale, limb);
                AppendTaperedLimbSegment(vertices, elbow, wrist,
                    .046f * scale, .038f * scale, joint);
                AppendOctahedron(vertices, elbow, right, up, forward,
                    .055f * scale, .050f * scale, .060f * scale,
                    limb, joint);
                AppendRoundedCoconutPalm(vertices, palm, right, up, forward,
                    (major ? .122f : .104f) * scale,
                    (major ? .105f : .092f) * scale,
                    (major ? .168f : .145f) * scale,
                    claw, wildlife_render::ScaleColor(claw, .67f));

                const float opening = .030f + crab.clawPose * .032f;
                const Vector3 outerBase = palm + right * (side * .062f * scale)
                    + forward * (.100f * scale);
                const Vector3 innerBase = palm - right * (side * .040f * scale)
                    + forward * (.105f * scale);
                const Vector3 outerMid = palm + right * (side * .090f * scale)
                    + forward * (.195f * scale) + up * (.015f * scale);
                const Vector3 innerMid = palm - right * (side * .060f * scale)
                    + forward * (.190f * scale) - up * (.014f * scale);
                const Vector3 outerTip = palm + right * (side * opening * scale)
                    + forward * ((major ? .285f : .255f) * scale);
                const Vector3 innerTip = palm - right * (side * opening * .55f * scale)
                    + forward * ((major ? .278f : .248f) * scale);
                AppendPrismaticFinger(vertices, outerBase, outerMid, outerTip,
                    (major ? .027f : .023f) * scale, claw, plateShade);
                AppendPrismaticFinger(vertices, innerBase, innerMid, innerTip,
                    (major ? .023f : .020f) * scale, bodyLight, plateShade);
            }

            const Color eye(20, 19, 17, 255);
            const Color eyeGlint(184, 194, 186, 255);
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                const Vector3 eyeRoot = LocalPoint(centre, right, up, forward,
                    side * .105f, .335f, .210f, scale);
                const Vector3 eyeCentre = LocalPoint(centre, right, up, forward,
                    side * .120f, .385f, .300f, scale);
                AppendCrossedSegment(vertices, eyeRoot, eyeCentre,
                    .013f * scale, plateShade);
                AppendOctahedron(vertices, eyeCentre, right, up, forward,
                    .022f * scale, .020f * scale, .021f * scale, eye, eye);
                AppendOctahedron(vertices,
                    eyeCentre + forward * (.025f * scale) + up * (.008f * scale),
                    right, up, forward, .004f * scale, .004f * scale,
                    .003f * scale, eyeGlint, eyeGlint);

                const Vector3 antennaMid = LocalPoint(centre, right, up, forward,
                    side * .115f, .370f, .345f, scale);
                const Vector3 antennaTip = LocalPoint(centre, right, up, forward,
                    side * (.180f + .018f * std::sin(crab.legPhase)),
                    .435f, .490f, scale);
                AppendCrossedSegment(vertices, eyeRoot, antennaMid,
                    .006f * scale, joint);
                AppendCrossedSegment(vertices, antennaMid, antennaTip,
                    .0035f * scale, joint);
            }

            const Vector3 mouthCentre = LocalPoint(centre, right, up, forward,
                0.0f, .190f, .307f, scale);
            AddQuad(vertices,
                mouthCentre - right * (.060f * scale) + up * (.010f * scale),
                mouthCentre + right * (.060f * scale) + up * (.010f * scale),
                mouthCentre + right * (.050f * scale) - up * (.012f * scale),
                mouthCentre - right * (.050f * scale) - up * (.012f * scale), joint);
            AddDoubleSidedTriangle(vertices,
                mouthCentre - right * (.035f * scale) - up * (.014f * scale),
                mouthCentre + right * (.035f * scale) - up * (.014f * scale),
                mouthCentre - up * (.035f * scale), bodyShade);
            for (const float side : {-1.0f, 1.0f}) {
                AppendOctahedron(vertices,
                    mouthCentre + right * (side * .078f * scale)
                        + forward * (.015f * scale),
                    right, up, forward, .045f * scale, .032f * scale,
                    .046f * scale, limb, joint);
            }
        }

        void AppendFarCrab(std::vector<VertexPositionColor>& vertices,
                           const CrabState& crab)
        {
            const CrabPalette& palette = Palettes[crab.colorVariant % Palettes.size()];
            const Vector3 centre(crab.x, crab.y, crab.z);
            const Vector3 forward(std::cos(crab.bodyYawRadians), 0.0f,
                                  std::sin(crab.bodyYawRadians));
            const Vector3 right(-forward.Z, 0.0f, forward.X);
            const Vector3 up = Vector3::Up;
            const float scale = crab.scale;
            const Vector3 front = LocalPoint(
                centre, right, up, forward, 0.0f, 0.10f, 0.27f, scale);
            const Vector3 back = LocalPoint(
                centre, right, up, forward, 0.0f, 0.08f, -0.25f, scale);
            const Vector3 left = LocalPoint(
                centre, right, up, forward, -0.38f, 0.07f, 0.0f, scale);
            const Vector3 rightPoint = LocalPoint(
                centre, right, up, forward, 0.38f, 0.07f, 0.0f, scale);
            AddDoubleSidedTriangle(vertices, front, left, back, palette.shellTop);
            AddDoubleSidedTriangle(vertices, front, back, rightPoint, palette.shellSide);
        }

        void AppendBurrow(std::vector<VertexPositionColor>& vertices,
                          const CrabBurrow& burrow)
        {
            constexpr int Segments = 18;
            const Vector3 centre(burrow.x, burrow.y + .004f, burrow.z);
            const Color hole(47, 35, 26, 255);
            const Color innerSand(112, 83, 55, 255);
            const Color outerSand(167, 132, 87, 255);
            for (int segment = 0; segment < Segments; ++segment) {
                const float a0 = static_cast<float>(segment) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(Segments);
                const float a1 = static_cast<float>(segment + 1) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(Segments);
                const float irregular0 = 1.0f + .08f * std::sin(a0 * 3.0f + burrow.x);
                const float irregular1 = 1.0f + .08f * std::sin(a1 * 3.0f + burrow.x);
                const Vector3 inner0 = centre + Vector3(
                    std::cos(a0) * burrow.radius * irregular0, 0,
                    std::sin(a0) * burrow.radius * irregular0);
                const Vector3 inner1 = centre + Vector3(
                    std::cos(a1) * burrow.radius * irregular1, 0,
                    std::sin(a1) * burrow.radius * irregular1);
                const Vector3 outer0 = centre + Vector3(
                    std::cos(a0) * burrow.radius * 1.48f * irregular0, .018f,
                    std::sin(a0) * burrow.radius * 1.48f * irregular0);
                const Vector3 outer1 = centre + Vector3(
                    std::cos(a1) * burrow.radius * 1.48f * irregular1, .018f,
                    std::sin(a1) * burrow.radius * 1.48f * irregular1);
                AddTriangle(vertices, centre, inner1, inner0, hole);
                AddTriangle(vertices, inner0, inner1, outer1, innerSand);
                AddTriangle(vertices, inner0, outer1, outer0,
                    segment % 3 == 0 ? outerSand : innerSand);
            }
        }
    }

    CrabRenderer::CrabRenderer(GraphicsDevice& device, const float maximumDistance)
        : device_(device),
          effect_(std::make_unique<BasicEffect>(device)),
          vertices_(std::make_unique<DynamicVertexBuffer>(
              device, VertexPositionColor::getVertexDeclarationStatic(), MaximumVertexCount,
              BufferUsage::WriteOnly)),
          maximumDistance_(maximumDistance)
    {
        if (!std::isfinite(maximumDistance_) || maximumDistance_ <= 0.0f) {
            throw std::invalid_argument("Crab maximum draw distance must be finite and positive");
        }
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        effect_->setFogColorProperty(Vector3(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue));
        // The land's own fog; the end of the range is a shrink, not a fade to the fog colour.
        effect_->setFogStartProperty(250.0f);
        effect_->setFogEndProperty(660.0f);
        stagingVertices_.reserve(static_cast<std::size_t>(MaximumVertexCount));
        std::cout << CrabDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabDiagnostic);
        std::cout << CrabDetailDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabDetailDiagnostic);
        std::cout << CrabSilhouetteDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabSilhouetteDiagnostic);
        std::cout << CrabSurfaceDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabSurfaceDiagnostic);
        std::cout << CrabAnatomyDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabAnatomyDiagnostic);
        std::cout << CrabUndersideDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabUndersideDiagnostic);
        std::cout << CrabClawDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabClawDiagnostic);
        std::cout << CrabHabitatDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabHabitatDiagnostic);
        std::cout << CrabSpacingDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CrabSpacingDiagnostic);
        std::cout << HermitCrabDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(HermitCrabDiagnostic);
        std::cout << HermitCrabAnatomyDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(HermitCrabAnatomyDiagnostic);
        std::cout << CoconutCrabDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CoconutCrabDiagnostic);
        std::cout << CoconutCrabAnatomyDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CoconutCrabAnatomyDiagnostic);
        std::cout << CoconutCrabProfileDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CoconutCrabProfileDiagnostic);
        std::cout << CoconutCrabClawDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CoconutCrabClawDiagnostic);
        std::cout << CoconutCrabColourDiagnostic << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(CoconutCrabColourDiagnostic);
    }

    CrabRenderer::~CrabRenderer() = default;

    void CrabRenderer::Draw(const Matrix& view, const Matrix& projection,
                            const Vector3& cameraPosition,
                            const std::span<const CrabState> crabs,
                            const std::span<const CrabBurrow> burrows,
                            const std::span<const HermitCrabState> hermitCrabs,
                            const std::span<const CoconutCrabState> coconutCrabs)
    {
        stagingVertices_.clear();
        visibleCrabCount_ = 0;
        visibleNearCrabCount_ = 0;
        visibleFarCrabCount_ = 0;
        visibleHermitCrabCount_ = 0;
        visibleCoconutCrabCount_ = 0;
        drawCallCount_ = 0;
        const BoundingFrustum frustum(view * projection);
        // Every crab, and every burrow, shrinks into the sand over the end of the range.
        const auto shrinkFrom = [&](const std::size_t first, const float x, const float y, const float z,
                                    const Vector3& centre) {
            vegetation_fade::ShrinkAbout(stagingVertices_, first, x, y, z, vegetation_fade::FarShrink(
                Vector3::Distance(centre, cameraPosition), maximumDistance_));
        };
        for (const CrabBurrow& burrow : burrows) {
            const Vector3 centre(burrow.x, burrow.y, burrow.z);
            if (Vector3::DistanceSquared(centre, cameraPosition)
                    <= maximumDistance_ * maximumDistance_
                && frustum.Intersects(BoundingSphere(centre, burrow.radius * 1.6f))) {
                const std::size_t first = stagingVertices_.size();
                AppendBurrow(stagingVertices_, burrow);
                shrinkFrom(first, burrow.x, burrow.y, burrow.z, centre);
            }
        }
        for (const CrabState& crab : crabs) {
            const Vector3 centre(crab.x, crab.y + 0.15f * crab.scale, crab.z);
            const float distanceSquared = Vector3::DistanceSquared(centre, cameraPosition);
            if (distanceSquared > maximumDistance_ * maximumDistance_
                || !frustum.Intersects(BoundingSphere(centre, crab.scale * 0.75f))) {
                continue;
            }
            if (distanceSquared <= NearLodDistance * NearLodDistance) {
                const std::size_t firstVertex = stagingVertices_.size();
                AppendNearCrab(stagingVertices_, crab);
                if (stagingVertices_.size() - firstVertex != NearCrabVertexCount) {
                    throw std::logic_error("Near crab geometry violated its exact vertex budget");
                }
                shrinkFrom(firstVertex, crab.x, crab.y, crab.z, centre);
                ++visibleNearCrabCount_;
            } else {
                const std::size_t firstVertex = stagingVertices_.size();
                AppendFarCrab(stagingVertices_, crab);
                if (stagingVertices_.size() - firstVertex != FarCrabVertexCount) {
                    throw std::logic_error("Far crab geometry violated its exact vertex budget");
                }
                shrinkFrom(firstVertex, crab.x, crab.y, crab.z, centre);
                ++visibleFarCrabCount_;
            }
            ++visibleCrabCount_;
        }
        for (const HermitCrabState& crab : hermitCrabs) {
            const Vector3 centre(crab.x, crab.y + .10f * crab.scale, crab.z);
            if (Vector3::DistanceSquared(centre, cameraPosition)
                    > maximumDistance_ * maximumDistance_
                || !frustum.Intersects(BoundingSphere(centre, crab.scale * .34f))) {
                continue;
            }
            const std::size_t firstVertex = stagingVertices_.size();
            const bool far = Vector3::DistanceSquared(centre, cameraPosition)
                > HermitDetailDistance * HermitDetailDistance;
            AppendHermitCrab(stagingVertices_, crab, far);
            if (stagingVertices_.size() - firstVertex
                    != static_cast<std::size_t>(far ? FarHermitCrabVertexCount : HermitCrabVertexCount)) {
                throw std::logic_error(
                    "Hermit crab geometry violated its exact vertex budget");
            }
            shrinkFrom(firstVertex, crab.x, crab.y, crab.z, centre);
            ++visibleHermitCrabCount_;
        }
        for (const CoconutCrabState& crab : coconutCrabs) {
            const Vector3 centre(crab.x, crab.y + .18f * crab.scale, crab.z);
            if (Vector3::DistanceSquared(centre, cameraPosition)
                    > maximumDistance_ * maximumDistance_
                || !frustum.Intersects(BoundingSphere(centre, crab.scale * .85f))) {
                continue;
            }
            const std::size_t firstVertex = stagingVertices_.size();
            AppendCoconutCrab(stagingVertices_, crab);
            if (stagingVertices_.size() - firstVertex != CoconutCrabVertexCount) {
                throw std::logic_error(
                    "Coconut crab geometry violated its exact vertex budget");
            }
            shrinkFrom(firstVertex, crab.x, crab.y, crab.z, centre);
            ++visibleCoconutCrabCount_;
        }
        if (stagingVertices_.empty()) return;

        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        vertices_->SetData(stagingVertices_.data(), 0,
                           static_cast<int>(stagingVertices_.size()), SetDataOptions::Discard);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0,
                               static_cast<int>(stagingVertices_.size() / 3U));
        ++drawCallCount_;
    }
}
