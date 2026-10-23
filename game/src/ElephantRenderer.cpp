#include "IslandSimulator/ElephantRenderer.hpp"
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
    const float x=std::floor(p.X),y=std::floor(p.Y),z=std::floor(p.Z);
    return Hash(static_cast<std::uint32_t>(static_cast<std::int32_t>(x)*73856093^static_cast<std::int32_t>(y)*19349663^static_cast<std::int32_t>(z)*83492791));
}
Vector3 V(const ElephantPoint& p) {return {p.x,p.y,p.z};}
Vector3 Unit(Vector3 v,const Vector3& fallback=Vector3::Up) {
    if(v.LengthSquared()<1e-10f) return fallback;
    v.Normalize();return v;
}

// The hide's colour at a point with its outward normal: a grey-brown skin, dusted paler over
// the back and the head by the sand it throws on itself, darker in the folds of its wrinkles
// (rings `bands` to the metre along `along`), mottled; freckled pink where its kind loses its
// pigment (the ears' edges, the face and the trunk's root) as `pink` says.
struct Skin {
    Vector3 grey{98,91,86},dust{150,131,108},belly{78,72,68};
    float bands=0.0f,bandDepth=0.0f,pink=0.0f,wet=0.0f;
    Color At(const Vector3& p,const Vector3& n,const float along,const float pinkHere=0.0f) const {
        const float upness=n.Y;
        Vector3 c=upness>0.0f?Vector3::Lerp(grey,dust,std::clamp(upness*upness*.55f*(1.0f-wet),0.0f,1.0f))
                             :Vector3::Lerp(grey,belly,std::clamp(-upness*1.4f,0.0f,1.0f));
        if(bands>0.0f) {
            const float fold=std::pow(.5f+.5f*std::sin(along*bands*2.0f*Pi),6.0f);
            c*=1.0f-bandDepth*fold;
        }
        const float freckle=std::max(pink,pinkHere);
        if(freckle>0.0f&&Noise3(p*16.0f)>.45f) c=Vector3::Lerp(c,Vector3(200,158,146),freckle*.75f);
        c*=.92f+.12f*Noise3(p*6.0f);
        c*=wildlife_render::WetCoatFactor()*(1.0f-.12f*wet);
        const float light=.60f+.38f*std::max(0.0f,Vector3::Dot(n,wildlife_render::FixedSunDirection))+.10f*std::max(0.0f,n.Y);
        c*=light;
        return Color(static_cast<int>(std::clamp(c.X,0.0f,255.0f)),static_cast<int>(std::clamp(c.Y,0.0f,255.0f)),
                     static_cast<int>(std::clamp(c.Z,0.0f,255.0f)));
    }
};

