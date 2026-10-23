#include "IslandSimulator/TRexRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
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
constexpr int Capacity=60000;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Noise3(const Vector3& p) {
    // a cheap lattice noise for the scales' mottling
    const float x=std::floor(p.X),y=std::floor(p.Y),z=std::floor(p.Z);
    return Hash(static_cast<std::uint32_t>(static_cast<std::int32_t>(x)*73856093^static_cast<std::int32_t>(y)*19349663^static_cast<std::int32_t>(z)*83492791));
}
Vector3 V(const TRexPoint& p) {return {p.x,p.y,p.z};}
Vector3 Unit(Vector3 v,const Vector3& fallback=Vector3::Up) {
    if(v.LengthSquared()<1e-10f) return fallback;
    v.Normalize();return v;
}

// The hide's colour at a point with its outward normal: a dark olive-brown back with faint bands
// across it, tawny flanks, a pale buff belly, all broken by the scales' mottling.
struct Hide {
    Vector3 back{60,50,38},flank{126,106,78},belly{188,172,140};
    Color At(const Vector3& p,const Vector3& n,const float bandAlong) const {
        const float upness=n.Y;
        Vector3 c=upness>0.0f?Vector3::Lerp(flank,back,std::clamp(upness*1.6f,0.0f,1.0f))
                             :Vector3::Lerp(flank,belly,std::clamp(-upness*1.8f,0.0f,1.0f));
        // the bands, over the back only
        const float band=.5f+.5f*std::sin(bandAlong*2.4f);
        c*=1.0f-.26f*band*std::clamp(upness*2.0f+.3f,0.0f,1.0f);
        const float mottle=.80f+.26f*Noise3(p*7.0f)+.14f*Noise3(p*2.1f);
        c*=mottle;
        const float light=.56f+.40f*std::max(0.0f,Vector3::Dot(n,wildlife_render::FixedSunDirection))+.10f*std::max(0.0f,n.Y);
        c*=light;
        return Color(static_cast<int>(std::clamp(c.X,0.0f,255.0f)),static_cast<int>(std::clamp(c.Y,0.0f,255.0f)),
                     static_cast<int>(std::clamp(c.Z,0.0f,255.0f)));
    }
};

