#include "IslandSimulator/ContactShadowRenderer.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include <limits>
#include <stdexcept>

#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/SunShadowFade.hpp"
#include "IslandSimulator/ContactShadowPatch.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/MonkeyTroop.hpp"
#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/GoatHerd.hpp"
#include "IslandSimulator/SpiderContactShadow.hpp"
#include "IslandSimulator/SnakeContactShadow.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/ShipwreckLayout.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/HelicopterLayout.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
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
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include <iostream>
#include <string>
#include <type_traits>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int EllipseSegments = 12;
        constexpr int VerticesPerShadow = EllipseSegments * 3;
        constexpr float SurfaceOffset = 0.035f;
        constexpr float PalmMaximumDistance = 92.0f;
        constexpr float CanopyMaximumDistance = 88.0f;
        constexpr float PropMaximumDistance = 78.0f;
        constexpr float MonkeyMaximumDistance = 105.0f;
        constexpr float CameraRefreshDistanceSquared = 0.25f * 0.25f;
        constexpr float MonkeyRefreshDistanceSquared = 0.08f * 0.08f;
        constexpr float ViewRefreshThreshold = 0.008f;
        constexpr float ProjectionRefreshThreshold = 0.001f;
        constexpr std::size_t MaximumSnakeShadows=16;
        constexpr std::size_t MaximumSpiderShadows=16;

        [[nodiscard]] std::size_t MaximumShadowCount(
            const VegetationLayout& vegetation, const JungleLayout& jungle,
            const NaturalPropLayout& naturalProps) noexcept
        {
            const auto landRockCount = static_cast<std::size_t>(std::count_if(
                naturalProps.Rocks().begin(), naturalProps.Rocks().end(),
                [](const RockPlacement& rock) { return !rock.offshore; }));
            return vegetation.Palms().size() + jungle.Canopy().size() + landRockCount
                + naturalProps.FallenLogs().size() + naturalProps.Driftwood().size()
                + MonkeyTroop::MonkeyCount + BoarHerd::BoarCount;
        }

        [[nodiscard]] float DistanceFade(const float distanceSquared,
                                         const float maximumDistance) noexcept
        {
            const float fadeStart = maximumDistance * 0.78f;
            if (distanceSquared <= fadeStart * fadeStart) {
                return 1.0f;
            }
            const float distance = std::sqrt(distanceSquared);
            return std::clamp((maximumDistance - distance)
                                  / (maximumDistance - fadeStart),
                              0.0f, 1.0f);
        }

        [[nodiscard]] std::uint8_t FadedAlpha(const int alpha, const float fade) noexcept
        {
            return static_cast<std::uint8_t>(
                std::clamp(static_cast<int>(std::lround(static_cast<float>(alpha) * fade)),
                           0, 255));
        }

        template <class GroundHeight>
        void AppendTerrainEllipse(std::vector<VertexPositionColor>& vertices,
                                  const GroundHeight& groundHeight,
                                  const float x, const float z,
                                  const float forwardX, const float forwardZ,
                                  const float forwardRadius, const float sideRadius,
                                  const float offsetX, const float offsetZ,
                                  const std::uint8_t centreAlpha)
        {
            const float centreX = x + offsetX;
            const float centreZ = z + offsetZ;
            const float sideX = -forwardZ;
            const float sideZ = forwardX;
            const Vector3 centre(centreX, groundHeight(centreX, centreZ) + SurfaceOffset,
                                 centreZ);
            const Color centreColor(0, 0, 0, static_cast<int>(centreAlpha));
            const Color edgeColor(0, 0, 0, 0);

            std::array<Vector3, EllipseSegments> edgePoints;
            for (int segment = 0; segment < EllipseSegments; ++segment) {
                const float angle = static_cast<float>(segment)
                    * 2.0f * std::numbers::pi_v<float>
                    / static_cast<float>(EllipseSegments);
                const float along = std::cos(angle) * forwardRadius;
                const float across = std::sin(angle) * sideRadius;
                const float edgeX = centreX + forwardX * along + sideX * across;
                const float edgeZ = centreZ + forwardZ * along + sideZ * across;
                edgePoints[static_cast<std::size_t>(segment)] = Vector3(
                    edgeX, groundHeight(edgeX, edgeZ) + SurfaceOffset, edgeZ);
            }
            for (int segment = 0; segment < EllipseSegments; ++segment) {
                const int nextSegment = (segment + 1) % EllipseSegments;
                vertices.emplace_back(centre, centreColor);
                vertices.emplace_back(edgePoints[static_cast<std::size_t>(segment)], edgeColor);
                vertices.emplace_back(edgePoints[static_cast<std::size_t>(nextSegment)], edgeColor);
            }
        }

        // A static shadow's vertex on the GPU: its place, its shadow's centre and reach, its
        // base opacity and whether it is a structure's mass shadow (which gives way to the sun
        // shadow map where the map's own shadows reach).
        struct StaticGpuVertex
        {
            float position[3];
            float centreReach[4];
            float alphaStructure[2];
        };
        static_assert(std::is_trivially_copyable_v<StaticGpuVertex> && sizeof(StaticGpuVertex) == 36);

        const VertexDeclaration& StaticDeclaration()
        {
            static const VertexDeclaration declaration{
                VertexElement(offsetof(StaticGpuVertex, position), VertexElementFormat::Vector3,
                              VertexElementUsage::Position, 0),
                VertexElement(offsetof(StaticGpuVertex, centreReach), VertexElementFormat::Vector4,
                              VertexElementUsage::TextureCoordinate, 0),
                VertexElement(offsetof(StaticGpuVertex, alphaStructure), VertexElementFormat::Vector2,
                              VertexElementUsage::TextureCoordinate, 1)};
            return declaration;
        }

        // The CPU's DistanceFade and sun_shadow_fade::Strength, per vertex: full to 78 % of the
        // shadow's reach, nothing past it (the vertex then leaves the clip volume, so a faded
        // shadow costs no fragments); a structure's mass shadow takes the share of its shadow
        // that the map has faded out, measured along the hour's shading light as the map is.
        std::string StaticVertexShader()
        {
            namespace f = sun_shadow_fade;
            return std::string(R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aCentreReach;
layout(location = 2) in vec2 aAlphaStructure;
uniform mat4 uViewProjection;
uniform vec3 uCamera;
uniform vec4 uMap;   // x: 1 while a sun shadow map casts the structures, yz: its anchor (the camera's cell)
uniform vec3 uSun;   // toward the hour's shading light
out float vAlpha;
const float MapRadius = )") + std::to_string(f::MapRadius) + R"(;
const float TargetHeight = )" + std::to_string(f::TargetHeight) + R"(;
const float EdgeBand = )" + std::to_string(f::EdgeBand) + R"(;
const float FarFadeStart = )" + std::to_string(f::FarFadeStart) + R"(;
const float FarFadeEnd = )" + std::to_string(f::FarFadeEnd) + R"(;
vec2 lightOffset(vec3 p, vec2 column) {
    float horizontal = max(length(uSun.xz), 1e-4);
    vec2 azimuth = uSun.xz / horizontal;
    vec2 d = p.xz - column;
    return vec2(d.x * azimuth.y - d.y * azimuth.x, -(d.x * azimuth.x + d.y * azimuth.y) * uSun.y + (p.y - TargetHeight) * horizontal);
}
float mapStrength(vec3 p) {
    vec2 fromAnchor = lightOffset(p, uMap.yz);
    float edge = (MapRadius - max(abs(fromAnchor.x), abs(fromAnchor.y))) / (2.0 * MapRadius);
    if (edge <= 0.0) return 0.0;
    vec2 v = lightOffset(p, uCamera.xz);
    vec2 v2 = v * v;
    float far = smoothstep(FarFadeStart, FarFadeEnd, sqrt(sqrt(v2.x * v2.x + v2.y * v2.y)));
    return smoothstep(0.0, EdgeBand, edge) * (1.0 - far);
}
void main() {
    float reach = aCentreReach.w;
    float distanceToCamera = distance(aCentreReach.xyz, uCamera);
    float fade = clamp((reach - distanceToCamera) / (reach * 0.22), 0.0, 1.0);
    float share = (aAlphaStructure.y > 0.5 && uMap.x > 0.5) ? 1.0 - mapStrength(aPosition) : 1.0;
    vAlpha = aAlphaStructure.x * fade * share;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    if (vAlpha < 0.002) gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
}
)";
        }

        constexpr const char* StaticFragmentShader = R"(