class Builder {
public:
    explicit Builder(std::vector<VertexPositionColor>& out):out_(out){}
    // A loft through cross sections: centre, the section's right and up axes, half width and
    // height; `along` for the wrinkles and `pink` for the freckling. Ends closed if asked.
    void Loft(const std::vector<Vector3>& centres,const std::vector<Vector3>& rights,const std::vector<Vector3>& ups,
              const std::vector<float>& widths,const std::vector<float>& heights,const std::vector<float>& along,
              const int sides,const Skin& skin,const bool capEnds=true,const std::vector<float>* pinks=nullptr) {
        const std::size_t n=centres.size();
        const auto point=[&](const std::size_t i,const int k) {
            const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);
            return centres[i]+rights[i]*(std::cos(a)*widths[i])+ups[i]*(std::sin(a)*heights[i]);
        };
        const auto normal=[&](const std::size_t i,const int k) {
            const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);
            return Unit(rights[i]*(std::cos(a)/std::max(widths[i],1e-3f))+ups[i]*(std::sin(a)/std::max(heights[i],1e-3f)));
        };
        const auto colour=[&](const std::size_t i,const int k) {
            return skin.At(point(i,k),normal(i,k),along[i],pinks?(*pinks)[i]:0.0f);
        };
        for(std::size_t i=0;i+1<n;++i) {
            for(int k=0;k<sides;++k) {
                const Vector3 a=point(i,k),b=point(i,k+1),c=point(i+1,k+1),d=point(i+1,k);
                const Color ca=colour(i,k),cb=colour(i,k+1),cc=colour(i+1,k+1),cd=colour(i+1,k);
                out_.emplace_back(a,ca);out_.emplace_back(b,cb);out_.emplace_back(c,cc);
                out_.emplace_back(a,ca);out_.emplace_back(c,cc);out_.emplace_back(d,cd);
            }
        }
        if(!capEnds) return;
        for(const std::size_t i:{std::size_t{0},n-1}) {
            const Vector3 tip=centres[i]+(i==0?centres[0]-centres[1]:centres[n-1]-centres[n-2])*.15f;
            for(int k=0;k<sides;++k) {
                const Color c=skin.At(tip,Unit(tip-centres[i]),along[i],pinks?(*pinks)[i]:0.0f);
                out_.emplace_back(point(i,k),c);out_.emplace_back(point(i,k+1),c);out_.emplace_back(tip,c);
            }
        }
    }
    // A tapering limb from a to b in `segments` spans (its wrinkles along it), its section an
    // ellipse flattened by `flat` sideways.
    void Limb(const Vector3& a,const Vector3& b,const float ra,const float rb,const Vector3& sideHint,const int sides,
              const Skin& skin,const float flat=1.0f,const int segments=1,const float alongStart=0.0f) {
        const Vector3 axis=Unit(b-a);
        const Vector3 side=Unit(Vector3::Cross(axis,sideHint),Vector3::UnitX);
        const Vector3 other=Unit(Vector3::Cross(side,axis));
        std::vector<Vector3> c,r,u;std::vector<float> w,h,al;
        const float length=Vector3::Distance(a,b);
        for(int i=0;i<=segments;++i) {
            const float t=static_cast<float>(i)/static_cast<float>(segments);
            const float radius=ra+(rb-ra)*t;
            c.push_back(Vector3::Lerp(a,b,t));r.push_back(side);u.push_back(other);
            w.push_back(radius*flat);h.push_back(radius);al.push_back(alongStart+length*t);
        }
        Loft(c,r,u,w,h,al,sides,skin,false);
    }
    void Ellipsoid(const Vector3& c,const Vector3& right,const Vector3& up,const Vector3& forward,const float rx,const float ry,
                   const float rz,const int sides,const Skin& skin,const float pink=0.0f) {
        std::vector<Vector3> centres,rights,ups;std::vector<float> w,h,al,pk;
        constexpr int Rings=7;
        for(int i=0;i<=Rings;++i) {
            const float t=-1.0f+2.0f*static_cast<float>(i)/Rings;
            const float s=std::sqrt(std::max(0.02f,1.0f-t*t));
            centres.push_back(c+forward*(t*rz));rights.push_back(right);ups.push_back(up);
            w.push_back(rx*s);h.push_back(ry*s);al.push_back(0.0f);pk.push_back(pink);
        }
        Loft(centres,rights,ups,w,h,al,sides,skin,false,&pk);
    }
    void Disc(const Vector3& centre,const Vector3& a,const Vector3& b,const float ra,const float rb,const Color& colour,const int sides=10) {
        for(int k=0;k<sides;++k) {
            const float a0=2.0f*Pi*k/sides,a1=2.0f*Pi*(k+1)/sides;
            out_.emplace_back(centre,colour);
            out_.emplace_back(centre+a*(std::cos(a0)*ra)+b*(std::sin(a0)*rb),colour);
            out_.emplace_back(centre+a*(std::cos(a1)*ra)+b*(std::sin(a1)*rb),colour);
        }
    }
    void Triangle(const Vector3& a,const Vector3& b,const Vector3& c,const Color& ca,const Color& cb,const Color& cc) {
        out_.emplace_back(a,ca);out_.emplace_back(b,cb);out_.emplace_back(c,cc);
    }
private:
    std::vector<VertexPositionColor>& out_;
};

