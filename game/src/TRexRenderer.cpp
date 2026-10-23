#include "IslandSimulator/TRexRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "CNA/GraphicsCapability.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
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
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElementUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
const VertexDeclaration& SkinDeclaration() {
    static const VertexDeclaration declaration{
        VertexElement(offsetof(TRexSkinVertex,position),VertexElementFormat::Vector3,VertexElementUsage::Position,0),
        VertexElement(offsetof(TRexSkinVertex,normal),VertexElementFormat::Vector3,VertexElementUsage::Normal,0),
        VertexElement(offsetof(TRexSkinVertex,pigment),VertexElementFormat::Color,VertexElementUsage::Color,0),
        VertexElement(offsetof(TRexSkinVertex,rest),VertexElementFormat::Vector4,VertexElementUsage::TextureCoordinate,0),
        VertexElement(offsetof(TRexSkinVertex,surface),VertexElementFormat::Color,VertexElementUsage::Color,1)};
    return declaration;
}

constexpr const char* SkinVertex=R"(
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aPigment;
layout(location = 3) in vec4 aRest;
layout(location = 4) in vec4 aSurface;
uniform mat4 uViewProjection;
out vec3 vWorld;
out vec3 vNormal;
out vec4 vPigment;
out vec4 vRest;
out vec2 vSurface;
flat out float vMaterial;
void main()
{
    vWorld = aPosition;
    vNormal = aNormal;
    vPigment = aPigment;
    vRest = aRest;
    vSurface = aSurface.gb;
    vMaterial = floor(aSurface.r * 255.0 + 0.5);
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";

constexpr const char* SkinFragment=R"(
in vec3 vWorld;
in vec3 vNormal;
in vec4 vPigment;
in vec4 vRest;
in vec2 vSurface;
flat in float vMaterial;
uniform vec3 uCameraPosition;
uniform vec2 uFogRange;
uniform vec3 uToSun;
uniform float uWetness;
out vec4 fragColor;

float hash13(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.zyx + 31.32);
    return fract((p.x + p.y) * p.z);
}
vec3 hash33(vec3 p)
{
    p = fract(p * vec3(0.1031, 0.1030, 0.0973));
    p += dot(p, p.yxz + 33.33);
    return fract((p.xxy + p.yxx) * p.zyx);
}
float noise3(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = mix(hash13(i), hash13(i + vec3(1.0, 0.0, 0.0)), f.x);
    float b = mix(hash13(i + vec3(0.0, 1.0, 0.0)), hash13(i + vec3(1.0, 1.0, 0.0)), f.x);
    float c = mix(hash13(i + vec3(0.0, 0.0, 1.0)), hash13(i + vec3(1.0, 0.0, 1.0)), f.x);
    float d = mix(hash13(i + vec3(0.0, 1.0, 1.0)), hash13(i + vec3(1.0, 1.0, 1.0)), f.x);
    return mix(mix(a, b, f.y), mix(c, d, f.y), f.z);
}
// The scales: the nearest and second nearest of the jittered cell points (in scale units),
// the nearest one's own random value and the way to it.
void scales(vec3 p, out float f1, out float f2, out float id, out vec3 toCentre)
{
    vec3 cell = floor(p);
    vec3 offset = fract(p);
    f1 = 8.0;
    f2 = 8.0;
    id = 0.0;
    toCentre = vec3(0.0);
    for (int z = -1; z <= 1; ++z)
        for (int y = -1; y <= 1; ++y)
            for (int x = -1; x <= 1; ++x) {
                vec3 g = vec3(float(x), float(y), float(z));
                vec3 r = g + 0.12 + 0.76 * hash33(cell + g) - offset;
                float d = dot(r, r);
                if (d < f1) {
                    f2 = f1;
                    f1 = d;
                    id = hash13(cell + g + 17.0);
                    toCentre = r;
                } else if (d < f2) {
                    f2 = d;
                }
            }
    f1 = sqrt(f1);
    f2 = sqrt(f2);
}

// One family of the hide's creases: across `dir` every `spacing` metres (wandering a little),
// `depth` deep where `weight` says; adds its slope (per metre at rest) and how deep in a crease
// the point lies, faded out before the creases would shimmer.
void fold(vec3 p, vec3 dir, float spacing, float depth, float weight, float footprint, inout vec3 slope,
          inout float crease)
{
    // in patches, not rings all round: a hide creases where it bends
    float fade = weight * (1.0 - smoothstep(0.12, 0.30, footprint / spacing))
        * smoothstep(0.38, 0.62, noise3(p * (0.55 / spacing) + 3.7));
    if (fade <= 0.0) return;
    float k = 6.2832 / spacing;
    float phase = dot(p, dir) * k + 4.0 * noise3(p * (0.30 / spacing)) + 1.5 * noise3(p * (1.1 / spacing));
    float c = 0.5 + 0.5 * cos(phase);
    float c5 = c * c * c * c * c;
    slope += dir * (depth * 3.0 * c5 * sin(phase) * k * fade);
    crease = max(crease, c5 * c * fade);
}

