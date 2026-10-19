#include "IslandSimulator/LookoutRenderer.hpp"
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
LookoutRenderer::LookoutRenderer(GraphicsDevice& device,const LookoutPlacement& lookout)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    StructureMeshBuilder b;
    const Vector3 dir(lookout.dirX,0,lookout.dirZ),across(lookout.AcrossX(),0,lookout.AcrossZ());
    const Vector3 centre(lookout.x,0,lookout.z);
    const Vector3 post(96,74,52),plank(166,138,98),plankSide(124,100,72),rail(150,122,86);
    constexpr float Half=LookoutPlacement::Half;
    // Posts, sunk into the ground, rising through the deck to carry the rails.
    const auto posts=lookout.PostObstacles();
    for(std::size_t i=0;i<4;++i) {
        const Vector3 foot(posts[i].x,lookout.postBottomY[i],posts[i].z);
        const Vector3 top(posts[i].x,lookout.platformY+LookoutPlacement::RailHeight+0.08f,posts[i].z);
        b.Cylinder(foot,top,LookoutPlacement::PostRadius,8,post);
    }
    // Diagonal braces between neighbouring posts under the deck.
    for(int side=0;side<4;++side) {
        const Vector3 a(posts[side].x,0,posts[side].z),c(posts[(side+1)%4].x,0,posts[(side+1)%4].z);
        if(side==1||side==2) {}  // every side gets one brace
        const Vector3 low=a+Vector3(0,lookout.groundY+0.5f,0),high=c+Vector3(0,lookout.platformY-0.35f,0);
        b.Cylinder(low,high,0.045f,5,post,false);
    }
    // Platform planks across the ramp direction.
    constexpr float PlankPitch=0.27f,PlankWidth=0.24f;
    int plankIndex=0;
    for(float u=-Half+PlankWidth*0.5f;u<Half;u+=PlankPitch,++plankIndex) {
        const float weather=0.86f+0.14f*std::sin(static_cast<float>(plankIndex)*1.9f);
        b.Box(centre+dir*u+Vector3(0,lookout.platformY-0.025f,0),across,Vector3::Up,dir,
            Half,0.025f,PlankWidth*0.5f,plank*weather,plankSide*weather);
    }
    // Rails on three sides (the ramp side stays open), two bars each.
    for(const float height:{LookoutPlacement::RailHeight,LookoutPlacement::RailHeight*0.55f}) {
        const float y=lookout.platformY+height;
        b.Box(centre-dir*Half+Vector3(0,y,0),across,Vector3::Up,dir,Half+0.05f,0.03f,0.035f,rail,post);
        for(const float side:{-1.0f,1.0f})
            b.Box(centre+across*(side*Half)+Vector3(0,y,0),across,Vector3::Up,dir,0.035f,0.03f,Half+0.05f,rail,post);
    }
    // The ramp: two stringers, planks across, and a cleat every fourth plank.
    const Vector3 topEdge=centre+dir*Half+Vector3(0,lookout.platformY,0);
    const Vector3 foot=centre+dir*(Half+LookoutPlacement::RampLength)+Vector3(0,lookout.rampFootY,0);
    Vector3 rampDir=foot-topEdge;const float rampLength=rampDir.Length();rampDir.Normalize();
    Vector3 rampUp=Vector3::Cross(rampDir,across);if(rampUp.Y<0) rampUp=-rampUp;rampUp.Normalize();
    for(const float side:{-1.0f,1.0f}) {
        const Vector3 a=topEdge+across*(side*(LookoutPlacement::RampHalfWidth-0.04f))-rampUp*0.08f;
        const Vector3 c=foot+across*(side*(LookoutPlacement::RampHalfWidth-0.04f))-rampUp*0.08f;
        b.Box((a+c)*0.5f,across,rampUp,rampDir,0.05f,0.07f,rampLength*0.5f,rail,post);
    }
    int step=0;
    for(float along=0.14f;along<rampLength-0.1f;along+=0.28f,++step) {
        const Vector3 at=topEdge+rampDir*along;
        const float weather=0.84f+0.16f*std::sin(static_cast<float>(step)*2.3f);
        b.Box(at-rampUp*0.02f,across,rampUp,rampDir,LookoutPlacement::RampHalfWidth,0.02f,0.12f,plank*weather,plankSide*weather);
        if(step%4==2) b.Box(at+rampUp*0.02f,across,rampUp,rampDir,LookoutPlacement::RampHalfWidth-0.05f,0.02f,0.03f,post,post);
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
    std::cout<<"island-simulator: lookout tower stands "<<LookoutPlacement::PlatformRise<<" m beside the hut clearing with a "
        <<LookoutPlacement::RampLength<<" m cleated ramp; "<<triangles_<<" triangles in one draw\n";
    const char* message="island-simulator: resident's lookout tower beside the hut carries a railed platform the walker climbs by a cleated ramp";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void LookoutRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>320*320
        ||BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);drawCalls_=1;
}

void LookoutRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try { sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"lookout"); }
        catch(const std::exception& error) {
            std::cout<<"island-simulator: lookout sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
}
}
