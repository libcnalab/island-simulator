#include "IslandSimulator/UnderwaterEffectRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr const char* VertexBody=R"(
layout(location=0) in vec3 aPosition;
layout(location=1) in vec4 aColor;
out vec2 vUv;
void main(){
    vUv=aPosition.xy*0.5+0.5;
    gl_Position=vec4(aPosition.xy,0.0,1.0);
}
)";
// Premultiplied output: the shafts add light with zero coverage, the bubbles add a little veil.
constexpr const char* FragmentBody=R"(
in vec2 vUv;
uniform vec4 uWater;      // camera depth, swim intensity, bubble burst, seconds
uniform vec4 uView;       // sun screen tilt x, sun facing 0..1, aspect, sun height above view
out vec4 fragColor;
float hash12(vec2 p){
    vec3 p3=fract(vec3(p.xyx)*0.1031);
    p3+=dot(p3,p3.yzx+33.33);
    return fract((p3.x+p3.y)*p3.z);
}
void main(){
    float depth=uWater.x;
    float t=uWater.w;
    vec3 light=vec3(0.0);
    float cover=0.0;
    // Light shafts: streaks leaning toward the sun's side of the frame, brightest near the
    // surface, broken by two slow carriers so they flicker like refracted sunlight.
    float lean=clamp(uView.x,-0.9,0.9);
    float x=(vUv.x-0.5)*uView.z+(1.0-vUv.y)*lean*0.8;
    float carrier=0.5+0.5*sin(x*21.0+t*0.55+sin(x*5.3-t*0.21)*1.7);
    float carrier2=0.5+0.5*sin(x*47.0-t*0.37+sin(x*9.1+t*0.16)*2.2);
    float shaft=smoothstep(0.62,0.98,carrier*0.7+carrier2*0.3);
    float height=pow(clamp(vUv.y,0.0,1.0),1.5);
    float shaftStrength=shaft*height*exp(-depth*0.16)*mix(0.35,1.0,uView.y)*(0.55+0.45*uView.w);
    light+=vec3(0.30,0.42,0.44)*shaftStrength*0.55;
    // Bubbles: the diver's breath rising past the camera in a few lanes, more after a descent.
    float swim=uWater.y;
    float burst=uWater.z;
    if(swim>0.2&&depth>0.25){
        for(int lane=0;lane<3;++lane){
            float speed=0.09+0.035*float(lane);
            vec2 grid=vec2(vUv.x*7.0+float(lane)*2.3,(vUv.y-t*speed)*3.0);
            vec2 cell=floor(grid);
            float seed=hash12(cell+float(lane)*17.0);
            float present=step(1.0-(0.22+burst*0.55),seed);
            vec2 centre=cell+vec2(0.2+0.6*fract(seed*7.1),0.2+0.6*fract(seed*13.7));
            vec2 d=(grid-centre)*vec2(1.0/7.0*uView.z,1.0/3.0);
            float r=0.006+0.006*fract(seed*3.3);
            float dist=length(d);
            float ring=smoothstep(r,r*0.78,dist)-smoothstep(r*0.70,r*0.42,dist);
            float glint=smoothstep(r*0.45,0.0,length(d-vec2(-r*0.3,r*0.3)));
            float bubble=(ring*0.55+glint*0.6)*present*swim;
            light+=vec3(0.55,0.70,0.72)*bubble;
            cover+=bubble*0.35;
        }
    }
    fragColor=vec4(light,clamp(cover,0.0,0.6));
}
)";
}

UnderwaterEffectRenderer::UnderwaterEffectRenderer(GraphicsDevice& device):device_(device) {
    using CNA::Graphics::ShaderCodeEXT;
    const auto source=[](bool es,const char* body){
        return std::string(es?"#version 300 es\nprecision highp float;\n":"#version 330 core\n")+body;};
    try {
        const CNA::Graphics::ShaderPackageEXT package({
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","underwater.es.vert",source(true,VertexBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","underwater.es.frag",source(true,FragmentBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","underwater.vert",source(false,VertexBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","underwater.frag",source(false,FragmentBody))},
            {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment},{});
        auto effect=std::make_unique<ShaderEffect>(device,package);
        if(!effect->IsEffectValid()) {
            std::cout<<"island-simulator: underwater effect shader unavailable: "<<effect->GetCompileErrorEXT()<<'\n';
            return;
        }
        effect_=std::move(effect);
    } catch(const std::exception& error) {
        std::cout<<"island-simulator: underwater effect shader unavailable: "<<error.what()<<'\n';
        return;
    }
    // One clip-space triangle covers the viewport; the vertex shader ignores every matrix.
    const std::array<VertexPositionColor,3> triangle{
        VertexPositionColor(Vector3(-1,-1,0),Color(255,255,255,255)),
        VertexPositionColor(Vector3(3,-1,0),Color(255,255,255,255)),
        VertexPositionColor(Vector3(-1,3,0),Color(255,255,255,255))};
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),3,BufferUsage::WriteOnly);
    vertices_->SetData(triangle.data(),3);
    const char* message="island-simulator: underwater view adds sun-tilted light shafts, the diver's rising bubbles and moving seabed caustics";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void UnderwaterEffectRenderer::Draw(const float cameraDepth,const float swimIntensity,const float bubbleBurst,
                                    const float seconds,const Matrix& view,const int viewportWidth,
                                    const int viewportHeight) {
    drawCalls_=0;
    if(!effect_||!std::isfinite(cameraDepth)||cameraDepth<=0.03f||viewportWidth<=0||viewportHeight<=0) return;
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);
    sun.Normalize();
    const Vector3 sunView=Vector3::TransformNormal(sun,view);
    // The view looks down -Z: the sun ahead of the camera means shafts lean little and glow more.
    const float facing=std::clamp(0.5f-0.5f*sunView.Z,0.0f,1.0f);
    const float tilt=std::clamp(sunView.X,-1.0f,1.0f);
    const float above=std::clamp(sunView.Y,0.0f,1.0f);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::None);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->Apply();
    effect_->SetUniformVec4("uWater",cameraDepth,std::clamp(swimIntensity,0.0f,1.0f),
        std::clamp(bubbleBurst,0.0f,1.0f),std::isfinite(seconds)?std::remainder(seconds,628.31853f):0.0f);
    effect_->SetUniformVec4("uView",tilt,facing,static_cast<float>(viewportWidth)/static_cast<float>(viewportHeight),above);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,1);
    drawCalls_=1;
}
}