void main()
{
    vec3 n = normalize(vNormal);
    vec3 toEye = normalize(uCameraPosition - vWorld);
    vec3 dpdx = dFdx(vWorld);
    vec3 dpdy = dFdy(vWorld);
    vec3 drdx = dFdx(vRest.xyz);
    vec3 drdy = dFdy(vRest.xyz);
    // the side of the surface seen: the mouth's lining shows either face
    vec3 face = normalize(cross(dpdx, dpdy));
    if (dot(face, toEye) < 0.0) face = -face;
    if (dot(n, face) < 0.0) n = -n;
    int material = int(vMaterial);
    vec3 base = vPigment.rgb;
    float occlusion = vPigment.a;
    float gloss = vSurface.x;
    float wet = clamp(uWetness, 0.0, 1.0);
    vec3 L = normalize(uToSun);
    if (material == 2) {
        // the eye: an amber iris streaked toward the round pupil, a dark rim, the cornea's glint
        vec3 e = normalize(vRest.xyz);
        float r = length(e.xy);
        float angle = atan(e.y, e.x);
        vec3 iris = mix(vec3(0.42, 0.27, 0.06), vec3(0.78, 0.56, 0.16), smoothstep(0.18, 0.55, r));
        iris *= 0.86 + 0.14 * sin(angle * 23.0 + r * 9.0);
        base = mix(iris, vec3(0.05, 0.035, 0.03), 1.0 - smoothstep(0.17, 0.21, r));
        base = mix(base, vec3(0.10, 0.07, 0.05), smoothstep(0.58, 0.66, r));
        if (e.z < 0.2) base = vec3(0.12, 0.09, 0.07);
    }
    float footprint = max(length(drdx), length(drdy));
    float scale = vRest.w;
    float plain = vSurface.y;
    if (material == 0 && scale > 0.0) {
        vec3 p = vRest.xyz;
        float ax = abs(p.x);
        // the mottling between the blotches the pigment carries and dark spots over the back
        // and flanks, gone before they would shimmer
        float mottle = noise3(p * 4.5) - 0.5;
        base *= 1.0 + 0.16 * mottle * (1.0 - smoothstep(0.06, 0.18, footprint));
        float spots = smoothstep(0.62, 0.74, noise3(p * 3.1 + 7.3)) * smoothstep(-0.8, -0.2, p.y)
            * (1.0 - smoothstep(3.4, 3.8, p.z));
        base *= 1.0 - 0.20 * spots * (1.0 - smoothstep(0.10, 0.30, footprint));
        // the folds of the hide: rings round the neck and the tail, upright creases down the
        // flanks, rings at the knees and ankles, long creases under the throat
        vec3 slope = vec3(0.0);
        float crease = 0.0;
        fold(p, normalize(vec3(0.0, 0.35, 1.0)), 0.17, 0.008,
             smoothstep(2.75, 3.15, p.z) * (1.0 - smoothstep(4.25, 4.55, p.z)), footprint, slope, crease);
        fold(p, vec3(0.0, 0.0, 1.0), 0.28, 0.009,
             smoothstep(-1.6, -1.0, p.z) * (1.0 - smoothstep(1.2, 1.7, p.z)) * (1.0 - smoothstep(-0.25, 0.25, p.y))
             * smoothstep(0.25, 0.40, ax), footprint, slope, crease);
        fold(p, vec3(0.0, 1.0, 0.0), 0.05, 0.005,
             max(1.0 - smoothstep(0.15, 0.32, abs(p.y + 1.40)), 1.0 - smoothstep(0.08, 0.20, abs(p.y + 2.36)))
             * smoothstep(0.30, 0.40, ax) * (1.0 - smoothstep(2.0, 2.3, p.z)), footprint, slope, crease);
        fold(p, vec3(1.0, 0.0, 0.0), 0.06, 0.005,
             smoothstep(3.5, 3.9, p.z) * (1.0 - smoothstep(5.2, 5.5, p.z)) * (1.0 - smoothstep(0.10, 0.35, p.y))
             * (1.0 - smoothstep(0.25, 0.40, ax)), footprint, slope, crease);
        fold(p, vec3(0.0, 0.0, 1.0), 0.22, 0.006, 1.0 - smoothstep(-1.4, -0.9, p.z), footprint, slope, crease);
        base *= 1.0 - 0.17 * crease * plain;
        slope *= plain;
        // the scales in relief, while each still spans a few pixels
        float fade = (1.0 - smoothstep(0.20, 0.45, footprint / scale)) * plain;
        if (fade > 0.0) {
            float f1;
            float f2;
            float id;
            vec3 toCentre;
            scales(vRest.xyz / scale, f1, f2, id, toCentre);
            float border = f2 - f1;
            float rim = clamp(border / 0.22, 0.0, 1.0);
            float shoulder = rim * rim * (3.0 - 2.0 * rim);
            float groove = 1.0 - smoothstep(0.0, 0.12, border);
            base *= mix(1.0, (0.90 + 0.20 * id) * (1.0 - 0.24 * groove), fade);
            // each scale a low dome: its slope (per metre at rest) toward the scale's middle
            vec3 dome = toCentre * (2.0 * shoulder)
                + normalize(toCentre + 1e-5) * ((1.0 - f1 * f1) * 6.0 * rim * (1.0 - rim) / 0.22);
            slope += dome * (0.16 * fade);
            gloss *= 1.0 - 0.6 * groove * fade;
        }
        // the relief bends the normal: the slope at rest carried onto the surface as drawn
        float dhdx = dot(slope, drdx);
        float dhdy = dot(slope, drdy);
        vec3 r1 = cross(dpdy, n);
        vec3 r2 = cross(n, dpdx);
        float det = dot(dpdx, r1);
        vec3 surfaceGradient = sign(det) * (dhdx * r1 + dhdy * r2);
        n = normalize(abs(det) * n - surfaceGradient);
    }
    // rain darkens the hide and gives everything a sheen
    if (material == 0) base *= 1.0 - 0.26 * wet;
    gloss = clamp(gloss + 0.45 * wet, 0.0, 1.0);
    // the hour's sun (wrapped a little: a living hide is not chalk), the trees' and the clouds'
    // shade, the sky's light above and the ground's bounce beneath
    float ndl = dot(n, L);
    float direct = mix(max(ndl, 0.0), max((ndl + 0.3) / 1.3, 0.0), 0.35);
    float facing = smoothstep(0.0, 0.25, ndl);
    float sun = direct * mix(1.0, sunShadowVisibility(vWorld), facing);
    float sky = 0.80 + 0.20 * n.y;
    vec3 colour = base * (0.56 * occlusion * sky + 0.46 * sun);
    vec3 halfway = normalize(L + toEye);
    float power = mix(10.0, material == 2 ? 140.0 : 70.0, gloss);
    // glints only where the sun itself shines (none at night, little under overcast); the eye
    // keeps a faint one, and a wet hide the sky's sheen at a grazing look
    float specular = pow(max(dot(n, halfway), 0.0), power) * (0.04 + 0.55 * gloss * gloss) * sun
        * max(uDirectSun, material == 2 ? 0.35 : 0.10);
    float grazing = pow(1.0 - max(dot(n, toEye), 0.0), 3.0);
    if (material == 2) specular = specular * 2.2 + 0.05 * grazing;
    if (material == 4) specular *= 0.2;
    colour += vec3(specular) + base * (0.10 * wet * grazing);
    float fog = clamp((length(vWorld - uCameraPosition) - uFogRange.x) / (uFogRange.y - uFogRange.x), 0.0, 1.0);
    fragColor = vec4(mix(colour, islandAtmosphereFog, fog), 1.0);
}
)";
}