class Builder {
public:
    explicit Builder(std::vector<VertexPositionColor>& out):out_(out){}
    // A loft through cross sections: centre, the section's right and up axes, half width and
    // height; `along` for the bands. Ends closed.
    void Loft(const std::vector<Vector3>& centres,const std::vector<Vector3>& rights,const std::vector<Vector3>& ups,
              const std::vector<float>& widths,const std::vector<float>& heights,const std::vector<float>& along,
              const int sides,const Hide& hide,const bool capEnds=true) {
        const std::size_t n=centres.size();
        const auto point=[&](const std::size_t i,const int k) {
            const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);
            return centres[i]+rights[i]*(std::cos(a)*widths[i])+ups[i]*(std::sin(a)*heights[i]);
        };
        const auto normal=[&](const std::size_t i,const int k) {
            const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);
            return Unit(rights[i]*(std::cos(a)/std::max(widths[i],1e-3f))+ups[i]*(std::sin(a)/std::max(heights[i],1e-3f)));
        };
        for(std::size_t i=0;i+1<n;++i) {
            for(int k=0;k<sides;++k) {
                const Vector3 a=point(i,k),b=point(i,k+1),c=point(i+1,k+1),d=point(i+1,k);
                const Color ca=hide.At(a,normal(i,k),along[i]),cb=hide.At(b,normal(i,k+1),along[i]);
                const Color cc=hide.At(c,normal(i+1,k+1),along[i+1]),cd=hide.At(d,normal(i+1,k),along[i+1]);
                out_.emplace_back(a,ca);out_.emplace_back(b,cb);out_.emplace_back(c,cc);
                out_.emplace_back(a,ca);out_.emplace_back(c,cc);out_.emplace_back(d,cd);
            }
        }
        if(!capEnds) return;
        for(const std::size_t i:{std::size_t{0},n-1}) {
            const Vector3 tip=centres[i]+(i==0?centres[0]-centres[1]:centres[n-1]-centres[n-2])*.15f;
            for(int k=0;k<sides;++k) {
                const Color c=hide.At(tip,Unit(tip-centres[i]),along[i]);
                out_.emplace_back(point(i,k),c);out_.emplace_back(point(i,k+1),c);out_.emplace_back(tip,c);
            }
        }
    }
    // A tapering limb from a to b, its section an ellipse flattened by `flat` sideways.
    void Limb(const Vector3& a,const Vector3& b,const float ra,const float rb,const Vector3& sideHint,const int sides,
              const Hide& hide,const float flat=1.0f) {
        const Vector3 axis=Unit(b-a);
        Vector3 side=Unit(Vector3::Cross(axis,sideHint),Vector3::UnitX);
        const Vector3 other=Unit(Vector3::Cross(side,axis));
        Loft({a,b},{side,side},{other,other},{ra*flat,rb*flat},{ra,rb},{0.0f,0.0f},sides,hide,false);
    }
    void Ellipsoid(const Vector3& c,const Vector3& right,const Vector3& up,const Vector3& forward,const float rx,const float ry,
                   const float rz,const int sides,const Hide& hide) {
        std::vector<Vector3> centres,rights,ups;std::vector<float> w,h,al;
        constexpr int Rings=7;
        for(int i=0;i<=Rings;++i) {
            const float t=-1.0f+2.0f*static_cast<float>(i)/Rings;
            const float s=std::sqrt(std::max(0.02f,1.0f-t*t));
            centres.push_back(c+forward*(t*rz));rights.push_back(right);ups.push_back(up);
            w.push_back(rx*s);h.push_back(ry*s);al.push_back(0.0f);
        }
        Loft(centres,rights,ups,w,h,al,sides,hide,false);
    }
    void Cone(const Vector3& base,const Vector3& tip,const float radius,const Color& colour,const int sides=5) {
        const Vector3 axis=Unit(tip-base);
        const Vector3 side=Unit(Vector3::Cross(axis,Vector3::Up),Vector3::UnitX),other=Unit(Vector3::Cross(side,axis));
        for(int k=0;k<sides;++k) {
            const float a0=2.0f*Pi*k/sides,a1=2.0f*Pi*(k+1)/sides;
            const Vector3 p0=base+(side*std::cos(a0)+other*std::sin(a0))*radius,p1=base+(side*std::cos(a1)+other*std::sin(a1))*radius;
            const Color lit=wildlife_render::FacetColor(p0,p1,tip,colour,true);
            out_.emplace_back(p0,lit);out_.emplace_back(p1,lit);out_.emplace_back(tip,lit);
        }
    }
    void Disc(const Vector3& centre,const Vector3& a,const Vector3& b,const float ra,const float rb,const Color& colour,const int sides=10) {
        for(int k=0;k<sides;++k) {
            const float a0=2.0f*Pi*k/sides,a1=2.0f*Pi*(k+1)/sides;
            out_.emplace_back(centre,colour);
            out_.emplace_back(centre+a*(std::cos(a0)*ra)+b*(std::sin(a0)*rb),colour);
            out_.emplace_back(centre+a*(std::cos(a1)*ra)+b*(std::sin(a1)*rb),colour);
        }
    }
private:
    std::vector<VertexPositionColor>& out_;
};
}

TRexRenderer::TRexRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground,
                           const float maximumDistance)
    :device_(device),terrain_(terrain),ground_(ground),maximumDistance_(maximumDistance),effect_(std::make_unique<BasicEffect>(device)) {
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),Capacity,BufferUsage::WriteOnly);
    shadowBuffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),Capacity,BufferUsage::WriteOnly);
    vertices_.reserve(Capacity);shadow_.reserve(Capacity);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);
    effect_->setFogEndProperty(760.0f);
}

TRexRenderer::~TRexRenderer()=default;

void TRexRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const TRexMotion& rex) {
    visible_=false;triangles_=0;
    const TRexState& s=rex.State();
    const Vector3 hips=V(rex.ToWorld(0,0,0));
    if(Vector3::DistanceSquared(hips,camera)>maximumDistance_*maximumDistance_) return;
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(hips+Vector3(0,-.5f,0),8.0f))) return;
    vertices_.clear();
    Builder b(vertices_);
    const Hide hide;
    const Vector3 origin=hips;
    const Vector3 right=V(rex.ToWorld(1,0,0))-origin,up=V(rex.ToWorld(0,1,0))-origin,ahead=V(rex.ToWorld(0,0,1))-origin;
    const auto local=[&](const float x,const float y,const float z) {return origin+right*x+up*y+ahead*z;};
    // ---- the body along its spine: tail tip to the neck's top
    struct Station {float z,y,w,h;};
    // (a short thick neck in an S, a deep chest, the tail tall and narrow behind the hips)
    static constexpr std::array<Station,16> Spine{{
        {-6.6f,-.86f,.05f,.07f},{-6.0f,-.80f,.10f,.15f},{-5.2f,-.68f,.16f,.27f},{-4.3f,-.52f,.24f,.40f},
        {-3.4f,-.36f,.32f,.52f},{-2.5f,-.18f,.42f,.62f},{-1.6f,.02f,.54f,.70f},{-.8f,.14f,.63f,.74f},
        {0.0f,.16f,.68f,.78f},{.8f,.08f,.76f,.92f},{1.6f,.04f,.74f,.92f},{2.3f,.14f,.62f,.76f},
        {2.8f,.40f,.52f,.62f},{3.15f,.66f,.48f,.56f},{3.45f,.84f,.45f,.52f},{3.7f,.92f,.42f,.48f}}};
    std::vector<Vector3> centres,rights,ups;std::vector<float> widths,heights,along;
    // each span of the spine divided in three, smoothly (Catmull-Rom), for the hide's detail
    std::vector<Station> spine;
    for(std::size_t i=0;i+1<Spine.size();++i) {
        const Station& p0=Spine[i==0?0:i-1];const Station& p1=Spine[i];const Station& p2=Spine[i+1];
        const Station& p3=Spine[std::min(i+2,Spine.size()-1)];
        for(int k=0;k<3;++k) {
            const float t=static_cast<float>(k)/3.0f,t2=t*t,t3=t2*t;
            const auto cr=[&](const float a,const float bb,const float c,const float d) {
                return .5f*((2.0f*bb)+(-a+c)*t+(2.0f*a-5.0f*bb+4.0f*c-d)*t2+(-a+3.0f*bb-3.0f*c+d)*t3);
            };
            spine.push_back({cr(p0.z,p1.z,p2.z,p3.z),cr(p0.y,p1.y,p2.y,p3.y),cr(p0.w,p1.w,p2.w,p3.w),cr(p0.h,p1.h,p2.h,p3.h)});
        }
    }
    spine.push_back(Spine.back());
    for(const Station& st:spine) {
        // the tail swings, more toward its tip
        const float behind=std::max(0.0f,-st.z-.8f);
        const float sway=s.tailSway*std::pow(behind,1.35f)*.30f;
        centres.push_back(local(sway,st.y,st.z));
        // each section square to the spine's run
        rights.push_back(right);ups.push_back(up);
        widths.push_back(st.w);heights.push_back(st.h);along.push_back(st.z);
    }
    b.Loft(centres,rights,ups,widths,heights,along,28,hide);
    // ---- the head on the neck, turned and raised
    const Vector3 neckTop=local(0,.92f,3.7f);
    // turned by the neck's yaw about the body's up, then raised by its pitch about the head's right
    const float yaw=s.neckYaw,pitch=s.neckPitch;
    const Vector3 yawAhead=Unit(ahead*std::cos(yaw)+right*std::sin(yaw));
    const Vector3 headRight=Unit(right*std::cos(yaw)-ahead*std::sin(yaw));
    const Vector3 headAhead=Unit(yawAhead*std::cos(pitch)+up*std::sin(pitch));
    const Vector3 hu=Unit(up*std::cos(pitch)-yawAhead*std::sin(pitch));
    const auto head=[&](const float x,const float y,const float z) {return neckTop+headRight*x+hu*y+headAhead*(z-.10f);};
    // (the deep, box-like skull: broad behind for the jaw muscles and the forward-set eyes,
    // tall all along the snout, its top nearly level)
    static constexpr std::array<Station,8> Skull{{
        {0.0f,.05f,.40f,.44f},{.18f,.10f,.46f,.52f},{.40f,.07f,.42f,.52f},{.68f,.02f,.34f,.47f},
        {.96f,-.03f,.26f,.40f},{1.22f,-.06f,.20f,.33f},{1.42f,-.08f,.14f,.24f},{1.53f,-.10f,.07f,.12f}}};
    centres.clear();rights.clear();ups.clear();widths.clear();heights.clear();along.clear();
    for(const Station& st:Skull) {
        centres.push_back(head(0,st.y,st.z));rights.push_back(headRight);ups.push_back(hu);
        widths.push_back(st.w);heights.push_back(st.h);along.push_back(st.z*2.0f+4.0f);
    }
    b.Loft(centres,rights,ups,widths,heights,along,24,hide);
    // the lower jaw, hinged at the back of the skull and swung down by the gape
    const float gape=std::clamp(s.jaw,0.0f,1.0f)*.62f;
    const Vector3 hinge=head(0,-.30f,.05f);
    const Vector3 jawAhead=Unit(headAhead*std::cos(gape)-hu*std::sin(gape));
    const Vector3 jawUp=Unit(hu*std::cos(gape)+headAhead*std::sin(gape));
    const auto jaw=[&](const float x,const float y,const float z) {return hinge+headRight*x+jawUp*y+jawAhead*z;};
    static constexpr std::array<Station,5> Jaw{{{0.0f,-.02f,.31f,.15f},{.42f,-.05f,.28f,.14f},{.86f,-.06f,.21f,.12f},
                                                {1.24f,-.06f,.14f,.09f},{1.40f,-.05f,.08f,.06f}}};
    centres.clear();rights.clear();ups.clear();widths.clear();heights.clear();along.clear();
    for(const Station& st:Jaw) {
        centres.push_back(jaw(0,st.y,st.z));rights.push_back(headRight);ups.push_back(jawUp);
        widths.push_back(st.w);heights.push_back(st.h);along.push_back(4.0f);
    }
    Hide pale=hide;pale.back=hide.flank;
    b.Loft(centres,rights,ups,widths,heights,along,16,pale);
    // the teeth: stout curved cones along both jaws, ivory stained at the roots; and the mouth's
    // dark lining between when it gapes
    const Color tooth(222,212,184),mouth(96,42,38);
    // (lipped, as current work on theropods' teeth and jaws suggests: the teeth hidden behind
    // the lips with the mouth shut, bared as it opens)
    const float bared=std::clamp((gape-.06f)/.15f,0.0f,1.0f);
    for(const float side:{-1.0f,1.0f}) {
        for(int k=0;k<11&&bared>0.0f;++k) {
            const float z=.50f+.09f*static_cast<float>(k);
            const float w=.30f-.15f*static_cast<float>(k)/10.0f;
            const float length=(.10f-.035f*std::abs(static_cast<float>(k)-3.0f)/8.0f)*bared;
            b.Cone(head(side*w*.92f,-.30f+.02f*static_cast<float>(k)/10.0f,z),head(side*w*.88f,-.30f-length,z+.02f),.026f,tooth);
            if(k<9) b.Cone(jaw(side*w*.86f,.04f,z-.05f),jaw(side*w*.84f,.04f+length*.85f,z-.03f),.022f,tooth);
        }
        if(gape>.05f) b.Disc(head(side*.18f,-.33f,.95f),headAhead,hu,.52f,.06f+gape*.18f,mouth,8);
    }
    // the eyes: set forward on the broad skull, looking ahead together; a pale yellow iris, a
    // slit pupil, a heavy brow over each and the horn-like boss before it
    for(const float side:{-1.0f,1.0f}) {
        const Vector3 out=Unit(headRight*side*.75f+headAhead*.55f+hu*.15f);
        const Vector3 eye=head(side*.36f,.22f,.30f);
        const Vector3 eyeA=Unit(Vector3::Cross(out,hu)),eyeB=Unit(Vector3::Cross(eyeA,out));
        b.Disc(eye+out*.01f,eyeA,eyeB,.075f,.060f,Color(196,168,64),12);
        b.Disc(eye+out*.018f,eyeA,eyeB,.014f,.050f,Color(18,14,10),8);
        b.Disc(eye+out*.022f+eyeB*.025f+eyeA*.02f,eyeA,eyeB,.010f,.010f,Color(240,236,220),5);
        b.Ellipsoid(head(side*.34f,.34f,.30f),headRight,hu,headAhead,.10f,.06f,.16f,8,hide);
        b.Ellipsoid(head(side*.24f,.40f,.52f),headRight,hu,headAhead,.07f,.06f,.10f,7,hide);
        // the nostril near the snout's tip
        const Vector3 nostrilOut=Unit(headRight*side*.6f+headAhead*.6f+hu*.5f);
        const Vector3 nA=Unit(Vector3::Cross(nostrilOut,hu)),nB=Unit(Vector3::Cross(nA,nostrilOut));
        b.Disc(head(side*.08f,.02f,1.46f)+nostrilOut*.01f,nA,nB,.035f,.022f,Color(30,24,20),7);
    }
    // ---- the legs from the motion's joints: the thigh's great muscle mass, the shin, the
    // metatarsus, three toes with their claws and the small dewclaw
    for(std::size_t i=0;i<2;++i) {
        const TRexLeg& leg=s.legs[i];
        const float side=i==0?-1.0f:1.0f;
        const Vector3 hip=V(leg.hip),knee=V(leg.knee),ankle=V(leg.ankle),ball=V(leg.ball);
        // the thigh: the great caudofemoral and thigh muscles, merging up into the hips
        b.Ellipsoid(Vector3::Lerp(hip,knee,.38f)+up*.22f-ahead*.15f+right*(side*.10f),right,up,Unit(knee-hip),.62f,.92f,1.20f,16,hide);
        b.Limb(hip,knee,.56f,.32f,right,14,hide,.85f);
        // the shin's drumstick of muscle behind it, tapering to the ankle
        b.Ellipsoid(Vector3::Lerp(knee,ankle,.30f)-ahead*.08f,right,up,Unit(ankle-knee),.24f,.26f,.48f,12,hide);
        b.Limb(knee,ankle,.28f,.14f,right,12,hide,.80f);
        b.Ellipsoid(knee,right,up,ahead,.24f,.24f,.24f,10,hide);
        b.Limb(ankle,ball,.13f,.11f,right,10,hide,.85f);
        b.Ellipsoid(ankle,right,up,ahead,.14f,.13f,.14f,8,hide);
        for(std::size_t t=0;t<3;++t) {
            const Vector3 tip=V(leg.toes[t]);
            const Vector3 toeDir=Unit(tip-ball);
            const Vector3 nail=tip+toeDir*.10f-up*.03f;
            b.Limb(ball,tip,.10f,.06f,up,8,hide,1.3f);
            b.Cone(tip,nail,.045f,Color(54,46,38),5);
        }
        b.Cone(V(leg.ankle)-up*.25f-right*side*.08f,V(leg.ankle)-up*.40f-right*side*.10f-ahead*.05f,.04f,Color(54,46,38),4);
    }
    // ---- the arms: tiny and stout under the chest, two clawed fingers each
    for(const float side:{-1.0f,1.0f}) {
        const Vector3 shoulder=local(side*.46f,-.42f,2.0f),elbow=local(side*.52f,-.80f,2.25f),wrist=local(side*.46f,-.92f,2.55f);
        b.Limb(shoulder,elbow,.085f,.065f,ahead,8,hide);
        b.Limb(elbow,wrist,.065f,.05f,ahead,8,hide);
        for(const float finger:{-.025f,.025f}) {
            const Vector3 tip=wrist+ahead*.13f-up*.06f+right*finger;
            b.Limb(wrist,tip,.035f,.022f,up,5,hide);
            b.Cone(tip,tip+ahead*.05f-up*.04f,.02f,Color(54,46,38),4);
        }
    }
    triangles_=static_cast<int>(vertices_.size()/3);
    if(vertices_.empty()||vertices_.size()>static_cast<std::size_t>(Capacity)) return;
    // ---- its shadow: the hide's triangles laid on the ground along the sun, dark and soft
    shadow_.clear();
    const Vector3 sun=wildlife_render::FixedSunDirection;
    if(sun.Y>.12f) {
        const float groundY=DrawnOrAnalyticHeight(ground_,terrain_,hips.X,hips.Z)+.03f;
        const Color shade(0,0,0,70);
        for(const auto& v:vertices_) {
            const float lift=v.Position.Y-groundY;
            const Vector3 p(v.Position.X-sun.X*lift/sun.Y,0.0f,v.Position.Z-sun.Z*lift/sun.Y);
            shadow_.emplace_back(Vector3(p.X,DrawnOrAnalyticHeight(ground_,terrain_,p.X,p.Z)+.035f,p.Z),shade);
        }
    }
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    if(!shadow_.empty()) {
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        shadowBuffer_->SetData(shadow_.data(),0,static_cast<int>(shadow_.size()),SetDataOptions::Discard);
        device_.SetVertexBuffer(shadowBuffer_.get());
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(shadow_.size()/3U));
    }
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(vertices_.data(),0,static_cast<int>(vertices_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    visible_=true;
}

}
