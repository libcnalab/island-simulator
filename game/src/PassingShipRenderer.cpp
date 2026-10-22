#include "IslandSimulator/PassingShipRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/StructureMeshBuilder.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <sstream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr std::size_t LightCapacity=1024;
// the paints, past full for the build's light, and the haze of the lane's distance over them
const Vector3 Hull(58,64,74),Boot(150,62,50),Deck(112,100,88),House(240,238,230),Windows(40,46,54),
    Funnel(52,86,140),FunnelTop(30,30,30),Hatch(76,104,82),Crane(210,170,64);
Vector3 Hazed(const Vector3& colour) {
    const Vector3 fog(environment::FogColorRed*255.0f,environment::FogColorGreen*255.0f,environment::FogColorBlue*255.0f);
    return Vector3::Lerp(colour,fog*1.25f,0.5f);
}
}

PassingShipRenderer::PassingShipRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),lightEffect_(std::make_unique<BasicEffect>(device))
{
    // built round its middle at the waterline, turned to the lane's heading (the lane is straight)
    const PassingShip::State lane=PassingShip::At(0.0f);
    const Vector3 f(lane.headingX,0.0f,lane.headingZ),s(-lane.headingZ,0.0f,lane.headingX),up=Vector3::Up;
    const auto at=[&](const float x,const float y,const float z) {return f*x+up*y+s*z;};
    StructureMeshBuilder b;
    const auto quad=[&](const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3,const Vector3& pigment,const Vector3& outward) {
        // wound to face `outward`
        const Vector3 n=Vector3::Cross(p1-p0,p2-p0);
        if(Vector3::Dot(n,outward)>=0.0f) b.Quad(p0,p1,p2,p3,Hazed(pigment));
        else b.Quad(p0,p3,p2,p1,Hazed(pigment));
    };
    const auto box=[&](const float x0,const float x1,const float y0,const float y1,const float z0,const float z1,const Vector3& pigment,
                       const Vector3& top) {
        const Vector3 c=at((x0+x1)*0.5f,(y0+y1)*0.5f,(z0+z1)*0.5f);
        const std::array<Vector3,8> p{{at(x0,y0,z0),at(x1,y0,z0),at(x1,y0,z1),at(x0,y0,z1),at(x0,y1,z0),at(x1,y1,z0),at(x1,y1,z1),at(x0,y1,z1)}};
        quad(p[4],p[5],p[6],p[7],top,up);
        quad(p[0],p[1],p[5],p[4],pigment,p[0]+p[5]-c*2.0f);quad(p[3],p[2],p[6],p[7],pigment,p[3]+p[6]-c*2.0f);
        quad(p[0],p[3],p[7],p[4],pigment,p[0]+p[7]-c*2.0f);quad(p[1],p[2],p[6],p[5],pigment,p[1]+p[6]-c*2.0f);
    };
    // the hull: square-sterned, full amidships, fining to a raked bow with its sheer rising
    constexpr std::array<float,8> X{{-31.0f,-26.0f,-12.0f,8.0f,20.0f,26.0f,29.5f,31.5f}};
    constexpr std::array<float,8> Half{{4.4f,5.0f,5.0f,5.0f,4.5f,3.0f,1.4f,0.15f}};
    constexpr std::array<float,8> Top{{3.8f,3.5f,3.4f,3.4f,3.6f,4.3f,4.9f,5.3f}};
    for(std::size_t i=0;i+1<X.size();++i) {
        for(const float side:{-1.0f,1.0f}) {
            const Vector3 out=s*side;
            // the boot-top red from below the water to just over it, the dark topsides above
            quad(at(X[i],-0.6f,side*Half[i]),at(X[i+1],-0.6f,side*Half[i+1]),at(X[i+1],0.7f,side*Half[i+1]),at(X[i],0.7f,side*Half[i]),Boot,out);
            quad(at(X[i],0.7f,side*Half[i]),at(X[i+1],0.7f,side*Half[i+1]),at(X[i+1],Top[i+1],side*Half[i+1]),at(X[i],Top[i],side*Half[i]),Hull,out);
        }
        quad(at(X[i],Top[i]-0.1f,-Half[i]),at(X[i+1],Top[i+1]-0.1f,-Half[i+1]),at(X[i+1],Top[i+1]-0.1f,Half[i+1]),at(X[i],Top[i]-0.1f,Half[i]),Deck,up);
    }
    quad(at(X[0],-0.6f,-Half[0]),at(X[0],-0.6f,Half[0]),at(X[0],Top[0],Half[0]),at(X[0],Top[0],-Half[0]),Hull,-f);
    // the hatches between the cranes, the bridge house aft with its band of windows, the funnel
    for(const float x:{-9.0f,1.0f,12.0f}) box(x-4.0f,x+4.0f,3.3f,4.4f,-3.4f,3.4f,Hatch,Hatch*1.05f);
    box(-28.5f,-19.0f,3.4f,10.6f,-4.6f,4.6f,House,House);
    box(-19.05f,-18.95f,8.6f,9.6f,-4.4f,4.4f,Windows,Windows);
    box(-26.0f,-20.0f,10.6f,12.4f,-3.0f,3.0f,House,House);
    box(-19.9f,-19.8f,11.0f,11.9f,-2.8f,2.8f,Windows,Windows);
    box(-27.5f,-24.0f,12.4f,16.0f,-1.4f,1.4f,Funnel,FunnelTop);
    // the cranes on their posts and the foremast
    for(const float x:{-4.0f,7.0f}) {
        box(x-0.4f,x+0.4f,3.4f,12.5f,-0.4f,0.4f,Crane,Crane);
        box(x,x+11.0f,9.0f,9.4f,-0.25f,0.25f,Crane,Crane);
    }
    box(22.6f,23.2f,4.0f,14.0f,-0.3f,0.3f,House,House);
    auto& mesh=b.mesh;
    triangles_=static_cast<int>(mesh.size()/3U);
    hull_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    hull_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(260.0f);effect_->setFogEndProperty(1300.0f);
    lightEffect_->setVertexColorEnabledProperty(true);lightEffect_->setLightingEnabledProperty(false);
    lights_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(LightCapacity),BufferUsage::WriteOnly);
    staging_.reserve(LightCapacity);
    std::ostringstream message;
    message<<"island-simulator: a coastal freighter passes far offshore every "<<static_cast<int>(PassingShip::Period/60.0f)
           <<" minutes, "<<static_cast<int>(PassingShip::Offshore)<<" m out, its wake behind it and its navigation lights burning at night; "
           <<triangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void PassingShipRenderer::Draw(const Matrix& view,const Matrix& projection,const float seconds) {
    drawCalls_=0;
    const PassingShip::State ship=PassingShip::At(seconds);
    visible_=ship.visible;
    if(!ship.visible) return;
    const Vector3 centre(ship.x,0.0f,ship.z);
    if(BoundingFrustum(view*projection).Contains(BoundingSphere(centre,40.0f))==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    effect_->setWorldProperty(Matrix::CreateTranslation(centre));
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.SetVertexBuffer(hull_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);++drawCalls_;
    // its wake: a pale V spreading behind its stern on the water, fading out
    staging_.clear();
    const Vector3 f(ship.headingX,0.0f,ship.headingZ),s(-ship.headingZ,0.0f,ship.headingX);
    const Vector3 stern=centre-f*31.0f+Vector3::Up*0.15f;
    const Color white(46,52,54),none(0,0,0);
    for(const float side:{-1.0f,1.0f}) {
        const Vector3 far=stern-f*140.0f+s*(side*26.0f),mid=stern-f*40.0f+s*(side*9.0f);
        staging_.emplace_back(stern+s*(side*4.0f),white);staging_.emplace_back(mid,Color(30,34,36));staging_.emplace_back(stern,white);
        staging_.emplace_back(mid,Color(30,34,36));staging_.emplace_back(far,none);staging_.emplace_back(stern-f*60.0f,none);
    }
    staging_.emplace_back(stern-s*3.0f,white);staging_.emplace_back(stern+s*3.0f,white);staging_.emplace_back(stern-f*90.0f,none);
    lights_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    lightEffect_->setWorldProperty(Matrix::getIdentityProperty());
    lightEffect_->setViewProperty(view);lightEffect_->setProjectionProperty(projection);lightEffect_->Apply();
    device_.SetVertexBuffer(lights_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));++drawCalls_;
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}

