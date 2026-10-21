#include "IslandSimulator/BridgeRenderer.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/StructureMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
BridgeRenderer::BridgeRenderer(GraphicsDevice& device,const BridgePlacement& bridge)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    StructureMeshBuilder b;
    const Vector3 dir(bridge.dirX,0,bridge.dirZ),across(bridge.AcrossX(),0,bridge.AcrossZ());
    const Vector3 anchor(bridge.ax,0,bridge.az);
    const Vector3 plank(158,128,92),plankSide(118,94,66),post(92,72,52),rope(176,154,112);
    const auto deckPoint=[&](float along,float acrossOffset,float lift) {
        return anchor+dir*along+across*acrossOffset+Vector3(0,bridge.DeckHeight(along)+lift,0);
    };
    // Planks across the walkway, each following the local slope of the sag.
    int plankIndex=0;
    for(float along=BridgePlacement::PlankWidth*0.5f;along<bridge.span-0.05f;
        along+=BridgePlacement::PlankPitch,++plankIndex) {
        const Vector3 behind=deckPoint(std::max(0.0f,along-0.1f),0,0);
        const Vector3 ahead=deckPoint(std::min(bridge.span,along+0.1f),0,0);
        Vector3 forward=ahead-behind;forward.Normalize();
        Vector3 up=Vector3::Cross(forward,across);if(up.Y<0) up=-up;up.Normalize();
        const float weather=0.84f+0.16f*std::sin(static_cast<float>(plankIndex)*2.3f);
        b.Box(deckPoint(along,0,-0.02f),across,up,forward,BridgePlacement::DeckHalfWidth,0.02f,
            BridgePlacement::PlankWidth*0.5f,plank*weather,plankSide*weather);
    }
    // Two carrying ropes under the plank ends and two hand ropes above, all sagging alike.
    constexpr int RopeSegments=12;
    for(const float side:{-1.0f,1.0f}) {
        for(int segment=0;segment<RopeSegments;++segment) {
            const float a0=bridge.span*static_cast<float>(segment)/RopeSegments;
            const float a1=bridge.span*static_cast<float>(segment+1)/RopeSegments;
            const float edge=side*(BridgePlacement::DeckHalfWidth+0.02f);
            b.Cylinder(deckPoint(a0,edge,-0.04f),deckPoint(a1,edge,-0.04f),0.025f,5,rope,false);
            b.Cylinder(deckPoint(a0,side*BridgePlacement::PostAcross,BridgePlacement::RailHeight),
                deckPoint(a1,side*BridgePlacement::PostAcross,BridgePlacement::RailHeight),0.022f,5,rope,false);
        }
        // Vertical ties from the hand rope down to the walkway every other plank.
        for(float along=0.55f;along<bridge.span-0.4f;along+=0.60f) {
            b.Cylinder(deckPoint(along,side*(BridgePlacement::DeckHalfWidth+0.02f),-0.03f),
                deckPoint(along,side*BridgePlacement::PostAcross,BridgePlacement::RailHeight),0.012f,4,rope,false);
        }
        // End posts, sunk into the bank, carrying both ropes.
        for(const float along:{0.0f,bridge.span}) {
            const Vector3 foot=deckPoint(along,side*BridgePlacement::PostAcross,-0.45f);
            const Vector3 top=deckPoint(along,side*BridgePlacement::PostAcross,BridgePlacement::RailHeight+0.12f);
            b.Cylinder(foot,top,0.075f,7,post);
        }
    }
    triangles_=static_cast<int>(b.mesh.size()/3);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:b.mesh) { low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position); }
    bounds_=BoundingBox(low-Vector3(0.2f,0.2f,0.2f),high+Vector3(0.2f,0.2f,0.2f));
    centre_=(low+high)*0.5f;
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(b.mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(b.mesh.data(),static_cast<int>(b.mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try { receiver_=std::make_unique<StructureShadowReceiver>(device,"bridge",false,300.0f,760.0f,0.46f); }
    catch(const std::exception& error) {
        std::cout<<"island-simulator: bridge sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::cout<<"island-simulator: rope bridge spans "<<bridge.span<<" m of the stream at station "
        <<bridge.station<<"; "<<triangles_<<" triangles in one draw\n";
    const char* message="island-simulator: rope bridge crosses the stream's lower reach on a sagging plank walkway with hand ropes; the walker crosses dry";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void BridgeRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>300*300
        ||BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);drawCalls_=1;
}

void BridgeRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try { sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"bridge"); }
        catch(const std::exception& error) {
            std::cout<<"island-simulator: bridge sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
}
}
