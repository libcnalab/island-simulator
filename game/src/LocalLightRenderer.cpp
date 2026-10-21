#include "IslandSimulator/LocalLightRenderer.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr int SpotSpokes = 25, SpotRings = 18, PointSpokes = 32, PointRings = 12;
        constexpr int Capacity = std::max((SpotSpokes - 1) * (SpotRings - 1), PointSpokes * (PointRings - 1)) * 6;

        constexpr const char* VertexBody = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aGround;
uniform mat4 uViewProjection;
out vec3 vWorld;
out vec3 vNormal;
out float vGround;
out float vReach;
void main()
{
    vWorld = aPosition;
    vNormal = aNormal;
    vGround = aGround.x;
    vReach = aGround.y;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";
        constexpr const char* FragmentBody = R"(
in vec3 vWorld;
in vec3 vNormal;
in float vGround;
in float vReach;
uniform vec3 uCamera;
uniform vec3 uLight;
uniform vec3 uAxis;
uniform vec3 uShape;   // x: 0 a spot, 1 a point, 2 a point on a plane; y: the reach in metres; z: the strength
uniform vec3 uTint;    // the light's colour: the lit pixels are tinted toward it before the lift
out vec4 fragColor;
void main()
{
    // The surface this pixel shows lies on, or stands near, the ground where its view ray
    // comes down to it beyond the slab: the light reaches it as it reaches that ground.
    // (On a plane the pixel is lit where it lies.)
    vec3 ray = vWorld - uCamera;
    float fall = uCamera.y - vWorld.y;
    vec3 lit = uCamera + ray * vReach;
    if (uShape.x > 1.5) lit = vWorld;
    else if (fall > 0.02) lit = uCamera + ray * min((uCamera.y - vGround) / fall, 60.0 / max(length(ray), 0.01));
    vec3 toLit = lit - uLight;
    float d = max(length(toLit), 0.05);
    vec3 dir = toLit / d;
    float facing = 0.4 + 0.6 * smoothstep(0.0, 0.2, dot(normalize(vNormal), -dir));
    float strength;
    if (uShape.x < 0.5) {
        float axial = dot(dir, uAxis);
        float beam = smoothstep(0.86, 0.97, axial) + 0.25 * smoothstep(0.70, 0.88, axial);
        strength = beam * facing / (1.0 + 0.009 * d * d) * (1.0 - smoothstep(26.0, uShape.y, d));
    } else {
        float near = d / (0.22 * uShape.y);
        strength = facing / (1.0 + near * near) * (1.0 - smoothstep(0.55 * uShape.y, uShape.y, d));
    }
    float lift = clamp(strength * uShape.z, 0.0, 0.92);
    fragColor = vec4(mix(vec3(1.0), uTint, clamp(lift * 1.4, 0.0, 1.0)), 1.0 - lift);
}
)";
    }

    LocalLightRenderer::LocalLightRenderer(GraphicsDevice& device) : device_(device)
    {
        if (!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects))
            throw std::runtime_error("custom effects unavailable");
        using CNA::Graphics::ShaderCodeEXT;
        const auto source = [](const bool embedded, const bool fragment) {
            return std::string(embedded ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + (fragment ? FragmentBody : VertexBody);
        };
        const CNA::Graphics::ShaderPackageEXT package(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main", "local-light/es.vert", source(true, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main", "local-light/es.frag", source(true, true)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main", "local-light/gl.vert", source(false, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main", "local-light/gl.frag", source(false, true))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        effect_ = std::make_unique<ShaderEffect>(device, package);
        if (!effect_->IsEffectValid()) throw std::runtime_error("local light failed: " + effect_->GetCompileErrorEXT());
        // the frame's colour times the light's tint (only darkening, so nothing clips before
        // the grade), its alpha times one minus the light's strength
        blend_ = std::make_unique<BlendState>();
        blend_->setColorSourceBlendProperty(Blend::Zero);
        blend_->setColorDestinationBlendProperty(Blend::SourceColor);
        blend_->setAlphaSourceBlendProperty(Blend::Zero);
        blend_->setAlphaDestinationBlendProperty(Blend::SourceAlpha);
        staging_.reserve(static_cast<std::size_t>(Capacity));
        mesh_ = std::make_unique<DynamicVertexBuffer>(device, Vertex::getVertexDeclarationStatic(), Capacity, BufferUsage::WriteOnly);
    }

    LocalLightRenderer::~LocalLightRenderer() = default;

    namespace
    {
        // A ground point of the lit area drawn nearer along the eye's ray to it: `lift` below
        // the eye, and never before the plane `along` (> 0) past `origin` on `ahead`.
        VertexPositionNormalTexture SlabVertex(const Vector3& camera, const Vector3& onGround, const BikeGround& ground,
                                               const float lift, const bool limited, const Vector3& origin,
                                               const Vector3& ahead, const float cameraAlong)
        {
            const float drop = camera.Y - ground.height;
            float reach = drop > 0.05f ? std::clamp((drop - lift) / drop, 0.1f, 0.98f) : 0.98f;
            if (limited) {
                const float along = Vector3::Dot(onGround - origin, ahead);
                if (cameraAlong < -0.3f && along > 0.12f)
                    reach = std::max(reach, std::min((0.12f - cameraAlong) / (along - cameraAlong), 0.995f));
            }
            return VertexPositionNormalTexture(camera + (onGround - camera) * reach,
                Vector3(ground.normal.x, ground.normal.y, ground.normal.z), Vector2(ground.height, 1.0f / reach));
        }
    }

    void LocalLightRenderer::DrawSpot(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                      const Vector3& lamp, const Vector3& axis, const GroundSampler& ground,
                                      const float strength, const Vector3& tint)
    {
        if (!(strength > 0.01f) || (lamp - camera).LengthSquared() > 90.0f * 90.0f) return;
        Vector3 ahead(axis.X, 0.0f, axis.Z);
        if (ahead.LengthSquared() < 1e-4f) return;   // pointing at the sky or the ground
        ahead.Normalize();
        const Vector3 side(ahead.Z, 0.0f, -ahead.X);
        const float eye = camera.Y - ground(camera.X, camera.Z).height;
        const float lift = std::clamp(eye - 0.5f, 0.25f, 1.2f);
        const float cameraAlong = Vector3::Dot(camera - lamp, ahead);
        grid_.clear();
        for (int ring = 0; ring < SpotRings; ++ring) {
            const float t = static_cast<float>(ring) / (SpotRings - 1);
            const float radius = 0.25f + 40.0f * std::pow(t, 1.6f);
            for (int spoke = 0; spoke < SpotSpokes; ++spoke) {
                const float angle = -0.85f + 1.7f * static_cast<float>(spoke) / (SpotSpokes - 1);
                const Vector3 at = lamp + (ahead * std::cos(angle) + side * std::sin(angle)) * radius;
                const BikeGround g = ground(at.X, at.Z);
                grid_.push_back(SlabVertex(camera, Vector3(at.X, g.height, at.Z), g, lift, true, lamp, ahead, cameraAlong));
            }
        }
        Submit(view, projection, camera, lamp, Vector3::Normalize(axis), 0.0f, 40.0f, strength, tint, SpotRings, SpotSpokes);
    }

    void LocalLightRenderer::DrawPoint(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                       const Vector3& light, const float radius, const GroundSampler& ground,
                                       const float strength, const Vector3& tint)
    {
        if (!(strength > 0.01f) || !(radius > 0.5f) || (light - camera).LengthSquared() > 120.0f * 120.0f) return;
        const float eye = camera.Y - ground(camera.X, camera.Z).height;
        const float lift = std::clamp(eye - 0.5f, 0.25f, 1.2f);
        grid_.clear();
        // rings outward from the light (the first a point), spokes round it; the last spoke
        // closes the circle
        for (int ring = 0; ring < PointRings; ++ring) {
            const float r = radius * std::pow(static_cast<float>(ring) / (PointRings - 1), 1.3f);
            for (int spoke = 0; spoke <= PointSpokes; ++spoke) {
                const float angle = 6.2831853f * static_cast<float>(spoke % PointSpokes) / PointSpokes;
                const float x = light.X + std::cos(angle) * r, z = light.Z + std::sin(angle) * r;
                const BikeGround g = ground(x, z);
                grid_.push_back(SlabVertex(camera, Vector3(x, g.height, z), g, lift, false, light, Vector3::UnitX, 0.0f));
            }
        }
        Submit(view, projection, camera, light, Vector3::Up, 1.0f, radius, strength, tint, PointRings, PointSpokes + 1);
    }

    void LocalLightRenderer::DrawPlane(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                       const Vector3& light, const float radius, const std::span<const Vector3> triangles,
                                       const Vector3& normal, const float strength, const Vector3& tint)
    {
        if (!(strength > 0.01f) || !(radius > 0.5f) || triangles.size() < 3U
            || (light - camera).LengthSquared() > 120.0f * 120.0f) return;
        staging_.clear();
        for (const Vector3& corner : triangles) staging_.emplace_back(corner, normal, Vector2(0.0f, 1.0f));
        Flush(view, projection, camera, light, Vector3::Up, 2.0f, radius, strength, tint);
    }

    void LocalLightRenderer::Submit(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                    const Vector3& light, const Vector3& axis, const float kind, const float radius,
                                    const float strength, const Vector3& tint, const int rings, const int spokes)
    {
        staging_.clear();
        const auto at = [&](int ring, int spoke) { return grid_[static_cast<std::size_t>(ring * spokes + spoke)]; };
        for (int ring = 0; ring + 1 < rings; ++ring) {
            for (int spoke = 0; spoke + 1 < spokes; ++spoke) {
                staging_.push_back(at(ring, spoke));
                staging_.push_back(at(ring + 1, spoke));
                staging_.push_back(at(ring + 1, spoke + 1));
                staging_.push_back(at(ring, spoke));
                staging_.push_back(at(ring + 1, spoke + 1));
                staging_.push_back(at(ring, spoke + 1));
            }
        }
        Flush(view, projection, camera, light, axis, kind, radius, strength, tint);
    }

    void LocalLightRenderer::Flush(const Matrix& view, const Matrix& projection, const Vector3& camera,
                                   const Vector3& light, const Vector3& axis, const float kind, const float radius,
                                   const float strength, const Vector3& tint)
    {
        const int count = static_cast<int>(std::min<std::size_t>(staging_.size(), static_cast<std::size_t>(Capacity)) / 3U * 3U);
        if (count <= 0) return;
        mesh_->SetData(staging_.data(), 0, count, SetDataOptions::Discard);
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(*blend_);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        float viewProjection[16];
        (view * projection).ToColumnMajor(viewProjection);
        effect_->Apply();
        effect_->SetUniformMat4("uViewProjection", viewProjection);
        effect_->SetUniformVec3("uCamera", camera.X, camera.Y, camera.Z);
        effect_->SetUniformVec3("uLight", light.X, light.Y, light.Z);
        effect_->SetUniformVec3("uAxis", axis.X, axis.Y, axis.Z);
        effect_->SetUniformVec3("uShape", kind, radius, strength);
        effect_->SetUniformVec3("uTint", tint.X, tint.Y, tint.Z);
        device_.SetVertexBuffer(mesh_.get());
        device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, count / 3);
        ++drawCalls_;
    }
}
