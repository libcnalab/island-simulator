#include "IslandSimulator/SeagrassRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/TerrainShaderNoise.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <type_traits>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        // a blade: two tapering quads and a pointed tip, seven corners drawn as five triangles
        constexpr int CornersPerBlade = 7, IndicesPerBlade = 15;
        constexpr std::array<std::uint16_t, IndicesPerBlade> BladeTriangles{0, 1, 3, 0, 3, 2, 2, 3, 5, 2, 5, 4, 4, 5, 6};
        // the most blades a bed can hold: every grid cell planted with the most blades a clump has
        constexpr int MaximumBladesPerClump = 7;
        constexpr float LagoonInner = 150.0f, LagoonOuter = 330.0f;
        float Unit(const int x, const int z, const std::uint32_t salt) noexcept
        {
            std::uint32_t h = static_cast<std::uint32_t>(x) * 0x8da6b343U ^ static_cast<std::uint32_t>(z) * 0xd8163841U ^ salt * 0xcb1ab31fU;
            h ^= h >> 13; h *= 0x5bd1e995U; h ^= h >> 15;
            return static_cast<float>(h & 0xffffffU) / 16777215.0f;
        }
        std::int64_t BucketKey(const int bx, const int bz) noexcept
        {
            return (static_cast<std::int64_t>(bx) << 32) ^ static_cast<std::int64_t>(static_cast<std::uint32_t>(bz));
        }
        using terrain_noise::Smooth;
        using terrain_noise::ValueNoise;
        // How strongly the terrain paints seagrass meadow at (x, z) over a bed `depth` down.
        float MeadowAt(const float x, const float z, const float depth) noexcept
        {
            const float meadowField = ValueNoise(x * 0.038f + 43.7f, z * 0.038f - 21.6f);
            const float medium = ValueNoise(x * 0.31f + 17.3f, z * 0.31f - 9.7f);
            const float meadow = Smooth(-0.16f, 0.17f, meadowField + medium * 0.22f)
                * Smooth(0.45f, 0.85f, depth) * (1.0f - Smooth(1.8f, 2.6f, depth));
            const float broad = ValueNoise(x * 0.060f, z * 0.060f);
            const float length = std::max(std::hypot(x, z), 1.0f);
            const float arcX = x / length * 250.0f, arcZ = z / length * 250.0f;
            const float channel = ValueNoise(arcX * 0.065f + 5.1f + broad * 0.65f, arcZ * 0.065f + 19.3f + broad * 0.65f);
            const float submerged = Smooth(0.25f, 0.65f, depth) * (1.0f - Smooth(5.0f, 8.0f, depth));
            return meadow * submerged * (1.0f - Smooth(0.10f, 0.28f, channel));
        }

        struct SeagrassVertex
        {
            std::array<float, 3> position;   // at rest
            std::array<float, 4> blade;      // the clump's surge phase, its thinning rank 0..1, the blade's height, this corner's rise
            std::array<std::uint8_t, 4> color;
        };
        static_assert(std::is_trivially_copyable_v<SeagrassVertex>);
        static_assert(std::is_standard_layout_v<SeagrassVertex> && sizeof(SeagrassVertex) == 32);

        const VertexDeclaration& SeagrassDeclaration()
        {
            static const VertexDeclaration declaration{
                VertexElement(offsetof(SeagrassVertex, position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0),
                VertexElement(offsetof(SeagrassVertex, blade), VertexElementFormat::Vector4, VertexElementUsage::TextureCoordinate, 0),
                VertexElement(offsetof(SeagrassVertex, color), VertexElementFormat::Color, VertexElementUsage::Color, 0)};
            return declaration;
        }

        // The surge leans each blade by the square of its height fraction, as the CPU blades did;
        // towards the radius the blades shrink into the bed, a thinning share of them sooner.
        constexpr const char* VertexBody = R"(
layout(location=0) in vec3 aPosition;
layout(location=1) in vec4 aBlade;
layout(location=2) in vec4 aColor;
uniform mat4 uViewProjection;
uniform vec4 uSea;   // seconds (wrapped), the surge, the drawing radius, the light from above
uniform vec3 uCamera;
out vec3 vColor;
out float vDistance;
void main(){
    float d=distance(uCamera.xz,aPosition.xz);
    float fade=1.0-smoothstep(uSea.z*mix(0.45,0.70,aBlade.y),uSea.z,d);
    float t=clamp(aBlade.w/max(aBlade.z,0.001),0.0,1.0);
    float sway=sin(uSea.x*0.9+aBlade.x+aPosition.x*0.05)*uSea.y;
    vec3 p=aPosition-vec3(0.0,aBlade.w*(1.0-fade),0.0)+vec3(sway,0.0,sway*0.6)*(aBlade.z*t*t*fade);
    vColor=aColor.rgb*uSea.w;
    vDistance=distance(uCamera,p);
    gl_Position=uViewProjection*vec4(p,1.0);
    // XNA projection uses [0,1] depth; desktop GL and GLES require [-1,1].
    gl_Position.z=2.0*gl_Position.z-gl_Position.w;
}
)";
        constexpr const char* FragmentBody = R"(
