#include "IslandSimulator/AlienRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
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
#include <cmath>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Capacity=16000;
// The grey skin (pigment past full: the build lights faces 0.46 + 0.54 n.s), the black eyes.
const Vector3 Skin(172,178,174),SkinDark(150,156,152),Eye(10,10,14),Glint(150,170,200),Mouth(80,84,84);

Vector3 V(const AlienPoint& p) {return {p.x,p.y,p.z};}
Vector3 Unit(Vector3 v,const Vector3& fallback=Vector3::Up) {
    if(v.LengthSquared()<1e-10f) return fallback;
    v.Normalize();return v;
}

// An ellipsoid: centre, its three axes and half extents.
void Ellipsoid(SculptMeshBuilder& b,const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& ahead,
               const float hw,const float hh,const float hd,const Vector3& pigment,const int ringCount=7,const int segments=10) {
    std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
    for(int i=0;i<ringCount;++i) {
        const float t=-0.96f+1.92f*static_cast<float>(i)/static_cast<float>(ringCount-1);
        const float w=std::sqrt(1.0f-t*t);
        std::vector<Vector3> ring;
        for(int k=0;k<segments;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(segments);
            ring.push_back(centre+up*(t*hh)+right*(std::cos(a)*hw*w)+ahead*(std::sin(a)*hd*w));
        }
        rings.push_back(ring);centres.push_back(centre+up*(t*hh));
    }
    b.Loft(rings,centres,[&](std::size_t,std::size_t) {return pigment;});
}
}

AlienRenderer::AlienRenderer(GraphicsDevice& device,const float maximumDistance)
    :device_(device),maximumDistance_(maximumDistance),effect_(std::make_unique<BasicEffect>(device)) {
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),Capacity,BufferUsage::WriteOnly);
    vertices_.reserve(Capacity);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);
    effect_->setFogEndProperty(760.0f);
}

AlienRenderer::~AlienRenderer()=default;

void AlienRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const AlienMotion& alien) {
    visible_=false;triangles_=0;
    const AlienState& s=alien.State();
    const Vector3 middle=V(s.pelvis);
    if(Vector3::DistanceSquared(middle,camera)>maximumDistance_*maximumDistance_) return;
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(middle,1.2f))) return;
    SculptMeshBuilder b;
    const Vector3 body=V(alien.ToWorld(0,0,1))-V(alien.ToWorld(0,0,0));   // horizontal ahead
    const Vector3 right=V(alien.ToWorld(1,0,0))-V(alien.ToWorld(0,0,0));
    const Vector3 pelvis=V(s.pelvis),chest=V(s.chest),neck=V(s.neck),head=V(s.head);
    const Vector3 hr=V(s.headRight),hu=V(s.headUp),hf=V(s.headAhead);
    // a faint mottling of the grey over the body
    b.tintPerVertex=true;
    b.tint=[](const Vector3& pigment,const Vector3&,const Vector3& p) {
        if(pigment.X<100.0f) return pigment;
        return pigment*(0.94f+0.08f*std::sin(p.X*23.0f+p.Y*17.0f)*std::sin(p.Z*19.0f-p.Y*11.0f));
    };

    // ---- the body: narrow hips, a slim waist, the chest, lofted along the spine
    {
        struct Station {float t,w,d;};
        static constexpr Station Sections[]{{0.0f,0.085f,0.060f},{0.35f,0.075f,0.052f},{0.7f,0.105f,0.062f},{1.0f,0.100f,0.058f}};
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        const Vector3 spine=chest-pelvis;
        for(const Station& st:Sections) {
            const Vector3 c=pelvis+spine*st.t;
            std::vector<Vector3> ring;
            for(int k=0;k<12;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/12.0f;
                ring.push_back(c+right*(std::cos(a)*st.w)+body*(std::sin(a)*st.d));
            }
            rings.push_back(ring);centres.push_back(c);
        }
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return Skin;});
    }
    // ---- the slender neck, the legs, the arms
    b.Tube({chest,neck,head-hu*0.17f},{0.05f,0.032f,0.04f},8,SkinDark);
    for(const AlienLeg& leg:s.legs) {
        b.Tube({V(leg.hip),V(leg.knee),V(leg.foot)+Vector3(0,0.035f,0)},{0.052f,0.04f,0.028f},7,Skin);
        const Vector3 foot=V(leg.foot);
        const Vector3 shin=Unit(V(leg.knee)-V(leg.foot));
        const Vector3 along=Unit(body-shin*Vector3::Dot(body,shin));
        b.Box(foot+along*0.04f+Vector3(0,0.022f,0),right,Vector3::Up,along,0.036f,0.02f,0.10f,SkinDark,SkinDark);
    }
    for(const AlienArm& arm:s.arms) {
        const Vector3 shoulder=V(arm.shoulder),elbow=V(arm.elbow),hand=V(arm.hand);
        b.Tube({shoulder,elbow,hand},{0.036f,0.027f,0.019f},7,Skin);
        // the long three-fingered hand and its thumb, spread along the forearm's line
        const Vector3 dir=Unit(hand-elbow);
        Vector3 across=Unit(Vector3::Cross(dir,Vector3::Up),right);
        const Vector3 palm=Unit(Vector3::Cross(across,dir));
        for(int f=0;f<3;++f) {
            const float spread=(static_cast<float>(f)-1.0f)*0.3f;
            const Vector3 tip=hand+Unit(dir+across*spread-palm*0.08f)*0.12f;
            b.Tube({hand,(hand+tip)*0.5f+palm*-0.01f,tip},{0.011f,0.008f,0.005f},5,SkinDark);
        }
        b.Tube({hand,hand+Unit(dir*0.4f+across*0.8f+palm*0.2f)*0.06f},{0.011f,0.006f},5,SkinDark);
    }
    // ---- the head: an egg widest above the eyes, tapering to a small chin
    {
        constexpr int Rings=10,Segments=16;
        constexpr float HalfHeight=0.205f,HalfWidth=0.155f,HalfDepth=0.135f;
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(int i=0;i<Rings;++i) {
            const float t=-0.95f+1.9f*static_cast<float>(i)/static_cast<float>(Rings-1);
            const float round=std::sqrt(1.0f-t*t);
            // widest a little above the middle, a small chin
            const float taper=0.55f+0.45f*std::clamp((t+1.0f)*0.62f,0.0f,1.0f);
            const Vector3 c=head+hu*(t*HalfHeight);
            std::vector<Vector3> ring;
            for(int k=0;k<Segments;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Segments;
                ring.push_back(c+hr*(std::cos(a)*HalfWidth*round*taper)+hf*(std::sin(a)*HalfDepth*round*(0.8f+0.2f*taper)));
            }
            rings.push_back(ring);centres.push_back(c);
        }
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return Skin;});
        // the eyes: big slanted ovals on the face, black, each with a glint
        for(const float side:{-1.0f,1.0f}) {
            const float slant=0.5f;
            const Vector3 long_=Unit(hr*std::cos(slant)-hu*(std::sin(slant)*side));
            const Vector3 tall=Unit(Vector3::Cross(hf,long_));
            const Vector3 at=head+hr*(side*0.068f)+hu*0.01f+hf*(HalfDepth*0.84f);
            Ellipsoid(b,at,long_,tall,hf,0.075f,0.040f,0.022f,Eye,6,12);
            b.Box(at+long_*(side*0.0f)+tall*0.016f+hf*0.02f+hr*(side*0.02f),hr,hu,hf,0.008f,0.006f,0.003f,Glint,Glint);
        }
        // nostril dots and the thin mouth
        for(const float side:{-1.0f,1.0f})
            b.Box(head-hu*0.08f+hr*(side*0.012f)+hf*(HalfDepth*0.80f),hr,hu,hf,0.004f,0.005f,0.004f,Eye,Eye);
        b.Box(head-hu*0.145f+hf*(HalfDepth*0.62f),hr,hu,hf,0.032f,0.003f,0.004f,Mouth,Mouth);
    }
    b.tint=nullptr;
    vertices_.assign(b.builder.mesh.begin(),b.builder.mesh.end());
    triangles_=static_cast<int>(vertices_.size()/3);
    if(vertices_.empty()||vertices_.size()>static_cast<std::size_t>(Capacity)) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(vertices_.data(),0,static_cast<int>(vertices_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    visible_=true;
}
}
