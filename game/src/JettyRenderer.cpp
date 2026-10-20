#include "IslandSimulator/JettyRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
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
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
    constexpr float Pi=std::numbers::pi_v<float>;
}

JettyRenderer::JettyRenderer(GraphicsDevice& device,const JettyPlacement& jetty)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    StructureMeshBuilder b;
    const Vector3 dir(jetty.dirX,0,jetty.dirZ),across(jetty.AcrossX(),0,jetty.AcrossZ());
    const Vector3 root(jetty.rootX,0,jetty.rootZ);
    const Vector3 plank(168,142,104),plankSide(126,104,76),post(98,78,56),rail(150,124,88);
    const Vector3 canoeWood(96,66,40),canoeInside(172,138,96),paddle(142,108,66);
    // Deck planks across the stringers, each with its own weathering.
    constexpr float PlankPitch=0.30f,PlankWidth=0.26f,PlankThickness=0.05f;
    int plankIndex=0;
    for(float along=0.15f;along<JettyPlacement::Length-0.05f;along+=PlankPitch,++plankIndex) {
        const float y=jetty.DeckHeight(along);
        const float yNext=jetty.DeckHeight(std::min(along+PlankWidth*0.5f,JettyPlacement::Length));
        const float yPrev=jetty.DeckHeight(std::max(along-PlankWidth*0.5f,0.0f));
        Vector3 up=Vector3::Up;
        Vector3 forward=dir*PlankWidth+Vector3(0,yNext-yPrev,0);forward.Normalize();
        up=Vector3::Cross(forward,across);if(up.Y<0) up=-up;up.Normalize();
        const float weather=0.86f+0.14f*std::sin(static_cast<float>(plankIndex)*1.7f);
        const float gap=(plankIndex%7==3)?0.08f:0.0f;    // an occasional sprung plank
        b.Box(root+dir*along+Vector3(0,y-PlankThickness*0.5f,0)+across*gap,across,up,forward,
            JettyPlacement::DeckHalfWidth,PlankThickness*0.5f,PlankWidth*0.5f,
            plank*weather,plankSide*weather);
    }
    // Two stringers under the planks, following the ramp in short segments.
    for(float along=0.0f;along<JettyPlacement::Length;along+=2.0f) {
        const float end=std::min(along+2.0f,JettyPlacement::Length);
        const float y0=jetty.DeckHeight(along)-0.11f,y1=jetty.DeckHeight(end)-0.11f;
        for(const float side:{-1.0f,1.0f}) {
            const Vector3 a=root+dir*along+across*(side*0.62f)+Vector3(0,y0,0);
            const Vector3 c=root+dir*end+across*(side*0.62f)+Vector3(0,y1,0);
            Vector3 forward=c-a;forward.Normalize();
            const Vector3 centre=(a+c)*0.5f;
            b.Box(centre,across,Vector3::Up,forward,0.06f,0.06f,(c-a).Length()*0.5f,rail,post);
        }
    }
    for(std::size_t index=0;index<JettyPlacement::PostCount;++index) {
        const auto& p=jetty.posts[index];
        const bool mooring=index==JettyPlacement::PostCount-1;
        b.Cylinder(Vector3(p.x,p.bottomY,p.z),Vector3(p.x,p.topY,p.z),
            mooring?JettyPlacement::PostRadius*1.25f:JettyPlacement::PostRadius,8,post);
    }
    // Dugout canoe: a lofted hollow log with a thick gunwale, resting slightly sunk in the sand.
    const Vector3 cAxis(std::sin(jetty.canoeYaw),0,std::cos(jetty.canoeYaw));
    const Vector3 cSide(cAxis.Z,0,-cAxis.X);
    const Vector3 cCentre(jetty.canoeX,jetty.canoeY-0.04f,jetty.canoeZ);
    constexpr int Rings=9,Segments=6;
    const auto hullPoint=[&](int ring,int segment,float scale,float lift) {
        const float t=static_cast<float>(ring)/static_cast<float>(Rings-1);
        const float along=(t-0.5f)*JettyPlacement::CanoeLength;
        const float halfBeam=JettyPlacement::CanoeBeam*0.5f*std::pow(std::sin(t*Pi),0.55f)*scale;
        const float depth=JettyPlacement::CanoeDepth*(0.55f+0.45f*std::sin(t*Pi))*scale;
        const float a=Pi*static_cast<float>(segment)/static_cast<float>(Segments);   // 0..pi, gunwale to gunwale
        return cCentre+cAxis*along+cSide*(std::cos(a)*halfBeam)
            +Vector3(0,JettyPlacement::CanoeDepth-std::sin(a)*depth+lift,0);
    };
    for(int ring=0;ring+1<Rings;++ring) for(int segment=0;segment<Segments;++segment) {
        b.Quad(hullPoint(ring,segment,1.0f,0),hullPoint(ring+1,segment,1.0f,0),
            hullPoint(ring+1,segment+1,1.0f,0),hullPoint(ring,segment+1,1.0f,0),canoeWood);
        b.Quad(hullPoint(ring,segment+1,0.80f,0.02f),hullPoint(ring+1,segment+1,0.80f,0.02f),
            hullPoint(ring+1,segment,0.80f,0.02f),hullPoint(ring,segment,0.80f,0.02f),canoeInside);
    }
    for(int ring=0;ring+1<Rings;++ring) {
        // Gunwale strips close the hull between the outer and inner surfaces.
        b.Quad(hullPoint(ring,0,1.0f,0),hullPoint(ring+1,0,1.0f,0),hullPoint(ring+1,0,0.80f,0.02f),
            hullPoint(ring,0,0.80f,0.02f),canoeWood*1.15f);
        b.Quad(hullPoint(ring,Segments,0.80f,0.02f),hullPoint(ring+1,Segments,0.80f,0.02f),
            hullPoint(ring+1,Segments,1.0f,0),hullPoint(ring,Segments,1.0f,0),canoeWood*1.15f);
    }
    // A paddle laid across the gunwales near the stern.
    const Vector3 paddleCentre=cCentre+cAxis*(-1.1f)+Vector3(0,JettyPlacement::CanoeDepth+0.04f,0);
    b.Box(paddleCentre,cAxis,Vector3::Up,cSide,0.025f,0.018f,0.62f,paddle,paddle*0.85f);
    b.Box(paddleCentre+cSide*0.80f,cAxis,Vector3::Up,cSide,0.085f,0.012f,0.22f,paddle,paddle*0.85f);

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
    // The same linear band the ocean and terrain fade across, so the deck dissolves with them.
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try { receiver_=std::make_unique<StructureShadowReceiver>(device,"jetty",false,300.0f,760.0f); }
    catch(const std::exception& error) {
        std::cout<<"island-simulator: jetty sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::cout<<"island-simulator: jetty "<<JettyPlacement::PostCount<<" posts carry a "
        <<JettyPlacement::Length<<" m plank deck from the beach over the shallows; "<<triangles_
        <<" triangles in one draw with the dugout canoe\n";
    const char* message="island-simulator: resident's jetty runs from the beach below the hut to a mooring-post perch, with a dugout canoe drawn up beside the root; the water collars its posts with foam";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void JettyRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>320*320
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

void JettyRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try { sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"jetty"); }
        catch(const std::exception& error) {
            std::cout<<"island-simulator: jetty sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
}
}
