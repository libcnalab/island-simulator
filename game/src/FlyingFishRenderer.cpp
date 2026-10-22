#include "IslandSimulator/FlyingFishRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
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
constexpr int Sides=8;
// the body's stations: along it (metres, the snout forward) and its half depth and width there
constexpr std::array<float,7> Station{{0.142f,0.128f,0.100f,0.050f,-0.020f,-0.085f,-0.122f}};
constexpr std::array<float,7> Depth{{0.002f,0.011f,0.017f,0.020f,0.018f,0.011f,0.005f}};
constexpr std::array<float,7> Width{{0.002f,0.010f,0.016f,0.019f,0.016f,0.009f,0.004f}};
constexpr std::size_t TrianglesPerFish=(Station.size()-1U)*Sides*2U+2U*3U+2U+2U+2U;
constexpr std::size_t Capacity=FlyingFishShoal::MaximumFish*TrianglesPerFish*3U;
const Color Back(34,58,104),Flank(96,128,170),Belly(214,220,226),Fin(118,138,164),Tail(52,74,116);
}

FlyingFishRenderer::FlyingFishRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(220.0f);effect_->setFogEndProperty(520.0f);
    std::ostringstream message;
    message<<"island-simulator: now and then by day a few flying fish burst out of the sea off the shore, glide low over the waves "
             "on their spread fins and drop back in a little spray; "<<TrianglesPerFish<<" triangles a fish";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void FlyingFishRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                              const std::span<const FlyingFishState> fish) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    const BoundingFrustum frustum(view*projection);
    for(const FlyingFishState& one:fish) {
        if(!one.visible) continue;
        const Vector3 at(one.x,one.y,one.z);
        if((at-camera).LengthSquared()>DrawDistance*DrawDistance) continue;
        if(frustum.Contains(BoundingSphere(at,0.4f))==ContainmentType::Disjoint) continue;
        if(staging_.size()+TrianglesPerFish*3U>Capacity) break;
        ++visible_;
        const float cp=std::cos(one.pitchRadians);
        const Vector3 f(std::cos(one.headingRadians)*cp,std::sin(one.pitchRadians),std::sin(one.headingRadians)*cp);
        Vector3 s0=Vector3::Cross(f,Vector3::Up);
        if(s0.LengthSquared()<1e-6f) s0=Vector3::UnitX;
        s0.Normalize();
        const Vector3 u0=Vector3::Cross(s0,f);
        const float cb=std::cos(one.bankRadians),sb=std::sin(one.bankRadians);
        const Vector3 s=s0*cb+u0*sb,u=u0*cb-s0*sb;
        const float k=one.scale;
        const auto world=[&](const float x,const float y,const float z) {return at+(f*x+u*y+s*z)*k;};
        // outward-wound facets for the body, two-sided for the fins
        const auto body=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& axis,const Color& colour) {
            const Vector3 n=Vector3::Cross(b-a,c-a);
            const bool out=Vector3::Dot(n,(a+b+c)/3.0f-axis)>=0.0f;
            const Vector3& second=out?b:c;const Vector3& third=out?c:b;
            const Color lit=wildlife_render::FacetColor(a,second,third,colour);
            staging_.emplace_back(a,lit);staging_.emplace_back(second,lit);staging_.emplace_back(third,lit);
        };
        const auto fin=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour) {
            const Color lit=wildlife_render::FacetColor(a,b,c,colour,true);
            staging_.emplace_back(a,lit);staging_.emplace_back(b,lit);staging_.emplace_back(c,lit);
        };
        // the body: a slender spindle, dark above, silver below
        std::array<std::array<Vector3,Sides>,Station.size()> ring{};
        std::array<std::array<Color,Sides>,Station.size()> tone{};
        for(std::size_t i=0;i<Station.size();++i) for(int j=0;j<Sides;++j) {
            const float a=2.0f*Pi*static_cast<float>(j)/Sides;
            const float up=std::sin(a);
            ring[i][static_cast<std::size_t>(j)]=world(Station[i],up*Depth[i],std::cos(a)*Width[i]);
            tone[i][static_cast<std::size_t>(j)]=up>0.55f?Back:(up>-0.2f?Flank:Belly);
        }
        for(std::size_t i=0;i+1<Station.size();++i) {
            const Vector3 axis=world(0.5f*(Station[i]+Station[i+1]),0.0f,0.0f);
            for(int j=0;j<Sides;++j) {
                const auto j0=static_cast<std::size_t>(j),j1=static_cast<std::size_t>((j+1)%Sides);
                body(ring[i][j0],ring[i+1][j0],ring[i+1][j1],axis,tone[i][j0]);
                body(ring[i][j0],ring[i+1][j1],ring[i][j1],axis,tone[i][j0]);
            }
        }
        // the pectoral fins, high on the flanks behind the head: spread wide and swept back with
        // a little dihedral in the glide, folded flat along the body as it drops back in
        const float w=std::clamp(one.wings,0.0f,1.0f);
        for(const float side:{-1.0f,1.0f}) {
            const auto mix=[&](const Vector3& folded,const Vector3& spread) {
                const Vector3 p=folded+(spread-folded)*w;
                return world(p.X,p.Y,p.Z*side);
            };
            const Vector3 rootLead=world(0.088f,0.008f,0.016f*side),rootTrail=world(0.058f,0.004f,0.017f*side);
            const Vector3 tipLead=mix(Vector3(-0.060f,0.010f,0.024f),Vector3(-0.010f,0.024f,0.175f));
            const Vector3 tipTrail=mix(Vector3(-0.096f,0.008f,0.022f),Vector3(-0.090f,0.020f,0.150f));
            const Vector3 midTrail=mix(Vector3(-0.020f,0.006f,0.020f),Vector3(-0.040f,0.012f,0.060f));
            fin(rootLead,tipLead,tipTrail,Fin);fin(rootLead,tipTrail,midTrail,Fin);fin(rootLead,midTrail,rootTrail,Fin);
            // the pelvic pair, smaller, under and behind
            const Vector3 pelvicRoot=world(-0.030f,-0.012f,0.008f*side);
            fin(pelvicRoot,mix(Vector3(-0.085f,-0.012f,0.012f),Vector3(-0.060f,-0.010f,0.070f)),
                mix(Vector3(-0.095f,-0.012f,0.010f),Vector3(-0.090f,-0.010f,0.048f)),Fin);
        }
        // the tail: a deep fork, its lower lobe the longer (it sculls the water on a touch down)
        const Vector3 peduncleTop=world(-0.118f,0.005f,0.0f),peduncleBottom=world(-0.118f,-0.005f,0.0f),notch=world(-0.140f,0.0f,0.0f);
        fin(peduncleTop,world(-0.180f,0.040f,0.0f),notch,Tail);
        fin(peduncleBottom,notch,world(-0.205f,-0.052f,0.0f),Tail);
        // and a low dorsal fin far back
        fin(world(-0.040f,0.016f,0.0f),world(-0.095f,0.028f,0.0f),world(-0.100f,0.010f,0.0f),Tail);
        fin(world(-0.040f,0.016f,0.0f),world(-0.100f,0.010f,0.0f),world(-0.060f,0.017f,0.0f),Tail);
    }
    if(staging_.empty()) return;
    vertices_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));
    ++drawCalls_;
}

}
