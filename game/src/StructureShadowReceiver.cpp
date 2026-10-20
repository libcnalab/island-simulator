#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include <stdexcept>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    namespace
    {
        constexpr const char* VertexBody = R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
#ifdef STRUCTURE_TEXTURED
layout(location = 2) in vec2 aTextureCoordinate;
out vec2 vTextureCoordinate;
#endif
uniform mat4 uViewProjection;
out vec4 vColor;
out vec3 vWorld;
void main()
{
    vColor = aColor;
    vWorld = aPosition;
#ifdef STRUCTURE_TEXTURED
    vTextureCoordinate = aTextureCoordinate;
#endif
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";

        constexpr const char* FragmentBody = R"(
in vec4 vColor;
in vec3 vWorld;
#ifdef STRUCTURE_TEXTURED
in vec2 vTextureCoordinate;
uniform sampler2D uTexture;
#endif
uniform vec3 uCameraPosition;
uniform vec2 uFogRange;
out vec4 fragColor;
void main()
{
    vec4 color = vColor;
#ifdef STRUCTURE_TEXTURED
    color *= texture(uTexture, vTextureCoordinate);
#endif
    color.rgb *= mix(vec3(0.56, 0.60, 0.66), vec3(1.0), sunShadowVisibility(vWorld));
    float fog = clamp((length(vWorld - uCameraPosition) - uFogRange.x) / (uFogRange.y - uFogRange.x), 0.0, 1.0);
    fragColor = vec4(mix(color.rgb, islandAtmosphereFog, fog), color.a);
}
)";
    }

    StructureShadowReceiver::StructureShadowReceiver(GraphicsDevice& device, const std::string& name,
                                                     const bool textured, const float fogStart, const float fogEnd)
        : textured_(textured), fogStart_(fogStart), fogEnd_(fogEnd)
    {
        if (!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects))
            throw std::runtime_error("custom effects unavailable");
        using CNA::Graphics::ShaderCodeEXT;
        const std::string define = textured ? "#define STRUCTURE_TEXTURED 1\n" : "";
        const auto source = [&](const bool embedded, const bool fragment) {
            return std::string(embedded ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + define + environment::FogShaderDefinition
                + (fragment ? SunShadowMap::ReceiverShader : std::string())
                + (fragment ? FragmentBody : VertexBody);
        };
        const CNA::Graphics::ShaderPackageEXT package(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main",
                           name + "/receiver.es.vert", source(true, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main",
                           name + "/receiver.es.frag", source(true, true)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex, "main",
                           name + "/receiver.gl.vert", source(false, false)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment, "main",
                           name + "/receiver.gl.frag", source(false, true))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        effect_ = std::make_unique<ShaderEffect>(device, package);
        if (!effect_->IsEffectValid())
            throw std::runtime_error(name + " shadow receiver shader failed: " + effect_->GetCompileErrorEXT());
    }

    StructureShadowReceiver::~StructureShadowReceiver() = default;

    void StructureShadowReceiver::Apply(const Matrix& viewProjection, const Vector3& camera,
                                        const SunShadowMap* map, Texture2D* texture)
    {
        float matrix[16];
        viewProjection.ToColumnMajor(matrix);
        effect_->Apply();
        effect_->SetUniformMat4("uViewProjection", matrix);
        effect_->SetUniformVec3("uCameraPosition", camera.X, camera.Y, camera.Z);
        effect_->SetUniformVec2("uFogRange", fogStart_, fogEnd_);
        if (textured_ && texture) {
            effect_->SetTexture(0, *texture);
            effect_->SetUniformInt("uTexture", 0);
        }
        if (map) map->Bind(*effect_);
        else SunShadowMap::BindDisabled(*effect_);
    }
}
