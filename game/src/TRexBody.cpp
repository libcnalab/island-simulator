#include "IslandSimulator/TRexBody.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <numbers>
#include <optional>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;

struct V3 {float x=0,y=0,z=0;};
V3 operator+(const V3& a,const V3& b) {return {a.x+b.x,a.y+b.y,a.z+b.z};}
V3 operator-(const V3& a,const V3& b) {return {a.x-b.x,a.y-b.y,a.z-b.z};}
V3 operator-(const V3& a) {return {-a.x,-a.y,-a.z};}
V3 operator*(const V3& a,const float s) {return {a.x*s,a.y*s,a.z*s};}
float Dot(const V3& a,const V3& b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
V3 Cross(const V3& a,const V3& b) {return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
float Length(const V3& a) {return std::sqrt(Dot(a,a));}
V3 Unit(const V3& a,const V3& fallback=V3{0,1,0}) {
    const float l=Length(a);
    return l>1e-6f?a*(1.0f/l):fallback;
}
V3 Lerp(const V3& a,const V3& b,const float t) {return a+(b-a)*t;}
V3 P(const TRexPoint& p) {return {p.x,p.y,p.z};}
TRexPoint Q(const V3& v) {return {v.x,v.y,v.z};}
float Mix(const float a,const float b,const float t) {return a+(b-a)*t;}
float Smooth(const float t) {const float k=std::clamp(t,0.0f,1.0f);return k*k*(3.0f-2.0f*k);}
float Step(const float from,const float to,const float v) {return Smooth((v-from)/(to-from));}

float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Lattice(const int x,const int y,const int z) {
    return Hash(static_cast<std::uint32_t>(x*73856093^y*19349663^z*83492791));
}
// smooth value noise in 0..1, for the hide's blotches and bands
float Noise(const V3& p) {
    const float fx=std::floor(p.x),fy=std::floor(p.y),fz=std::floor(p.z);
    const int x=static_cast<int>(fx),y=static_cast<int>(fy),z=static_cast<int>(fz);
    const float u=Smooth(p.x-fx),v=Smooth(p.y-fy),w=Smooth(p.z-fz);
    const auto edge=[&](const int dy,const int dz) {return Mix(Lattice(x,y+dy,z+dz),Lattice(x+1,y+dy,z+dz),u);};
    return Mix(Mix(edge(0,0),edge(1,0),v),Mix(edge(0,1),edge(1,1),v),w);
}

// A cross section: half its width, the half heights above and below the centre, narrower over
// the top by `pear`, boxier as `square` rises above 2.
struct Section {float w=0,top=0,bottom=0,pear=0,square=2.0f;};
Section MixSection(const Section& a,const Section& b,const float t) {
    return {Mix(a.w,b.w,t),Mix(a.top,b.top,t),Mix(a.bottom,b.bottom,t),Mix(a.pear,b.pear,t),Mix(a.square,b.square,t)};
}
struct Ring {V3 centre,right,up;Section s;};

// The point at angle `a` round a ring (-pi/2 under it, 0 to its right, pi/2 over it).
V3 RingPoint(const Ring& r,const float a) {
    const float c=std::cos(a),s=std::sin(a);
    const float e=2.0f/std::max(r.s.square,1.0f);
    const auto shape=[e](const float v) {return std::copysign(std::pow(std::abs(v),e),v);};
    const float px=r.s.w*shape(c)*(1.0f-r.s.pear*s);
    const float py=(s>=0.0f?r.s.top:r.s.bottom)*shape(s);
    return r.centre+r.right*px+r.up*py;
}
float Angle(const int k,const int sides) {return -.5f*Pi+2.0f*Pi*static_cast<float>(k)/static_cast<float>(sides);}

// Frames square to the run of the centres: the right toward `right`, the up toward `up`.
void Orient(std::vector<Ring>& rings,const std::vector<V3>& right,const std::vector<V3>& up) {
    const std::size_t n=rings.size();
    for(std::size_t i=0;i<n;++i) {
        const V3 along=Unit(rings[std::min(i+1,n-1)].centre-rings[i==0?0:i-1].centre,V3{0,0,1});
        const V3 x=Unit(right[i]-along*Dot(right[i],along),Unit(Cross(up[i],along),V3{1,0,0}));
        V3 y=Unit(Cross(along,x));
        if(Dot(y,up[i])<0.0f) y=-y;
        rings[i].right=x;rings[i].up=y;
    }
}

float CatmullRom(const float p0,const float p1,const float p2,const float p3,const float t) {
    const float t2=t*t,t3=t2*t;
    return .5f*((2.0f*p1)+(-p0+p2)*t+(2.0f*p0-5.0f*p1+4.0f*p2-p3)*t2+(-p0+3.0f*p1-3.0f*p2+p3)*t3);
}

// A station of a lofted part: where along it, the centre's height, and its section.
struct Station {float z,y;Section s;};
template<std::size_t N>
std::vector<Station> Resample(const std::array<Station,N>& table,const int per) {
    std::vector<Station> out;
    for(std::size_t i=0;i+1<N;++i) {
        const Station& a=table[i==0?0:i-1];const Station& b=table[i];const Station& c=table[i+1];
        const Station& d=table[std::min(i+2,N-1)];
        for(int k=0;k<per;++k) {
            const float t=static_cast<float>(k)/static_cast<float>(per);
            const auto f=[&](const auto get) {return CatmullRom(get(a),get(b),get(c),get(d),t);};
            out.push_back({f([](const Station& s){return s.z;}),f([](const Station& s){return s.y;}),
                           {std::max(.005f,f([](const Station& s){return s.s.w;})),std::max(.005f,f([](const Station& s){return s.s.top;})),
                            std::max(.005f,f([](const Station& s){return s.s.bottom;})),f([](const Station& s){return s.s.pear;}),
                            std::max(1.6f,f([](const Station& s){return s.s.square;}))}});
        }
    }
    out.push_back(table[N-1]);
    return out;
}
// the table's section at z, straight between its stations
template<std::size_t N>
Station At(const std::array<Station,N>& table,const float z) {
    if(z<=table[0].z) return table[0];
    for(std::size_t i=0;i+1<N;++i) {
        if(z<=table[i+1].z) {
            const float t=(z-table[i].z)/(table[i+1].z-table[i].z);
            return {z,Mix(table[i].y,table[i+1].y,t),MixSection(table[i].s,table[i+1].s,t)};
        }
    }
    return table[N-1];
}
// how far the surface stands above (or below, `sign` -1) the centre at a fraction of its half
// width out from the middle
float SurfaceHeight(const Section& s,const float across,const float sign) {
    const float n=std::max(s.square,1.0f);
    return (sign>0.0f?s.top:s.bottom)*std::pow(std::max(0.0f,1.0f-std::pow(std::clamp(across,0.0f,1.0f),n)),1.0f/n);
}

// ---- the parts' shapes, at rest (the head's in its own frame from the top of the neck, the
// jaw's from its hinge)

// the body from the tail's tip to the chest: z, the centre's height, half width, half heights
// (the dorsal line is y+top and the belly's y-bottom), pear, square
constexpr std::array<Station,15> Torso{{
    {-6.62f,-.680f,{.025f,.025f,.025f,0.0f,2.0f}},{-6.30f,-.630f,{.050f,.080f,.080f,0.0f,2.0f}},
    {-5.80f,-.555f,{.085f,.145f,.145f,.10f,2.0f}},{-5.10f,-.465f,{.130f,.225f,.225f,.15f,2.1f}},
    {-4.30f,-.370f,{.190f,.320f,.320f,.20f,2.2f}},{-3.50f,-.285f,{.260f,.435f,.435f,.20f,2.2f}},
    {-2.70f,-.220f,{.340f,.560f,.560f,.20f,2.2f}},{-1.90f,-.170f,{.430f,.690f,.690f,.20f,2.2f}},
    {-1.10f,-.125f,{.520f,.825f,.825f,.18f,2.2f}},{-.40f,-.105f,{.600f,.945f,.945f,.16f,2.2f}},
    {.30f,-.160f,{.650f,1.040f,1.040f,.12f,2.2f}},{.90f,-.230f,{.680f,1.070f,1.070f,.05f,2.1f}},
    {1.60f,-.250f,{.700f,1.030f,1.030f,0.0f,2.1f}},{2.30f,-.215f,{.660f,.945f,.945f,-.05f,2.1f}},
    {2.90f,-.120f,{.580f,.840f,.840f,-.05f,2.1f}}}};
// the skull (cranium and upper jaw) along the head from behind the neck's top to the snout
constexpr std::array<Station,13> Skull{{
    {-.20f,.06f,{.20f,.18f,.22f,.30f,2.0f}},{-.12f,.04f,{.38f,.30f,.38f,.38f,2.3f}},
    {0.0f,.02f,{.46f,.35f,.39f,.42f,2.4f}},{.14f,0.0f,{.48f,.37f,.385f,.45f,2.5f}},
    {.30f,-.02f,{.45f,.37f,.385f,.45f,2.6f}},{.46f,-.05f,{.38f,.35f,.365f,.40f,2.7f}},
    {.62f,-.07f,{.30f,.32f,.33f,.30f,2.9f}},{.80f,-.09f,{.25f,.29f,.29f,.24f,3.1f}},
    {1.00f,-.11f,{.215f,.26f,.26f,.20f,3.1f}},{1.18f,-.13f,{.19f,.23f,.23f,.17f,3.0f}},
    {1.32f,-.15f,{.16f,.19f,.19f,.13f,2.8f}},{1.42f,-.18f,{.12f,.14f,.16f,.08f,2.5f}},
    {1.48f,-.21f,{.06f,.06f,.10f,0.0f,2.0f}}}};
// the lower jaw from behind its hinge to the chin
constexpr std::array<Station,10> Jaw{{
    {-.14f,-.06f,{.28f,.10f,.10f,0.0f,2.0f}},{-.04f,-.10f,{.40f,.10f,.18f,.05f,2.2f}},
    {.12f,-.13f,{.41f,.10f,.20f,.05f,2.3f}},{.32f,-.13f,{.37f,.09f,.20f,.05f,2.4f}},
    {.55f,-.12f,{.30f,.08f,.17f,.05f,2.5f}},{.80f,-.10f,{.24f,.07f,.14f,.05f,2.5f}},
    {1.05f,-.08f,{.20f,.06f,.12f,.05f,2.5f}},{1.25f,-.07f,{.155f,.058f,.10f,.05f,2.4f}},
    {1.36f,-.06f,{.11f,.055f,.08f,0.0f,2.2f}},{1.41f,-.055f,{.055f,.04f,.045f,0.0f,2.0f}}}};
// the skull's modelling over its loft, in the head's frame (x either side): the brows hooding
// the eyes, the hornlets before them, the bosses on the cheeks, the jaw muscles' swell behind
// the eyes, the hollow before them and the low ridge along the snout
struct Bump {float x,y,z,rx,ry,rz,height;};
constexpr std::array<Bump,6> SkullBumps{{
    {.30f,.215f,.25f,.07f,.055f,.12f,.045f},{.19f,.255f,.50f,.05f,.045f,.085f,.032f},
    {.45f,-.12f,.17f,.06f,.07f,.10f,.030f},{.44f,.14f,.06f,.08f,.12f,.12f,.025f},
    {.27f,0.0f,.66f,.06f,.09f,.15f,-.018f},{0.0f,.17f,.95f,.07f,.06f,.35f,.012f}}};
constexpr float HingeY=-.30f,HingeZ=.06f;
// the neck's top at rest, in the body's frame, and the point its bend turns about
constexpr V3 NeckTop{0.0f,.62f,4.15f},NeckBase{0.0f,.20f,3.05f};
constexpr V3 EyeAt{0.0f,.150f,.330f};   // its height and place along the head; set into the side

// A limb's station: along its bone (0 the upper joint, 1 the lower), half width across, half
// depths before and behind, pear, and how far its centre is drawn in toward the body.
struct LimbStation {float t,w,front,back,pear,inward;};
constexpr std::array<LimbStation,10> Thigh{{
    {-.36f,.18f,.28f,.34f,0.0f,.22f},{-.22f,.26f,.40f,.54f,.05f,.17f},{-.06f,.33f,.46f,.64f,.08f,.10f},
    {.12f,.36f,.47f,.66f,.10f,.04f},{.32f,.35f,.44f,.58f,.10f,0.0f},{.52f,.32f,.40f,.48f,.08f,0.0f},
    {.70f,.28f,.33f,.37f,.05f,0.0f},{.86f,.24f,.27f,.28f,0.0f,0.0f},{1.0f,.21f,.23f,.23f,0.0f,0.0f},
    {1.10f,.18f,.19f,.19f,0.0f,0.0f}}};
constexpr std::array<LimbStation,9> Shin{{
    {-.12f,.20f,.19f,.22f,0.0f,0.0f},{0.0f,.22f,.19f,.27f,0.0f,0.0f},{.14f,.225f,.18f,.34f,0.0f,0.0f},
    {.30f,.21f,.17f,.34f,0.0f,0.0f},{.48f,.185f,.15f,.27f,0.0f,0.0f},{.66f,.16f,.13f,.19f,0.0f,0.0f},
    {.84f,.14f,.12f,.14f,0.0f,0.0f},{1.0f,.135f,.115f,.125f,0.0f,0.0f},{1.07f,.115f,.10f,.10f,0.0f,0.0f}}};
constexpr std::array<LimbStation,8> Metatarsus{{
    {-.10f,.125f,.125f,.125f,0.0f,0.0f},{0.0f,.14f,.135f,.15f,0.0f,0.0f},{.20f,.13f,.12f,.12f,0.0f,0.0f},
    {.45f,.125f,.105f,.105f,0.0f,0.0f},{.70f,.13f,.105f,.105f,0.0f,0.0f},{.90f,.15f,.11f,.11f,0.0f,0.0f},
    {1.0f,.16f,.11f,.11f,0.0f,0.0f},{1.06f,.135f,.085f,.085f,0.0f,0.0f}}};
constexpr std::array<LimbStation,6> UpperArm{{
    {-.25f,.10f,.10f,.10f,0.0f,0.0f},{0.0f,.11f,.11f,.11f,0.0f,0.0f},{.30f,.105f,.12f,.10f,0.0f,0.0f},
    {.60f,.08f,.09f,.075f,0.0f,0.0f},{.90f,.068f,.07f,.065f,0.0f,0.0f},{1.08f,.06f,.06f,.06f,0.0f,0.0f}}};
constexpr std::array<LimbStation,5> Forearm{{
    {-.15f,.06f,.06f,.06f,0.0f,0.0f},{0.0f,.062f,.06f,.06f,0.0f,0.0f},{.50f,.055f,.055f,.05f,0.0f,0.0f},
    {1.0f,.045f,.045f,.04f,0.0f,0.0f},{1.10f,.04f,.04f,.035f,0.0f,0.0f}}};
// a toe from the ball of the foot to its tip: half width, top and bottom (the pad), with the
// knuckles standing out
constexpr std::array<LimbStation,8> Toe{{
    {-.12f,.080f,.056f,.050f,.10f,0.0f},{0.0f,.092f,.062f,.055f,.12f,0.0f},{.22f,.083f,.056f,.052f,.12f,0.0f},
    {.40f,.086f,.058f,.050f,.12f,0.0f},{.58f,.074f,.050f,.046f,.12f,0.0f},{.74f,.076f,.050f,.044f,.12f,0.0f},
    {.88f,.064f,.044f,.038f,.10f,0.0f},{1.0f,.048f,.035f,.030f,.05f,0.0f}}};

// The head's frame on the pose: the neck bends a little toward where it looks, the head turns
// and lifts on its top; the lower jaw swings down about its hinge by the gape.
struct HeadAxes {V3 pivot,right,up,ahead,hinge,jawUp,jawAhead;float gape=0;};
HeadAxes HeadAxesOf(const TRexState& s,const V3& O,const V3& R,const V3& U,const V3& A) {
    HeadAxes h;
    const float yaw=s.neckYaw,pitch=s.neckPitch;
    const float bend=yaw*.45f;
    const V3 reach=NeckTop-NeckBase;
    h.pivot=O+R*(NeckBase.x+reach.z*std::sin(bend))+U*(NeckBase.y+reach.y)+A*(NeckBase.z+reach.z*std::cos(bend));
    const V3 yawAhead=Unit(A*std::cos(yaw)+R*std::sin(yaw));
    h.right=Unit(R*std::cos(yaw)-A*std::sin(yaw));
    h.ahead=Unit(yawAhead*std::cos(pitch)+U*std::sin(pitch));
    h.up=Unit(U*std::cos(pitch)-yawAhead*std::sin(pitch));
    h.gape=std::clamp(s.jaw,0.0f,1.0f)*.62f;
    h.jawAhead=Unit(h.ahead*std::cos(h.gape)-h.up*std::sin(h.gape));
    h.jawUp=Unit(h.up*std::cos(h.gape)+h.ahead*std::sin(h.gape));
    h.hinge=h.pivot+h.up*HingeY+h.ahead*HingeZ;
    return h;
}

// The painting rules the parts are coloured by once, at rest.
enum class Rule : std::uint8_t {Body,Skull,Jaw,Limb,Foot,Tooth,Claw,Eye,Mouth,Nostril};
struct Paint {Rule rule=Rule::Body;float scale=.05f;};

// the legs solved in the body's own frame for a standing (or lying) rest, as the motion solves
// them: the ball of each foot under the hip, the metatarsus up at sixty degrees, the knee forward
void SolveLegs(TRexState& s,const float lie) {
    const float ground=-s.hipHeight;
    for(int i=0;i<2;++i) {
        const float side=i==0?-1.0f:1.0f;
        TRexLeg& leg=s.legs[static_cast<std::size_t>(i)];
        const float along=.62f*lie;
        leg.ball={side*.52f,ground+.07f,along+.30f};
        for(int t=0;t<3;++t) {
            const float splay=(static_cast<float>(t)-1.0f)*side*.36f;
            const float length=TRexMotion::ToeLength*(t==1?1.0f:.82f);
            leg.toes[static_cast<std::size_t>(t)]={leg.ball.x+std::sin(splay)*length,ground+.02f,leg.ball.z+std::cos(splay)*length};
        }
        const float rise=TRexMotion::MetatarsusLength*(.866f*(1.0f-lie)+.12f*lie);
        const float back=TRexMotion::MetatarsusLength*(.5f*(1.0f-lie)+.99f*lie);
        leg.ankle={leg.ball.x,leg.ball.y+rise,leg.ball.z-back};
        leg.hip={side*TRexMotion::HipAcross,-.10f,-.05f};
        const V3 hip=P(leg.hip);
        V3 d=P(leg.ankle)-hip;
        float reach=Length(d);
        const float most=TRexMotion::FemurLength+TRexMotion::TibiaLength-.02f;
        if(reach>most) {d=d*(most/reach);reach=most;leg.ankle=Q(hip+d);}
        const V3 u=d*(1.0f/reach);
        const float a=(TRexMotion::FemurLength*TRexMotion::FemurLength-TRexMotion::TibiaLength*TRexMotion::TibiaLength+reach*reach)/(2.0f*reach);
        const float h=std::sqrt(std::max(0.0f,TRexMotion::FemurLength*TRexMotion::FemurLength-a*a));
        const V3 bend=Unit(V3{0,0,1}-u*u.z);
        leg.knee=Q(hip+u*a+bend*h);
        leg.planted=true;
    }
}
}

// The frame the body is built in at rest: turned as the world turns it (its right is its ahead
// crossed with its up), so the rest pose is no mirror image of the posed one and the triangles
// turned out at rest stay turned out. Its right is therefore toward -x.
namespace {
TRexBody::Frame RestFrame() {return {{0,0,0},{-1,0,0},{0,1,0},{0,0,1}};}
// a state whose leg joints are in the body's own frame, carried into the rest frame
TRexState RestWorld(TRexState state) {
    for(TRexLeg& leg:state.legs) {
        for(TRexPoint* p:{&leg.hip,&leg.knee,&leg.ankle,&leg.ball,&leg.toes[0],&leg.toes[1],&leg.toes[2]}) p->x=-p->x;
    }
    return state;
}
}

struct TRexBody::Recording {
    std::vector<Paint> paints;
    std::vector<std::vector<V3>> centres;   // each patch's rows' centres at rest
    std::vector<float> along;               // each vertex's place along its part (0..1)
    std::vector<std::optional<V3>> rests;   // a vertex's own rest coordinates (the eyes')
};

TRexState TRexBody::RestState(const float lie) {
    TRexState s;
    const float k=Smooth(lie);
    s.lie=std::clamp(lie,0.0f,1.0f);
    s.hipHeight=TRexMotion::HipHeight-.02f+(TRexMotion::LieHipHeight-TRexMotion::HipHeight)*k;
    s.behavior=lie>.5f?TRexBehavior::Rest:TRexBehavior::Stand;
    s.neckPitch=-.30f*k;
    SolveLegs(s,s.lie);
    return s;
}

TRexBody::~TRexBody()=default;

void TRexBody::Build(const TRexState& s,const Frame& frame) {
    const V3 O=P(frame.origin),R=P(frame.right),U=P(frame.up),A=P(frame.ahead);
    const auto local=[&](const float x,const float y,const float z) {return O+R*x+U*y+A*z;};
    std::size_t cursor=0;
    const auto put=[&](const V3& p) {
        if(recording_) vertices_.emplace_back();
        if(cursor<vertices_.size()) {
            TRexSkinVertex& v=vertices_[cursor];
            v.position[0]=p.x;v.position[1]=p.y;v.position[2]=p.z;
        }
        ++cursor;
    };
    // a closed loft through the rings, `sides` points round each, capped at either end by a
    // pole set out from the end ring (or at the point given); `eye`: the rings' places along a
    // unit sphere's axis, whose coordinates the vertices keep at rest
    const auto loft=[&](const std::vector<Ring>& rings,const int sides,const bool startPole,const bool endPole,const Paint& paint,
                        const std::optional<V3> startAt=std::nullopt,const std::optional<V3> endAt=std::nullopt,
                        const std::vector<float>* eye=nullptr,const std::function<V3(const Ring&,const V3&)>* displace=nullptr) {
        const std::size_t first=cursor;
        for(const Ring& r:rings)
            for(int k=0;k<sides;++k) {
                const V3 point=RingPoint(r,Angle(k,sides));
                put(displace?(*displace)(r,point):point);
            }
        const auto pole=[](const Ring& end,const Ring& next) {
            const float reach=.6f*std::min(end.s.w,.5f*(end.s.top+end.s.bottom));
            return end.centre+Unit(end.centre-next.centre,end.up)*reach;
        };
        if(startPole) put(startAt?*startAt:pole(rings.front(),rings[1]));
        if(endPole) put(endAt?*endAt:pole(rings.back(),rings[rings.size()-2]));
        if(!recording_) return;
        patches_.push_back({static_cast<std::uint32_t>(first),static_cast<int>(rings.size()),sides,true,startPole,endPole});
        Recording& rec=*recording_;
        std::vector<V3> centres;
        for(const Ring& r:rings) centres.push_back(r.centre);
        rec.centres.push_back(centres);
        const std::size_t count=cursor-first;
        rec.paints.insert(rec.paints.end(),count,paint);
        for(std::size_t i=0;i<rings.size();++i)
            for(int k=0;k<sides;++k) {
                rec.along.push_back(rings.size()>1?static_cast<float>(i)/static_cast<float>(rings.size()-1):0.0f);
                if(eye) {
                    const float t=(*eye)[i],r=std::sqrt(std::max(0.0f,1.0f-t*t)),a=Angle(k,sides);
                    rec.rests.push_back(V3{std::cos(a)*r,std::sin(a)*r,t});
                } else rec.rests.push_back(std::nullopt);
            }
        if(startPole) {rec.along.push_back(0.0f);rec.rests.push_back(eye?std::optional<V3>(V3{0,0,-1}):std::nullopt);}
        if(endPole) {rec.along.push_back(1.0f);rec.rests.push_back(eye?std::optional<V3>(V3{0,0,1}):std::nullopt);}
    };
    // an open sheet of rows by columns of points
    const auto sheet=[&](const std::vector<V3>& points,const int rows,const int columns,const Paint& paint) {
        const std::size_t first=cursor;
        for(const V3& p:points) put(p);
        if(!recording_) return;
        patches_.push_back({static_cast<std::uint32_t>(first),rows,columns,false,false,false});
        Recording& rec=*recording_;
        rec.centres.emplace_back();
        rec.paints.insert(rec.paints.end(),points.size(),paint);
        for(int r=0;r<rows;++r)
            for(int c=0;c<columns;++c) {
                rec.along.push_back(static_cast<float>(r)/static_cast<float>(std::max(1,rows-1)));
                rec.rests.push_back(std::nullopt);
            }
    };
    // an ellipsoid about `centre` with radii along x, y and its axis z, from behind to before
    const auto ellipsoid=[&](const V3& centre,const V3& x,const V3& y,const V3& z,const float rx,const float ry,const float rz,
                             const int rows,const int sides,const Paint& paint,const bool eye=false) {
        std::vector<Ring> rings;
        std::vector<float> axis;
        for(int i=0;i<rows;++i) {
            const float t=std::sin(-.5f*Pi+Pi*static_cast<float>(i+1)/static_cast<float>(rows+1));
            const float r=std::sqrt(std::max(0.0f,1.0f-t*t));
            rings.push_back({centre+z*(rz*t),x,y,{rx*r,ry*r,ry*r,0.0f,2.0f}});
            axis.push_back(t);
        }
        loft(rings,sides,true,true,paint,centre-z*rz,centre+z*rz,eye?&axis:nullptr);
    };
    // a limb from one joint to the next through its stations, its front toward `front`
    const auto limb=[&](const V3& from,const V3& to,const V3& out,const V3& front,const auto& stations,const int sides,
                        const Paint& paint) {
        const V3 axis=Unit(to-from,-U);
        const float length=Length(to-from);
        const V3 f=Unit(front-axis*Dot(front,axis),Unit(Cross(out,axis)));
        const V3 x=Unit(out-axis*Dot(out,axis)-f*Dot(out,f),Unit(Cross(axis,f)));
        std::vector<Ring> rings;
        for(const LimbStation& st:stations) rings.push_back({from+axis*(st.t*length)-x*st.inward,x,f,{st.w,st.front,st.back,st.pear,2.1f}});
        loft(rings,sides,true,true,paint);
    };

    // ---- the head's frame and the jaw's
    const HeadAxes axes=HeadAxesOf(s,O,R,U,A);
    const V3 pivot=axes.pivot,hr=axes.right,hu=axes.up,ha=axes.ahead,hinge=axes.hinge,jawUp=axes.jawUp,jawAhead=axes.jawAhead;
    const float gape=axes.gape;
    const auto head=[&](const float x,const float y,const float z) {return pivot+hr*x+hu*y+ha*z;};
    const auto jaw=[&](const float x,const float y,const float z) {return hinge+hr*x+jawUp*y+jawAhead*z;};
    const float lie=Smooth(s.lie);

    // ---- the body: the tail swinging against the stride (and curled and laid down at rest),
    // the chest breathing, on into the neck and the head
    {
        std::vector<Ring> rings;
        std::vector<V3> rights,ups;
        const float breath=std::sin(s.breath);
        for(const Station& st:Resample(Torso,3)) {
            const float behind=std::max(0.0f,-st.z-.8f);
            const float sway=s.tailSway*std::pow(behind,1.35f)*.30f+lie*.55f*std::pow(behind/5.8f,2.0f);
            const float droop=lie*std::clamp((-st.z-.8f)/3.0f,0.0f,1.0f)*.55f;
            Section section=st.s;
            const float chest=st.z>.2f&&st.z<2.7f?std::sin(Pi*(st.z-.2f)/2.5f):0.0f;
            section.w*=1.0f+.012f*breath*chest;
            section.bottom*=1.0f+.015f*breath*chest;
            rings.push_back({local(sway,st.y-droop,st.z),R,U,section});
            rights.push_back(R);ups.push_back(U);
        }
        // the neck: a curve from the chest rising into the back of the skull
        const Ring chest=rings.back();
        const V3 p0=chest.centre,t0=Unit(A+U*.30f);
        const V3 p3=head(0.0f,-.12f,-.02f),t3=Unit(ha*.70f+hu*.72f);
        const V3 p1=p0+t0*.50f,p2=p3-t3*.42f;
        const Section end{.33f,.26f,.40f,.12f,2.2f};
        constexpr int NeckRings=10;
        for(int j=1;j<=NeckRings;++j) {
            const float u=static_cast<float>(j)/NeckRings,v=1.0f-u;
            const V3 c=p0*(v*v*v)+p1*(3.0f*v*v*u)+p2*(3.0f*v*u*u)+p3*(u*u*u);
            Section section=MixSection(chest.s,end,Smooth(u));
            const float swell=std::sin(Pi*u);
            section.top+=.07f*swell*(1.0f-.3f*u);section.bottom+=.05f*swell;section.w+=.02f*swell;
            rings.push_back({c,R,U,section});
            rights.push_back(Unit(Lerp(R,hr,std::pow(u,1.2f))));ups.push_back(Unit(Lerp(U,hu,u)));
        }
        Orient(rings,rights,ups);
        loft(rings,36,true,true,{Rule::Body,.040f});
    }
    // ---- the skull, deep and boxy, broad at the cheeks, narrow over the snout
    {
        std::vector<Ring> rings;
        for(const Station& st:Resample(Skull,2)) rings.push_back({head(0.0f,st.y,st.z),hr,hu,st.s});
        const std::function<V3(const Ring&,const V3&)> model=[&](const Ring& ring,const V3& point) {
            const V3 d=point-pivot;
            const float x=std::abs(Dot(d,hr)),y=Dot(d,hu),z=Dot(d,ha);
            float height=0.0f;
            for(const Bump& b:SkullBumps) {
                const float qx=(x-b.x)/b.rx,qy=(y-b.y)/b.ry,qz=(z-b.z)/b.rz;
                height+=b.height*std::exp(-2.0f*(qx*qx+qy*qy+qz*qz));
            }
            return point+Unit(point-ring.centre,hu)*height;
        };
        loft(rings,32,true,true,{Rule::Skull,.046f},std::nullopt,std::nullopt,nullptr,&model);
    }
    // ---- the lower jaw on its hinge
    {
        std::vector<Ring> rings;
        for(const Station& st:Resample(Jaw,2)) rings.push_back({jaw(0.0f,st.y,st.z),hr,jawUp,st.s});
        loft(rings,24,true,true,{Rule::Jaw,.036f});
    }
    const auto skullWidth=[](const float z) {return At(Skull,z).s.w;};
    const auto palate=[&](const float z,const float across) {
        const Station st=At(Skull,z);
        return st.y-SurfaceHeight(st.s,across,-1.0f);
    };
    const auto jawWidth=[](const float z) {return At(Jaw,z).s.w;};
    const auto gum=[&](const float z,const float across) {
        const Station st=At(Jaw,z);
        return st.y+SurfaceHeight(st.s,across,1.0f);
    };
    // ---- the mouth's lining: the cheeks from the corners of the mouth back and the throat
    // between them, stretched open as the jaw drops (shut, it folds away inside the head)
    {
        constexpr int Columns=11,Rows=4;
        struct Edge {float side,z,across;};
        static constexpr std::array<Edge,Columns> Path{{{-1.0f,.42f,.62f},{-1.0f,.28f,.62f},{-1.0f,.13f,.62f},{-1.0f,-.02f,.62f},
                                                       {-1.0f,-.04f,.35f},{0.0f,-.04f,0.0f},{1.0f,-.04f,.35f},
                                                       {1.0f,-.02f,.62f},{1.0f,.13f,.62f},{1.0f,.28f,.62f},{1.0f,.42f,.62f}}};
        std::vector<V3> points;
        points.reserve(static_cast<std::size_t>(Rows*Columns));
        for(int r=0;r<Rows;++r) {
            const float t=static_cast<float>(r)/static_cast<float>(Rows-1);
            for(const Edge& e:Path) {
                const float zj=e.z-HingeZ;
                const V3 upper=head(e.side*e.across*skullWidth(e.z),palate(e.z,e.across)+.07f,e.z);
                const V3 lower=jaw(e.side*e.across*jawWidth(zj),gum(zj,e.across)-.05f,zj);
                const bool corner=e.z>.40f;
                const float bulge=e.across>.5f?.03f*std::sin(Pi*t):0.0f;
                points.push_back(corner?Lerp(upper,lower,.5f):Lerp(upper,lower,t)+hr*(e.side*bulge));
            }
        }
        sheet(points,Rows,Columns,{Rule::Mouth,0.0f});
    }
    // ---- the teeth: stout blades along both jaws, curved back a little, hidden by the lips
    // with the mouth shut and bared as it opens
    {
        const float bared=std::clamp((gape-.06f)/.15f,0.0f,1.0f);
        static constexpr std::array<float,11> Upper{.060f,.085f,.100f,.110f,.105f,.095f,.085f,.075f,.065f,.055f,.045f};
        static constexpr std::array<float,11> Lower{.070f,.090f,.095f,.090f,.085f,.080f,.070f,.060f,.055f,.050f,.040f};
        for(const float side:{-1.0f,1.0f}) {
            for(std::size_t k=0;k<Upper.size();++k) {
                const float z=.38f+.098f*static_cast<float>(k);
                const float across=.62f;
                const V3 base=head(side*across*skullWidth(z),palate(z,across)+.05f,z);
                const V3 down=Unit(-hu-ha*.12f-hr*(side*.06f));
                const float length=.045f+bared*Upper[k];
                const float size=1.0f+3.2f*Upper[k];
                std::vector<Ring> rings{{base,hr,ha,{.016f*size,.024f*size,.024f*size,0.0f,2.0f}},
                                        {base+down*(.5f*length)-ha*(.008f*bared),hr,ha,{.012f*size,.017f*size,.017f*size,0.0f,2.0f}}};
                loft(rings,6,true,true,{Rule::Tooth,0.0f},std::nullopt,base+down*length-ha*(.014f*bared));
            }
            for(std::size_t k=0;k<Lower.size();++k) {
                const float zj=.40f+.088f*static_cast<float>(k);
                const float across=.60f;
                const V3 base=jaw(side*across*jawWidth(zj),gum(zj,across)-.05f,zj);
                const V3 up=Unit(jawUp+jawAhead*.10f-hr*(side*.05f));
                const float length=.045f+bared*Lower[k];
                const float size=1.0f+3.2f*Lower[k];
                std::vector<Ring> rings{{base,hr,jawAhead,{.015f*size,.022f*size,.022f*size,0.0f,2.0f}},
                                        {base+up*(.5f*length)+jawAhead*(.006f*bared),hr,jawAhead,{.011f*size,.016f*size,.016f*size,0.0f,2.0f}}};
                loft(rings,6,true,true,{Rule::Tooth,0.0f},std::nullopt,base+up*length+jawAhead*(.010f*bared));
            }
        }
    }
    // ---- the eyes, set forward for its binocular sight under the brows the skull is modelled
    // with, and the nostrils near the snout's tip
    const float eyeX=[] {
        // the skull's side at the eye's height, the eye sunk a little into it
        const Station st=At(Skull,EyeAt.z);
        const float e=2.0f/st.s.square,rise=std::clamp((EyeAt.y-st.y)/st.s.top,0.0f,1.0f);
        const float sine=std::pow(rise,1.0f/e),cosine=std::sqrt(std::max(0.0f,1.0f-sine*sine));
        return st.s.w*std::pow(cosine,e)*(1.0f-st.s.pear*sine)-.026f;
    }();
    for(const float side:{-1.0f,1.0f}) {
        const V3 centre=head(side*eyeX,EyeAt.y,EyeAt.z);
        const V3 out=Unit(hr*(side*.74f)+ha*.60f+hu*.10f);
        const V3 across=Unit(Cross(hu,out)),up=Unit(Cross(out,across));
        ellipsoid(centre,across,up,out,.054f,.054f,.054f,6,10,{Rule::Eye,0.0f},true);
        // the nostril: a dark slit on the side of the snout's tip, open forward and out
        const float z=1.36f;
        const Station snout=At(Skull,z);
        const float rim=.80f;
        const V3 surface=head(side*rim*snout.s.w,snout.y+SurfaceHeight(snout.s,rim,1.0f)*.55f,z);
        const V3 normal=Unit(hr*(side*.80f)+hu*.45f+ha*.25f),along=Unit(ha-hu*.15f);
        const V3 nostrilAcross=Unit(Cross(normal,along));
        ellipsoid(surface-normal*.010f,nostrilAcross,normal,along,.016f,.014f,.042f,4,8,{Rule::Nostril,0.0f});
    }
    // ---- the legs from the motion's joints: the thigh's great mass flaring into the hips and
    // the tail's base, the shin and its drumstick, the metatarsus, the toes with their pads and
    // claws, and the dewclaw high behind
    for(std::size_t i=0;i<2;++i) {
        const TRexLeg& leg=s.legs[i];
        const float side=i==0?-1.0f:1.0f;
        const V3 out=R*side;
        const V3 hip=P(leg.hip),knee=P(leg.knee),ankle=P(leg.ankle),ball=P(leg.ball);
        limb(hip,knee,out,A,Thigh,24,{Rule::Limb,.034f});
        limb(knee,ankle,out,A,Shin,18,{Rule::Limb,.030f});
        limb(ankle,ball,out,A+U,Metatarsus,14,{Rule::Limb,.030f});
        for(std::size_t t=0;t<3;++t) {
            const V3 tip=P(leg.toes[t]);
            const V3 base=ball;
            const V3 dir=Unit(tip-base,A);
            const float length=Length(tip-base);
            const float wide=t==1?1.1f:1.0f;
            std::vector<Ring> rings;
            std::vector<V3> rights,ups;
            for(const LimbStation& st:Toe) {
                rings.push_back({base+dir*(st.t*length)+U*(.025f*std::clamp(st.t,0.0f,1.0f)),R,U,{st.w*wide,st.front,st.back,st.pear,2.4f}});
                rights.push_back(Unit(Cross(dir,U),R));ups.push_back(U);
            }
            Orient(rings,rights,ups);
            loft(rings,12,true,true,{Rule::Foot,.022f});
            // the claw, curving down to its point
            const float clawLength=t==1?.14f:.12f;
            const V3 root=tip+U*.030f-dir*.01f;
            const V3 hook=Unit(dir-U*.35f);
            std::vector<Ring> claw{{root,R,U,{.030f,.034f,.030f,0.0f,2.0f}},
                                   {root+hook*(.40f*clawLength)+U*.004f,R,U,{.024f,.026f,.022f,0.0f,2.0f}},
                                   {root+hook*(.75f*clawLength)-U*.010f,R,U,{.015f,.015f,.013f,0.0f,2.0f}}};
            std::vector<V3> clawRights(3,Unit(Cross(dir,U),R)),clawUps(3,U);
            Orient(claw,clawRights,clawUps);
            loft(claw,8,true,true,{Rule::Claw,0.0f},std::nullopt,root+hook*clawLength-U*.035f);
        }
        // the dewclaw, small and clear of the ground, behind and inside the metatarsus
        {
            const V3 metatarsus=Unit(ball-ankle,-U);
            const V3 root=ankle+metatarsus*(.70f*Length(ball-ankle))-out*.05f-A*.07f;
            const V3 dir=Unit(-A*.55f-U*.60f-out*.25f);
            std::vector<Ring> rings;
            std::vector<V3> rights,ups;
            for(int k=0;k<4;++k) {
                const float t=static_cast<float>(k)/3.0f;
                rings.push_back({root+dir*(.14f*t),R,U,{.034f-.010f*t,.032f-.008f*t,.030f-.008f*t,0.0f,2.0f}});
                rights.push_back(out);ups.push_back(-A);
            }
            Orient(rings,rights,ups);
            loft(rings,8,true,true,{Rule::Foot,.018f});
            const V3 root2=root+dir*.15f;
            const V3 hook=Unit(dir-U*.4f);
            std::vector<Ring> claw{{root2,R,U,{.020f,.020f,.020f,0.0f,2.0f}},{root2+hook*.03f,R,U,{.014f,.014f,.014f,0.0f,2.0f}}};
            std::vector<V3> clawRights(2,out),clawUps(2,-A);
            Orient(claw,clawRights,clawUps);
            loft(claw,6,true,true,{Rule::Claw,0.0f},std::nullopt,root2+hook*.065f);
        }
    }
    // ---- the arms: small and stout under the chest, the elbows out, two clawed fingers each,
    // stirring with the breath; folded close lying down
    for(const float side:{-1.0f,1.0f}) {
        const float stir=.02f*std::sin(s.breath+.8f);
        const V3 out=R*side;
        const V3 shoulder=local(side*.40f,-.42f,2.62f);
        const V3 elbow=local(side*(.50f-.06f*lie),-.86f+.20f*lie,2.78f+stir);
        const V3 wrist=local(side*(.47f-.06f*lie),-.90f+.18f*lie,3.04f+stir+.04f*lie);
        limb(shoulder,elbow,out,A,UpperArm,10,{Rule::Limb,.022f});
        limb(elbow,wrist,out,A,Forearm,10,{Rule::Limb,.020f});
        for(int f=0;f<2;++f) {
            const V3 tip=wrist+A*(f==0?.12f:.15f)-U*(f==0?.06f:.05f)+out*(f==0?-.025f:.018f);
            const V3 dir=Unit(tip-wrist);
            std::vector<Ring> rings;
            std::vector<V3> rights,ups;
            for(int k=0;k<4;++k) {
                const float t=-.05f+1.05f*static_cast<float>(k)/3.0f;
                const float r=(f==0?.031f:.028f)-.010f*t;
                rings.push_back({wrist+(tip-wrist)*t,R,U,{r,r,r*.9f,0.0f,2.0f}});
                rights.push_back(out);ups.push_back(U);
            }
            Orient(rings,rights,ups);
            loft(rings,6,true,true,{Rule::Limb,.016f});
            const V3 hook=Unit(dir-U*.6f);
            const float clawLength=f==0?.075f:.06f;
            std::vector<Ring> claw{{tip,R,U,{.018f,.020f,.018f,0.0f,2.0f}},{tip+hook*(.45f*clawLength),R,U,{.012f,.013f,.011f,0.0f,2.0f}}};
            std::vector<V3> clawRights(2,out),clawUps(2,U);
            Orient(claw,clawRights,clawUps);
            loft(claw,6,true,true,{Rule::Claw,0.0f},std::nullopt,tip+hook*clawLength-U*.02f);
        }
    }
}

void TRexBody::Normals() {
    accumulated_.assign(vertices_.size()*3U,0.0f);
    for(std::size_t i=0;i+2<indices_.size();i+=3) {
        const std::uint16_t ia=indices_[i],ib=indices_[i+1],ic=indices_[i+2];
        const auto at=[&](const std::uint16_t k) {const float* p=vertices_[k].position;return V3{p[0],p[1],p[2]};};
        const V3 a=at(ia),n=Cross(at(ib)-a,at(ic)-a);
        for(const std::uint16_t k:{ia,ib,ic}) {
            accumulated_[k*3U]+=n.x;accumulated_[k*3U+1U]+=n.y;accumulated_[k*3U+2U]+=n.z;
        }
    }
    for(std::size_t k=0;k<vertices_.size();++k) {
        const V3 n=Unit(V3{accumulated_[k*3U],accumulated_[k*3U+1U],accumulated_[k*3U+2U]});
        vertices_[k].normal[0]=n.x;vertices_[k].normal[1]=n.y;vertices_[k].normal[2]=n.z;
    }
}

TRexBody::TRexBody() {
    Recording recording;
    recording_=&recording;
    Build(RestWorld(RestState()),RestFrame());
    recording_=nullptr;
    // ---- the triangles: each patch's quads and its poles' fans, each turned to face out of its
    // part at rest (a sheet's either way: the shader lights the side it sees)
    for(std::size_t p=0;p<patches_.size();++p) {
        const Patch& patch=patches_[p];
        const auto& centres=recording.centres[p];
        const auto index=[&](const int r,const int c) {
            return static_cast<std::uint16_t>(patch.first+static_cast<std::uint32_t>(r*patch.columns+(c%patch.columns)));
        };
        const auto position=[&](const std::uint16_t k) {const float* q=vertices_[k].position;return V3{q[0],q[1],q[2]};};
        const auto triangle=[&](std::uint16_t a,std::uint16_t b,std::uint16_t c,const V3& inside) {
            if(patch.wrap) {
                const V3 pa=position(a),n=Cross(position(b)-pa,position(c)-pa);
                const V3 centroid=(pa+position(b)+position(c))*(1.0f/3.0f);
                if(Dot(n,centroid-inside)<0.0f) std::swap(b,c);
            }
            indices_.push_back(a);indices_.push_back(b);indices_.push_back(c);
        };
        const int across=patch.wrap?patch.columns:patch.columns-1;
        for(int r=0;r+1<patch.rows;++r)
            for(int c=0;c<across;++c) {
                const V3 inside=patch.wrap?(centres[static_cast<std::size_t>(r)]+centres[static_cast<std::size_t>(r+1)])*.5f:V3{};
                triangle(index(r,c),index(r,c+1),index(r+1,c+1),inside);
                triangle(index(r,c),index(r+1,c+1),index(r+1,c),inside);
            }
        auto pole=static_cast<std::uint16_t>(patch.first+static_cast<std::uint32_t>(patch.rows*patch.columns));
        if(patch.startPole) {
            for(int c=0;c<patch.columns;++c) triangle(pole,index(0,c+1),index(0,c),centres.front());
            ++pole;
        }
        if(patch.endPole)
            for(int c=0;c<patch.columns;++c) triangle(pole,index(patch.rows-1,c),index(patch.rows-1,c+1),centres.back());
    }
    Normals();
    // ---- the pigment, the hollows, the scales and the surfaces, from the pose at rest
    const V3 pivot=NeckTop;
    const V3 back{58,50,36},flank{128,108,78},belly{194,180,148};
    const V3 sand{176,158,122};
    const auto noise=[](const V3& p,const float scale) {return Noise(p*scale);};
    for(std::size_t k=0;k<vertices_.size();++k) {
        TRexSkinVertex& v=vertices_[k];
        const V3 p{v.position[0],v.position[1],v.position[2]},n{v.normal[0],v.normal[1],v.normal[2]};
        const Paint& paint=recording.paints[k];
        const float along=recording.along[k];
        const V3 rest=recording.rests[k]?*recording.rests[k]:p;
        V3 colour=flank;
        float light=1.0f,gloss=.16f,plain=1.0f;
        TRexMaterial material=TRexMaterial::Hide;
        // the countershading of a big animal in the sun: dark above, pale beneath, mottled, with
        // faint broken bands across the back and the tail
        const auto hide=[&](const float contrast) {
            const float up=n.y;
            V3 c=up>-.15f?Lerp(flank,back,Step(-.15f,.70f,up)*contrast):Lerp(flank,belly,Step(-.15f,-.70f,up)*contrast);
            const float band=Step(.40f,.85f,.5f+.5f*std::sin(p.z*2.0f*Pi/.95f+2.4f*noise(p,.9f)))*Step(-.20f,.50f,up);
            c=c*(1.0f-.30f*band*(p.z>2.9f?.5f:1.0f));
            c=c*(.86f+.26f*noise(p,2.3f)+.10f*(noise(p,7.0f)-.5f));
            return c;
        };
        switch(paint.rule) {
        case Rule::Body: {
            colour=hide(1.0f);
            if(n.y<-.3f) light*=Mix(1.0f,.80f,Step(-.3f,-.85f,n.y));
            if(std::abs(p.x)>.30f&&p.z>-1.0f&&p.z<.8f&&p.y>-1.2f&&p.y<.5f) light*=.86f;
            if(p.z>3.0f&&n.y<-.2f) light*=.86f;
            break;
        }
        case Rule::Skull: case Rule::Jaw: {
            const V3 h=p-pivot;
            colour=hide(1.0f);
            if(paint.rule==Rule::Jaw) colour=Lerp(colour,V3{146,128,100},.20f);
            // a darker mask through the eyes
            const float mask=1.0f-Step(.08f,.24f,Length(V3{std::abs(h.x)-.33f,h.y-EyeAt.y,h.z-EyeAt.z}));
            colour=colour*(1.0f-.28f*mask);
            // the brows and hornlets rough and darker
            for(std::size_t b=0;b<2;++b) {
                const Bump& bump=SkullBumps[b];
                const float qx=(std::abs(h.x)-bump.x)/bump.rx,qy=(h.y-bump.y)/bump.ry,qz=(h.z-bump.z)/bump.rz;
                colour=colour*(1.0f-.20f*std::exp(-2.0f*(qx*qx+qy*qy+qz*qz)));
            }
            const bool upperJaw=paint.rule==Rule::Skull;
            const float width=upperJaw?At(Skull,h.z).s.w:At(Jaw,h.z-HingeZ).s.w;
            const bool under=(upperJaw?n.y<-.55f:n.y>.55f)&&h.z>-.05f;
            const bool inner=under&&std::abs(h.x)<.38f*width&&h.z<1.10f;
            const bool gum=under&&!inner&&std::abs(h.x)<.62f*width;
            if(gum) {
                // inside the lips, before the palate or the tongue: dark, so the shut mouth is a line
                colour=V3{58,40,36};
                light*=.75f;gloss=.40f;plain=0.0f;
            } else if(inner) {
                // the palate and the tongue between the teeth: wet, pink, smooth
                colour=upperJaw?V3{116,56,54}:V3{138,70,66};
                light*=.78f;gloss=.70f;plain=0.0f;
            } else if(upperJaw?(n.y<-.25f&&n.y>-.62f):(n.y>.18f&&n.y<.62f)) {
                // the lips: a little greyer and smoother
                colour=Lerp(colour,V3{132,118,98},.30f);plain=.55f;gloss=.26f;
            }
            if(!upperJaw&&n.y<-.3f) colour=Lerp(colour,belly,.45f);
            break;
        }
        case Rule::Limb: case Rule::Foot: {
            colour=hide(paint.rule==Rule::Limb?.85f:.4f);
            // darker and scalier down the leg, the feet dusty from the beach
            colour=Lerp(colour,V3{80,72,60},Step(-1.5f,-2.6f,p.y)*.75f);
            colour=Lerp(colour,sand,Step(-2.70f,-3.0f,p.y)*.28f);
            const float inside=std::abs(p.x)>.05f?-n.x*(p.x>0.0f?1.0f:-1.0f):0.0f;
            light*=Mix(1.0f,.72f,Step(.1f,.8f,inside));
            if(paint.rule==Rule::Foot&&n.y<-.3f) {colour=colour*.8f;light*=.75f;}
            if(p.z>2.3f&&p.y>-1.0f&&p.y<-.3f) light*=.8f;   // the arms' pits
            break;
        }
        case Rule::Tooth:
            colour=Lerp(V3{192,170,128},V3{232,222,194},Smooth(along*1.4f));
            material=TRexMaterial::Keratin;gloss=.55f;plain=0.0f;
            break;
        case Rule::Claw:
            colour=Lerp(V3{44,38,32},V3{122,110,94},along*along);
            material=TRexMaterial::Keratin;gloss=.45f;plain=0.0f;
            break;
        case Rule::Eye:
            colour=V3{196,150,52};
            material=TRexMaterial::Eye;gloss=1.0f;plain=0.0f;
            break;
        case Rule::Mouth:
            colour=Lerp(V3{132,58,54},V3{70,26,26},std::abs(p.x)<.25f?.6f:.2f);
            material=TRexMaterial::Mouth;gloss=.75f;plain=0.0f;light=.55f;
            break;
        case Rule::Nostril:
            colour=V3{38,28,24};
            material=TRexMaterial::Nostril;gloss=.30f;plain=0.0f;
            break;
        }
        const auto byte=[](const float value) {return static_cast<std::uint8_t>(std::lround(std::clamp(value,0.0f,255.0f)));};
        v.pigment[0]=byte(colour.x);v.pigment[1]=byte(colour.y);v.pigment[2]=byte(colour.z);
        v.pigment[3]=byte(255.0f*light);
        const float scale=paint.scale*(material==TRexMaterial::Hide?Mix(.80f,1.30f,Step(.15f,.90f,n.y)):1.0f);
        v.rest[0]=rest.x;v.rest[1]=rest.y;v.rest[2]=rest.z;v.rest[3]=scale;
        v.surface[0]=static_cast<std::uint8_t>(material);
        v.surface[1]=byte(255.0f*gloss);
        v.surface[2]=byte(255.0f*plain*(paint.scale>0.0f?1.0f:0.0f));
        v.surface[3]=0;
    }
}

TRexBody::Frame TRexBody::FrameOf(const TRexMotion& rex) {
    const TRexPoint o=rex.ToWorld(0.0f,0.0f,0.0f),r=rex.ToWorld(1.0f,0.0f,0.0f),u=rex.ToWorld(0.0f,1.0f,0.0f),a=rex.ToWorld(0.0f,0.0f,1.0f);
    return {o,{r.x-o.x,r.y-o.y,r.z-o.z},{u.x-o.x,u.y-o.y,u.z-o.z},{a.x-o.x,a.y-o.y,a.z-o.z}};
}

TRexPoint TRexBody::HeadPoint(const TRexState& state,const Frame& frame,const float x,const float y,const float z,const bool jaw) {
    const HeadAxes h=HeadAxesOf(state,P(frame.origin),P(frame.right),P(frame.up),P(frame.ahead));
    return Q(jaw?h.hinge+h.right*x+h.jawUp*y+h.jawAhead*z:h.pivot+h.right*x+h.up*y+h.ahead*z);
}

void TRexBody::Pose(const TRexMotion& rex) {
    Build(rex.State(),FrameOf(rex));
    Normals();
}

void TRexBody::PoseLocal(const TRexState& state) {
    Build(RestWorld(state),RestFrame());
    Normals();
}

}
