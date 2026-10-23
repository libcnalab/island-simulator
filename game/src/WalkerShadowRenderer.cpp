#include "IslandSimulator/WalkerShadowRenderer.hpp"
#include "IslandSimulator/CanopyLightField.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/SeabedShadowProjection.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int MaximumAcrossCells = 12;
        constexpr int MaximumAlongCells = 48;
        constexpr int VertexCapacity = MaximumAcrossCells * MaximumAlongCells * 6;
        constexpr float Margin = 0.35f;           // past the figure's own extent: the penumbra and the joints
        constexpr float LongestReach = 24.0f;     // a low sun down a falling bank, or the tyrannosaur's head
        constexpr float SurfaceOffset = 0.03f;
        constexpr float StepHeight = 0.4f;        // a rise within one cell that is a deck's edge, not a slope

        constexpr const char* Vertex = R"(
layout(location = 0) in vec3 aPosition;
uniform mat4 uViewProjection;
out vec3 vWorld;
void main() {
    vWorld = aPosition;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";
        constexpr const char* Fragment = R"(
in vec3 vWorld;
uniform vec3 uFeet;
uniform vec3 uToSun;
uniform vec2 uSea;             // x: the sea's level, y: the light's run per metre of depth once refracted
uniform vec4 uAxes;            // xy: the shadow's horizontal direction, z: the light's sine, w: strength
uniform vec2 uCapsuleA[CAPSULES];    // the capsules in shadow space (along x sine, across)
uniform vec2 uCapsuleB[CAPSULES];
uniform float uRadii[CAPSULES];
uniform vec3 uShadeTint;       // the terrain's colour in a crown's shade, against sunlit
out vec4 fragColor;
void main() {
    float visibility = sunShadowVisibility(vWorld);
    // up the light from this ground point to the feet's plane, where the silhouette was laid
    // Under the sea the light came down bent toward the vertical: back up the refracted ray to
    // the surface first, then up the light in the air (so a figure on the shallows' bed lies
    // shorter, as the fish's shadows do).
    vec3 ground = vWorld;
    float depth = uSea.x - ground.y;
    if (depth > 0.0) {
        vec2 sunward = uToSun.xz / max(length(uToSun.xz), 1e-4);
        ground = vec3(ground.xz + sunward * (uSea.y * depth), uSea.x).xzy;
    }
    vec3 onFeet = ground - uToSun * ((ground.y - uFeet.y) / uToSun.y);
    vec2 s = onFeet.xz - uFeet.xz;
    float along = dot(s, uAxes.xy);
    vec2 q = vec2(along * uAxes.z, dot(s, vec2(-uAxes.y, uAxes.x)));
    float d = 1000.0;
    for (int i = 0; i < CAPSULES; ++i) {
        vec2 a = uCapsuleA[i];
        vec2 ab = uCapsuleB[i] - a;
        float t = clamp(dot(q - a, ab) / max(dot(ab, ab), 1e-8), 0.0, 1.0);
        float part = length(q - a - ab * t) - uRadii[i];
        // a smooth union: the joints fill out like flesh instead of pinching where two parts meet
        float blend = max(JOINT_BLEND - abs(d - part), 0.0) / JOINT_BLEND;
        d = min(d, part) - blend * blend * JOINT_BLEND * 0.25;
    }
    // The penumbra widens with the light's path from the body down to the ground (the sun's
    // disc is half a degree across): crisp at the feet, a few centimetres soft at the head.
    float height = max(along, 0.0) * uAxes.z / max(sqrt(1.0 - uAxes.z * uAxes.z), 0.05);
    float soft = 0.012 + 0.0093 * height / uAxes.z;
    float cover = 1.0 - smoothstep(-soft, soft, d);
    // The ground's light with the walker against the light it already has: the same tint the
    // terrain takes in a crown's shade, and nothing where a crown or a cloud already shades it.
    vec3 before = mix(uShadeTint, vec3(1.0), visibility);
    vec3 after = mix(uShadeTint, vec3(1.0), visibility * (1.0 - uAxes.w * cover));
    fragColor = vec4(after / before, 1.0);
}
)";
    }

    WalkerShadowRenderer::WalkerShadowRenderer(GraphicsDevice& device) : device_(device)
    {
        using CNA::Graphics::ShaderCodeEXT;
        const auto source = [](const bool es, const bool fragment) {
            return std::string(es ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + (fragment ? "#define CAPSULES " + std::to_string(walker_shadow::CapsuleCount) + "\n#define JOINT_BLEND "
                                  + std::to_string(walker_shadow::JointBlend) + "\n" + SunShadowMap::ReceiverShader + Fragment
                            : std::string(Vertex));
        };
        const CNA::Graphics::ShaderPackageEXT package(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "walker-shadow/es.vert", source(true, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "walker-shadow/es.frag", source(true, true)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "walker-shadow/gl.vert", source(false, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "walker-shadow/gl.frag", source(false, true))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        effect_ = std::make_unique<ShaderEffect>(device, package);
        if (!effect_->IsEffectValid()) throw std::runtime_error("walker shadow shader failed: " + effect_->GetCompileErrorEXT());
        // The ground times the fragment's colour; the alpha (where the night's lamps wrote their
        // strength) is left as it is.
        multiply_ = std::make_unique<BlendState>();
        multiply_->setColorSourceBlendProperty(Blend::Zero);
        multiply_->setColorDestinationBlendProperty(Blend::SourceColor);
        multiply_->setAlphaSourceBlendProperty(Blend::Zero);
        multiply_->setAlphaDestinationBlendProperty(Blend::One);
        vertices_ = std::make_unique<DynamicVertexBuffer>(device, VertexPositionColor::getVertexDeclarationStatic(),
                                                          VertexCapacity, BufferUsage::WriteOnly);
        staging_.reserve(VertexCapacity);
        const char* message = "island-simulator: the walker casts the figure of a body on the ground along the hour's sun "
                              "(legs, arms and head from seventeen capsules, the gait swinging them), soft with the distance "
                              "from the body, long at the low sun, and only where the ground is still sunlit";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    WalkerShadowRenderer::~WalkerShadowRenderer() = default;

    void WalkerShadowRenderer::Draw(const Matrix& view, const Matrix& projection, const walker_shadow::Shape& shape,
                                    const Vector3& feet, const Vector3& toSun, const float strength,
                                    const std::function<float(float, float)>& groundHeight)
    {
        if (!shape.cast || !(strength > 0.01f) || !groundHeight) return;
        Vector3 light = toSun;
        if (!(light.LengthSquared() > 1e-8f)) return;
        light.Normalize();
        // The grid under every place the figure can land: the capsules' extent on level ground,
        // across and along, the far end stretched for a bank falling away from the sun.
        float nearest = 0.0f, farthest = 0.0f, leftmost = 0.0f, rightmost = 0.0f;
        for (const auto& capsule : shape.capsules) {
            if (!(capsule.radius > 0.0f)) continue;
            for (const float along : {capsule.ax, capsule.bx}) {
                nearest = std::min(nearest, (along - capsule.radius) / shape.sunSine);
                farthest = std::max(farthest, (along + capsule.radius) / shape.sunSine);
            }
            for (const float across : {capsule.ay, capsule.by}) {
                leftmost = std::min(leftmost, across - capsule.radius);
                rightmost = std::max(rightmost, across + capsule.radius);
            }
        }
        const float from = nearest - Margin;
        float to = std::min(LongestReach, 1.6f * farthest + Margin);
        // Where the ground falls away under the shadow (off a deck's end to the seabed, down a
        // bank) the figure lands that much further on: the drop times the light's run per metre.
        const float run = std::sqrt(std::max(1.0f - shape.sunSine * shape.sunSine, 0.0f)) / shape.sunSine;
        for (int pass = 0; pass < 2; ++pass) {
            const float drop = feet.Y - groundHeight(feet.X + shape.alongX * to, feet.Z + shape.alongZ * to);
            if (drop > 0.0f) to = std::min(LongestReach, 1.6f * farthest + Margin + drop * run);
        }
        const float acrossFrom = leftmost - Margin, acrossTo = rightmost + Margin;
        const int alongCells = std::clamp(static_cast<int>(std::ceil((to - from) / 0.30f)), 6, MaximumAlongCells);
        const int acrossCells = std::clamp(static_cast<int>(std::ceil((acrossTo - acrossFrom) / 0.30f)), 4, MaximumAcrossCells);
        const float acrossX = -shape.alongZ, acrossZ = shape.alongX;
        std::array<Vector3, (MaximumAcrossCells + 1) * (MaximumAlongCells + 1)> grid;
        for (int j = 0; j <= alongCells; ++j) {
            const float along = from + (to - from) * static_cast<float>(j) / static_cast<float>(alongCells);
            for (int i = 0; i <= acrossCells; ++i) {
                const float across = acrossFrom + (acrossTo - acrossFrom) * static_cast<float>(i) / static_cast<float>(acrossCells);
                const float x = feet.X + shape.alongX * along + acrossX * across;
                const float z = feet.Z + shape.alongZ * along + acrossZ * across;
                grid[static_cast<std::size_t>(j * (acrossCells + 1) + i)] = Vector3(x, groundHeight(x, z) + SurfaceOffset, z);
            }
        }
        staging_.clear();
        const Color white(255, 255, 255, 255);
        for (int j = 0; j < alongCells; ++j) {
            for (int i = 0; i < acrossCells; ++i) {
                const auto at = [&](const int a, const int b) {
                    return grid[static_cast<std::size_t>((j + a) * (acrossCells + 1) + i + b)];
                };
                // A cell across a deck's edge (the jetty's, the bridge's) would hang a slanted
                // sheet of shadow from the planks to the sand or the water below: left out.
                const float low = std::min({at(0, 0).Y, at(1, 0).Y, at(1, 1).Y, at(0, 1).Y});
                const float high = std::max({at(0, 0).Y, at(1, 0).Y, at(1, 1).Y, at(0, 1).Y});
                if (high - low > StepHeight) continue;
                for (const auto& corner : {at(0, 0), at(1, 0), at(1, 1), at(0, 0), at(1, 1), at(0, 1)})
                    staging_.emplace_back(corner, white);
            }
        }
        vertices_->SetData(staging_.data(), 0, static_cast<int>(staging_.size()), SetDataOptions::Discard);

        std::array<float, walker_shadow::CapsuleCount * 2> a{}, b{};
        std::array<float, walker_shadow::CapsuleCount> radii{};
        for (std::size_t c = 0; c < shape.capsules.size(); ++c) {
            a[2 * c] = shape.capsules[c].ax;
            a[2 * c + 1] = shape.capsules[c].ay;
            b[2 * c] = shape.capsules[c].bx;
            b[2 * c + 1] = shape.capsules[c].by;
            radii[c] = shape.capsules[c].radius;
        }
        float viewProjection[16];
        (view * projection).ToColumnMajor(viewProjection);
        effect_->Apply();
        effect_->SetUniformMat4("uViewProjection", viewProjection);
        effect_->SetUniformVec3("uFeet", feet.X, feet.Y, feet.Z);
        effect_->SetUniformVec3("uToSun", light.X, light.Y, light.Z);
        {
            const auto run = SeabedShadowProjection::Refracted(light.X, light.Y, light.Z);
            effect_->SetUniformVec2("uSea", 0.0f, std::hypot(run[0], run[1]));
        }
        effect_->SetUniformVec4("uAxes", shape.alongX, shape.alongZ, shape.sunSine, std::clamp(strength, 0.0f, 1.0f));
        effect_->SetUniformVec2Array("uCapsuleA", a.data(), walker_shadow::CapsuleCount);
        effect_->SetUniformVec2Array("uCapsuleB", b.data(), walker_shadow::CapsuleCount);
        effect_->SetUniformFloatArray("uRadii", radii.data(), walker_shadow::CapsuleCount);
        effect_->SetUniformVec3("uShadeTint", CanopyAmbientTint.red, CanopyAmbientTint.green, CanopyAmbientTint.blue);
        if (sunShadow_) sunShadow_->Bind(*effect_);
        else SunShadowMap::BindDisabled(*effect_);
        device_.setBlendStateProperty(*multiply_);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        const int triangles = static_cast<int>(staging_.size() / 3);
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, triangles);
        triangles_ += triangles;
        ++draws_;
    }
}