in float vAlpha;
out vec4 fragColor;
void main() { fragColor = vec4(0.0, 0.0, 0.0, vAlpha); }
)";

        [[nodiscard]] bool IsVisible(const BoundingFrustum& frustum,
                                     const Vector3& cameraPosition,
                                     const float x, const float y, const float z,
                                     const float radius, const float maximumDistance,
                                     float& fade) noexcept
        {
            const Vector3 centre(x, y, z);
            const float distanceSquared = Vector3::DistanceSquared(centre, cameraPosition);
            if (distanceSquared > maximumDistance * maximumDistance
                || !frustum.Intersects(BoundingSphere(centre, radius))) {
                return false;
            }
            fade = DistanceFade(distanceSquared, maximumDistance);
            return fade > 0.0f;
        }
    }

    ContactShadowRenderer::ContactShadowRenderer(
        GraphicsDevice& device, const IslandTerrain& terrain,
        const VegetationLayout& vegetation, const JungleLayout& jungle,
        const NaturalPropLayout& naturalProps, const ShipwreckLayout& shipwreck,
        const float maximumDistanceScale, const HutPlacement* hut,
        const HelicopterPlacement* helicopter, const RenderedTerrainSurface* ground)
        : device_(device),
          terrain_(terrain),
          ground_(ground),
          effect_(std::make_unique<BasicEffect>(device)),
          monkeyMaximumDistance_(MonkeyMaximumDistance * maximumDistanceScale)
    {
        if (!std::isfinite(maximumDistanceScale) || maximumDistanceScale <= 0.0f
            || maximumDistanceScale > 1.0f) {
            throw std::invalid_argument(
                "Contact-shadow distance scale must be finite and in (0, 1]");
        }
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(false);
        const std::size_t maximumShadowCount = MaximumShadowCount(
            vegetation, jungle, naturalProps);
        staticShadows_.reserve(maximumShadowCount - MonkeyTroop::MonkeyCount - BoarHerd::BoarCount);
        staticVertices_.reserve((maximumShadowCount - MonkeyTroop::MonkeyCount - BoarHerd::BoarCount)
                                * static_cast<std::size_t>(VerticesPerShadow));
        stagingVertices_.reserve(maximumShadowCount
                                 * static_cast<std::size_t>(VerticesPerShadow));

        const auto cacheShadow = [this](const StaticShadowKind kind,
                                        const float x, const float y, const float z,
                                        const float radius, const float maximumDistance,
                                        const float forwardX, const float forwardZ,
                                        const float forwardRadius, const float sideRadius,
                                        const float forwardOffset,
                                        const std::uint8_t centreAlpha) {
            const std::size_t firstVertex = staticVertices_.size();
            AppendTerrainEllipse(staticVertices_, [this](const float hx, const float hz) { return terrain_.Height(hx, hz); }, x, z, forwardX, forwardZ,
                                 forwardRadius, sideRadius, forwardX*forwardOffset,
                                 forwardZ*forwardOffset, centreAlpha);
            staticShadows_.push_back({Vector3(x, y, z), radius, maximumDistance,
                                      firstVertex, VerticesPerShadow, kind});
        };

        const float horizontalSunLength = std::hypot(environment::SunDirectionX,
                                                      environment::SunDirectionZ);
        const float shadowDirectionX = -environment::SunDirectionX / horizontalSunLength;
        const float shadowDirectionZ = -environment::SunDirectionZ / horizontalSunLength;

        for (const PalmPlacement& palm : vegetation.Palms()) {
            if (palm.condition == PalmCondition::Fallen) {
                const float directionX = std::sin(palm.damageYawRadians);
                const float directionZ = std::cos(palm.damageYawRadians);
                const float length = (8.6f + 0.45f * static_cast<float>(palm.variant))
                    * palm.scale;
                const float centreX = palm.x + directionX * length * 0.48f;
                const float centreZ = palm.z + directionZ * length * 0.48f;
                cacheShadow(StaticShadowKind::Palm, centreX,
                            terrain_.Height(centreX, centreZ), centreZ,
                            length * 0.62f, PalmMaximumDistance * maximumDistanceScale,
                            directionX, directionZ, length * 0.54f,
                            0.34f + 0.12f * palm.scale, 0.0f, 38U);
                continue;
            }
            cacheShadow(StaticShadowKind::Palm, palm.x, palm.y, palm.z,
                        1.2f * palm.scale, PalmMaximumDistance * maximumDistanceScale,
                        shadowDirectionX, shadowDirectionZ,
                        0.95f * palm.scale, 0.48f * palm.scale,
                        0.20f * palm.scale, 44U);
        }
        for (const CanopyPlacement& tree : jungle.Canopy()) {
            cacheShadow(StaticShadowKind::Canopy, tree.x, tree.y, tree.z,
                        1.5f * tree.scale, CanopyMaximumDistance * maximumDistanceScale,
                        shadowDirectionX, shadowDirectionZ,
                        1.18f * tree.scale, 0.72f * tree.scale,
                        0.24f * tree.scale, 40U);
        }
        const auto cachePatch = [&](const ContactFootprint& footprint, const StaticShadowKind kind,
                                    const float distance, const ContactPatchDetail detail) {
            const std::size_t first = staticVertices_.size();
            for (const auto& vertex : BuildContactPatch(terrain_, footprint, detail)) {
                staticVertices_.emplace_back(Vector3(vertex.x, vertex.y, vertex.z),
                    Color(0, 0, 0, static_cast<int>(std::lround(vertex.opacity * 255))));
            }
            if (staticVertices_.size() == first) return;
            staticShadows_.push_back({{}, 0, distance * maximumDistanceScale, first,
                                      staticVertices_.size() - first, kind});
        };
        for (const auto& rock : naturalProps.Rocks()) {
            if (!rock.offshore) cachePatch(LandRockContactFootprint(rock), StaticShadowKind::Rock,
                                          PropMaximumDistance, ContactPatchDetail::Compact);
        }
        for (const auto& log : naturalProps.FallenLogs()) {
            cachePatch(LogContactFootprint(log), StaticShadowKind::Wood,
                       PropMaximumDistance, ContactPatchDetail::Compact);
        }
        for (const auto& wood : naturalProps.Driftwood()) {
            cachePatch(DriftwoodContactFootprint(wood), StaticShadowKind::Wood,
                       PropMaximumDistance, ContactPatchDetail::Compact);
        }
        const auto cacheCoastal = [&](const ContactFootprint& footprint) {
            cachePatch(footprint, StaticShadowKind::Coastal, 110.0f, ContactPatchDetail::Coastal);
        };
        for (const auto& rock : naturalProps.Rocks()) {
            if (rock.offshore) cacheCoastal(RockContactFootprint(rock));
        }
        for (const auto& footprint : HullContactFootprints(shipwreck.Ship())) cacheCoastal(footprint);
        for (const auto& debris : shipwreck.Debris()) cacheCoastal(DebrisContactFootprint(debris));
        if(hut) for(const auto& footprint:HutContactFootprints(*hut))
            cachePatch(footprint,StaticShadowKind::Hut,90.0f,ContactPatchDetail::Coastal);

        // Permanent midday and only three landmark receivers make a bounded projected-footprint
        // fallback more useful than a cross-platform shadow-map pass. These broad, low-alpha
        // patches extend the existing batch and project the elevated mass opposite the fixed sun.
        const auto cacheStructureShadow=[&](const float x,const float y,const float z,
                                            const float yaw,const float halfLength,
                                            const float halfWidth,const float height,
                                            const float distance,const std::uint8_t alpha) {
            const float axisX=std::cos(yaw),axisZ=std::sin(yaw);
            const float castLength=height*horizontalSunLength/environment::SunDirectionY;
            const std::size_t first=staticVertices_.size();
            AppendTerrainEllipse(staticVertices_,[this](const float hx,const float hz){return terrain_.Height(hx,hz);},x,z,axisX,axisZ,
                halfLength+castLength*.24f,halfWidth+castLength*.18f,
                shadowDirectionX*castLength*.5f,shadowDirectionZ*castLength*.5f,alpha);
            staticShadows_.push_back({Vector3(x,y,z),halfLength+castLength,
                distance*maximumDistanceScale,first,VerticesPerShadow,
                StaticShadowKind::Structure});
        };
        const auto& wreck=shipwreck.Ship();
        cacheStructureShadow(wreck.x,wreck.y,wreck.z,wreck.yawRadians,
            wreck.length*.49f,wreck.beam*.48f,wreck.hullHeight,125.0f,28U);
        if(hut) cacheStructureShadow(hut->x,hut->floorY,hut->z,hut->yawRadians,
            (HutLayout::Depth+HutLayout::RoofOverhang*2.0f)*.5f,
            (HutLayout::Width+HutLayout::RoofOverhang*2.0f)*.5f,3.7f,95.0f,34U);
        if(helicopter) cacheStructureShadow(helicopter->x,helicopter->groundY,
            helicopter->z,helicopter->yawRadians,
            (HelicopterLayout::BodyFront-HelicopterLayout::BodyRear)*.55f,
            HelicopterLayout::BodyHalfWidth*1.1f,2.9f,105.0f,32U);

        // Bounds cover the actual projected/slope-conforming patch, not the source object's pivot.
        for (auto& shadow : staticShadows_) {
            Vector3 minimum(std::numeric_limits<float>::max()), maximum(std::numeric_limits<float>::lowest());
            for (std::size_t index = shadow.firstVertex; index < shadow.firstVertex + shadow.vertexCount; ++index) {
                minimum = Vector3::Min(minimum, staticVertices_[index].Position);
                maximum = Vector3::Max(maximum, staticVertices_[index].Position);
            }
            shadow.centre = (minimum + maximum) * 0.5f;
            shadow.radius = Vector3::Distance(minimum, maximum) * 0.5f + 0.001f;
        }
        // The static shadows go up once.
        {
            std::vector<StaticGpuVertex> gpu;
            gpu.reserve(staticVertices_.size());
            for (const StaticShadow& shadow : staticShadows_) {
                const float structure = shadow.kind == StaticShadowKind::Structure ? 1.0f : 0.0f;
                for (std::size_t index = shadow.firstVertex; index < shadow.firstVertex + shadow.vertexCount; ++index) {
                    const auto& vertex = staticVertices_[index];
                    gpu.push_back({{vertex.Position.X, vertex.Position.Y, vertex.Position.Z},
                                   {shadow.centre.X, shadow.centre.Y, shadow.centre.Z, shadow.maximumDistance},
                                   {static_cast<float>(vertex.Color.getAProperty()) / 255.0f, structure}});
                }
            }
            staticVertexTotal_ = static_cast<int>(gpu.size());
            if (staticVertexTotal_ > 0) {
                staticBuffer_ = std::make_unique<VertexBuffer>(device, StaticDeclaration(), staticVertexTotal_, BufferUsage::WriteOnly);
                staticBuffer_->SetData(gpu.data(), staticVertexTotal_);
            }
            using CNA::Graphics::ShaderCodeEXT;
            const auto source = [](const bool es, const std::string& body) {
                return std::string(es ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n") + body;
            };
            const std::string vertexBody = StaticVertexShader();
            const CNA::Graphics::ShaderPackageEXT package(
                {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "contact-static/es.vert", source(true, vertexBody)),
                 ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "contact-static/es.frag", source(true, StaticFragmentShader)),
                 ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "contact-static/gl.vert", source(false, vertexBody)),
                 ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "contact-static/gl.frag", source(false, StaticFragmentShader))},
                {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
            staticEffect_ = std::make_unique<ShaderEffect>(device, package);
            if (!staticEffect_->IsEffectValid())
                throw std::runtime_error("static contact shadow shader failed: " + staticEffect_->GetCompileErrorEXT());
            std::cout << "island-simulator: contact shadows: " << staticShadows_.size() << " static shadows ("
                      << staticVertexTotal_ << " vertices) uploaded once, faded by distance in the vertex shader; "
                         "only the moving ones are restaged\n";
        }
        vertexCapacity_ = (MonkeyTroop::MonkeyCount + BoarHerd::BoarCount + GoatHabitat::Count + 1U) * VerticesPerShadow   // +1: the walker
            + MaximumSnakeShadows*SnakeShadowVertexLimit
            + MaximumSpiderShadows*SpiderShadowVertexLimit;
        if (vertexCapacity_ > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
            throw std::length_error("Contact-shadow batch exceeds supported vertex capacity");
        }
        stagingVertices_.reserve(vertexCapacity_);
        vertices_ = std::make_unique<DynamicVertexBuffer>(device,
            VertexPositionColor::getVertexDeclarationStatic(), static_cast<int>(vertexCapacity_), BufferUsage::WriteOnly);
    }

    ContactShadowRenderer::~ContactShadowRenderer() = default;

    void ContactShadowRenderer::Draw(const Matrix& view, const Matrix& projection,
                                     const Vector3& cameraPosition,
                                     const std::span<const MonkeyState> monkeys,
                                     const std::span<const BoarState> boars, const SnakePopulation& population,
                                     const std::span<const GoatState> goats,const std::span<const SpiderState> spiders,
        const PlayerShadow& player)
    {
        const auto snakes=population.Snakes();
        // The moving shadows are draped afresh at every rebuild (every frame while the animals
        // move): on the drawn mesh, ~10 ns a point, not the analytic terrain's microseconds.
        const auto movingHeight = [this](const float x, const float z) {
            return ground_ ? DrawnOrAnalyticHeight(*ground_, terrain_, x, z) : terrain_.Height(x, z);
        };
        drawCallCount_ = 0;
        if (monkeys.size() > MonkeyTroop::MonkeyCount || boars.size() > BoarHerd::BoarCount
            || goats.size()>GoatHabitat::Count || spiders.size()>SpiderHabitat::GroundCount) {
            throw std::invalid_argument("Contact-shadow population exceeds the reserved GPU capacity");
        }
        // Contact shadows are diffuse enough to tolerate sub-decimetre refresh thresholds. This
        // avoids repeating culling and a dynamic-buffer orphan for nearly identical frames.
        // The static shadows (palms, crowns, rocks, wood, structures, the coast) depend only on
        // the view; the animals' and the walker's on their own movement. A spider stepping by
        // used to rebuild the whole batch, hundreds of terrain-draped static patches included.
        const bool cameraRebuild = !cacheValid_
            || Vector3::DistanceSquared(cameraPosition, cachedCameraPosition_)
                > CameraRefreshDistanceSquared
            || std::abs(view.M11 - cachedView_.M11) > ViewRefreshThreshold
            || std::abs(view.M12 - cachedView_.M12) > ViewRefreshThreshold
            || std::abs(view.M13 - cachedView_.M13) > ViewRefreshThreshold
            || std::abs(view.M21 - cachedView_.M21) > ViewRefreshThreshold
            || std::abs(view.M22 - cachedView_.M22) > ViewRefreshThreshold
            || std::abs(view.M23 - cachedView_.M23) > ViewRefreshThreshold
            || std::abs(view.M31 - cachedView_.M31) > ViewRefreshThreshold
            || std::abs(view.M32 - cachedView_.M32) > ViewRefreshThreshold
            || std::abs(view.M33 - cachedView_.M33) > ViewRefreshThreshold
            || std::abs(projection.M11 - cachedProjection_.M11)
                > ProjectionRefreshThreshold
            || std::abs(projection.M22 - cachedProjection_.M22)
                > ProjectionRefreshThreshold;
        bool rebuild = cameraRebuild
            || cachedMonkeyPositions_.size() != monkeys.size()
            || cachedBoarPoses_.size() != boars.size()
            || cachedGoatPoses_.size() != goats.size()
            || cachedSpiders_.size() != spiders.size()
            || cachedSnakePoints_.size() != snakes.size()*SnakeState::StationCount;
        if (!rebuild) {
            for (std::size_t index = 0; index < monkeys.size(); ++index) {
                const auto centre=MonkeyVisualCentre(monkeys[index]);
                const Vector3 position(centre.x,centre.y,centre.z);
                if (Vector3::DistanceSquared(position, cachedMonkeyPositions_[index])
                    > MonkeyRefreshDistanceSquared) {
                    rebuild = true;
                    break;
                }
            }
        }

        if (!rebuild) {
            for (std::size_t index = 0; index < boars.size(); ++index) {
                const auto& boar = boars[index];
                const auto& cached = cachedBoarPoses_[index];
                const float dx = boar.x - cached.X, dz = boar.z - cached.Z;
                if (dx * dx + dz * dz > MonkeyRefreshDistanceSquared
                    || std::abs(boar.y - cached.Y) > 0.04f
                    || std::abs(std::remainder(boar.headingRadians - cached.W,
                                              2.0f * std::numbers::pi_v<float>)) > 0.06f) {
                    rebuild = true;
                    break;
                }
            }
        }

        if(!rebuild) {
            const float snakeShadowDistance = monkeyMaximumDistance_ * (60.0f / 105.0f);
            const float snakeShadowDistanceSquared = snakeShadowDistance * snakeShadowDistance;
            for(std::size_t snakeIndex=0;snakeIndex<snakes.size()&&!rebuild;++snakeIndex) {
                const auto& snake=snakes[snakeIndex];
                const std::size_t first=snakeIndex*SnakeState::StationCount;
                const auto head=snake.body.front();
                const Vector3 currentHead(head.x,head.y,head.z);
                const Vector3& oldHead=cachedSnakePoints_[first];
                if(Vector3::DistanceSquared(cameraPosition,currentHead)>snakeShadowDistanceSquared
                    &&Vector3::DistanceSquared(cameraPosition,oldHead)>snakeShadowDistanceSquared)
                    continue;
                for(std::size_t pointIndex=0;pointIndex<SnakeState::StationCount;++pointIndex) {
                    const auto p=snake.body[pointIndex];
                    if(Vector3::DistanceSquared(Vector3(p.x,p.y,p.z),
                        cachedSnakePoints_[first+pointIndex])>0.000064f) {
                        rebuild=true;
                        break;
                    }
                }
            }
        }
        if(!rebuild) {
            for(std::size_t i=0;i<goats.size();++i) {
                const auto& goat=goats[i];const auto& cached=cachedGoatPoses_[i];
                const float dx=goat.x-cached.X,dz=goat.z-cached.Z;
                if(dx*dx+dz*dz>MonkeyRefreshDistanceSquared||std::abs(goat.y-cached.Y)>.04f
                    ||std::abs(std::remainder(goat.heading-cached.W,2*std::numbers::pi_v<float>))>.06f)
                    rebuild=true;
            }
        }
        if(!rebuild) {
            const float radius=monkeyMaximumDistance_*(8.0f/105.0f);
            for(std::size_t i=0;i<spiders.size();++i) {
                const auto& s=spiders[i];const auto& old=cachedSpiders_[i];
                // Millimetre foot patches cannot tolerate the larger animals'
                // movement threshold. Ignore distant spiders to retain idle caching.
                if((Vector3::DistanceSquared(cameraPosition,Vector3(s.x,s.y,s.z))<=radius*radius
                    ||Vector3::DistanceSquared(cameraPosition,Vector3(old.x,old.y,old.z))<=radius*radius)
                    &&s!=old){rebuild=true;break;}
            }
        }
        if (rebuild) {
            const bool staticRebuild = cameraRebuild || !staticValid_;
            stagingVertices_.clear();
            if (staticRebuild) {
                visibleStaticTriangles_ = 0;
                visiblePalmShadowCount_ = 0;
                visibleCanopyShadowCount_ = 0;
                visibleRockShadowCount_ = 0;
                visibleWoodShadowCount_ = 0;
                visibleHutShadowCount_ = 0;
                visibleStructureShadowCount_ = 0;
                visibleCoastalShadowCount_ = 0;
            }
            visibleShadowCount_ = 0;
            visibleMonkeyShadowCount_ = 0;
            visibleBoarShadowCount_ = 0;
            visibleGoatShadowCount_ = 0;
            visiblePlayerShadowCount_ = 0;
            visibleSpiderShadowCount_ = 0;
            visibleSnakeShadowCount_ = 0;
            primitiveCount_ = 0;
            const BoundingFrustum frustum(view * projection);

            // The moving shadows lean away from the hour's light (the static ones were laid
            // once, as soft grounding pools).
            const auto shading = sun_light::Current().shading;
            const float horizontalSunLength = std::max(std::hypot(shading.x, shading.z), 1e-4f);
            const float shadowDirectionX = -shading.x / horizontalSunLength;
            const float shadowDirectionZ = -shading.z / horizontalSunLength;

            // The static shadows are drawn from their own buffer (Draw's end); here they are
            // only counted as the camera moves, for the inventory.
            for (const StaticShadow& shadow : staticShadows_) {
                if (!staticRebuild) break;
                // A structure's mass shadow under an active sun shadow map counts where the map
                // has faded out at any of its vertices.
                const bool beyondMapOnly = shadow.kind == StaticShadowKind::Structure && sunShadowMapActive_;
                float fade = 0.0f;
                if (!IsVisible(frustum, cameraPosition,
                               shadow.centre.X, shadow.centre.Y, shadow.centre.Z,
                               shadow.radius, shadow.maximumDistance, fade)) {
                    continue;
                }
                if (beyondMapOnly) {
                    const auto toSun = sun_light::Current().shading;
                    bool anyShare = false;
                    for (std::size_t vertexIndex = 0; vertexIndex < shadow.vertexCount && !anyShare; ++vertexIndex) {
                        const Vector3& at = staticVertices_[shadow.firstVertex + vertexIndex].Position;
                        anyShare = 1.0f - sun_shadow_fade::Strength(at.X, at.Y, at.Z, cameraPosition.X, cameraPosition.Z,
                                                                    toSun.x, toSun.y, toSun.z) > 0.01f;
                    }
                    if (!anyShare) continue;   // wholly inside the map's shadows
                }
                visibleStaticTriangles_ += static_cast<int>(shadow.vertexCount / 3U);
                switch (shadow.kind) {
                case StaticShadowKind::Palm:
                    ++visiblePalmShadowCount_;
                    break;
                case StaticShadowKind::Canopy:
                    ++visibleCanopyShadowCount_;
                    break;
                case StaticShadowKind::Rock:
                    ++visibleRockShadowCount_;
                    break;
                case StaticShadowKind::Wood:
                    ++visibleWoodShadowCount_;
                    break;
                case StaticShadowKind::Hut:
                    ++visibleHutShadowCount_;
                    break;
                case StaticShadowKind::Coastal:
                    ++visibleCoastalShadowCount_;
                    break;
                case StaticShadowKind::Structure:
                    ++visibleStructureShadowCount_;
                    break;
                }
            }
            if (staticRebuild) staticValid_ = true;

            for (const MonkeyState& monkey : monkeys) {
                const auto centre=MonkeyVisualCentre(monkey);
                float fade = 0.0f;
                if (!IsVisible(frustum, cameraPosition, centre.x, centre.y, centre.z,
                               0.85f * monkey.scale, monkeyMaximumDistance_, fade)) {
                    continue;
                }
                const float elevation=std::max(0.0f,centre.y-monkey.y-.72f*monkey.scale);
                fade*=std::clamp(1-elevation/3.0f,0.0f,1.0f);
                AppendTerrainEllipse(stagingVertices_, movingHeight, centre.x, centre.z,
                                     shadowDirectionX, shadowDirectionZ,
                                     0.62f * monkey.scale, 0.38f * monkey.scale,
                                     shadowDirectionX*0.12f*monkey.scale,
                                     shadowDirectionZ*0.12f*monkey.scale,FadedAlpha(48, fade));
                ++visibleMonkeyShadowCount_;
            }

            for (const auto& boar : boars) {
                float fade = 0.0f;
                if (!IsVisible(frustum, cameraPosition, boar.x, boar.y, boar.z,
                               1.3f * boar.scale, monkeyMaximumDistance_, fade)) continue;
                AppendTerrainEllipse(stagingVertices_, movingHeight, boar.x, boar.z,
                                     std::cos(boar.headingRadians), std::sin(boar.headingRadians),
                                     1.05f * boar.scale, 0.48f * boar.scale,
                                     std::cos(boar.headingRadians)*0.10f*boar.scale,
                                     std::sin(boar.headingRadians)*0.10f*boar.scale,
                                     FadedAlpha(90, fade));
                ++visibleBoarShadowCount_;
            }

            for(const auto& goat:goats) {
                float fade=0;
                if(!IsVisible(frustum,cameraPosition,goat.x,goat.y,goat.z,1.8f*goat.scale,
                    monkeyMaximumDistance_,fade))continue;
                AppendTerrainEllipse(stagingVertices_,movingHeight,goat.x,goat.z,
                    std::cos(goat.heading),std::sin(goat.heading),
                    .70f*goat.scale,.30f*goat.scale,
                    std::cos(goat.heading)*.08f*goat.scale,
                    std::sin(goat.heading)*.08f*goat.scale,FadedAlpha(70,fade));
                ++visibleGoatShadowCount_;
            }
            if (player.visible) {
                // The walker's sun shadow is a figure of its own (WalkerShadowRenderer); this is
                // only the soft pool of occluded sky right under the feet, which stays at night.
                AppendTerrainEllipse(stagingVertices_, movingHeight, player.x, player.z,
                                     1.0f, 0.0f, 0.34f, 0.27f, 0.0f, 0.0f, 46);
                ++visiblePlayerShadowCount_;
            }
            std::vector<std::pair<float,const SpiderState*>> visibleSpiders;
            visibleSpiders.reserve(spiders.size());
            for(const auto& spider:spiders) {
                float fade=0;
                if(!IsVisible(frustum,cameraPosition,spider.x,spider.y,spider.z,.16f,
                    monkeyMaximumDistance_*(8.0f/105.0f),fade))continue;
                visibleSpiders.emplace_back(Vector3::DistanceSquared(
                    Vector3(spider.x,spider.y,spider.z),cameraPosition),&spider);
            }
            std::sort(visibleSpiders.begin(),visibleSpiders.end(),[](const auto& a,const auto& b){return a.first<b.first;});
            if(visibleSpiders.size()>MaximumSpiderShadows)visibleSpiders.resize(MaximumSpiderShadows);
            for(const auto& candidate:visibleSpiders) {
                const auto& spider=*candidate.second;
                const float fade=DistanceFade(candidate.first,monkeyMaximumDistance_*(8.0f/105.0f));
                for(const auto& v:BuildSpiderContactShadow(spider,terrain_,population.Surface()))
                    stagingVertices_.emplace_back(Vector3(v.x,v.y,v.z),
                        Color(0,0,0,static_cast<int>(std::lround(v.opacity*fade*255.0f))));
                ++visibleSpiderShadowCount_;
            }
            std::vector<std::pair<float,const SnakeState*>> visibleSnakes;
            visibleSnakes.reserve(snakes.size());
            for(const auto& snake:snakes) {
                const auto head=snake.body.front();
                float fade=0;
                if(!IsVisible(frustum,cameraPosition,head.x,head.y,head.z,2.5f,
                    monkeyMaximumDistance_*(60.0f/105.0f),fade)) continue;
                visibleSnakes.emplace_back(Vector3::DistanceSquared(
                    Vector3(head.x,head.y,head.z),cameraPosition),&snake);
            }
            std::sort(visibleSnakes.begin(),visibleSnakes.end(),[](const auto& a,const auto& b){return a.first<b.first;});
            if(visibleSnakes.size()>MaximumSnakeShadows)visibleSnakes.resize(MaximumSnakeShadows);
            for(const auto& candidate:visibleSnakes) {
                const auto& snake=*candidate.second;
                const float fade=DistanceFade(candidate.first,monkeyMaximumDistance_*(60.0f/105.0f));
                for(const auto& v:BuildSnakeContactShadow(snake,population.Surface()))
                    stagingVertices_.emplace_back(Vector3(v.x,v.y,v.z),
                        Color(0,0,0,static_cast<int>(std::lround(v.opacity*fade*255.0f))));
                ++visibleSnakeShadowCount_;
            }
            visibleShadowCount_ = visiblePalmShadowCount_ + visibleCanopyShadowCount_
                + visibleRockShadowCount_ + visibleWoodShadowCount_ + visibleHutShadowCount_
                + visibleStructureShadowCount_
                + visibleMonkeyShadowCount_ + visibleBoarShadowCount_ + visibleCoastalShadowCount_
                + visibleSnakeShadowCount_ + visibleGoatShadowCount_ + visibleSpiderShadowCount_;
            primitiveCount_ = static_cast<int>(stagingVertices_.size() / 3U);
            if (stagingVertices_.size() > vertexCapacity_) {
                throw std::length_error("Contact-shadow upload exceeds reserved vertex capacity");
            }
            if (!stagingVertices_.empty()) {
                vertices_->SetData(stagingVertices_.data(), 0,
                                   static_cast<int>(stagingVertices_.size()),
                                   SetDataOptions::Discard);
            }
            if (staticRebuild) {   // the camera reference the static shadows were culled for
                cachedView_ = view;
                cachedProjection_ = projection;
                cachedCameraPosition_ = cameraPosition;
            }
            cachedMonkeyPositions_.clear();
            cachedMonkeyPositions_.reserve(monkeys.size());
            for (const MonkeyState& monkey : monkeys) {
                const auto centre=MonkeyVisualCentre(monkey);
                cachedMonkeyPositions_.emplace_back(centre.x,centre.y,centre.z);
            }
            cachedBoarPoses_.clear();
            for (const auto& boar : boars) {
                cachedBoarPoses_.emplace_back(boar.x, boar.y, boar.z, boar.headingRadians);
            }
            cachedSnakePoints_.clear();
            cachedGoatPoses_.clear();
            cachedSpiders_.assign(spiders.begin(),spiders.end());
            for(const auto& goat:goats)cachedGoatPoses_.emplace_back(goat.x,goat.y,goat.z,goat.heading);
            for(const auto& snake:snakes) for(const auto& p:snake.body)
                cachedSnakePoints_.emplace_back(p.x,p.y,p.z);
            cacheValid_ = true;
        }

        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        if (staticBuffer_ && staticEffect_ && staticVertexTotal_ > 0) {
            float viewProjection[16];
            (view * projection).ToColumnMajor(viewProjection);
            const auto shading = sun_light::Current().shading;
            Vector3 toSun(shading.x, shading.y, shading.z);
            if (toSun.LengthSquared() < 1e-8f) toSun = Vector3::Up;
            toSun.Normalize();
            staticEffect_->Apply();
            staticEffect_->SetUniformMat4("uViewProjection", viewProjection);
            staticEffect_->SetUniformVec3("uCamera", cameraPosition.X, cameraPosition.Y, cameraPosition.Z);
            staticEffect_->SetUniformVec4("uMap", sunShadowMapActive_ ? 1.0f : 0.0f, sun_shadow_fade::AnchorOf(cameraPosition.X),
                                          sun_shadow_fade::AnchorOf(cameraPosition.Z), 0.0f);
            staticEffect_->SetUniformVec3("uSun", toSun.X, toSun.Y, toSun.Z);
            device_.SetVertexBuffer(staticBuffer_.get());
            device_.setIndicesProperty(nullptr);
            device_.DrawPrimitives(PrimitiveType::TriangleList, 0, staticVertexTotal_ / 3);
            ++drawCallCount_;
        }
        if (stagingVertices_.empty() || primitiveCount_ <= 0) {
            return;
        }
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, primitiveCount_);
        ++drawCallCount_;
    }
}
