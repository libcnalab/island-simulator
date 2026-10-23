#include "IslandSimulator/LandCrabRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// a red crab's carapace is some 11 cm across
constexpr float CrabWidth=.115f;
using TV=LandCrabRenderer::TemplateVertex;

struct Builder {
    std::vector<TV>& out;
    // lit once, from the face's own normal, by the fixed sun: the template is built once
    void Tri(const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour,const std::int8_t leg=-1,
             const float ra=0,const float rb=0,const float rc=0) {
        const Color lit=wildlife_render::FacetColor(a,b,c,colour,true);
        out.push_back({a.X,a.Y,a.Z,lit.getRProperty(),lit.getGProperty(),lit.getBProperty(),leg,ra});
        out.push_back({b.X,b.Y,b.Z,lit.getRProperty(),lit.getGProperty(),lit.getBProperty(),leg,rb});
        out.push_back({c.X,c.Y,c.Z,lit.getRProperty(),lit.getGProperty(),lit.getBProperty(),leg,rc});
    }
    // a dome: rings from the rim up to the crown, the rim's outline wider than long
    void Dome(const Vector3& centre,const float halfX,const float halfZ,const float height,const int segments,
              const int rings,const Color& top,const Color& rim) {
        for(int r=0;r<rings;++r) {
            const float v0=static_cast<float>(r)/rings,v1=static_cast<float>(r+1)/rings;
            const auto ring=[&](const float v,const float a) {
                const float spread=std::cos(v*Pi*.5f);
                // the front edge straight between the eyes, the back rounded
                const float front=std::sin(a)>0.0f?1.0f-.25f*std::pow(std::sin(a),4.0f):1.0f;
                return centre+Vector3(std::cos(a)*halfX*spread,height*std::sin(v*Pi*.5f),std::sin(a)*halfZ*spread*front);
            };
            for(int k=0;k<segments;++k) {
                const float a0=2.0f*Pi*k/segments,a1=2.0f*Pi*(k+1)/segments;
                const Color c=Color::Lerp(rim,top,v1);
                Tri(ring(v0,a0),ring(v1,a0),ring(v1,a1),c);
                if(r+1<rings) Tri(ring(v0,a0),ring(v1,a1),ring(v0,a1),c);
            }
        }
        // the flat underside
        for(int k=0;k<segments;++k) {
            const float a0=2.0f*Pi*k/segments,a1=2.0f*Pi*(k+1)/segments;
            Tri(centre,centre+Vector3(std::cos(a1)*halfX,0,std::sin(a1)*halfZ),
                centre+Vector3(std::cos(a0)*halfX,0,std::sin(a0)*halfZ),Color::Lerp(rim,Color(40,20,16),.4f));
        }
    }
    // a limb segment: a thin three-sided prism from a to b
    void Segment(const Vector3& a,const Vector3& b,const float ra,const float rb,const Color& colour,const std::int8_t leg,
                 const float reachA,const float reachB) {
        Vector3 axis=b-a;
        axis.Normalize();
        Vector3 side=Vector3::Cross(axis,Vector3::Up);
        if(side.LengthSquared()<1e-6f) side=Vector3::UnitX;
        side.Normalize();
        const Vector3 other=Vector3::Cross(side,axis);
        for(int k=0;k<3;++k) {
            const float a0=2.0f*Pi*k/3.0f+.5f,a1=2.0f*Pi*(k+1)/3.0f+.5f;
            const Vector3 d0=side*std::cos(a0)+other*std::sin(a0),d1=side*std::cos(a1)+other*std::sin(a1);
            Tri(a+d0*ra,b+d0*rb,b+d1*rb,colour,leg,reachA,reachB,reachB);
            Tri(a+d0*ra,b+d1*rb,a+d1*ra,colour,leg,reachA,reachB,reachA);
        }
    }
};