// Frames along a curve carried from one station to the next (no twist), the first `right` given.
void CurveFrames(const std::vector<Vector3>& centres,Vector3 right,std::vector<Vector3>& rights,std::vector<Vector3>& ups) {
    rights.clear();ups.clear();
    for(std::size_t i=0;i<centres.size();++i) {
        const Vector3 t=Unit(centres[std::min(i+1,centres.size()-1)]-centres[i==0?0:i-1]);
        right=Unit(right-t*Vector3::Dot(right,t),Unit(Vector3::Cross(t,Vector3::Up),Vector3::UnitX));
        rights.push_back(right);
        ups.push_back(Unit(Vector3::Cross(right,t)));
    }
}

// Catmull-Rom through points, `per` stations to each span.
std::vector<Vector3> Smooth(const std::vector<Vector3>& p,const int per) {
    std::vector<Vector3> out;
    for(std::size_t i=0;i+1<p.size();++i) {
        const Vector3& p0=p[i==0?0:i-1];const Vector3& p1=p[i];const Vector3& p2=p[i+1];const Vector3& p3=p[std::min(i+2,p.size()-1)];
        for(int k=0;k<per;++k) out.push_back(Vector3::CatmullRom(p0,p1,p2,p3,static_cast<float>(k)/static_cast<float>(per)));
    }
    out.push_back(p.back());
    return out;
}
}

ElephantRenderer::ElephantRenderer(GraphicsDevice& device,const float maximumDistance)
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

ElephantRenderer::~ElephantRenderer()=default;

void ElephantRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const ElephantMotion& elephant) {
    visible_=false;triangles_=0;
    const ElephantState& s=elephant.State();
    const Vector3 origin=V(elephant.ToWorld(0,0,0));
    if(Vector3::DistanceSquared(origin,camera)>maximumDistance_*maximumDistance_) return;
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(origin+Vector3(0,-.4f,.5f),5.0f))) return;
    vertices_.clear();
    Builder b(vertices_);
    Skin skin;
    // dark and wet from the sea, drying in the sun
    skin.wet=std::clamp(s.wet,0.0f,1.0f);
    const Vector3 right=V(elephant.ToWorld(1,0,0))-origin,up=V(elephant.ToWorld(0,1,0))-origin,ahead=V(elephant.ToWorld(0,0,1))-origin;
    const auto local=[&](const float x,const float y,const float z) {return origin+right*x+up*y+ahead*z;};
    // ---- the body: the barrel from the rump's slope over the arched back to the neck, each
    // section deeper than it is wide, the back highest behind the shoulders as in its kind
    struct Station {float z,y,w,h;};
    static constexpr std::array<Station,10> Body{{
        {-2.30f,.03f,.20f,.27f},{-2.15f,-.04f,.50f,.56f},{-1.70f,-.06f,.70f,.74f},{-1.00f,-.06f,.80f,.84f},
        {-.25f,-.06f,.83f,.90f},{.50f,-.06f,.82f,.88f},{1.20f,-.03f,.76f,.79f},{1.75f,-.01f,.64f,.65f},
        {2.10f,.02f,.52f,.53f},{2.40f,.08f,.46f,.48f}}};
    std::vector<Vector3> centres,rights,ups;std::vector<float> widths,heights,along;
    {
        std::vector<Vector3> raw;
        for(const Station& st:Body) raw.push_back(Vector3(st.w,st.y,st.h));
        std::vector<Vector3> zs;for(const Station& st:Body) zs.push_back(Vector3(st.z,0,0));
        const auto shapes=Smooth(raw,3),positions=Smooth(zs,3);
        for(std::size_t i=0;i<shapes.size();++i) {
            centres.push_back(local(0,shapes[i].Y,positions[i].X));rights.push_back(right);ups.push_back(up);
            widths.push_back(shapes[i].X);heights.push_back(shapes[i].Z);along.push_back(positions[i].X);
        }
    }
    Skin body=skin;body.bands=.9f;body.bandDepth=.06f;
    b.Loft(centres,rights,ups,widths,heights,along,30,body);
    // ---- the legs from the motion's joints: the upper arm and thigh swelling into the body,
    // the pillars of the forearm and shin, wrinkled about the wrist and the hock, the round sole
    // and the toenails round its front
    Skin leg=skin;leg.bands=7.0f;leg.bandDepth=.16f;
    const Color nail(156,148,132);
    for(std::size_t i=0;i<4;++i) {
        const ElephantLeg& l=s.legs[i];
        const bool front=i<2;
        const float side=i%2==0?-1.0f:1.0f;
        const Vector3 top=V(l.top),middle=V(l.middle),low=V(l.low),foot=V(l.foot);
        if(front) {
            // (the upper arm's muscle under the hide, hardly standing out of the barrel)
            b.Ellipsoid(Vector3::Lerp(top,middle,.55f)-right*(side*.04f),right,Unit(top-middle),ahead,.24f,.50f,.34f,14,skin);
            b.Limb(top,middle,.33f,.27f,right,14,leg,.85f,4,0.0f);
            b.Limb(middle,low,.27f,.22f,right,14,leg,.95f,6,.9f);
        } else {
            b.Ellipsoid(Vector3::Lerp(top,middle,.50f)-right*(side*.04f),right,Unit(top-middle),ahead,.27f,.55f,.40f,14,skin);
            b.Limb(top,middle,.36f,.27f,right,14,leg,.80f,4,0.0f);
            b.Limb(middle,low,.26f,.21f,right,14,leg,.95f,6,.9f);
        }
        b.Ellipsoid(middle,right,up,ahead,.26f,.24f,.26f,10,leg);
        // the foot: a short column flaring to the sole
        const Vector3 sole=foot+Vector3(0,.02f,0);
        const float soleRadius=front?.26f:.23f;
        b.Limb(low,sole+Vector3(0,.08f,0),.22f,soleRadius,ahead,16,leg,1.0f,3,1.8f);
        b.Limb(sole+Vector3(0,.08f,0),sole,soleRadius,soleRadius*.97f,ahead,16,skin,1.0f,1,0.0f);
        // the toenails, five on the forefoot and four behind, pale half-moons round the front
        const int nails=front?5:4;
        const Vector3 footAhead=Unit(Vector3(ahead.X,0,ahead.Z)),footRight=Unit(Vector3(right.X,0,right.Z));
        for(int k=0;k<nails;++k) {
            const float a=(static_cast<float>(k)/static_cast<float>(nails-1)-.5f)*(front?2.0f:1.6f);
            const Vector3 out=footAhead*std::cos(a)+footRight*std::sin(a);
            const Vector3 c=sole+out*(soleRadius+.01f)+Vector3(0,.06f,0);
            const Vector3 across=Unit(Vector3::Cross(Vector3::Up,out));
            b.Disc(c,across,Vector3::Up,.05f,.035f,wildlife_render::ScaleColor(nail,.72f+.30f*std::max(0.0f,Vector3::Dot(out,wildlife_render::FixedSunDirection))),7);
        }
    }
    // ---- the tail: thin, hanging from the top of the rump, swinging, with its tuft of bristles
    {
        std::vector<Vector3> tail;
        for(int k=0;k<=6;++k) {
            const float t=static_cast<float>(k)/6.0f;
            const float sway=s.tailSwing*t*t*.9f;
            tail.push_back(local(sway,.34f-1.15f*t,-2.26f-.22f*std::sin(Pi*.5f*t)+.1f*t*t));
        }
        std::vector<float> w,h,al;
        for(int k=0;k<=6;++k) {const float t=static_cast<float>(k)/6.0f;w.push_back(.07f-.045f*t);h.push_back(.07f-.045f*t);al.push_back(t);}
        CurveFrames(tail,right,rights,ups);
        b.Loft(tail,rights,ups,w,h,al,8,skin);
        const Vector3 end=tail.back();
        const Color bristle(34,30,28);
        for(int k=0;k<9;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/9.0f;
            const Vector3 o=right*(std::cos(a)*.035f)+ahead*(std::sin(a)*.035f);
            const Vector3 tip=end+o*2.2f-up*(.20f+.06f*Hash(static_cast<std::uint32_t>(k)*7U));
            b.Triangle(end+o-up*.04f,end+o*.4f+up*.05f,tip,bristle,bristle,bristle);
        }
    }
    // ---- the head on the neck, turned and raised
    const ElephantMotion::HeadFrame frame=elephant.Head();
    const Vector3 pivot=V(frame.pivot),hr=V(frame.right),hu=V(frame.up),ha=V(frame.ahead);
    const auto head=[&](const float x,const float y,const float z) {return pivot+hr*x+hu*y+ha*z;};
    {
        // the skull deep and short, the brow sloping to the trunk's root
        static constexpr std::array<Station,6> Skull{{
            {-.15f,-.04f,.42f,.47f},{0.0f,-.05f,.49f,.55f},{.30f,-.02f,.55f,.64f},{.62f,-.07f,.53f,.66f},
            {.90f,-.19f,.43f,.54f},{1.02f,-.38f,.21f,.23f}}};
        centres.clear();widths.clear();heights.clear();along.clear();
        std::vector<float> pinks;
        for(const Station& st:Skull) {
            centres.push_back(head(0,st.y,st.z));widths.push_back(st.w);heights.push_back(st.h);along.push_back(st.z);
            pinks.push_back(std::clamp((st.z-.6f)*1.2f,0.0f,.55f));
        }
        rights.assign(centres.size(),hr);ups.assign(centres.size(),hu);
        b.Loft(centres,rights,ups,widths,heights,along,26,skin,true,&pinks);
        // the twin domes over the brow, the dip between them; and the lower lip
        for(const float side:{-1.0f,1.0f}) b.Ellipsoid(head(side*.18f,.45f,.46f),hr,hu,ha,.26f,.19f,.29f,12,skin);
        b.Ellipsoid(head(0,-.62f,.80f),hr,hu,Unit(ha-hu*.6f),.12f,.08f,.16f,9,skin,.4f);
        // the small eyes with their heavy lids, a dark brown iris catching the light; and the
        // temporal gland's dark seep behind each
        for(const float side:{-1.0f,1.0f}) {
            const Vector3 out=Unit(hr*side+ha*.45f+hu*.05f);
            const Vector3 eye=head(side*.49f,.0f,.70f);
            const Vector3 ea=Unit(Vector3::Cross(out,hu)),eb=Unit(Vector3::Cross(ea,out));
            b.Ellipsoid(eye-out*.01f,ea,eb,out,.085f,.06f,.035f,8,skin);
            b.Disc(eye+out*.03f,ea,eb,.040f,.030f,Color(46,30,20),10);
            b.Disc(eye+out*.034f,ea,eb,.022f,.020f,Color(14,10,8),8);
            b.Disc(eye+out*.037f+eb*.012f+ea*.01f,ea,eb,.007f,.007f,Color(220,214,200),5);
            const Vector3 gland=head(side*.55f,-.08f,.46f);
            b.Disc(gland+hr*side*.015f,ha,hu,.03f,.06f,Color(62,54,48),8);
        }
        // the tusks: from their sheaths beside the trunk's root, forward and down, then curving
        // up and a little out; ivory, stained at the root
        Skin ivory;ivory.grey=Vector3(214,202,176);ivory.dust=Vector3(226,214,190);ivory.belly=Vector3(176,162,132);
        for(const float side:{-1.0f,1.0f}) {
            std::vector<Vector3> tusk;
            static constexpr std::array<std::array<float,3>,6> Path{{{.20f,-.50f,.90f},{.21f,-.66f,1.06f},{.22f,-.80f,1.28f},
                                                                    {.25f,-.86f,1.52f},{.29f,-.84f,1.74f},{.33f,-.76f,1.92f}}};
            for(const auto& q:Path) tusk.push_back(head(side*q[0],q[1],q[2]));
            const auto smooth=Smooth(tusk,3);
            std::vector<float> w,h,al;
            for(std::size_t k=0;k<smooth.size();++k) {
                const float t=static_cast<float>(k)/static_cast<float>(smooth.size()-1);
                const float r=.068f*(1.0f-.75f*t*t)+.004f;
                w.push_back(r);h.push_back(r);al.push_back(t);
            }
            CurveFrames(smooth,hr,rights,ups);
            b.Loft(smooth,rights,ups,w,h,al,10,ivory);
            b.Ellipsoid(head(side*.20f,-.50f,.92f),hr,hu,Unit(ha-hu*.7f),.10f,.10f,.13f,10,skin,.5f);
        }
        // the ears: small, as in its kind, fanned out from the sides of the head and laid back
        // on the neck; the top edge folded over; freckled pink toward the margin
        const float spread=std::clamp(s.earSpread,0.0f,1.0f);
        for(const float side:{-1.0f,1.0f}) {
            const float angle=.22f+1.05f*spread;
            const Vector3 back=Unit(-ha*std::cos(angle)+hr*side*std::sin(angle)-hu*.08f);
            constexpr int Rows=7,Columns=5;
            std::array<std::array<Vector3,Columns+1>,Rows+1> grid{};
            for(int v=0;v<=Rows;++v) {
                const float tv=static_cast<float>(v)/Rows;
                const Vector3 root=head(side*.44f,.38f-.76f*tv,.20f+.10f*tv);
                // the ear's breadth down its height: widest above the middle, the lobe narrower
                const float breadth=.62f*std::pow(std::sin(Pi*(.06f+.90f*tv)),.7f)*(1.0f-.35f*tv*tv);
                for(int k=0;k<=Columns;++k) {
                    const float tu=static_cast<float>(k)/Columns;
                    const Vector3 cup=Unit(Vector3::Cross(back,hu))*(-side)*(.06f*std::sin(Pi*tu));
                    Vector3 p=root+back*(breadth*tu)+cup;
                    // the top margin folded forward over the ear
                    if(v==0) p+=hu*-.04f*tu+ha*.03f*tu;
                    grid[static_cast<std::size_t>(v)][static_cast<std::size_t>(k)]=p;
                }
            }
            const Vector3 n=Unit(Vector3::Cross(back,hu)*side);
            const Vector3 lit=n.Y<0.0f?-n:n;
            for(int v=0;v<Rows;++v) {
                for(int k=0;k<Columns;++k) {
                    const auto at=[&](const int vv,const int kk){return grid[static_cast<std::size_t>(vv)][static_cast<std::size_t>(kk)];};
                    const auto col=[&](const int vv,const int kk) {
                        const float margin=std::clamp((static_cast<float>(kk)/Columns-.55f)*2.2f,0.0f,1.0f);
                        return skin.At(at(vv,kk),lit,0.0f,.85f*margin);
                    };
                    b.Triangle(at(v,k),at(v,k+1),at(v+1,k+1),col(v,k),col(v,k+1),col(v+1,k+1));
                    b.Triangle(at(v,k),at(v+1,k+1),at(v+1,k),col(v,k),col(v+1,k+1),col(v+1,k));
                }
            }
        }
    }
    // ---- the trunk along the motion's curve, ringed with wrinkles, its root freckled pink,
    // ending in the one finger above its tip
    {
        const auto points=elephant.TrunkPoints();
        std::vector<Vector3> raw;
        raw.push_back(head(0,-.28f,.92f));
        for(const auto& p:points) raw.push_back(V(p));
        const auto curve=Smooth(raw,3);
        CurveFrames(curve,hr,rights,ups);
        std::vector<float> w,h,al,pk;
        float travelled=0.0f;
        for(std::size_t k=0;k<curve.size();++k) {
            if(k>0) travelled+=Vector3::Distance(curve[k],curve[k-1]);
            const float t=static_cast<float>(k)/static_cast<float>(curve.size()-1);
            const float r=.25f-.18f*std::pow(t,.8f);
            w.push_back(r);h.push_back(r*.9f);al.push_back(travelled);
            pk.push_back(std::clamp(.6f-t*3.0f,0.0f,.6f));
        }
        Skin trunk=skin;trunk.bands=9.0f;trunk.bandDepth=.24f;
        b.Loft(curve,rights,ups,w,h,al,14,trunk,true,&pk);
        // the finger on the upper lip of the tip, and the nostrils' dark openings
        const Vector3 tip=curve.back(),tipDir=Unit(curve.back()-curve[curve.size()-2]);
        const Vector3 tipUp=ups.back();
        b.Ellipsoid(tip+tipUp*.05f+tipDir*.03f,rights.back(),tipUp,tipDir,.03f,.025f,.06f,6,skin);
        for(const float side:{-1.0f,1.0f})
            b.Disc(tip+tipDir*.012f+rights.back()*(side*.026f),rights.back(),tipUp,.018f,.024f,Color(26,22,20),7);
    }
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
