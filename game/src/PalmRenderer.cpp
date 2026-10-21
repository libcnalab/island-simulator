#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/VegetationFade.hpp"
#include "IslandSimulator/PalmRenderer.hpp"
#include "IslandSimulator/SunShadowMap.hpp"

#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/JungleMaterial.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/WindGust.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/ProceduralMeshValidation.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SurfaceNormals.hpp"
#include "IslandSimulator/TrunkSurfaceMaterial.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBufferBinding.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColorTexture.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr float NearLodMinimumDistance = 125.0f;
        constexpr float NearLodDistanceRange = 20.0f;
        // Past a few tens of metres a crown's leaflets are a pixel or two wide and closer
        // together than that: the middle LOD draws every other strap at twice the width.
        constexpr float MidLodMinimumDistance = 26.0f;
        constexpr float MidLodDistanceRange = 8.0f;
        constexpr int NearLeafletStations = 34;
        constexpr int MidLeafletStations = NearLeafletStations / 2;
        constexpr int NearFrondSegments = 9;
        constexpr int FarFrondSegments = 5;
        constexpr float MaximumDistance = 520.0f;

        struct PalmGeometry
        {
            std::vector<VertexPositionColorTexture> vertices;
            std::vector<std::uint16_t> indices;
            // Custom shader only, parallel to vertices once any solid surface is appended.
            // A unit normal tags trunk/root wood; zero keeps the fronds' two-sided facet light.
            std::vector<Color> albedo;
            std::vector<Vector3> normals;
            // Parallel too, for the leaflets' edge corners only: the half-width across the strap,
            // packed into the frond's otherwise zero normal so the shader can keep a receding
            // leaflet about a pixel wide.
            std::vector<Vector3> across;
            // Filled while fronds are appended, for a fallen palm's crown: the frond each vertex
            // belongs to (NoFrond for wood and fruit), and each frond's bearing and arch.
            static constexpr std::uint8_t NoFrond = 255U;
            std::vector<std::uint8_t> frond;
            struct FrondShape { float angle = 0.0f, length = 1.0f, lift = 0.0f, drop = 0.0f; };
            std::vector<FrondShape> fronds;
        };

        struct PalmGpuVertex
        {
            float position[3];
            std::uint8_t color[4];
            float wind[2];
            float normal[3];
        };
        static_assert(std::is_trivially_copyable_v<PalmGpuVertex>);
        static_assert(std::is_standard_layout_v<PalmGpuVertex> && sizeof(PalmGpuVertex) == 36);

        const VertexDeclaration& PalmDeclaration()
        {
            static const VertexDeclaration declaration{
                VertexElement(offsetof(PalmGpuVertex, position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
                VertexElement(offsetof(PalmGpuVertex, color), VertexElementFormat::Color, VertexElementUsage::Color, 0),
                VertexElement(offsetof(PalmGpuVertex, wind), VertexElementFormat::Vector2, VertexElementUsage::TextureCoordinate, 0),
                VertexElement(offsetof(PalmGpuVertex, normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0)};
            return declaration;
        }

        // Weathered coconut stems are grey below and browner toward the crown.
        constexpr std::array<std::array<std::uint8_t, 3>, 4> PalmBarkLower{{
            {150, 143, 129}, {158, 151, 137}, {142, 134, 120}, {154, 146, 131}}};
        constexpr std::array<std::array<std::uint8_t, 3>, 4> PalmBarkUpper{{
            {132, 115, 92}, {138, 120, 96}, {125, 108, 86}, {135, 117, 93}}};
        constexpr float PalmTrunkBaseRadius = 0.21f, PalmTrunkTipRadius = 0.14f;
        constexpr float PalmBoleFlare = 0.55f, PalmBoleHeight = 0.38f;

        [[nodiscard]] Color PalmBarkAlbedo(const int variant, const float t)
        {
            const auto& lower = PalmBarkLower.at(static_cast<std::size_t>(variant));
            const auto& upper = PalmBarkUpper.at(static_cast<std::size_t>(variant));
            const float w = JungleMaterial::Smooth(0.15f, 0.85f, t);
            const auto channel = [w](std::uint8_t a, std::uint8_t b) {
                return static_cast<std::uint8_t>(std::lround(std::lerp(
                    static_cast<float>(a), static_cast<float>(b), w)));
            };
            return Color(channel(lower[0], upper[0]), channel(lower[1], upper[1]),
                         channel(lower[2], upper[2]), static_cast<std::uint8_t>(255));
        }

        void SetSolidSurface(PalmGeometry& geometry, const Color& albedo, const Vector3& normal)
        {
            geometry.albedo.resize(geometry.vertices.size(), Color(0, 0, 0, 0));
            geometry.normals.resize(geometry.vertices.size(), Vector3::Zero);
            geometry.albedo.back() = albedo;
            geometry.normals.back() = normal;
        }

        constexpr const char* VertexBody = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aWindData;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec4 aInstance0;
layout(location = 5) in vec4 aInstance1;
layout(location = 6) in vec4 aInstance2;
layout(location = 7) in vec4 aInstance3;

uniform mat4 uViewProjection;
uniform float uTime;
uniform vec3 uStormWind;   // xy: the direction the wind blows toward (x, z); z: the weather's wind (1 the trade wind)
uniform vec3 uCameraPosition;
uniform float uPixelSpan;   // radians one screen pixel spans
uniform float uLeafletPixels;   // the half-width in pixels a receding leaflet keeps

out vec3 vColor;
out vec3 vWorld;
out vec3 vLocal;
out vec3 vNormal;
out float vSolid;
out float vGroundHeight;
out float vWindWeight;
out float vLocalHeight;

void main()
{
    vec3 local = aPosition;
    float phase = aWindData.y * 6.28318530718;
    float broad = sin(uTime * 0.73 + phase + local.y * 0.11);
    float detail = sin(uTime * 1.19 + phase * 1.67 + local.x * 0.31 - local.z * 0.27);
    // A passing gust front leans the fronds together downwind within the same amplitude.
    float gust = windGust(aInstance3.xz, uTime);
    broad = mix(broad, 1.0, gust * 0.85);
    detail *= 1.0 - 0.4 * gust;
    // Above the trade wind the fronds sway harder and, below, blow downwind and thrash.
    float storm = clamp(uStormWind.z - 1.0, 0.0, 1.2);
    float sway = (broad * 0.115 + detail * 0.032) * aWindData.x * (1.0 + 0.8 * storm);
    local.x += sway;
    local.z += sway * 0.37;
    local.y -= abs(sway) * 0.045 * aWindData.x;

    mat4 instanceTransform = mat4(aInstance0, aInstance1, aInstance2, aInstance3);
    vec4 world = instanceTransform * vec4(local, 1.0);
    if (storm > 0.0) {
        float thrash = 0.5 + 0.5 * sin(uTime * 3.1 + phase * 2.3 + aPosition.x * 0.9 - aPosition.z * 0.7);
        float blown = storm * aWindData.x * (0.55 + 0.30 * thrash + 0.40 * gust);
        world.xz += uStormWind.xy * blown;
        world.y -= blown * 0.22;
    }
    // Reuse deterministic scale/yaw coefficients as a stable per-palm colour seed. The
    // green-only mask leaves trunk, root, and coconut materials unchanged, while near/far
    // meshes preserve one tint identity without another instance attribute.
    float tintSeed = fract(abs(aInstance0.x * 1.91 + aInstance0.z * 2.83
        + aInstance2.x * 3.47 + aInstance2.z * 4.29));
    float signedTint = tintSeed - 0.5;
    float foliageMask = smoothstep(0.14, 0.24, aColor.g - aColor.r);
    vec3 frondTint = vec3(1.0 + signedTint * 0.14,
                          1.0 + signedTint * 0.20,
                          1.0 - signedTint * 0.08);
    vColor = aColor.rgb * mix(vec3(1.0), frondTint, foliageMask);
    // Every vertex of a trunk or root primitive carries a unit normal; fronds carry zero.
    vSolid = step(0.25, dot(aNormal, aNormal));
    if (vSolid > 0.5) vColor *= 0.92 + 0.16 * tintSeed;
    // A leaflet's edge corners carry its half-width across the strap: as the palm recedes the
    // strap keeps about half a pixel either side, thinning into the crown smoothly instead of
    // breaking into sparkling sub-pixel slivers.
    float halfWidth = length(aNormal);
    if (vSolid < 0.5 && halfWidth > 0.0001) {
        float pixel = length(world.xyz - uCameraPosition) * uPixelSpan;
        float widen = min(max(0.0, uLeafletPixels * pixel - halfWidth) / halfWidth, 4.0);
        world.xyz += mat3(instanceTransform) * aNormal * widen;
    }
    // Uniform positive scale and yaw: no inverse-transpose is needed.
    vNormal = mat3(instanceTransform) * aNormal;
    vLocal = aPosition;
    vGroundHeight = aInstance3.y;
    vWorld = world.xyz;
    vWindWeight = aWindData.x;
    vLocalHeight = aPosition.y;
    gl_Position = uViewProjection * world;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";

        constexpr const char* FragmentBody = R"(
in vec3 vColor;
in vec3 vWorld;
in vec3 vLocal;
in vec3 vNormal;
in float vSolid;
in float vGroundHeight;
in float vWindWeight;
in float vLocalHeight;

uniform vec3 uCameraPosition;
uniform vec3 uToSun;
uniform vec2 uFaceDetailFade;   // the frond face shading fades out between these distances
uniform float uWetness;         // 0..1 rain-wet (WeatherSystem)

out vec4 fragColor;

void main()
{
    float distanceToCamera = length(vWorld - uCameraPosition);
    float fog = smoothstep(230.0, 620.0, distanceToCamera);
    // Derivatives stay outside the per-primitive material branch.
    vec3 faceVector = cross(dFdx(vWorld), dFdy(vWorld));
    // Closely spaced, slightly tilted leaf-scar rings; local metres, filtered by footprint.
    float scarPhase = vLocalHeight * 8.5 + vLocal.x * 0.45 - vLocal.z * 0.30
        + sin(vLocalHeight * 1.9) * 0.35 + sin(vLocalHeight * 4.3 + vLocal.x * 2.1) * 0.22;
    float scarFootprint = fwidth(scarPhase);
    vec3 material = vColor;
    if (vSolid > 0.5) {
        vec3 normal = vNormal * inversesqrt(max(dot(vNormal, vNormal), 0.00000001));
        vec3 sunDirection = normalize(uToSun);
        float aboveGround = vWorld.y - vGroundHeight;
        float daylight = dot(woodDaylightTerms, vec3(1.0,
            max(dot(normal, sunDirection), 0.0), max(normal.y, 0.0)));
        float scarDistance = abs(fract(scarPhase) - 0.5);
        // A narrow line per cycle; unresolved rings converge to its 11.5% area mean.
        float scar = mix(0.115, smoothstep(0.41, 0.475, scarDistance),
            materialDetailWeight(scarFootprint));
        float boleShade = mix(0.86, 1.0, smoothstep(0.0, 1.6, aboveGround));
        material *= daylight * (1.0 - scar * 0.13) * boleShade
            * trunkContactOcclusion(aboveGround);
    } else {
        // The frond faces' shading fades with the fragment's own distance on the near and far
        // meshes alike; switching it with the mesh stepped each crown's brightness by up to 8 %.
        float inverseFaceLength = inversesqrt(max(dot(faceVector, faceVector), 0.00000001));
        vec3 faceNormal = faceVector * inverseFaceLength;
        float twoSidedDiffuse = abs(dot(faceNormal, uToSun));
        float upwardFill = abs(faceNormal.y);
        float faceDetail = 1.0 - smoothstep(uFaceDetailFade.x, uFaceDetailFade.y,
                                            length(vWorld.xz - uCameraPosition.xz));
        material *= mix(1.0, 0.88 + twoSidedDiffuse * 0.16 + upwardFill * 0.04, faceDetail);
    }
    // Rain soaks the palms: the grey bark goes dark, the fronds a little.
    material *= 1.0 - clamp(uWetness, 0.0, 1.0) * (vSolid > 0.5 ? 0.28 : 0.10);
    // Directional sun shadow from neighbouring palms, crowns and structures.
    material *= mix(vec3(0.56, 0.60, 0.66), vec3(1.0), sunShadowVisibility(vWorld));
    vec3 color = mix(material, islandAtmosphereFog, fog);
    fragColor = vec4(color, 1.0);
}
)";

        [[nodiscard]] std::string ShaderSource(const bool embedded, const char* body,
                                               const bool fragment = false)
        {
            // Fragment stages share the jungle wood daylight and trunk contact constants.
            const std::string materials = fragment
                ? JungleMaterial::Shader() + std::string(BarkMaterial::Shader) + TrunkSurface::Shader()
                      + SunShadowMap::ReceiverShader
                : std::string();
            if (embedded) {
                return std::string("#version 300 es\nprecision highp float;\nprecision highp int;\n")
                    + environment::FogShaderDefinition + WindGust::Shader + materials + body;
            }
            return std::string("#version 330 core\n")
                + environment::FogShaderDefinition + WindGust::Shader + materials + body;
        }

        [[nodiscard]] CNA::Graphics::ShaderPackageEXT CreateShaderPackage()
        {
            using CNA::Graphics::ShaderCodeEXT;
            return CNA::Graphics::ShaderPackageEXT(
                {
                    ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,
                                  CNA::ShaderStageEXT::Vertex, "main",
                                  "palms/palms.es.vert.glsl", ShaderSource(true, VertexBody)),
                    ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,
                                  CNA::ShaderStageEXT::Fragment, "main",
                                  "palms/palms.es.frag.glsl", ShaderSource(true, FragmentBody, true)),
                    ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,
                                  CNA::ShaderStageEXT::Vertex, "main",
                                  "palms/palms.desktop.vert.glsl", ShaderSource(false, VertexBody)),
                    ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,
                                  CNA::ShaderStageEXT::Fragment, "main",
                                  "palms/palms.desktop.frag.glsl", ShaderSource(false, FragmentBody, true)),
                },
                {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        }

        [[nodiscard]] Color ShadeColor(const Color& color, const float factor)
        {
            const auto channel = [factor](const std::uint8_t value) {
                return static_cast<std::uint8_t>(std::lround(std::clamp(
                    static_cast<float>(value) * factor, 0.0f, 255.0f)));
            };
            return Color(channel(color.getRProperty()), channel(color.getGProperty()),
                         channel(color.getBProperty()), color.getAProperty());
        }

        void AppendRoots(PalmGeometry& geometry, const int variant)
        {
            // A coconut palm's exposed root mass: many low rounded ridges around a swollen
            // bole, not tall fins. Five columns per station give each ridge a convex section.
            const Color color(96 + variant * 3, 84 + variant * 2, 68, 255);
            constexpr int RootCount = 8;
            constexpr int Stations = 4;
            constexpr std::array<float, 5> Columns{-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
            for (int root = 0; root < RootCount; ++root) {
                const float rootIndex = static_cast<float>(root + 1);
                const float angleVariation = std::sin(
                    rootIndex * 2.17f + static_cast<float>(variant) * 1.31f);
                const float lengthVariation = 0.5f + 0.5f * std::sin(
                    rootIndex * 4.13f + static_cast<float>(variant) * 0.73f);
                const float widthVariation = 0.5f + 0.5f * std::sin(
                    rootIndex * 3.11f - static_cast<float>(variant) * 0.91f);
                const float angle = static_cast<float>(root) * 2.0f
                    * std::numbers::pi_v<float> / static_cast<float>(RootCount)
                    + static_cast<float>(variant) * 0.29f + angleVariation * 0.16f;
                const Vector3 direction(std::cos(angle), 0.0f, std::sin(angle));
                const Vector3 side(-direction.Z, 0.0f, direction.X);
                const float innerDistance = 0.16f;
                const float outerDistance = 0.40f + lengthVariation * 0.16f;
                const float crestHeight = 0.11f + lengthVariation * 0.06f;
                const float innerWidth = 0.050f + widthVariation * 0.020f;
                const float outerWidth = 0.020f + widthVariation * 0.010f;
                const auto base = static_cast<std::uint16_t>(geometry.vertices.size());
                for (int station = 0; station < Stations; ++station) {
                    const float s = static_cast<float>(station) / static_cast<float>(Stations - 1);
                    const Vector3 centre = direction * std::lerp(innerDistance, outerDistance, s)
                        + side * (0.035f * angleVariation * std::sin(std::numbers::pi_v<float> * s));
                    const float crest = crestHeight * std::pow(1.0f - s, 1.4f) + 0.014f * s;
                    const float halfWidth = std::lerp(innerWidth, outerWidth, s);
                    for (const float column : Columns) {
                        const float lift = std::pow(1.0f - column * column, 0.7f);
                        geometry.vertices.emplace_back(
                            centre + side * (column * halfWidth)
                                + Vector3::Up * std::max(0.012f, crest * lift),
                            ShadeColor(color, 0.80f + 0.22f * lift), Vector2(0.0f, 0.0f));
                        SetSolidSurface(geometry, color, Vector3::Zero);
                    }
                }
                constexpr auto columns = static_cast<std::uint16_t>(Columns.size());
                std::vector<std::array<float, 3>> positions;
                std::vector<std::array<float, 3>> outward;
                std::vector<std::uint16_t> local;
                for (std::size_t i = base; i < geometry.vertices.size(); ++i) {
                    const auto& position = geometry.vertices[i].Position;
                    const float column = Columns[(i - base) % columns];
                    const Vector3 reference = side * column
                        + Vector3::Up * (1.0f - std::abs(column) * 0.8f) + direction * 0.15f;
                    positions.push_back({position.X, position.Y, position.Z});
                    outward.push_back({reference.X, reference.Y, reference.Z});
                }
                for (int station = 0; station + 1 < Stations; ++station) {
                    for (std::uint16_t column = 0; column + 1 < columns; ++column) {
                        const auto a = static_cast<std::uint16_t>(station * columns + column);
                        const auto b = static_cast<std::uint16_t>(a + columns);
                        const auto c = static_cast<std::uint16_t>(a + 1U);
                        const auto d = static_cast<std::uint16_t>(b + 1U);
                        for (const auto index : {a, c, b, b, c, d}) {
                            local.push_back(index);
                            geometry.indices.push_back(static_cast<std::uint16_t>(base + index));
                        }
                    }
                }
                const auto normals = BuildOutwardSurfaceNormals(positions, local, outward);
                for (std::size_t i = 0; i < normals.size(); ++i) {
                    geometry.normals[base + i] = Vector3(normals[i][0], normals[i][1], normals[i][2]);
                }
            }
        }

        // A living palm stem's centre line and girth, t from the soil (0) to the crown (1).
        struct TrunkShape
        {
            float height = 9.4f, curveAngle = 0.72f, lean = 0.55f;
        };

        [[nodiscard]] TrunkShape PalmTrunkShape(const int variant)
        {
            return {9.4f + static_cast<float>(variant) * 0.55f,
                    0.72f + static_cast<float>(variant) * 1.37f,
                    0.55f + static_cast<float>((variant * 5) % 3) * 0.25f};
        }

        [[nodiscard]] Vector3 PalmTrunkCentre(const TrunkShape& shape, const float t)
        {
            const float bend = shape.lean * t * t;
            return Vector3(std::cos(shape.curveAngle) * bend
                               + std::sin(t * std::numbers::pi_v<float>) * 0.10f,
                           shape.height * t,
                           std::sin(shape.curveAngle) * bend
                               + std::cos(t * std::numbers::pi_v<float> * 0.8f) * 0.08f * t);
        }

        [[nodiscard]] float PalmTrunkRadius(const float y, const float t)
        {
            return (PalmTrunkBaseRadius + (PalmTrunkTipRadius - PalmTrunkBaseRadius) * t)
                * (1.0f + PalmBoleFlare * std::exp(-y / PalmBoleHeight));
        }

        void AppendTrunk(PalmGeometry& geometry, const int variant, const bool farMesh,
                         float& crownX, float& crownY, float& crownZ)
        {
            const int sides = farMesh ? 6 : 10;
            const TrunkShape shape = PalmTrunkShape(variant);
            const float height = shape.height;
            const float curveAngle = shape.curveAngle;
            const float lean = shape.lean;
            const std::uint16_t base = static_cast<std::uint16_t>(geometry.vertices.size());
            // Slender stem on a swollen bole. Near rings concentrate on the flare.
            std::vector<float> ringTs = farMesh
                ? std::vector<float>{0.0f, 0.06f}
                : std::vector<float>{0.0f, 0.014f, 0.036f, 0.075f};
            const int upperRings = farMesh ? 4 : 8;
            const float firstUpper = ringTs.back();
            for (int ring = 1; ring <= upperRings; ++ring) {
                ringTs.push_back(firstUpper + (1.0f - firstUpper) * static_cast<float>(ring)
                    / static_cast<float>(upperRings));
            }

            for (const float t : ringTs) {
                const Vector3 centre = PalmTrunkCentre(shape, t);
                const float centreX = centre.X;
                const float centreZ = centre.Z;
                const float y = centre.Y;
                const float radius = PalmTrunkRadius(y, t);
                // The flare's radial slope tilts the smooth normal upward near the soil.
                const float flareSlope = PalmTrunkBaseRadius * PalmBoleFlare / PalmBoleHeight
                    * std::exp(-y / PalmBoleHeight);
                const Color albedo = PalmBarkAlbedo(variant, t);
                for (int sideIndex = 0; sideIndex < sides; ++sideIndex) {
                    const float angle = 2.0f * std::numbers::pi_v<float>
                        * static_cast<float>(sideIndex) / static_cast<float>(sides);
                    // Stock-effect fallback keeps a baked fixed-sun side and alternating rings.
                    const float light = 0.66f + 0.32f
                        * std::max(0.0f, std::cos(angle) * 0.62f + std::sin(angle) * 0.35f);
                    const float ringVariation = static_cast<int>(std::lround(y)) % 2 == 0 ? 1.04f : 0.94f;
                    geometry.vertices.emplace_back(
                        Vector3(centreX + std::cos(angle) * radius, y,
                                centreZ + std::sin(angle) * radius),
                        ShadeColor(albedo, light * ringVariation), Vector2(0.0f, 0.0f));
                    SetSolidSurface(geometry, albedo, Vector3::Normalize(
                        Vector3(std::cos(angle), flareSlope, std::sin(angle))));
                }
            }

            const int rings = static_cast<int>(ringTs.size()) - 1;
            for (int ring = 0; ring < rings; ++ring) {
                for (int sideIndex = 0; sideIndex < sides; ++sideIndex) {
                    const int nextSide = (sideIndex + 1) % sides;
                    const auto lower = static_cast<std::uint16_t>(base + ring * sides + sideIndex);
                    const auto lowerNext = static_cast<std::uint16_t>(
                        base + ring * sides + nextSide);
                    const auto upper = static_cast<std::uint16_t>(
                        base + (ring + 1) * sides + sideIndex);
                    const auto upperNext = static_cast<std::uint16_t>(
                        base + (ring + 1) * sides + nextSide);
                    geometry.indices.insert(geometry.indices.end(),
                                            {lower, upper, lowerNext,
                                             lowerNext, upper, upperNext});
                }
            }

            crownX = std::cos(curveAngle) * lean;
            crownY = height;
            crownZ = std::sin(curveAngle) * lean;
        }

        void AppendBrokenTrunk(PalmGeometry& geometry, const int variant, const bool farMesh)
        {
            const int sides = farMesh ? 6 : 10;
            const int rings = farMesh ? 5 : 8;
            const float height = 4.15f + static_cast<float>(variant) * 0.42f;
            const float curveAngle = 0.61f + static_cast<float>(variant) * 1.19f;
            const float lean = 0.16f + 0.05f * static_cast<float>(variant % 3);
            const auto base = static_cast<std::uint16_t>(geometry.vertices.size());
            for (int ring = 0; ring <= rings; ++ring) {
                const float t = static_cast<float>(ring) / static_cast<float>(rings);
                const float y = height * t;
                const float bend = lean * t * t;
                const float centreX = std::cos(curveAngle) * bend;
                const float centreZ = std::sin(curveAngle) * bend;
                const float radius = (PalmTrunkBaseRadius
                    + (PalmTrunkTipRadius - PalmTrunkBaseRadius * 0.92f) * t)
                    * (1.0f + PalmBoleFlare * std::exp(-y / PalmBoleHeight));
                const float flareSlope = PalmTrunkBaseRadius * PalmBoleFlare / PalmBoleHeight
                    * std::exp(-y / PalmBoleHeight);
                const Color albedo = PalmBarkAlbedo(variant, t * 0.55f);
                for (int side = 0; side < sides; ++side) {
                    const float angle = 2.0f * std::numbers::pi_v<float>
                        * static_cast<float>(side) / static_cast<float>(sides);
                    geometry.vertices.emplace_back(
                        Vector3(centreX + std::cos(angle) * radius, y,
                                centreZ + std::sin(angle) * radius),
                        ShadeColor(albedo, 0.72f + 0.28f * std::max(0.0f,
                            std::cos(angle) * 0.62f + std::sin(angle) * 0.35f)),
                        Vector2(0.0f, 0.0f));
                    SetSolidSurface(geometry, albedo, Vector3::Normalize(
                        Vector3(std::cos(angle), flareSlope, std::sin(angle))));
                }
            }
            for (int ring = 0; ring < rings; ++ring) {
                for (int side = 0; side < sides; ++side) {
                    const int next = (side + 1) % sides;
                    const auto lower = static_cast<std::uint16_t>(base + ring * sides + side);
                    const auto lowerNext = static_cast<std::uint16_t>(base + ring * sides + next);
                    const auto upper = static_cast<std::uint16_t>(base + (ring + 1) * sides + side);
                    const auto upperNext = static_cast<std::uint16_t>(base + (ring + 1) * sides + next);
                    geometry.indices.insert(geometry.indices.end(),
                                            {lower, upper, lowerNext, lowerNext, upper, upperNext});
                }
            }

            // Pale splintered fracture plane, with an uneven perimeter rather than a sawn disk.
            const float centreX = std::cos(curveAngle) * lean;
            const float centreZ = std::sin(curveAngle) * lean;
            const Color fracture(190, 170, 132, 255);
            const auto capBase = static_cast<std::uint16_t>(geometry.vertices.size());
            geometry.vertices.emplace_back(Vector3(centreX, height - 0.025f, centreZ),
                                             fracture, Vector2(0.0f, 0.0f));
            SetSolidSurface(geometry, fracture, Vector3::Up);
            for (int side = 0; side < sides; ++side) {
                const float angle = 2.0f * std::numbers::pi_v<float>
                    * static_cast<float>(side) / static_cast<float>(sides);
                const float jag = 0.04f + 0.13f
                    * (0.5f + 0.5f * std::sin(static_cast<float>(side + 1) * 4.17f
                                              + static_cast<float>(variant)));
                geometry.vertices.emplace_back(
                    Vector3(centreX + std::cos(angle) * PalmTrunkTipRadius,
                            height + jag,
                            centreZ + std::sin(angle) * PalmTrunkTipRadius),
                    ShadeColor(fracture, 0.92f + 0.08f * static_cast<float>(side % 2)),
                    Vector2(0.0f, 0.0f));
                SetSolidSurface(geometry, fracture, Vector3::Normalize(
                    Vector3(std::cos(angle) * 0.25f, 1.0f, std::sin(angle) * 0.25f)));
            }
            for (int side = 0; side < sides; ++side) {
                geometry.indices.insert(geometry.indices.end(), {
                    capBase,
                    static_cast<std::uint16_t>(capBase + 1U + static_cast<std::uint16_t>(side)),
                    static_cast<std::uint16_t>(capBase + 1U
                        + static_cast<std::uint16_t>((side + 1) % sides))});
                const auto trunk = static_cast<std::uint16_t>(base + rings * sides + side);
                const auto trunkNext = static_cast<std::uint16_t>(
                    base + rings * sides + (side + 1) % sides);
                const auto fractureEdge = static_cast<std::uint16_t>(
                    capBase + 1U + static_cast<std::uint16_t>(side));
                const auto fractureEdgeNext = static_cast<std::uint16_t>(
                    capBase + 1U + static_cast<std::uint16_t>((side + 1) % sides));
                geometry.indices.insert(geometry.indices.end(),
                                        {trunk, fractureEdge, trunkNext,
                                         trunkNext, fractureEdge, fractureEdgeNext});
            }
        }

        void AppendCoconuts(PalmGeometry& geometry, const int variant,
                            const float crownX, const float crownY, const float crownZ)
        {
            if (variant != 1 && variant != 3) {
                return;
            }

            const int fruitCount = variant == 1 ? 5 : 4;
            for (int fruit = 0; fruit < fruitCount; ++fruit) {
                const float angle = static_cast<float>(variant) * 0.47f
                    + static_cast<float>(fruit) * 2.39996323f;
                const float orbit = 0.22f + 0.08f * static_cast<float>(fruit % 2);
                const Vector3 centre(crownX + std::cos(angle) * orbit,
                                     crownY - 0.30f - 0.12f * static_cast<float>(fruit % 3),
                                     crownZ + std::sin(angle) * orbit);
                const float radius = 0.22f + 0.025f * static_cast<float>((fruit + variant) % 3);
                const float halfHeight = radius * 1.14f;
                const Color baseColor = variant == 1
                    ? Color(103, 116 + fruit * 3, 45, 255)
                    : Color(119, 92 + fruit * 2, 42, 255);
                const float windPhase = std::fmod(
                    static_cast<float>(fruit) * 0.173f + static_cast<float>(variant) * 0.11f,
                    1.0f);
                const Vector2 windData(0.06f, windPhase);
                const auto base = static_cast<std::uint16_t>(geometry.vertices.size());
                geometry.vertices.emplace_back(
                    centre + Vector3::Up * halfHeight, ShadeColor(baseColor, 1.12f), windData);
                geometry.vertices.emplace_back(
                    centre - Vector3::Up * halfHeight, ShadeColor(baseColor, 0.70f), windData);
                geometry.vertices.emplace_back(
                    centre + Vector3(radius, 0.0f, 0.0f), ShadeColor(baseColor, 1.03f), windData);
                geometry.vertices.emplace_back(
                    centre - Vector3(radius, 0.0f, 0.0f), ShadeColor(baseColor, 0.78f), windData);
                geometry.vertices.emplace_back(
                    centre + Vector3(0.0f, 0.0f, radius), baseColor, windData);
                geometry.vertices.emplace_back(
                    centre - Vector3(0.0f, 0.0f, radius), ShadeColor(baseColor, 0.88f), windData);
                geometry.indices.insert(geometry.indices.end(), {
                    base, static_cast<std::uint16_t>(base + 2U),
                    static_cast<std::uint16_t>(base + 4U),
                    base, static_cast<std::uint16_t>(base + 4U),
                    static_cast<std::uint16_t>(base + 3U),
                    base, static_cast<std::uint16_t>(base + 3U),
                    static_cast<std::uint16_t>(base + 5U),
                    base, static_cast<std::uint16_t>(base + 5U),
                    static_cast<std::uint16_t>(base + 2U),
                    static_cast<std::uint16_t>(base + 1U),
                    static_cast<std::uint16_t>(base + 4U),
                    static_cast<std::uint16_t>(base + 2U),
                    static_cast<std::uint16_t>(base + 1U),
                    static_cast<std::uint16_t>(base + 3U),
                    static_cast<std::uint16_t>(base + 4U),
                    static_cast<std::uint16_t>(base + 1U),
                    static_cast<std::uint16_t>(base + 5U),
                    static_cast<std::uint16_t>(base + 3U),
                    static_cast<std::uint16_t>(base + 1U),
                    static_cast<std::uint16_t>(base + 2U),
                    static_cast<std::uint16_t>(base + 5U)});
            }
        }

        void AppendFronds(PalmGeometry& geometry, const int variant, const bool farMesh,
                          const float crownX, const float crownY, const float crownZ,
                          const int leafletStations = NearLeafletStations)
        {
            const int segments = farMesh ? FarFrondSegments : NearFrondSegments;
            const int frondCount = farMesh ? 12 : 12 + variant % 3;
            const float rotation = static_cast<float>(variant) * 0.41f;
            for (int frond = 0; frond < frondCount; ++frond) {
                const float angleJitter = 0.12f * std::sin(
                    static_cast<float>(frond + 1) * 2.31f
                    + static_cast<float>(variant) * 1.17f);
                const float angle = rotation + 2.0f * std::numbers::pi_v<float>
                    * static_cast<float>(frond) / static_cast<float>(frondCount) + angleJitter;
                const Vector3 direction(std::cos(angle), 0.0f, std::sin(angle));
                const Vector3 side(-direction.Z, 0.0f, direction.X);
                const bool emerging = farMesh
                    ? frond >= frondCount - 2
                    : (frond == variant % 3 || frond == frondCount / 2 + variant % 2);
                const float matureLength = 3.85f + 0.31f
                    * static_cast<float>((frond * 3 + variant) % 5);
                const float length = matureLength * (emerging ? 0.76f : 1.0f);
                const float lift = emerging ? 1.68f + 0.12f * static_cast<float>(variant % 2)
                    : 0.46f + 0.21f * static_cast<float>((frond + 2 * variant) % 4);
                const float drop = emerging ? 0.42f
                    : 1.00f + 0.19f * static_cast<float>((frond * 2 + variant) % 5);
                const float lateralBend = 0.10f * std::sin(
                    static_cast<float>(frond + 1) * 1.73f
                    + static_cast<float>(variant) * 0.91f);
                const std::uint16_t base = static_cast<std::uint16_t>(geometry.vertices.size());
                geometry.frond.resize(geometry.vertices.size(), PalmGeometry::NoFrond);
                const auto frondTag = static_cast<std::uint8_t>(geometry.fronds.size());
                geometry.fronds.push_back({angle, length, lift, drop});
                const Color leafBase = frond % 3 == 0
                    ? Color(42, 128 + variant * 3, 52, 255)
                    : (frond % 3 == 1 ? Color(51, 147, 58 + variant * 2, 255)
                                      : Color(34, 112, 49, 255));
                const float frondShade = 0.78f + 0.22f
                    * std::max(0.0f, direction.X * 0.62f + direction.Z * 0.35f);
                const float windPhase = std::fmod(
                    static_cast<float>(frond) / static_cast<float>(frondCount)
                        + static_cast<float>(variant) * 0.117f,
                    1.0f);

                for (int segment = 0; segment <= segments; ++segment) {
                    const float t = static_cast<float>(segment) / static_cast<float>(segments);
                    const float radial = length * t;
                    const float arch = lift * std::sin(t * std::numbers::pi_v<float>)
                        - drop * t * t;
                    const float lateral = lateralBend * length
                        * std::sin(t * std::numbers::pi_v<float>) * t;
                    const float width = segment == segments
                        ? (farMesh ? 0.035f : 0.018f)
                        : (farMesh
                               ? (emerging ? 0.055f : 0.12f) + (emerging ? 0.22f : 0.61f)
                                   * std::pow(std::sin(t * std::numbers::pi_v<float>), 0.62f)
                               : 0.035f + 0.075f
                                   * std::pow(std::sin(t * std::numbers::pi_v<float>), 0.70f));
                    const Vector3 centre(crownX + direction.X * radial + side.X * lateral,
                                         crownY + arch,
                                         crownZ + direction.Z * radial + side.Z * lateral);
                    const Color color = ShadeColor(
                        leafBase, frondShade * (0.92f + 0.08f * t));
                    const Vector2 windData(t * t, windPhase);
                    geometry.vertices.emplace_back(centre - side * width, color, windData);
                    geometry.vertices.emplace_back(centre + side * width, color, windData);
                }

                for (int segment = 0; segment < segments; ++segment) {
                    const auto left = static_cast<std::uint16_t>(base + segment * 2);
                    const auto right = static_cast<std::uint16_t>(left + 1U);
                    const auto nextLeft = static_cast<std::uint16_t>(left + 2U);
                    const auto nextRight = static_cast<std::uint16_t>(left + 3U);
                    geometry.indices.insert(geometry.indices.end(),
                                            {left, nextLeft, right,
                                             right, nextLeft, nextRight});
                }

                if (farMesh) {
                    geometry.frond.resize(geometry.vertices.size(), frondTag);
                    continue;
                }

                // A coconut frond carries a hundred or more leaflets a side: long narrow straps
                // that leave the rachis in a shallow V and hang down along their length. Short
                // broad triangles at a quarter-metre spacing read as a thorny fishbone instead.
                // the middle LOD's straps stand between the near mesh's pairs, as wide as both
                const int stride = NearLeafletStations / leafletStations;
                const float widthScale = static_cast<float>(stride);
                for (int station = 0; station < leafletStations; ++station) {
                    const float t = 0.060f + (static_cast<float>(station * stride)
                                              + 0.5f * static_cast<float>(stride - 1))
                        * (0.895f / static_cast<float>(NearLeafletStations - 1));
                    const float radial = length * t;
                    const float arch = lift * std::sin(t * std::numbers::pi_v<float>)
                        - drop * t * t;
                    const float lateral = lateralBend * length
                        * std::sin(t * std::numbers::pi_v<float>) * t;
                    const Vector3 centre(crownX + direction.X * radial + side.X * lateral,
                                         crownY + arch,
                                         crownZ + direction.Z * radial + side.Z * lateral);
                    const float envelope = std::pow(
                        std::sin(t * std::numbers::pi_v<float>), 0.50f);
                    const float lengthVariation = 0.90f + 0.06f
                        * static_cast<float>((station + frond + variant) % 3);
                    const float leafletLength = 0.82f * envelope * lengthVariation
                        * (emerging ? 0.78f : 1.0f);
                    const float sweep = 0.10f + 0.14f * (1.0f - t);
                    // emerging spears hold their leaflets up; mature fronds let them hang
                    const float rise = emerging ? 0.10f : 0.07f * (1.0f - t);
                    const float droop = (emerging ? 0.04f : 0.10f) + 0.30f * t;
                    const float pairVariation = std::sin(
                        static_cast<float>(station + 1) * 2.17f
                        + static_cast<float>(frond + 1) * 1.43f
                        + static_cast<float>(variant) * 0.79f);
                    const float rootWeight = t * t;
                    const float midT = std::min(1.0f, t + 0.04f), tipT = std::min(1.0f, t + 0.08f);
                    const Vector2 rootWind(rootWeight, windPhase);
                    const Vector2 midWind(midT * midT, windPhase);
                    const Vector2 tipWind(tipT * tipT, windPhase);

                    for (int sideIndex = 0; sideIndex < 2; ++sideIndex) {
                        const float sideSign = sideIndex == 0 ? -1.0f : 1.0f;
                        const float sideVariation = std::sin(
                            static_cast<float>(station + 1) * 1.71f
                            + static_cast<float>(frond + 1) * 2.37f
                            + static_cast<float>(variant) * 0.83f
                            + static_cast<float>(sideIndex) * 2.11f);
                        const Vector3 outward = side * sideSign;
                        const float stagger = sideSign * (0.012f + pairVariation * 0.008f);
                        const float sideLength = leafletLength * (0.97f + sideVariation * 0.075f);
                        const Vector3 root = centre + direction * stagger + outward * 0.02f;
                        const Vector3 reach = outward * sideLength
                            + direction * (sweep + pairVariation * 0.05f + sideVariation * 0.04f) * sideLength;
                        const float hang = droop * (0.94f + sideVariation * 0.14f);
                        // the strap rises a little off the rachis, then bends down to its tip
                        const Vector3 mid = root + reach * 0.5f + Vector3::Up * (rise - hang * 0.28f);
                        const Vector3 tip = root + reach - Vector3::Up * hang;
                        const float halfWidth = (0.021f + 0.004f * sideVariation) * widthScale;
                        const Vector3 across = direction * halfWidth;
                        const float sideShade = sideIndex == 0 ? 0.90f : 1.02f;
                        const Color leafletColor = ShadeColor(
                            leafBase, frondShade * sideShade * (0.96f + 0.05f * t));
                        const auto leafletBase = static_cast<std::uint16_t>(
                            geometry.vertices.size());
                        geometry.vertices.emplace_back(root - across, ShadeColor(leafletColor, 0.88f), rootWind);
                        geometry.vertices.emplace_back(root + across, ShadeColor(leafletColor, 0.94f), rootWind);
                        geometry.vertices.emplace_back(mid - across * 0.9f, ShadeColor(leafletColor, 1.02f), midWind);
                        geometry.vertices.emplace_back(mid + across * 0.9f, ShadeColor(leafletColor, 1.06f), midWind);
                        geometry.vertices.emplace_back(tip, leafletColor, tipWind);
                        const auto at = [leafletBase](const unsigned offset) {
                            return static_cast<std::uint16_t>(leafletBase + offset);
                        };
                        geometry.indices.insert(geometry.indices.end(), {
                            at(0U), at(1U), at(3U), at(0U), at(3U), at(2U), at(2U), at(3U), at(4U)});
                        geometry.across.resize(geometry.vertices.size(), Vector3::Zero);
                        geometry.across[at(0U)] = -across;
                        geometry.across[at(1U)] = across;
                        geometry.across[at(2U)] = -across * 0.9f;
                        geometry.across[at(3U)] = across * 0.9f;
                    }
                }
                geometry.frond.resize(geometry.vertices.size(), frondTag);
            }
        }

        void AppendDryFronds(PalmGeometry& geometry, const int variant,
                             const float crownX, const float crownY, const float crownZ)
        {
            if (variant != 0 && variant != 2) {
                return;
            }

            const int frondCount = variant == 0 ? 2 : 3;
            constexpr int Segments = 5;
            constexpr int LeafletStations = 4;
            for (int frond = 0; frond < frondCount; ++frond) {
                const float angle = static_cast<float>(variant) * 0.83f
                    + static_cast<float>(frond) * 2.21f + 0.34f;
                const Vector3 direction(std::cos(angle), 0.0f, std::sin(angle));
                const Vector3 side(-direction.Z, 0.0f, direction.X);
                const float length = 2.15f
                    + 0.24f * static_cast<float>((frond + variant) % 3);
                const float drop = 2.35f
                    + 0.28f * static_cast<float>((2 * frond + variant) % 3);
                const float phase = std::fmod(
                    0.19f + static_cast<float>(variant) * 0.17f
                        + static_cast<float>(frond) * 0.29f,
                    1.0f);
                const Color dryBase = variant == 0
                    ? Color(137, 98, 43, 255)
                    : Color(112, 77, 36, 255);
                const auto base = static_cast<std::uint16_t>(geometry.vertices.size());

                for (int segment = 0; segment <= Segments; ++segment) {
                    const float t = static_cast<float>(segment)
                        / static_cast<float>(Segments);
                    const float radial = 0.16f + length * t;
                    const float lateral = 0.10f
                        * std::sin(t * std::numbers::pi_v<float> * 1.35f
                                   + static_cast<float>(frond));
                    const Vector3 centre(crownX + direction.X * radial + side.X * lateral,
                                         crownY - 0.10f - drop * std::pow(t, 1.32f),
                                         crownZ + direction.Z * radial + side.Z * lateral);
                    const float width = 0.022f + 0.11f * (1.0f - t);
                    const Color color = ShadeColor(
                        dryBase, 1.04f - 0.20f * t
                            + 0.04f * static_cast<float>((segment + frond) % 2));
                    const Vector2 windData(0.12f + 0.30f * t * t, phase);
                    geometry.vertices.emplace_back(
                        centre - side * width, ShadeColor(color, 0.86f), windData);
                    geometry.vertices.emplace_back(
                        centre + side * width, color, windData);
                }

                for (int segment = 0; segment < Segments; ++segment) {
                    const auto left = static_cast<std::uint16_t>(base + segment * 2);
                    const auto right = static_cast<std::uint16_t>(left + 1U);
                    const auto nextLeft = static_cast<std::uint16_t>(left + 2U);
                    const auto nextRight = static_cast<std::uint16_t>(left + 3U);
                    geometry.indices.insert(geometry.indices.end(),
                                            {left, nextLeft, right,
                                             right, nextLeft, nextRight});
                }

                for (int station = 0; station < LeafletStations; ++station) {
                    const float t = 0.20f + static_cast<float>(station) * 0.19f;
                    const float radial = 0.16f + length * t;
                    const float lateral = 0.10f
                        * std::sin(t * std::numbers::pi_v<float> * 1.35f
                                   + static_cast<float>(frond));
                    const Vector3 centre(crownX + direction.X * radial + side.X * lateral,
                                         crownY - 0.10f - drop * std::pow(t, 1.32f),
                                         crownZ + direction.Z * radial + side.Z * lateral);
                    const float remnantLength = 0.14f + 0.38f * (1.0f - t);
                    const float rootWindWeight = 0.12f + 0.30f * t * t;
                    for (int sideIndex = 0; sideIndex < 2; ++sideIndex) {
                        if ((station * 2 + sideIndex + frond + variant) % 5 == 0) {
                            continue;
                        }
                        const float sideSign = sideIndex == 0 ? -1.0f : 1.0f;
                        const Vector3 outward = side * sideSign;
                        const float irregularity = 0.88f + 0.08f
                            * static_cast<float>((station + sideIndex + frond) % 3);
                        const Vector3 rootBack = centre - direction * 0.095f
                            + outward * 0.025f;
                        const Vector3 rootFront = centre + direction * 0.085f
                            + outward * 0.040f;
                        const Vector3 tip = centre + outward * remnantLength * irregularity
                            + direction * (0.05f + 0.035f * sideSign)
                            - Vector3::Up * (0.15f + 0.18f * t);
                        const Color color = ShadeColor(
                            dryBase, (sideIndex == 0 ? 0.82f : 0.96f) - 0.10f * t);
                        const Vector2 rootWind(rootWindWeight, phase);
                        const Vector2 tipWind(rootWindWeight + 0.08f, phase);
                        const auto leafletBase = static_cast<std::uint16_t>(
                            geometry.vertices.size());
                        geometry.vertices.emplace_back(rootBack, color, rootWind);
                        geometry.vertices.emplace_back(
                            tip, ShadeColor(color, 0.91f), tipWind);
                        geometry.vertices.emplace_back(
                            rootFront, ShadeColor(color, 1.05f), rootWind);
                        geometry.indices.insert(geometry.indices.end(), {
                            leafletBase,
                            static_cast<std::uint16_t>(leafletBase + 1U),
                            static_cast<std::uint16_t>(leafletBase + 2U)});
                    }
                }
            }
        }

        [[nodiscard]] PalmGeometry CreatePalmGeometry(const int variant, const bool farMesh,
                                                      const int leafletStations = NearLeafletStations)
        {
            PalmGeometry geometry;
            geometry.vertices.reserve(farMesh ? 160U : 1200U);
            geometry.indices.reserve(farMesh ? 480U : 3000U);
            if (!farMesh) {
                AppendRoots(geometry, variant);
            }
            float crownX = 0.0f;
            float crownY = 0.0f;
            float crownZ = 0.0f;
            AppendTrunk(geometry, variant, farMesh, crownX, crownY, crownZ);
            AppendFronds(geometry, variant, farMesh, crownX, crownY, crownZ, leafletStations);
            if (!farMesh) {
                AppendDryFronds(geometry, variant, crownX, crownY, crownZ);
                AppendCoconuts(geometry, variant, crownX, crownY, crownZ);
            }
            return geometry;
        }

        // A storm-felled palm: the stem as it stood, but the crown is no longer a wheel of live
        // fronds radiating about the fallen stem. It has collapsed onto the ground at the head:
        // the fronds that were underneath were crushed forward, the upper ones flopped over to
        // either side, and all of them lie on the drawn ground, dried to straw and tawny brown.
        // Built per palm, in its own transform, against the rendered terrain.
        [[nodiscard]] PalmGeometry CreateFallenPalmGeometry(const int variant, const bool farMesh,
                                                            const Matrix& transform,
                                                            const RenderedTerrainSurface& ground)
        {
            constexpr float Pi = std::numbers::pi_v<float>;
            PalmGeometry geometry;
            geometry.vertices.reserve(farMesh ? 160U : 4800U);
            geometry.indices.reserve(farMesh ? 480U : 9000U);
            if (!farMesh) AppendRoots(geometry, variant);
            float crownX = 0.0f;
            float crownY = 0.0f;
            float crownZ = 0.0f;
            AppendTrunk(geometry, variant, farMesh, crownX, crownY, crownZ);
            PalmGeometry crown;
            AppendFronds(crown, variant, farMesh, crownX, crownY, crownZ);

            const Matrix inverse = Matrix::Invert(transform);
            const Vector3 worldUp(0.0f, 1.0f, 0.0f);
            const float scale = Vector3::TransformNormal(worldUp, transform).Length();
            // world up seen from the palm's own frame, the stem laid along the ground, and across it
            const Vector3 up = Vector3::Normalize(Vector3::TransformNormal(worldUp, inverse));
            const Vector3 forward = Vector3::Normalize(worldUp - up * up.Y);
            const Vector3 sideways = Vector3::Cross(up, forward);
            const Vector3 crownLocal(crownX, crownY, crownZ);
            const Vector3 crownWorld = Vector3::Transform(crownLocal, transform);
            const float crownHeight = std::max(
                0.08f, crownWorld.Y - ground.Sample(crownWorld.X, crownWorld.Z).height);
            const Vector3 crownFoot = crownLocal - up * (crownHeight / scale);
            // the bearing, in the living crown, of the frond that pointed straight down
            const float downAngle = std::atan2(-up.Z, -up.X);
            const auto smooth = [](const float edge0, const float edge1, const float x) {
                const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
                return t * t * (3.0f - 2.0f * t);
            };
            const auto blend = [](const Color& from, const Color& to, const float amount) {
                const auto channel = [amount](const std::uint8_t a, const std::uint8_t b) {
                    return static_cast<std::uint8_t>(std::lround(
                        static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * amount));
                };
                return Color(channel(from.getRProperty(), to.getRProperty()),
                             channel(from.getGProperty(), to.getGProperty()),
                             channel(from.getBProperty(), to.getBProperty()), static_cast<std::uint8_t>(255U));
            };
            const auto light = [](const Color& color) {
                return 0.30f * static_cast<float>(color.getRProperty())
                    + 0.59f * static_cast<float>(color.getGProperty())
                    + 0.11f * static_cast<float>(color.getBProperty());
            };

            struct LaidFrond
            {
                Vector3 living, livingSide, along, lateral;
                float reach = 1.0f, arch = 0.0f, floor = 0.03f, seed = 0.0f, referenceLight = 1.0f;
                Color dry;
            };
            std::vector<LaidFrond> laid(crown.fronds.size());
            for (std::size_t f = 0; f < laid.size(); ++f) {
                const auto& shape = crown.fronds[f];
                const float seed = static_cast<float>(f) + 0.37f * static_cast<float>(variant);
                LaidFrond& frond = laid[f];
                frond.living = Vector3(std::cos(shape.angle), 0.0f, std::sin(shape.angle));
                frond.livingSide = Vector3(-frond.living.Z, 0.0f, frond.living.X);
                const float fromDown = std::remainder(shape.angle - downAngle, 2.0f * Pi);
                const float bearing = 0.80f * fromDown + 0.12f * std::sin(seed * 2.71f);
                frond.along = forward * std::cos(bearing) + sideways * std::sin(bearing);
                frond.lateral = Vector3::Cross(up, frond.along);
                // the crushed lower fronds meet the ground at once; the upper ones arch up and
                // over before they come down, heaping the crown at the stem's head
                const float upper = std::abs(fromDown) / Pi;
                frond.reach = (0.28f + 0.40f * upper) * shape.length;
                frond.arch = 0.10f + 0.45f * upper * upper;
                frond.floor = 0.03f + 0.025f * (0.5f + 0.5f * std::sin(seed * 1.93f));
                frond.seed = seed;
                const float age = 0.5f + 0.5f * std::sin(seed * 2.37f + 0.4f);
                const float weathered = 0.5f + 0.5f * std::sin(seed * 1.37f + 2.0f);
                frond.dry = blend(blend(Color(152, 118, 70), Color(98, 72, 46), age),
                                  Color(116, 106, 90), 0.50f * weathered * weathered);
            }
            for (std::size_t i = 0; i < crown.vertices.size(); ++i) {
                const std::uint8_t f = crown.frond[i];
                if (f != PalmGeometry::NoFrond && laid[f].referenceLight == 1.0f)
                    laid[f].referenceLight = std::max(1.0f, light(crown.vertices[i].Color));
            }
            const auto lay = [&](const Vector3& point, const LaidFrond& frond,
                                 const PalmGeometry::FrondShape& shape, const float narrow) {
                const Vector3 q = point - crownLocal;
                const float a = std::max(0.0f, Vector3::Dot(q, frond.living));
                const float b = Vector3::Dot(q, frond.livingSide) * narrow;
                const float t = std::clamp(a / shape.length, 0.0f, 1.0f);
                // a leaflet's hang below the rachis (or rise above it)
                const float hang = q.Y - (shape.lift * std::sin(t * Pi) - shape.drop * t * t);
                const Vector3 flat = crownFoot + frond.along * (a * 0.96f) + frond.lateral * b;
                const Vector3 world = Vector3::Transform(flat, transform);
                const float along = std::clamp(a / frond.reach, 0.0f, 1.0f);
                const float falling = 1.0f - smooth(0.0f, 1.0f, along);
                const float raised = crownHeight * falling * std::sqrt(falling)
                    + frond.arch * std::sin(along * Pi);
                // leaflets hang from a frond still in the air and flatten where it lies
                float height = raised + hang * scale * (0.25f + 0.75f * smooth(0.05f, 0.60f, raised));
                // a lying frond's stalk stands proud, its dead leaflets slope off it to the
                // ground and curl a little
                const float midrib = std::clamp(1.0f - (std::abs(b) * scale - 0.06f) / 0.35f, 0.0f, 1.0f);
                const float curl = 0.025f * (0.5f + 0.5f * std::sin(
                    a * scale * 23.0f + b * scale * 9.0f + frond.seed * 3.1f));
                height = std::max(height, frond.floor + 0.07f * midrib + curl * (1.0f - midrib));
                const float groundY = ground.Sample(world.X, world.Z).height;
                return Vector3::Transform(Vector3(world.X, groundY + height, world.Z), inverse);
            };

            const auto base = static_cast<std::uint16_t>(geometry.vertices.size());
            geometry.across.resize(geometry.vertices.size(), Vector3::Zero);
            // Each frond's block opens with its rachis ribbon, two corners per segment end. On
            // the near mesh, with the leaflets drawn, a dead frond's stalk is a narrow rib.
            const std::size_t rachisCorners = farMesh ? 0U
                : 2U * static_cast<std::size_t>(NearFrondSegments + 1);
            std::size_t blockStart = 0U;
            for (std::size_t i = 0; i < crown.vertices.size(); ++i) {
                VertexPositionColorTexture vertex = crown.vertices[i];
                const std::uint8_t f = crown.frond[i];
                if (i == 0U || f != crown.frond[i - 1U]) blockStart = i;
                Vector3 across = i < crown.across.size() ? crown.across[i] : Vector3::Zero;
                if (f != PalmGeometry::NoFrond) {
                    const LaidFrond& frond = laid[f];
                    const auto& shape = crown.fronds[f];
                    const float narrow = i - blockStart < rachisCorners ? 0.40f : 1.0f;
                    const Vector3 position = lay(vertex.Position, frond, shape, narrow);
                    if (across.LengthSquared() > 0.0f)
                        across = lay(vertex.Position + across, frond, shape, narrow) - position;
                    vertex.Position = position;
                    // dead: the vertex keeps its light and shade in the dried frond's colour
                    vertex.Color = ShadeColor(frond.dry, std::clamp(
                        light(vertex.Color) / frond.referenceLight, 0.70f, 1.25f));
                    // and, lying on the ground, no longer sways
                    vertex.TextureCoordinate.X = 0.0f;
                }
                geometry.vertices.push_back(vertex);
                geometry.across.push_back(across);
            }
            for (const std::uint16_t index : crown.indices)
                geometry.indices.push_back(static_cast<std::uint16_t>(base + index));
            return geometry;
        }

        [[nodiscard]] PalmGeometry CreateBrokenPalmGeometry(const int variant, const bool farMesh)
        {
            PalmGeometry geometry;
            geometry.vertices.reserve(farMesh ? 64U : 180U);
            geometry.indices.reserve(farMesh ? 220U : 620U);
            if (!farMesh) AppendRoots(geometry, variant);
            AppendBrokenTrunk(geometry, variant, farMesh);
            return geometry;
        }

        [[nodiscard]] std::size_t CellIndex(const float x, const float z) noexcept
        {
            constexpr float cellWidth = 2.0f * TerrainMeshLayout::Extent
                / static_cast<float>(TerrainMeshLayout::CellsPerAxis);
            const int column = std::clamp(static_cast<int>(
                std::floor((x + TerrainMeshLayout::Extent) / cellWidth)),
                0, TerrainMeshLayout::CellsPerAxis - 1);
            const int row = std::clamp(static_cast<int>(
                std::floor((z + TerrainMeshLayout::Extent) / cellWidth)),
                0, TerrainMeshLayout::CellsPerAxis - 1);
            return static_cast<std::size_t>(row * TerrainMeshLayout::CellsPerAxis + column);
        }
    }

    const VertexDeclaration& PalmRenderer::InstanceDeclaration()
    {
        static const VertexDeclaration declaration{
            VertexElement(0, VertexElementFormat::Vector4,
                          VertexElementUsage::TextureCoordinate, 1),
            VertexElement(16, VertexElementFormat::Vector4,
                          VertexElementUsage::TextureCoordinate, 2),
            VertexElement(32, VertexElementFormat::Vector4,
                          VertexElementUsage::TextureCoordinate, 3),
            VertexElement(48, VertexElementFormat::Vector4,
                          VertexElementUsage::TextureCoordinate, 4)};
        return declaration;
    }

    PalmRenderer::PalmRenderer(GraphicsDevice& device, const VegetationLayout& layout,
                               const float maximumDistanceScale,
                               const RenderedTerrainSurface* ground)
        : device_(device), maximumDistance_(MaximumDistance * maximumDistanceScale)
    {
        if (!std::isfinite(maximumDistanceScale) || maximumDistanceScale <= 0.0f
            || maximumDistanceScale > 1.0f) {
            throw std::invalid_argument("Palm maximum distance scale must be in (0, 1]");
        }
        instancingSupported_ = device_.SupportsCapability(CNA::GraphicsCapability::Instancing);
        effect_ = std::make_unique<BasicEffect>(device_);
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        effect_->setFogColorProperty(Vector3(environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue));
        effect_->setFogStartProperty(230.0f);
        effect_->setFogEndProperty(620.0f);

        if (!instancingSupported_) {
            fallbackReason_ = "the active renderer does not support hardware instancing";
        } else if (!device_.SupportsCapability(CNA::GraphicsCapability::CustomEffects)) {
            fallbackReason_ = "the active renderer does not support custom effects";
        } else {
            try {
                const CNA::Graphics::ShaderPackageEXT package = CreateShaderPackage();
                customEffect_ = std::make_unique<ShaderEffect>(device_, package);
                if (customEffect_->IsEffectValid()) {
                    customShaderReady_ = true;
                } else {
                    fallbackReason_ = "the palm shader did not compile: "
                        + customEffect_->GetCompileErrorEXT();
                    customEffect_.reset();
                }
            } catch (const std::exception& exception) {
                fallbackReason_ = std::string("the portable palm shader could not be created: ")
                    + exception.what();
                customEffect_.reset();
            }
        }

        CreateCells(layout, ground);
        CreateMeshes(ground);
        std::cout << "island-simulator: palms " << totalPalmCount_ << " in "
                  << TerrainMeshLayout::CellCount << " cells, "
                  << (instancingSupported_ ? "hardware instancing" : "per-palm fallback")
                  << '\n';
        std::cout << "island-simulator: palm storm history " << fallenPalmCount_
                  << " fallen, " << brokenPalmCount_ << " broken trunks\n";
        if (!fallen_.empty()) {
            constexpr const char* fallenDiagnostic =
                "island-simulator: fallen palms lie flat on the drawn ground, their crowns collapsed into dried fronds at the stem's head";
            std::cout << fallenDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(fallenDiagnostic);
        }
        if (customShaderReady_) {
            std::cout << "island-simulator: palm portable wind shader with near/far mesh LOD\n";
            std::cout << "island-simulator: palm near mesh segmented fuller leaflet silhouettes\n";
            std::cout << "island-simulator: palm middle LOD draws every other leaflet strap twice as wide past 26-34 m\n";
            constexpr const char* leafletDiagnostic =
                "island-simulator: palm near leaflets use staggered asymmetric silhouettes";
            std::cout << leafletDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(leafletDiagnostic);
            constexpr const char* crownDiagnostic =
                "island-simulator: palm crowns use dense narrow leaflets, irregular arcs, and emerging spear fronds";
            std::cout << crownDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(crownDiagnostic);
            constexpr const char* farCrownDiagnostic =
                "island-simulator: palm far crowns retain twelve arched ribbons and two narrow emerging spears";
            std::cout << farCrownDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(farCrownDiagnostic);
            constexpr const char* lightingDiagnostic =
                "island-simulator: palm near geometry uses fixed-sun facet shading";
            std::cout << lightingDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(lightingDiagnostic);
            std::cout << "island-simulator: palm near root ridges ground flared trunk bases\n";
            constexpr const char* windDiagnostic =
                "island-simulator: palms lean away from the fixed east trade wind by coastal windward exposure";
            std::cout << windDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(windDiagnostic);
            constexpr const char* trunkDiagnostic =
                "island-simulator: palm trunks use smooth normals, one-sided daylight, grey weathered bark and filtered leaf-scar rings";
            std::cout << trunkDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(trunkDiagnostic);
            std::cout << "island-simulator: palm procedural bark scars and restrained coconut "
                         "clusters\n";
            constexpr const char* dryFrondDiagnostic =
                "island-simulator: selected palm near crowns include sparse hanging dry fronds";
            std::cout << dryFrondDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(dryFrondDiagnostic);
            constexpr const char* tintDiagnostic =
                "island-simulator: palm per-instance frond tint reuses existing transforms";
            std::cout << tintDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(tintDiagnostic);
            constexpr const char* stormDiagnostic =
                "island-simulator: palm groves include grounded fallen crowns and splintered storm-broken trunks";
            std::cout << stormDiagnostic << '\n';
            MirrorRuntimeDiagnosticToPlatformLog(stormDiagnostic);
        } else {
            std::cerr << "island-simulator: palm stock-effect fallback: " << fallbackReason_
                      << '\n';
        }
    }

    PalmRenderer::~PalmRenderer() = default;

    void PalmRenderer::CreateMeshes(const RenderedTerrainSurface* ground)
    {
        const auto upload = [this](const PalmGeometry& geometry, PalmMesh& mesh,
                                   const char* label) {
            ValidateProceduralTriangleMeshOrThrow(
                geometry.vertices.size(), geometry.indices, label);
            mesh.vertexCount = static_cast<int>(geometry.vertices.size());
            mesh.primitiveCount = static_cast<int>(geometry.indices.size() / 3U);
            if (customShaderReady_) {
                std::vector<PalmGpuVertex> packed;
                packed.reserve(geometry.vertices.size());
                for (std::size_t i = 0; i < geometry.vertices.size(); ++i) {
                    const auto& vertex = geometry.vertices[i];
                    const bool solid = i < geometry.normals.size()
                        && geometry.normals[i].LengthSquared() > 0.25f;
                    const Vector3 normal = solid ? Vector3::Normalize(geometry.normals[i])
                        : (i < geometry.across.size() ? geometry.across[i] : Vector3::Zero);
                    if (!std::isfinite(normal.X) || !std::isfinite(normal.Y) || !std::isfinite(normal.Z))
                        throw std::runtime_error(std::string(label) + " contains an invalid solid normal");
                    const Color& color = solid ? geometry.albedo[i] : vertex.Color;
                    packed.push_back({{vertex.Position.X, vertex.Position.Y, vertex.Position.Z},
                        {color.getRProperty(), color.getGProperty(), color.getBProperty(), color.getAProperty()},
                        {vertex.TextureCoordinate.X, vertex.TextureCoordinate.Y},
                        {normal.X, normal.Y, normal.Z}});
                }
                mesh.vertices = std::make_unique<VertexBuffer>(device_, PalmDeclaration(),
                    mesh.vertexCount, BufferUsage::WriteOnly);
                mesh.vertices->SetData(packed.data(), mesh.vertexCount);
            } else {
                mesh.vertices = std::make_unique<VertexBuffer>(
                    device_, VertexPositionColorTexture::getVertexDeclarationStatic(),
                    mesh.vertexCount, BufferUsage::WriteOnly);
                mesh.vertices->SetData(geometry.vertices.data(), mesh.vertexCount);
            }
            mesh.indices = std::make_unique<IndexBuffer>(
                device_, IndexElementSize::SixteenBits, static_cast<int>(geometry.indices.size()),
                BufferUsage::WriteOnly);
            mesh.indices->SetData(geometry.indices.data(), static_cast<int>(geometry.indices.size()));
        };

        for (std::size_t variant = 0; variant < nearMeshes_.size(); ++variant) {
            upload(CreatePalmGeometry(static_cast<int>(variant), false), nearMeshes_[variant],
                   "palm near LOD");
            upload(CreatePalmGeometry(static_cast<int>(variant), false, MidLeafletStations),
                   midMeshes_[variant], "palm middle LOD");
            upload(CreatePalmGeometry(static_cast<int>(variant), true), farMeshes_[variant],
                   "palm far LOD");
            upload(CreateBrokenPalmGeometry(static_cast<int>(variant), false),
                   brokenNearMeshes_[variant], "broken palm near LOD");
            upload(CreateBrokenPalmGeometry(static_cast<int>(variant), true),
                   brokenFarMeshes_[variant], "broken palm far LOD");
        }
        if (ground == nullptr) return;
        for (FallenPalm& fallen : fallen_) {
            upload(CreateFallenPalmGeometry(fallen.variant, false, fallen.transform.front(), *ground),
                   fallen.nearMesh, "fallen palm near LOD");
            upload(CreateFallenPalmGeometry(fallen.variant, true, fallen.transform.front(), *ground),
                   fallen.farMesh, "fallen palm far LOD");
        }
    }

    void PalmRenderer::CreateCells(const VegetationLayout& layout,
                                   const RenderedTerrainSurface* ground)
    {
        for (const PalmPlacement& palm : layout.Palms()) {
            PalmCell& cell = cells_[CellIndex(palm.x, palm.z)];
            PalmDrawInstance instance;
            if (palm.condition == PalmCondition::Fallen) {
                const float side = palm.variant % 2U == 0U ? 1.0f : -1.0f;
                const auto fallenTransform = [&palm, side](const float tilt) {
                    return Matrix::CreateScale(palm.scale)
                        * Matrix::CreateRotationZ(side * tilt)
                        // Compensate the local Z-axis fall rotation so local +Y follows the same
                        // world-space direction used by placement, collision, and the contact patch.
                        * Matrix::CreateRotationY(palm.damageYawRadians
                                                  + side * std::numbers::pi_v<float> * 0.5f)
                        * Matrix::CreateTranslation(palm.x, palm.y + 0.24f * palm.scale, palm.z);
                };
                // A felled stem lies on the ground with its heavy crown down. Without the drawn
                // ground a fixed tilt keeps every stem's curve clear of flat soil; with it the
                // stem is laid as near flat as the ground along it allows, its crown resting low.
                float tilt = 1.43f;
                if (ground != nullptr) {
                    const TrunkShape shape = PalmTrunkShape(palm.variant);
                    for (float candidate = 1.62f; candidate > 1.43f; candidate -= 0.01f) {
                        const Matrix transform = fallenTransform(candidate);
                        bool clear = true;
                        for (int step = 2; step <= 12 && clear; ++step) {
                            const float t = static_cast<float>(step) / 12.0f;
                            const Vector3 centre = PalmTrunkCentre(shape, t);
                            const Vector3 world = Vector3::Transform(centre, transform);
                            const float underside = world.Y
                                - PalmTrunkRadius(centre.Y, t) * palm.scale;
                            clear = underside >= ground->Sample(world.X, world.Z).height - 0.04f;
                        }
                        if (clear) {
                            tilt = candidate;
                            break;
                        }
                    }
                }
                instance.transform = fallenTransform(tilt);
            } else {
                // Lean away from the fixed trade wind in world space, after yaw so the lean does
                // not depend on the authored facing. Collision stays at the base.
                const float lean = wind::PalmLeanRadians(
                    wind::Exposure(palm.x, palm.z, palm.shoreDistance));
                const Vector3 leanAxis(-wind::DirectionZ, 0.0f, wind::DirectionX);
                instance.transform = Matrix::CreateScale(palm.scale)
                    * Matrix::CreateRotationY(palm.yawRadians)
                    * Matrix::CreateFromAxisAngle(leanAxis, lean)
                    * Matrix::CreateTranslation(palm.x, palm.y, palm.z);
            }
            instance.x = palm.x;
            instance.z = palm.z;
            const float normalizedYaw = palm.yawRadians
                / (2.0f * std::numbers::pi_v<float>);
            const float nearLodDistance = NearLodMinimumDistance
                + NearLodDistanceRange * normalizedYaw;
            instance.nearLodDistanceSquared = nearLodDistance * nearLodDistance;
            const float midLodDistance = MidLodMinimumDistance
                + MidLodDistanceRange * std::fmod(normalizedYaw * 7.0f + 0.37f, 1.0f);
            instance.midLodDistanceSquared = midLodDistance * midLodDistance;
            instance.variant = palm.variant;
            instance.condition = palm.condition;
            if (palm.condition == PalmCondition::Fallen && ground != nullptr) {
                instance.fallen = static_cast<std::uint16_t>(fallen_.size());
                FallenPalm& fallen = fallen_.emplace_back();
                fallen.variant = palm.variant;
                fallen.transform.push_back(instance.transform);
            }
            cell.instances.push_back(instance);
            ++totalPalmCount_;
            fallenPalmCount_ += palm.condition == PalmCondition::Fallen ? 1 : 0;
            brokenPalmCount_ += palm.condition == PalmCondition::Broken ? 1 : 0;

            const float horizontalExtent = palm.condition == PalmCondition::Fallen
                ? 15.0f * palm.scale : 5.8f * palm.scale;
            const float verticalExtent = palm.condition == PalmCondition::Fallen
                ? 6.2f * palm.scale
                : (palm.condition == PalmCondition::Broken ? 6.5f : 15.0f) * palm.scale;
            const Vector3 minimum(palm.x - horizontalExtent, palm.y - 0.2f,
                                  palm.z - horizontalExtent);
            const Vector3 maximum(palm.x + horizontalExtent,
                                  palm.y + verticalExtent,
                                  palm.z + horizontalExtent);
            if (!cell.occupied) {
                cell.bounds = BoundingBox(minimum, maximum);
                cell.occupied = true;
            } else {
                cell.bounds = BoundingBox(Vector3::Min(cell.bounds.Min, minimum),
                                          Vector3::Max(cell.bounds.Max, maximum));
            }
        }

        for (auto& transforms : visibleNearTransforms_) {
            transforms.reserve(static_cast<std::size_t>(totalPalmCount_));
        }
        for (auto& transforms : visibleMidTransforms_) {
            transforms.reserve(static_cast<std::size_t>(totalPalmCount_));
        }
        for (auto& transforms : visibleFarTransforms_) {
            transforms.reserve(static_cast<std::size_t>(totalPalmCount_));
        }
        for (auto& transforms : visibleBrokenNearTransforms_) {
            transforms.reserve(VegetationLayout::BrokenPalmTargetCount);
        }
        for (auto& transforms : visibleBrokenFarTransforms_) {
            transforms.reserve(VegetationLayout::BrokenPalmTargetCount);
        }
    }

    namespace
    {
        constexpr const char* PalmSunShadowVertex = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aWindData;
layout(location = 3) in vec3 aNormal;
layout(location = 4) in vec4 aInstance0;
layout(location = 5) in vec4 aInstance1;
layout(location = 6) in vec4 aInstance2;
layout(location = 7) in vec4 aInstance3;
uniform mat4 uSunViewProjection;
out float vSunDepth;
void main()
{
    mat4 instanceTransform = mat4(aInstance0, aInstance1, aInstance2, aInstance3);
    vec4 clip = uSunViewProjection * (instanceTransform * vec4(aPosition, 1.0));
    vSunDepth = clip.z / clip.w;
    gl_Position = clip;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";
    }

    void PalmRenderer::DrawSunShadow(SunShadowMap& map)
    {
        if (!customShaderReady_ || !instancingSupported_ || sunShadowUnavailable_) return;
        if (!sunShadowEffect_) {
            try {
                sunShadowEffect_ = map.CreateCasterEffect(PalmSunShadowVertex, "palms");
            } catch (const std::exception& error) {
                std::cout << "island-simulator: palm sun shadow casters unavailable: "
                          << error.what() << '\n';
                sunShadowUnavailable_ = true;
                return;
            }
        }
        for (auto& list : visibleFarTransforms_) list.clear();
        for (auto& list : visibleBrokenFarTransforms_) list.clear();
        for (FallenPalm& fallen : fallen_) fallen.drawNear = fallen.drawFar = false;
        int casters = 0;
        for (const PalmCell& cell : cells_) {
            if (!cell.occupied) continue;
            for (const PalmDrawInstance& instance : cell.instances) {
                if (!map.CastsInto(instance.x, instance.z)) continue;
                if (instance.fallen != PalmDrawInstance::NoFallen) {
                    FallenPalm& fallen = fallen_[instance.fallen];
                    fallen.transform.front() = instance.transform;
                    fallen.drawFar = true;
                    ++casters;
                    continue;
                }
                (instance.condition == PalmCondition::Broken
                     ? visibleBrokenFarTransforms_[instance.variant]
                     : visibleFarTransforms_[instance.variant]).push_back(instance.transform);
                ++casters;
            }
        }
        float matrix[16];
        map.LightViewProjectionColumnMajor(matrix);
        sunShadowEffect_->Apply();
        sunShadowEffect_->SetUniformMat4("uSunViewProjection", matrix);
        for (std::size_t variant = 0; variant < farMeshes_.size(); ++variant) {
            DrawVariant(farMeshes_[variant], visibleFarTransforms_[variant]);
            DrawVariant(brokenFarMeshes_[variant], visibleBrokenFarTransforms_[variant]);
        }
        for (FallenPalm& fallen : fallen_)
            if (fallen.drawFar) DrawVariant(fallen.farMesh, fallen.transform);
        device_.SetVertexBuffers({});
        device_.setIndicesProperty(nullptr);
        map.CountCasters(casters);
    }

    void PalmRenderer::DrawVariant(PalmMesh& mesh, const std::vector<Matrix>& transforms)
    {
        if (transforms.empty()) {
            return;
        }
        device_.setIndicesProperty(mesh.indices.get());

        if (instancingSupported_) {
            const int count = static_cast<int>(transforms.size());
            if (mesh.instances == nullptr || count > mesh.instanceCapacity) {
                int newCapacity = std::max(16, mesh.instanceCapacity);
                while (newCapacity < count) {
                    newCapacity *= 2;
                }
                mesh.instances = std::make_unique<DynamicVertexBuffer>(
                    device_, InstanceDeclaration(), newCapacity, BufferUsage::WriteOnly);
                mesh.instanceCapacity = newCapacity;
            }
            ValidateInstancedDrawRangeOrThrow(
                mesh.instanceCapacity, 0, count, "palm instance");
            mesh.instances->SetDataRaw(transforms.data(), count, static_cast<int>(sizeof(Matrix)));
            device_.SetVertexBuffers({VertexBufferBinding(mesh.vertices.get(), 0, 0),
                                      VertexBufferBinding(mesh.instances.get(), 0, 1)});
            if (!customShaderReady_) {
                effect_->setWorldProperty(Matrix::getIdentityProperty());
                effect_->Apply();
            }
            device_.DrawInstancedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                            mesh.vertexCount, 0, mesh.primitiveCount, count);
            ++drawCallCount_;
            return;
        }

        device_.SetVertexBuffer(mesh.vertices.get());
        for (const Matrix& transform : transforms) {
            effect_->setWorldProperty(transform);
            effect_->Apply();
            device_.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                          mesh.vertexCount, 0, mesh.primitiveCount);
            ++drawCallCount_;
        }
    }

    void PalmRenderer::Draw(const Matrix& view, const Matrix& projection,
                            const Vector3& cameraPosition, const float timeSeconds)
    {
        for (auto& transforms : visibleNearTransforms_) {
            transforms.clear();
        }
        for (auto& transforms : visibleMidTransforms_) transforms.clear();
        for (FallenPalm& fallen : fallen_) fallen.drawNear = fallen.drawFar = false;
        for (auto& transforms : visibleFarTransforms_) {
            transforms.clear();
        }
        for (auto& transforms : visibleBrokenNearTransforms_) transforms.clear();
        for (auto& transforms : visibleBrokenFarTransforms_) transforms.clear();
        visiblePalmCount_ = 0;
        visibleNearPalmCount_ = 0;
        visibleFarPalmCount_ = 0;
        visibleCellCount_ = 0;
        drawCallCount_ = 0;

        const BoundingFrustum frustum(view * projection);
        for (const PalmCell& cell : cells_) {
            if (!cell.occupied || frustum.Contains(cell.bounds) == ContainmentType::Disjoint) {
                continue;
            }
            // Whole cells used to vanish together when their centre crossed the maximum
            // distance; the cell is now rejected only once all of it lies beyond, and each palm
            // shrinks to nothing over the last part of its own range.
            const float cellDeltaX = std::max({cell.bounds.Min.X - cameraPosition.X, 0.0f,
                                               cameraPosition.X - cell.bounds.Max.X});
            const float cellDeltaZ = std::max({cell.bounds.Min.Z - cameraPosition.Z, 0.0f,
                                               cameraPosition.Z - cell.bounds.Max.Z});
            if (cellDeltaX * cellDeltaX + cellDeltaZ * cellDeltaZ > maximumDistance_ * maximumDistance_) {
                continue;
            }
            bool cellVisible = false;
            for (const PalmDrawInstance& instance : cell.instances) {
                const float instanceDeltaX = instance.x - cameraPosition.X;
                const float instanceDeltaZ = instance.z - cameraPosition.Z;
                const float instanceDistanceSquared = instanceDeltaX * instanceDeltaX
                    + instanceDeltaZ * instanceDeltaZ;
                const float shrink = vegetation_fade::FarShrink(vegetation_fade::SlantDistance(instanceDistanceSquared, cameraPosition.Y - instance.transform.M42), maximumDistance_,
                                                                vegetation_fade::TreeShrinkFraction);
                if (shrink <= 0.0f) continue;
                const Matrix transform = shrink < 1.0f ? Matrix::CreateScale(shrink) * instance.transform
                                                       : instance.transform;
                const bool nearLod = instanceDistanceSquared <= instance.nearLodDistanceSquared;
                if (instance.fallen != PalmDrawInstance::NoFallen) {
                    FallenPalm& fallen = fallen_[instance.fallen];
                    fallen.transform.front() = transform;
                    (nearLod ? fallen.drawNear : fallen.drawFar) = true;
                    ++(nearLod ? visibleNearPalmCount_ : visibleFarPalmCount_);
                } else if (nearLod) {
                    (instance.condition == PalmCondition::Broken
                         ? visibleBrokenNearTransforms_[instance.variant]
                         : instanceDistanceSquared <= instance.midLodDistanceSquared
                         ? visibleNearTransforms_[instance.variant]
                         : visibleMidTransforms_[instance.variant]).push_back(transform);
                    ++visibleNearPalmCount_;
                } else {
                    (instance.condition == PalmCondition::Broken
                         ? visibleBrokenFarTransforms_[instance.variant]
                         : visibleFarTransforms_[instance.variant]).push_back(transform);
                    ++visibleFarPalmCount_;
                }
                ++visiblePalmCount_;
                cellVisible = true;
            }
            if (cellVisible) ++visibleCellCount_;
        }

        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        if (customShaderReady_) {
            float viewProjection[16];
            (view * projection).ToColumnMajor(viewProjection);
            customEffect_->Apply();
            customEffect_->SetUniformMat4("uViewProjection", viewProjection);
            customEffect_->SetUniformVec3("uCameraPosition", cameraPosition.X,
                                          cameraPosition.Y, cameraPosition.Z);
            customEffect_->SetUniformVec3(
                "uToSun", sun_light::UniformDirection().x, sun_light::UniformDirection().y,
                sun_light::UniformDirection().z);
            customEffect_->SetUniformFloat("uTime", timeSeconds);
            customEffect_->SetUniformFloat("uPixelSpan", 2.0f / std::max(1.0f, projection.M22
                * static_cast<float>(device_.getViewportProperty().getHeightProperty())));
            {
                const auto& gusts = WindGust::Live();
                customEffect_->SetUniformVec4("uGustClock", gusts.travelled, gusts.spacing, gusts.floor, gusts.live ? 1.0f : 0.0f);
            }
            if (sunShadow_) sunShadow_->Bind(*customEffect_);
            else SunShadowMap::BindDisabled(*customEffect_);
        } else {
            effect_->setViewProperty(view);
            effect_->setProjectionProperty(projection);
        }

        if (customShaderReady_) {
            customEffect_->SetUniformVec2("uFaceDetailFade", NearLodMinimumDistance - 10.0f,
                                          NearLodMinimumDistance + NearLodDistanceRange + 10.0f);
            customEffect_->SetUniformFloat("uWetness", weather_now::Current().wetness);
            customEffect_->SetUniformVec3("uStormWind", wind::DirectionX, wind::DirectionZ, weather_now::Current().wind);
        }
        for (std::size_t variant = 0; variant < nearMeshes_.size(); ++variant) {
            if (customShaderReady_) customEffect_->SetUniformFloat("uLeafletPixels", 0.5f);
            DrawVariant(nearMeshes_[variant], visibleNearTransforms_[variant]);
            if (customShaderReady_) customEffect_->SetUniformFloat("uLeafletPixels", 1.0f);
            DrawVariant(midMeshes_[variant], visibleMidTransforms_[variant]);
            DrawVariant(farMeshes_[variant], visibleFarTransforms_[variant]);
            DrawVariant(brokenNearMeshes_[variant], visibleBrokenNearTransforms_[variant]);
            DrawVariant(brokenFarMeshes_[variant], visibleBrokenFarTransforms_[variant]);
        }
        if (customShaderReady_) customEffect_->SetUniformFloat("uLeafletPixels", 0.5f);
        for (FallenPalm& fallen : fallen_) {
            if (fallen.drawNear) DrawVariant(fallen.nearMesh, fallen.transform);
            if (fallen.drawFar) DrawVariant(fallen.farMesh, fallen.transform);
        }
        device_.SetVertexBuffers({});
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
    }
}