// One crab a unit across in its own frame: x to its right, y up, z ahead.
void BuildCrab(std::vector<TV>& out,const bool near) {
    Builder b{out};
    const Color shell(196,38,26),shellTop(214,58,34),leg(176,44,28),claw(206,64,40),eye(22,18,16);
    const float bodyY=.30f;
    b.Dome(Vector3(0,bodyY,0),.50f,.40f,.26f,near?14:7,near?3:1,shellTop,shell);
    // four walking legs a side: out from the carapace's edge, up to the knee, down to the tip
    for(int side=-1;side<=1;side+=2) {
        for(int k=0;k<4;++k) {
            const auto index=static_cast<std::int8_t>((side<0?0:4)+k);
            const float along=.22f-static_cast<float>(k)*.15f;
            const float fan=(static_cast<float>(k)-1.5f)*.30f;
            const Vector3 hip(side*.44f,bodyY+.02f,along);
            const Vector3 knee(side*(.82f+.04f*std::cos(fan)),bodyY+.22f,along+std::sin(fan)*.30f);
            const Vector3 tip(side*(1.06f+.06f*std::cos(fan)),0.0f,along+std::sin(fan)*.62f);
            if(near) {
                b.Segment(hip,knee,.055f,.045f,leg,index,0.0f,.5f);
                b.Segment(knee,tip,.045f,.015f,leg,index,.5f,1.0f);
            } else {
                b.Tri(hip,knee,tip,leg,index,0.0f,.5f,1.0f);
                b.Tri(tip,knee,hip,leg,index,1.0f,.5f,0.0f);
            }
        }
        // the claws held up and in before the face
        const Vector3 shoulder(side*.30f,bodyY+.06f,.30f),wrist(side*.36f,bodyY+.12f,.56f),tip(side*.16f,bodyY+.10f,.66f);
        if(near) {
            b.Segment(shoulder,wrist,.07f,.07f,claw,-1,0,0);
            b.Segment(wrist,tip,.09f,.03f,claw,-1,0,0);
            const Vector3 stalk(side*.12f,bodyY+.24f,.40f),ball(side*.14f,bodyY+.36f,.43f);
            b.Segment(stalk,ball,.025f,.025f,shell,-1,0,0);
            b.Segment(ball,ball+Vector3(0,.05f,0),.055f,.01f,eye,-1,0,0);
        } else {
            b.Tri(shoulder,wrist,tip,claw);
            b.Tri(tip,wrist,shoulder,claw);
        }
    }
}
}

LandCrabRenderer::LandCrabRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)) {
    BuildCrab(near_,true);
    BuildCrab(far_,false);
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(near_.size()*MaximumCrabs),BufferUsage::WriteOnly);
    staging_.reserve(near_.size()*MaximumCrabs);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(250.0f);
    effect_->setFogEndProperty(660.0f);
    std::cout<<"island-simulator: migrating red land crabs drawn from a "<<near_.size()/3<<"-triangle near and a "
             <<far_.size()/3<<"-triangle far crab, legs stepping in a wave\n";
}

LandCrabRenderer::~LandCrabRenderer()=default;

void LandCrabRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                            const std::span<const MigratingCrab> crabs) {
    staging_.clear();
    visible_=0;draws_=0;
    if(crabs.empty()) return;
    const BoundingFrustum frustum(view*projection);
    static constexpr std::array<std::array<float,3>,3> Tones{{{1.0f,1.0f,1.0f},{1.08f,1.18f,.95f},{.78f,.70f,.78f}}};
    for(const MigratingCrab& crab:crabs) {
        if(static_cast<std::size_t>(visible_)>=MaximumCrabs) break;
        if(!(crab.scale>.02f)) continue;
        const Vector3 at(crab.x,crab.y,crab.z);
        const float size=CrabWidth*crab.scale;
        if(!frustum.Intersects(BoundingSphere(at+Vector3(0,size*.3f,0),size*1.3f))) continue;
        const bool near=Vector3::DistanceSquared(at,camera)<NearDistance*NearDistance;
        const auto& source=near?near_:far_;
        const float c=std::cos(crab.heading),s=std::sin(crab.heading);
        const auto& tone=Tones[crab.tone%Tones.size()];
        for(const TV& v:source) {
            float y=v.y,x=v.x,z=v.z;
            if(v.leg>=0) {
                // the legs step in a wave from front to back, each tip lifting and swinging
                const float phase=crab.stride+static_cast<float>(v.leg%4)*1.57f+(v.leg<4?0.0f:Pi);
                y+=std::max(0.0f,std::sin(phase))*.16f*v.reach;
                z+=std::cos(phase)*.10f*v.reach;
            }
            // the crab's frame: +z along its heading
            const Vector3 p=at+Vector3((x*s+z*c)*size,y*size,(-x*c+z*s)*size);
            staging_.emplace_back(p,Color(static_cast<int>(std::min(255.0f,v.r*tone[0])),static_cast<int>(std::min(255.0f,v.g*tone[1])),
                                          static_cast<int>(std::min(255.0f,v.b*tone[2]))));
        }
        ++visible_;
    }
    if(staging_.empty()) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));
    ++draws_;
}

namespace {
// an alate: some 12 mm of body and wings half as long again
constexpr float AlateLength=.026f;   // (with the wings reaching past the abdomen)
constexpr int AlateVertices=36;   // the body's two triangles and the four wings, each two-faced
}

AlateRenderer::AlateRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)) {
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(MaximumAlates*AlateVertices),BufferUsage::WriteOnly);
    staging_.reserve(MaximumAlates*AlateVertices);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(250.0f);
    effect_->setFogEndProperty(660.0f);
}

AlateRenderer::~AlateRenderer()=default;

void AlateRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const std::span<const Alate> alates) {
    staging_.clear();
    visible_=0;
    if(alates.empty()) return;
    const BoundingFrustum frustum(view*projection);
    // the wings milky and glinting as they beat
    const Color body(58,40,26),wingLit(236,232,218),wingDim(176,170,154);
    const auto tri=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour) {
        staging_.emplace_back(a,colour);staging_.emplace_back(b,colour);staging_.emplace_back(c,colour);
        staging_.emplace_back(c,colour);staging_.emplace_back(b,colour);staging_.emplace_back(a,colour);
    };
    for(const Alate& alate:alates) {
        if(static_cast<std::size_t>(visible_)>=MaximumAlates) break;
        if(!(alate.scale>.02f)) continue;
        const Vector3 at(alate.x,alate.y,alate.z);
        // past some 18 m an alate is under a pixel; the swarm there is carried by the near ones
        if(Vector3::DistanceSquared(at,camera)>18.0f*18.0f) continue;
        if(!frustum.Intersects(BoundingSphere(at,AlateLength*1.5f))) continue;
        const float size=AlateLength*alate.scale;
        const Vector3 forward(std::cos(alate.heading),0,std::sin(alate.heading));
        const Vector3 right(-forward.Z,0,forward.X);
        const Vector3 head=at+forward*(size*.45f),tail=at-forward*(size*.45f);
        tri(head,at+right*(size*.10f),tail,body);
        tri(head,tail,at-right*(size*.10f),body);
        // the four wings, longer than the body, beating: up and down about the thorax
        const float beat=std::sin(alate.flap);
        for(const float side:{-1.0f,1.0f}) {
            for(const float pair:{0.0f,1.0f}) {
                const Vector3 root=at+forward*(size*(.15f-.12f*pair));
                const Vector3 out=right*(side*size*.55f)+Vector3::Up*(size*.45f*beat*(1.0f-.3f*pair));
                const Vector3 tip=root+out-forward*(size*(.95f+.10f*pair));
                tri(root,root+out*.25f-forward*(size*.05f),tip,beat>0.0f?wingLit:wingDim);
            }
        }
        ++visible_;
    }
    if(staging_.empty()) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));
}

}