void PassingShipRenderer::DrawLights(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                     const float seconds,const float night) {
    const PassingShip::State ship=PassingShip::At(seconds);
    if(!ship.visible||!(night>0.02f)) return;
    const Vector3 centre(ship.x,0.0f,ship.z),f(ship.headingX,0.0f,ship.headingZ),s(-ship.headingZ,0.0f,ship.headingX);
    staging_.clear();
    Vector3 toEye=camera-centre;
    const float distance=std::max(toEye.Length(),1.0f);
    toEye/=distance;
    Vector3 right=Vector3::Cross(toEye,Vector3::Up);
    if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
    right.Normalize();
    const Vector3 up=Vector3::Cross(right,toEye);
    const float scale=std::max(0.6f,distance*0.0022f);
    const auto glow=[&](const Vector3& at,const float r,const float g,const float bl,const float size) {
        const auto colour=[&](const float k) {
            return Color(static_cast<int>(std::min(255.0f,r*k*night*255.0f)),static_cast<int>(std::min(255.0f,g*k*night*255.0f)),
                         static_cast<int>(std::min(255.0f,bl*k*night*255.0f)));
        };
        for(const auto& layer:{std::array<float,2>{{1.0f,1.0f}},std::array<float,2>{{3.2f,0.28f}}}) {
            const Color core=colour(layer[1]),rim(0,0,0);
            constexpr int Segments=8;
            for(int k=0;k<Segments&&staging_.size()+3U<=LightCapacity;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/Segments,a1=2.0f*Pi*static_cast<float>(k+1)/Segments;
                staging_.emplace_back(at,core);
                staging_.emplace_back(at+(right*std::cos(a0)+up*std::sin(a0))*(size*layer[0]*scale),rim);
                staging_.emplace_back(at+(right*std::cos(a1)+up*std::sin(a1))*(size*layer[0]*scale),rim);
            }
        }
    };
    const auto point=[&](const float x,const float y,const float z) {return centre+f*x+Vector3::Up*y+s*z;};
    glow(point(22.9f,14.4f,0.0f),1.0f,0.97f,0.86f,1.0f);
    glow(point(-25.8f,16.6f,0.0f),1.0f,0.97f,0.86f,1.0f);
    // the sidelights each seen only from its own side, the stern light from astern
    const float side=Vector3::Dot(camera-centre,s);
    if(side<0.0f) glow(point(-19.0f,11.0f,-4.7f),1.0f,0.16f,0.12f,0.9f);
    else glow(point(-19.0f,11.0f,4.7f),0.2f,1.0f,0.35f,0.9f);
    if(Vector3::Dot(camera-centre,f)<0.0f) glow(point(-31.2f,5.0f,0.0f),1.0f,0.96f,0.84f,0.8f);
    for(int k=0;k<5;++k) glow(point(-18.9f,9.1f,-3.2f+1.6f*static_cast<float>(k)),1.0f,0.82f,0.5f,0.45f);
    lights_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    lightEffect_->setWorldProperty(Matrix::getIdentityProperty());
    lightEffect_->setViewProperty(view);lightEffect_->setProjectionProperty(projection);lightEffect_->Apply();
    device_.SetVertexBuffer(lights_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));++drawCalls_;
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}

}
