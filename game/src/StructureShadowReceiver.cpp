#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "IslandSimulator/CastleStoneMaterial.hpp"
#include "IslandSimulator/BarkMaterial.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
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
#ifdef FALLEN_BARK
layout(location = 2) in vec3 aWoodCoordinate;
out vec3 vWoodCoordinate;
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
#ifdef FALLEN_BARK
    vWoodCoordinate = aWoodCoordinate;
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
#ifdef FALLEN_BARK
in vec3 vWoodCoordinate;
#endif
uniform vec3 uCameraPosition;
uniform vec2 uFogRange;
uniform vec3 uToSun;          // the hour's shading direction (at 14:00 the fixed sun)
uniform float uBakedAmbient;  // the build's sun-independent share of a face's light (0: not relit)
uniform float uWetness;       // 0..1 rain-wet planks: darker, a little cooler
out vec4 fragColor;
void main()
{
    vec4 color = vColor;
#ifdef STRUCTURE_TEXTURED
    color *= texture(uTexture, vTextureCoordinate);
#endif
    // The face (its normal from the world position's derivatives, turned to the eye).
    vec3 faceNormal = normalize(cross(dFdx(vWorld), dFdy(vWorld)));
    if (dot(faceNormal, uCameraPosition - vWorld) < 0.0) faceNormal = -faceNormal;
#ifdef CASTLE_STONE
    vec3 stone=castleStone(vWorld,faceNormal);
    // Timber/loose blocks use opaque 254; existing plain receiver alphas are untouched.
    color.rgb *= mix(vec3(1.0),stone,step(0.998,vColor.a));
    color.a=1.0;
#endif
#ifdef FALLEN_BARK
    // Derivatives are evaluated before material selection, including non-bark triangles.
    vec4 wood = woodMaterialCoordinates(vWoodCoordinate);
    // Existing moss/lichen colours soften the fine relief on the damp upper trunk.
    float moss = smoothstep(0.0, 0.12, vColor.g - vColor.r);
    if (vWoodCoordinate.z >= 0.0)
        color.rgb *= mix(barkMaterial(wood, 19u), 1.0, 0.70 * moss);
#endif
    if (uBakedAmbient > 0.0) {
        // Each face's light was baked for the fixed afternoon sun: it is relit by the hour's sun.
        float direct = 1.0 - uBakedAmbient;
        color.rgb *= (uBakedAmbient + direct * max(0.0, dot(faceNormal, normalize(uToSun))))
            / (uBakedAmbient + direct * max(0.0, dot(faceNormal, normalize(vec3(-0.45, 0.82, -0.34)))));
    }
    color.rgb *= mix(vec3(1.0), vec3(0.64, 0.66, 0.68), clamp(uWetness, 0.0, 1.0));
    // A shadow takes the sun's direct light, which a face turned from the sun never had: such
    // faces (a ceiling under its own roof, a wall's lee side) keep their ambient light whatever
    // the shadow map, the crowns' dapples included.
    float facing = smoothstep(0.0, 0.25, dot(faceNormal, normalize(uToSun)));
    color.rgb *= mix(vec3(0.56, 0.60, 0.66), vec3(1.0), mix(1.0, sunShadowVisibility(vWorld), facing));
    float fog = clamp((length(vWorld - uCameraPosition) - uFogRange.x) / (uFogRange.y - uFogRange.x), 0.0, 1.0);
    fragColor = vec4(mix(color.rgb, islandAtmosphereFog, fog), color.a);
}
)";
    }

    StructureShadowReceiver::StructureShadowReceiver(GraphicsDevice& device, const std::string& name,
                                                     const bool textured, const float fogStart, const float fogEnd,
                                                     const float bakedAmbient,
                                                     const StructureSurface surface)
        : textured_(textured), fogStart_(fogStart), fogEnd_(fogEnd), bakedAmbient_(bakedAmbient)
    {
        if (!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects))
            throw std::runtime_error("custom effects unavailable");
        using CNA::Graphics::ShaderCodeEXT;
        const bool stone=surface==StructureSurface::CastleStone;
        const bool bark=surface==StructureSurface::FallenBark;
        const std::string define = std::string(textured ? "#define STRUCTURE_TEXTURED 1\n" : "")
            +(stone?"#define CASTLE_STONE 1\n":"")+(bark?"#define FALLEN_BARK 1\n":"");
        if (bark && textured)
            throw std::invalid_argument("fallen bark uses wood coordinates, not a texture sampler");
        const auto source = [&](const bool embedded, const bool fragment) {
            return std::string(embedded ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n")
                + define + environment::FogShaderDefinition
                + (fragment ? SunShadowMap::ReceiverShader : std::string())
                + (fragment&&stone ? castle_stone::Shader : std::string())
                + (fragment&&bark ? JungleMaterial::Shader()+BarkMaterial::Shader : std::string())
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
        effect_->SetUniformFloat("uBakedAmbient", bakedAmbient_);
        effect_->SetUniformFloat("uWetness", weather_now::Current().wetness);
        effect_->SetUniformVec3("uToSun", sun_light::UniformDirection().x, sun_light::UniformDirection().y,
                                sun_light::UniformDirection().z);
        if (textured_ && texture) {
            effect_->SetTexture(0, *texture);
            effect_->SetUniformInt("uTexture", 0);
        }
        if (map) map->Bind(*effect_);
        else SunShadowMap::BindDisabled(*effect_);
    }
}