in vec3 vColor;
in float vDistance;
out vec4 fragColor;
void main(){
    fragColor=vec4(applyUnderwaterFog(vColor,vDistance),1.0);
}
)";
    }

    SeagrassRenderer::SeagrassRenderer(GraphicsDevice& device, const IslandTerrain& terrain)
        : device_(device), terrain_(terrain)
    {
        using CNA::Graphics::ShaderCodeEXT;
        const auto source = [](const bool es, const char* text) {
            return std::string(es ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + underwater_fog::ShaderDefinition + text;
        };
        try {
            const CNA::Graphics::ShaderPackageEXT package({
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "seagrass.es.vert", source(true, VertexBody)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "seagrass.es.frag", source(true, FragmentBody)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "seagrass.vert", source(false, VertexBody)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "seagrass.frag", source(false, FragmentBody))},
                {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment}, {});
            auto effect = std::make_unique<ShaderEffect>(device, package);
            if (!effect->IsEffectValid()) {
                std::cout << "island-simulator: seagrass shader unavailable: " << effect->GetCompileErrorEXT() << '\n';
                return;
            }
            effect_ = std::move(effect);
        } catch (const std::exception& error) {
            std::cout << "island-simulator: seagrass shader unavailable: " << error.what() << '\n';
            return;
        }
        const int cellsPerSide = static_cast<int>(std::ceil(BucketSize / Spacing)) + 1;
        const int maximumBlades = cellsPerSide * cellsPerSide * MaximumBladesPerClump;
        std::vector<std::uint16_t> indices;
        indices.reserve(static_cast<std::size_t>(maximumBlades * IndicesPerBlade));
        for (int blade = 0; blade < maximumBlades; ++blade) {
            for (const std::uint16_t corner : BladeTriangles) {
                indices.push_back(static_cast<std::uint16_t>(blade * CornersPerBlade + corner));
            }
        }
        indices_ = std::make_unique<IndexBuffer>(device, IndexElementSize::SixteenBits, static_cast<int>(indices.size()), BufferUsage::WriteOnly);
        indices_->SetData(indices.data(), static_cast<int>(indices.size()));
        const char* message = "island-simulator: seagrass meadows grow on the lagoon floor in 8 m beds as the eye nears them, "
                              "up to some eighty blades a square metre, and sway with the surge";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    SeagrassRenderer::~SeagrassRenderer() = default;

    void SeagrassRenderer::Build(const int bx, const int bz, Bucket& bucket)
    {
        const float x0 = static_cast<float>(bx) * BucketSize, z0 = static_cast<float>(bz) * BucketSize;
        const float x1 = x0 + BucketSize, z1 = z0 + BucketSize;
        // a bed wholly inside the island's shore or out past the lagoon grows nothing
        const float nearest = std::hypot(std::clamp(0.0f, x0, x1), std::clamp(0.0f, z0, z1));
        const float farthest = std::hypot(std::max(std::abs(x0), std::abs(x1)), std::max(std::abs(z0), std::abs(z1)));
        if (farthest < LagoonInner || nearest > LagoonOuter) return;
        std::vector<SeagrassVertex> vertices;
        bucket.low = 0.0f;
        bucket.high = -10.0f;
        const int g0x = static_cast<int>(std::floor(x0 / Spacing)), g1x = static_cast<int>(std::floor(x1 / Spacing));
        const int g0z = static_cast<int>(std::floor(z0 / Spacing)), g1z = static_cast<int>(std::floor(z1 / Spacing));
        for (int gz = g0z; gz <= g1z; ++gz) {
            for (int gx = g0x; gx <= g1x; ++gx) {
                // jittered in its grid cell; the cells straddling two beds belong to the one their point falls in
                const float x = (static_cast<float>(gx) + Unit(gx, gz, 1U)) * Spacing;
                const float z = (static_cast<float>(gz) + Unit(gx, gz, 2U)) * Spacing;
                if (x < x0 || x >= x1 || z < z0 || z >= z1) continue;
                const float radius = std::hypot(x, z);
                if (radius < LagoonInner || radius > LagoonOuter) continue;
                const float height = terrain_.Height(x, z);
                if (height > -0.45f || height < -2.6f) continue;
                const float suits = MeadowAt(x, z, -height);
                // thinning only at the meadow's fringe: inside it the blades close into a carpet
                if (suits <= 0.08f || Unit(gx, gz, 3U) > Smooth(0.0f, 0.40f, suits)) continue;
                const float size = 0.20f + 0.25f * Unit(gx, gz, 4U) * (0.5f + 0.5f * suits);
                const float phase = 6.2831853f * Unit(gx, gz, 5U);
                const int blades = 4 + static_cast<int>(Unit(gx, gz, 6U) * 3.99f);
                const float rank = Unit(gx, gz, 7U);
                const int oldBlade = static_cast<int>(Unit(gx, gz, 8U) * 4.99f);
                // deeper, darker
                const float depthLit = 1.0f - 0.12f * std::clamp(-height, 0.0f, 4.0f);
                bucket.low = std::min(bucket.low, height);
                bucket.high = std::max(bucket.high, height + size * 1.2f + 0.05f);
                for (int blade = 0; blade < blades; ++blade) {
                    const float a = phase * 3.0f + 2.39996f * static_cast<float>(blade);
                    const float spread = 0.03f + 0.04f * static_cast<float>(blade % 3);
                    const std::array<float, 3> root{x + std::cos(a) * spread, height + 0.01f, z + std::sin(a) * spread};
                    const float bladeHeight = size * (0.75f + 0.35f * static_cast<float>((blade * 7) % 5) / 4.0f);
                    const float width = 0.010f + 0.004f * static_cast<float>(blade % 2);
                    // a flat strap splaying outward and bending across its face, as turtle grass does
                    const float face = a + 1.5708f + 0.5f * std::sin(phase + static_cast<float>(blade));
                    const float sideX = std::cos(face), sideZ = std::sin(face);
                    const float leanX = std::cos(a) * 0.12f, leanZ = std::sin(a) * 0.12f;
                    // dark olive, paler towards the tip; one blade in five older, yellowing at its end
                    const bool old = blade % 5 == oldBlade;
                    const auto shade = [&](const float t) {
                        const float g = depthLit * (0.55f + 0.45f * t);
                        const float age = old ? t * t : 0.0f;
                        const auto c = [g](const float v) { return static_cast<std::uint8_t>(std::lround(std::clamp(v * g, 0.0f, 1.0f) * 255.0f)); };
                        return std::array<std::uint8_t, 4>{c(0.22f + 0.16f * age), c(0.30f + 0.04f * age), c(0.11f + 0.04f * age), 255};
                    };
                    const auto corner = [&](const float t, const float across) {
                        const float w = width * (1.0f - 0.35f * t) * across;
                        const float rise = bladeHeight * t, bend = bladeHeight * t * t;
                        vertices.push_back({{root[0] + leanX * bend + sideX * w, root[1] + rise, root[2] + leanZ * bend + sideZ * w},
                                            {phase, rank, bladeHeight, rise}, shade(t)});
                    };
                    // in BladeTriangles' order: the two sides at the root, a third and two thirds up, then the tip
                    corner(0.0f, -1.0f); corner(0.0f, 1.0f);
                    corner(1.0f / 3.0f, -1.0f); corner(1.0f / 3.0f, 1.0f);
                    corner(2.0f / 3.0f, -1.0f); corner(2.0f / 3.0f, 1.0f);
                    corner(1.0f, 0.0f);
                }
            }
        }
        bucket.count = static_cast<int>(vertices.size());
        if (bucket.count == 0) return;
        bucket.blades = bucket.count / CornersPerBlade;
        bucket.vertices = std::make_unique<VertexBuffer>(device_, SeagrassDeclaration(), bucket.count, BufferUsage::WriteOnly);
        bucket.vertices->SetData(vertices.data(), bucket.count);
    }

    void SeagrassRenderer::Draw(const Matrix& view, const Matrix& projection, const Vector3& camera, const float seconds,
                                const float seaState)
    {
        visible_ = 0;
        if (!effect_ || !std::isfinite(seconds) || !std::isfinite(camera.X) || !std::isfinite(camera.Z)) return;
        ++frame_;
        const BoundingFrustum frustum(view * projection);
        const float surge = 0.16f * std::clamp(std::isfinite(seaState) ? seaState : 1.0f, 1.0f, 1.8f);
        // the light from above dims with the hour
        const float sunUp = std::clamp(sun_light::Current().shading.y, 0.0f, 1.0f);
        bool prepared = false;
        const int b0x = static_cast<int>(std::floor((camera.X - Radius) / BucketSize)), b1x = static_cast<int>(std::floor((camera.X + Radius) / BucketSize));
        const int b0z = static_cast<int>(std::floor((camera.Z - Radius) / BucketSize)), b1z = static_cast<int>(std::floor((camera.Z + Radius) / BucketSize));
        // the beds coming into reach are grown nearest first, a few a frame so that a fast flight
        // over the lagoon never stalls on them (all of them on the first frame)
        std::vector<std::array<float, 3>> missing;
        for (int bz = b0z; bz <= b1z; ++bz) {
            for (int bx = b0x; bx <= b1x; ++bx) {
                const float x0 = static_cast<float>(bx) * BucketSize, z0 = static_cast<float>(bz) * BucketSize;
                const float nearX = std::clamp(camera.X, x0, x0 + BucketSize), nearZ = std::clamp(camera.Z, z0, z0 + BucketSize);
                const float reach = std::hypot(nearX - camera.X, nearZ - camera.Z);
                if (reach <= Radius && !buckets_.contains(BucketKey(bx, bz))) {
                    missing.push_back({reach, static_cast<float>(bx), static_cast<float>(bz)});
                }
            }
        }
        std::sort(missing.begin(), missing.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; });
        const std::size_t builds = frame_ == 1U ? missing.size() : std::min<std::size_t>(missing.size(), MaximumBuildsPerFrame);
        for (std::size_t i = 0; i < builds; ++i) {
            const int bx = static_cast<int>(missing[i][1]), bz = static_cast<int>(missing[i][2]);
            Build(bx, bz, buckets_[BucketKey(bx, bz)]);
        }
        for (int bz = b0z; bz <= b1z; ++bz) {
            for (int bx = b0x; bx <= b1x; ++bx) {
                const float x0 = static_cast<float>(bx) * BucketSize, z0 = static_cast<float>(bz) * BucketSize;
                const float nearX = std::clamp(camera.X, x0, x0 + BucketSize), nearZ = std::clamp(camera.Z, z0, z0 + BucketSize);
                if (std::hypot(nearX - camera.X, nearZ - camera.Z) > Radius) continue;
                const auto found = buckets_.find(BucketKey(bx, bz));
                if (found == buckets_.end()) continue;
                Bucket& bucket = found->second;
                bucket.seen = frame_;
                if (bucket.count == 0
                    || !frustum.Intersects(BoundingBox(Vector3(x0, bucket.low, z0), Vector3(x0 + BucketSize, bucket.high, z0 + BucketSize)))) {
                    continue;
                }
                if (!prepared) {
                    device_.setRasterizerStateProperty(RasterizerState::CullNone);
                    device_.setBlendStateProperty(BlendState::Opaque);
                    device_.setDepthStencilStateProperty(DepthStencilState::Default);
                    float matrix[16];
                    (view * projection).ToColumnMajor(matrix);
                    effect_->Apply();
                    effect_->SetUniformMat4("uViewProjection", matrix);
                    effect_->SetUniformVec4("uSea", std::remainder(seconds, 698.1317f), surge, Radius, 0.55f + 0.45f * sunUp);
                    effect_->SetUniformVec3("uCamera", camera.X, camera.Y, camera.Z);
                    underwater_fog::Bind(*effect_);
                    device_.setIndicesProperty(indices_.get());
                    prepared = true;
                }
                device_.SetVertexBuffer(bucket.vertices.get());
                device_.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0, bucket.count, 0, bucket.blades * (IndicesPerBlade / 3));
                visible_ += bucket.blades;
            }
        }
        // beds the eye left long ago are let go (they grow back the same if it returns)
        if (frame_ % 120U == 0U) {
            std::erase_if(buckets_, [this](const auto& entry) { return entry.second.seen + 600U < frame_; });
        }
    }
}
