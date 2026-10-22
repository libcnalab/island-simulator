#include "IslandSimulator/GlowingFungusRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numbers>
#include <sstream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Segments=8;
constexpr int VerticesPerCap=Segments*3,VerticesPerHalo=Segments*3;
// the pigments, past full for the build's light: pale buff caps, paler gills and stalks
const Vector3 CapColour(198,194,148),Gills(214,210,182),Stalk(206,200,168);
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

GlowingFungusRenderer::GlowingFungusRenderer(GraphicsDevice& device,const std::span<const FungusCluster> clusters)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),glowEffect_(std::make_unique<BasicEffect>(device)),
     clusters_(clusters.begin(),clusters.end())
{
    SculptMeshBuilder b;
    for(std::size_t c=0;c<clusters_.size();++c) {
        const FungusCluster& cluster=clusters_[c];
        const Vector3 at(cluster.at[0],cluster.at[1],cluster.at[2]);
        Vector3 n(cluster.normal[0],cluster.normal[1],cluster.normal[2]);
        if(n.LengthSquared()<1e-6f) n=Vector3::Up;
        n.Normalize();
        Vector3 t1=Vector3::Cross(n,Vector3::Up);
        if(t1.LengthSquared()<1e-6f) t1=Vector3::UnitX;
        t1.Normalize();
        const Vector3 t2=Vector3::Cross(t1,n);
        const std::size_t first=caps_.size();
        for(int m=0;m<cluster.count;++m) {
            const auto s=cluster.seed*0x9e3779b9U^static_cast<std::uint32_t>(m*0x61c8U+7U);
            // crowded toward the clump's middle, a little lower than it (they fruit from cracks)
            const float r=cluster.spread*std::sqrt(Hash(s)),a=2.0f*Pi*Hash(s^0x1U);
            const Vector3 base=at+t1*(std::cos(a)*r)+t2*(std::sin(a)*r*0.6f)-n*0.012f;
            const Vector3 dir=Vector3::Normalize(n*0.4f+Vector3::Up*0.9f+t1*(0.4f*(Hash(s^0x2U)-0.5f))+t2*(0.3f*(Hash(s^0x3U)-0.5f)));
            const float stalk=0.022f+0.03f*Hash(s^0x4U),cap=0.008f+0.009f*Hash(s^0x5U);
            const Vector3 top=base+dir*stalk;
            b.Tube({base,base+dir*(stalk*0.5f)+t1*(0.004f*(Hash(s^0x6U)-0.5f)),top},{0.0035f,0.003f,0.0026f},4,Stalk);
            // the cap: a shallow bell over a ring of gills
            Vector3 u=Vector3::Cross(dir,t1);
            if(u.LengthSquared()<1e-6f) u=t2;
            u.Normalize();
            const Vector3 v=Vector3::Cross(dir,u);
            const Vector3 apex=top+dir*(cap*0.6f),rimCentre=top-dir*(cap*0.12f);
            for(int k=0;k<7;++k) {
                const float q0=2.0f*Pi*static_cast<float>(k)/7.0f,q1=2.0f*Pi*static_cast<float>(k+1)/7.0f;
                const Vector3 r0=rimCentre+(u*std::cos(q0)+v*std::sin(q0))*cap,r1=rimCentre+(u*std::cos(q1)+v*std::sin(q1))*cap;
                const Vector3 m0=top+dir*(cap*0.35f)+(u*std::cos(q0)+v*std::sin(q0))*(cap*0.72f);
                const Vector3 m1=top+dir*(cap*0.35f)+(u*std::cos(q1)+v*std::sin(q1))*(cap*0.72f);
                b.inside=top;
                b.Face(apex,m0,m1,CapColour*1.04f);
                b.Quad(m0,r0,r1,m1,CapColour);
                b.inside=apex+dir*0.05f;
                b.Face(top,r1,r0,Gills);
            }
            caps_.push_back({top+dir*(cap*0.25f),cap,c});
        }
        Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
        for(std::size_t i=first;i<caps_.size();++i) {low=Vector3::Min(low,caps_[i].centre);high=Vector3::Max(high,caps_[i].centre);}
        clusterBounds_.emplace_back(low-Vector3(0.3f,0.3f,0.3f),high+Vector3(0.3f,0.3f,0.3f));
    }
    triangles_=static_cast<int>(b.builder.mesh.size()/3U);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:b.builder.mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.5f,0.5f,0.5f),high+Vector3(0.5f,0.5f,0.5f));
    if(!b.builder.mesh.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(b.builder.mesh.size()),BufferUsage::WriteOnly);
        vertices_->SetData(b.builder.mesh.data(),static_cast<int>(b.builder.mesh.size()));
    }
    const std::size_t capacity=caps_.size()*VerticesPerCap+clusters_.size()*VerticesPerHalo;
    if(capacity>0U) glow_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(capacity),BufferUsage::WriteOnly);
    staging_.reserve(capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(220.0f);effect_->setFogEndProperty(520.0f);
    glowEffect_->setVertexColorEnabledProperty(true);glowEffect_->setLightingEnabledProperty(false);glowEffect_->setFogEnabledProperty(false);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"glowing-fungi",false,220.0f,520.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: glowing fungus sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: "<<clusters_.size()<<" clumps of foxfire mushrooms ("<<caps_.size()
           <<" caps) on the dead wood glow yellow-green on dark nights; "<<triangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void GlowingFungusRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!vertices_) return;
    const BoundingFrustum frustum(view*projection);
    if(frustum.Contains(bounds_)==ContainmentType::Disjoint) return;
    // only when a clump is near enough to make out
    bool near=false;
    for(const BoundingBox& box:clusterBounds_) {
        const Vector3 nearest(std::clamp(camera.X,box.Min.X,box.Max.X),std::clamp(camera.Y,box.Min.Y,box.Max.Y),std::clamp(camera.Z,box.Min.Z,box.Max.Z));
        near=near||(nearest-camera).LengthSquared()<DrawDistance*DrawDistance;
    }
    if(!near) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    ++drawCalls_;
}