TRexRenderer::TRexRenderer(GraphicsDevice& device,const float maximumDistance)
    :device_(device),maximumDistance_(maximumDistance) {
    const auto vertices=body_.Vertices();
    const auto indices=body_.Indices();
    buffer_=std::make_unique<DynamicVertexBuffer>(device,SkinDeclaration(),static_cast<int>(vertices.size()),BufferUsage::WriteOnly);
    indices_=std::make_unique<IndexBuffer>(device,IndexElementSize::SixteenBits,static_cast<int>(indices.size()),BufferUsage::WriteOnly);
    indices_->SetData(indices.data(),static_cast<int>(indices.size()));
    try {
        if(!device.SupportsCapability(CNA::GraphicsCapability::CustomEffects)||!device.ExecutesShaderEffectSourceEXT())
            throw std::runtime_error("custom effects unavailable");
        using CNA::Graphics::ShaderCodeEXT;
        const auto source=[](const bool embedded,const bool fragment) {
            return std::string(embedded?"#version 300 es\nprecision highp float;\nprecision highp int;\n":"#version 330 core\n")
                +environment::FogShaderDefinition+(fragment?SunShadowMap::ReceiverShader:std::string())
                +(fragment?SkinFragment:SkinVertex);
        };
        const CNA::Graphics::ShaderPackageEXT package({
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","trex/skin.es.vert",source(true,false)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","trex/skin.es.frag",source(true,true)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","trex/skin.vert",source(false,false)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","trex/skin.frag",source(false,true))},
            {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment});
        skin_=std::make_unique<ShaderEffect>(device,package);
        if(!skin_->IsEffectValid()) throw std::runtime_error(skin_->GetCompileErrorEXT());
        std::cout<<"island-simulator: tyrannosaur skin "<<vertices.size()<<" vertices, "<<body_.TriangleCount()
                 <<" triangles, lit per pixel with raised scales\n";
    } catch(const std::exception& error) {
        skin_.reset();
        std::cout<<"island-simulator: tyrannosaur skin falls back to vertex lighting: "<<error.what()<<'\n';
        fallback_=std::make_unique<BasicEffect>(device);
        fallback_->setVertexColorEnabledProperty(true);
        fallback_->setLightingEnabledProperty(false);
        fallback_->setFogEnabledProperty(true);
        fallback_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
        fallback_->setFogStartProperty(300.0f);
        fallback_->setFogEndProperty(760.0f);
        buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(vertices.size()),
                                                      BufferUsage::WriteOnly);
        fallbackVertices_.resize(vertices.size());
    }
}

