#include "IslandSimulator/CanopyShaftRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
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
// Premultiplied output with zero coverage: the shafts only add light.
constexpr const char* FragmentBody=R"(
in vec2 vUv;
uniform vec4 uShaft;      // strength, sun screen tilt, sun facing 0..1, seconds
uniform vec2 uAspect;
out vec4 fragColor;
void main(){
    float t=uShaft.w;
    float lean=clamp(uShaft.y,-0.9,0.9);
    float x=(vUv.x-0.5)*uAspect.x+(1.0-vUv.y)*lean*0.7;
    // Broad beams between the crowns and finer ones within them, each drifting slowly as
    // the leaves above shift, brightest high in the frame where the gaps are.
    float broad=0.5+0.5*sin(x*17.0+t*0.08+sin(x*3.7-t*0.05)*1.9);
    float fine=0.5+0.5*sin(x*41.0-t*0.06+sin(x*7.9+t*0.04)*2.3);
    float shaft=smoothstep(0.64,0.98,broad*0.65+fine*0.35);
    float height=pow(clamp(vUv.y,0.0,1.0),1.3);
    float glow=shaft*height*uShaft.x*mix(0.35,1.0,uShaft.z);
    fragColor=vec4(vec3(0.52,0.47,0.33)*glow*0.55,0.0);
}
)";
}

CanopyShaftRenderer::CanopyShaftRenderer(GraphicsDevice& device):device_(device) {
    using CNA::Graphics::ShaderCodeEXT;
    const auto source=[](bool es,const char* body){
        return std::string(es?"#version 300 es\nprecision highp float;\n":"#version 330 core\n")+body;};
    try {
        const CNA::Graphics::ShaderPackageEXT package({
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","canopyshafts.es.vert",source(true,VertexBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","canopyshafts.es.frag",source(true,FragmentBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","canopyshafts.vert",source(false,VertexBody)),
            ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","canopyshafts.frag",source(false,FragmentBody))},
            {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment},{});
        auto effect=std::make_unique<ShaderEffect>(device,package);
        if(!effect->IsEffectValid()) {
            std::cout<<"island-simulator: canopy shaft shader unavailable: "<<effect->GetCompileErrorEXT()<<'\n';
            return;
        }
        effect_=std::move(effect);
    } catch(const std::exception& error) {
        std::cout<<"island-simulator: canopy shaft shader unavailable: "<<error.what()<<'\n';
        return;
    }
    const std::array<VertexPositionColor,3> triangle{
        VertexPositionColor(Vector3(-1,-1,0),Color(255,255,255,255)),
        VertexPositionColor(Vector3(3,-1,0),Color(255,255,255,255)),
        VertexPositionColor(Vector3(-1,3,0),Color(255,255,255,255))};
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),3,BufferUsage::WriteOnly);
    vertices_->SetData(triangle.data(),3);
    const char* message="island-simulator: under the canopy the view carries slow sun shafts leaning to the sun, gone in the open and under a cloud";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void CanopyShaftRenderer::Draw(const float strength,const float seconds,const Matrix& view,
                               const int viewportWidth,const int viewportHeight) {
    drawCalls_=0;
    if(!effect_||!std::isfinite(strength)||strength<=0.02f||viewportWidth<=0||viewportHeight<=0) return;
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);
    sun.Normalize();
    const Vector3 sunView=Vector3::TransformNormal(sun,view);
    const float facing=std::clamp(0.5f-0.5f*sunView.Z,0.0f,1.0f);
    const float tilt=std::clamp(sunView.X,-1.0f,1.0f);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::None);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->Apply();
    effect_->SetUniformVec4("uShaft",std::clamp(strength,0.0f,1.0f),tilt,facing,
        std::isfinite(seconds)?std::remainder(seconds,628.31853f):0.0f);
    effect_->SetUniformVec2("uAspect",static_cast<float>(viewportWidth)/static_cast<float>(viewportHeight),0.0f);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,1);
    drawCalls_=1;
}
}