void GlowingFungusRenderer::DrawGlow(const Matrix& view,const Matrix& projection,const Vector3& camera,const float dark) {
    glowing_=0;
    if(!glow_||!(dark>0.02f)) return;
    staging_.clear();
    const BoundingFrustum frustum(view*projection);
    const auto disc=[&](const Vector3& centre,const Vector3& right,const Vector3& up,const float radius,const Color& middle) {
        const Color edge(0,0,0,0);
        for(int i=0;i<Segments;++i) {
            const float a0=2.0f*Pi*static_cast<float>(i)/Segments,a1=2.0f*Pi*static_cast<float>(i+1)/Segments;
            staging_.emplace_back(centre,middle);
            staging_.emplace_back(centre+(right*std::cos(a0)+up*std::sin(a0))*radius,edge);
            staging_.emplace_back(centre+(right*std::cos(a1)+up*std::sin(a1))*radius,edge);
        }
    };
    const auto facing=[&](const Vector3& at,Vector3& toCamera,Vector3& right,Vector3& up,float& distance) {
        toCamera=camera-at;distance=toCamera.Length();
        if(distance<0.05f) return false;
        toCamera/=distance;
        right=Vector3::Cross(Vector3::Up,toCamera);
        if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
        right.Normalize();
        up=Vector3::Cross(toCamera,right);
        return true;
    };
    const auto channel=[](const float v) {return static_cast<int>(std::lround(std::clamp(v,0.0f,1.0f)*255.0f));};
    std::vector<float> clusterFade(clusters_.size(),0.0f);
    for(std::size_t c=0;c<clusters_.size();++c) {
        if(frustum.Contains(clusterBounds_[c])==ContainmentType::Disjoint) continue;
        const Vector3 centre=(clusterBounds_[c].Min+clusterBounds_[c].Max)*0.5f;
        const float distance=(centre-camera).Length();
        if(distance>GlowDistance) continue;
        // each clump a little brighter or dimmer than the next
        clusterFade[c]=dark*(0.75f+0.35f*Hash(clusters_[c].seed^0xf0U))*(1.0f-std::clamp((distance-30.0f)/(GlowDistance-30.0f),0.0f,1.0f));
        Vector3 toCamera,right,up;float d=0;
        if(!facing(centre,toCamera,right,up,d)) continue;
        // the faint halo the clump throws over the wood round it
        const float k=clusterFade[c];
        disc(centre+toCamera*0.05f,right,up,clusters_[c].spread+0.24f,Color(channel(0.07f*k),channel(0.18f*k),channel(0.05f*k),255));
    }
    for(const Cap& cap:caps_) {
        const float k=clusterFade[cap.cluster];
        if(!(k>0.01f)) continue;
        Vector3 toCamera,right,up;float distance=0;
        if(!facing(cap.centre,toCamera,right,up,distance)) continue;
        // a cap stays a point of light far off, a soft glow up close
        const float radius=std::max(cap.radius*2.2f,distance*0.0012f);
        disc(cap.centre+toCamera*(cap.radius*1.3f),right,up,radius,Color(channel(0.42f*k),channel(0.95f*k),channel(0.36f*k),255));
        ++glowing_;
    }
    if(staging_.empty()) return;
    glow_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(glow_.get());device_.setIndicesProperty(nullptr);
    glowEffect_->setWorldProperty(Matrix::getIdentityProperty());
    glowEffect_->setViewProperty(view);glowEffect_->setProjectionProperty(projection);glowEffect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}
}