TRexRenderer::~TRexRenderer()=default;

void TRexRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const TRexMotion& rex) {
    visible_=false;triangles_=0;
    const TRexPoint hips=rex.ToWorld(0.0f,0.0f,0.0f),middle=rex.ToWorld(0.0f,-.9f,-.4f);
    if(Vector3::DistanceSquared(Vector3(hips.x,hips.y,hips.z),camera)>maximumDistance_*maximumDistance_) return;
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(Vector3(middle.x,middle.y,middle.z),7.5f))) return;
    body_.Pose(rex);
    const auto vertices=body_.Vertices();
    const int count=static_cast<int>(vertices.size());
    triangles_=static_cast<int>(body_.TriangleCount());
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    if(skin_) {
        buffer_->SetData(vertices.data(),0,count,SetDataOptions::Discard);
        device_.SetVertexBuffer(buffer_.get());
        device_.setIndicesProperty(indices_.get());
        float matrix[16];
        (view*projection).ToColumnMajor(matrix);
        skin_->Apply();
        skin_->SetUniformMat4("uViewProjection",matrix);
        skin_->SetUniformVec3("uCameraPosition",camera.X,camera.Y,camera.Z);
        skin_->SetUniformVec2("uFogRange",300.0f,760.0f);
        const auto toSun=sun_light::UniformDirection();
        skin_->SetUniformVec3("uToSun",toSun.x,toSun.y,toSun.z);
        skin_->SetUniformFloat("uWetness",weather_now::Current().wetness);
        if(sunShadow_) sunShadow_->Bind(*skin_);
        else SunShadowMap::BindDisabled(*skin_);
    } else {
        // the same skin lit per vertex as the other animals are
        const Vector3 sun=wildlife_render::FixedSunDirection;
        for(std::size_t k=0;k<vertices.size();++k) {
            const TRexSkinVertex& v=vertices[k];
            const float light=.56f*static_cast<float>(v.pigment[3])/255.0f
                +.40f*std::max(0.0f,v.normal[0]*sun.X+v.normal[1]*sun.Y+v.normal[2]*sun.Z)+.10f*std::max(0.0f,v.normal[1]);
            const float wet=wildlife_render::WetCoatFactor();
            const auto channel=[&](const std::uint8_t c) {return static_cast<int>(std::clamp(static_cast<float>(c)*light*wet,0.0f,255.0f));};
            fallbackVertices_[k]=VertexPositionColor(Vector3(v.position[0],v.position[1],v.position[2]),
                                                     Color(channel(v.pigment[0]),channel(v.pigment[1]),channel(v.pigment[2])));
        }
        buffer_->SetData(fallbackVertices_.data(),0,count,SetDataOptions::Discard);
        device_.SetVertexBuffer(buffer_.get());
        device_.setIndicesProperty(indices_.get());
        fallback_->setWorldProperty(Matrix::getIdentityProperty());
        fallback_->setViewProperty(view);
        fallback_->setProjectionProperty(projection);
        fallback_->Apply();
    }
    device_.DrawIndexedPrimitives(PrimitiveType::TriangleList,0,0,count,0,triangles_);
    device_.setIndicesProperty(nullptr);
    visible_=true;
}

}
