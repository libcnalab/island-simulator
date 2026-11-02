#include "IslandSimulator/AlienBodyMesh.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/SurfaceNormals.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <utility>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
const Vector3 Skin(172,178,174),SkinDark(150,156,152),Eye(10,10,14),Glint(150,170,200),Mouth(80,84,84);
Vector3 V(const AlienPoint& p) {return {p.x,p.y,p.z};}
Vector3 Unit(Vector3 v,const Vector3& fallback=Vector3::Up) {
    if(v.LengthSquared()<1e-10f) return fallback;
    v.Normalize();return v;
}
struct BodyBuilder {
    std::vector<VertexPositionColor>& mesh;
    StructureMeshBuilder flat;
    template<class Pigment>
    void Loft(const std::vector<std::vector<Vector3>>& rings,const std::vector<Vector3>& centres,
              const Pigment& pigment,bool capStart=true,bool capEnd=true) {
        if(rings.size()<2||rings.front().size()<3) return;
        const std::size_t n=rings.size(),count=rings.front().size();
        std::vector<std::array<float,3>> positions;
        std::vector<Vector3> pigments;
        std::vector<std::uint16_t> indices;
        positions.reserve(n*count+2);pigments.reserve(n*count+2);indices.reserve(6*n*count);
        for(std::size_t i=0;i<n;++i) for(std::size_t k=0;k<count;++k) {
            const auto p=rings[i][k];positions.push_back({p.X,p.Y,p.Z});
            pigments.push_back(pigment(std::min(i,n-2),k));
        }
        const auto point=[&](std::size_t id) {const auto& p=positions[id];return Vector3(p[0],p[1],p[2]);};
        const auto face=[&](std::size_t a,std::size_t b,std::size_t c,const Vector3& outward) {
            const auto normal=Vector3::Cross(point(b)-point(a),point(c)-point(a));
            if(Vector3::Dot(normal,outward)<0) std::swap(b,c);
            indices.insert(indices.end(),{static_cast<std::uint16_t>(a),static_cast<std::uint16_t>(b),static_cast<std::uint16_t>(c)});
        };
        for(std::size_t i=0;i+1<n;++i) for(std::size_t k=0;k<count;++k) {
            const auto next=(k+1)%count,a=i*count+k,b=(i+1)*count+k;
            const auto c=(i+1)*count+next,d=i*count+next;
            const auto outward=(point(a)+point(b)+point(c)+point(d))*.25f-(centres[i]+centres[i+1])*.5f;
            face(a,b,c,outward);face(a,c,d,outward);
        }
        for(int end=0;end<2;++end) {
            if((end==0&&!capStart)||(end==1&&!capEnd)) continue;
            const std::size_t i=end==0?0:n-1,id=positions.size();const auto p=centres[i];
            positions.push_back({p.X,p.Y,p.Z});pigments.push_back(pigment(end==0?0:n-2,0));
            const auto outward=centres[i]-centres[end==0?1:n-2];
            for(std::size_t k=0;k<count;++k) face(id,i*count+k,i*count+(k+1)%count,outward);
        }
        const auto normals=BuildSurfaceNormals(positions,indices);
        for(const auto id:indices) {
            const auto p=point(id);const auto& a=normals[id];const Vector3 normal(a[0],a[1],a[2]);
            Vector3 colour=pigments[id];
            if(colour.X>=100) colour=colour*(.97f+.03f*std::sin(p.X*8+p.Y*7)*std::sin(p.Z*9-p.Y*5));
            const float light=.46f+.54f*std::max(0.0f,Vector3::Dot(normal,flat.sun));
            const auto channel=[&](float value) {return static_cast<int>(std::clamp(value*light,0.0f,255.0f));};
            mesh.emplace_back(p,Color(channel(colour.X),channel(colour.Y),channel(colour.Z),255));
        }
    }
    void Tube(const std::vector<Vector3>& points,const std::vector<float>& radii,int sides,
              const Vector3& pigment,float squash=1) {
        if(points.size()<2) return;
        std::vector<std::vector<Vector3>> rings;
        Vector3 carried=Vector3::Zero;
        for(std::size_t i=0;i<points.size();++i) {
            const auto tangent=Unit(points[std::min(i+1,points.size()-1)]-points[i==0?0:i-1]);
            auto across=carried-tangent*Vector3::Dot(carried,tangent);
            if(across.LengthSquared()<1e-8f) {
                across=Vector3::Cross(tangent,Vector3::Up);
                if(across.LengthSquared()<1e-8f) across=Vector3::Cross(tangent,Vector3::UnitX);
            }
            carried=Unit(across);const auto up=Vector3::Cross(carried,tangent);
            std::vector<Vector3> ring;
            for(int k=0;k<sides;++k) {
                const float a=2*Pi*static_cast<float>(k)/static_cast<float>(sides);
                ring.push_back(points[i]+carried*(std::cos(a)*radii[i])+up*(std::sin(a)*radii[i]*squash));
            }
            rings.push_back(std::move(ring));
        }
        Loft(rings,points,[&](std::size_t,std::size_t) {return pigment;});
    }
    void Box(const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& ahead,
             float hw,float hh,float hd,const Vector3& pigment,const Vector3& sides) {
        flat.mesh.clear();flat.Box(centre,right,up,ahead,hw,hh,hd,pigment,sides);
        mesh.insert(mesh.end(),flat.mesh.begin(),flat.mesh.end());
    }
};

// An ellipsoid: centre, its three axes and half extents.
void Ellipsoid(BodyBuilder& b,const Vector3& centre,const Vector3& right,const Vector3& up,const Vector3& ahead,
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

void BuildAlienBodyMesh(const AlienState& s,std::vector<VertexPositionColor>& vertices) {
    vertices.clear();BodyBuilder b{vertices,{}};
    const Vector3 body(std::cos(s.heading),0,std::sin(s.heading));
    const Vector3 right(std::sin(s.heading),0,-std::cos(s.heading));
    const Vector3 pelvis=V(s.pelvis),chest=V(s.chest),neck=V(s.neck),head=V(s.head);
    const Vector3 hr=V(s.headRight),hu=V(s.headUp),hf=V(s.headAhead);
    // ---- the body: narrow hips, a slim waist, the chest, lofted along the spine
    {
        struct Station {float t,w,d;};
        static constexpr Station Sections[]{{0,.092f,.065f},{.25f,.078f,.055f},
            {.5f,.092f,.063f},{.76f,.125f,.07f},{.9f,.14f,.065f},{1,.12f,.055f}};
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
    // A shallow deltoid runs down each upper arm, below the chest's top, rather than a joint ball.
    for(const auto& arm:s.arms) {
        const auto down=Unit(V(arm.elbow)-V(arm.shoulder));
        const auto across=Unit(Vector3::Cross(down,body),right);
        const auto depth=Unit(Vector3::Cross(across,down),body);
        Ellipsoid(b,V(arm.shoulder)+down*.045f,across,down,depth,.042f,.065f,.037f,Skin,5,8);
    }
    Ellipsoid(b,pelvis,right,Vector3::Up,body,.101f,.07f,.065f,Skin,5,10);
    // ---- the slender neck, the legs, the arms
    b.Tube({chest,neck,head-hu*0.17f},{0.05f,0.032f,0.04f},8,SkinDark);
    for(const AlienLeg& leg:s.legs) {
        b.Tube({V(leg.hip),V(leg.knee),V(leg.foot)+Vector3(0,0.035f,0)},{0.052f,0.04f,0.028f},7,Skin);
        const Vector3 foot=V(leg.foot);
        Ellipsoid(b,foot+body*.035f+Vector3(0,.024f,0),right,Vector3::Up,body,
                  .036f,.026f,.10f,SkinDark,5,8);
    }
    for(const AlienArm& arm:s.arms) {
        const Vector3 shoulder=V(arm.shoulder),elbow=V(arm.elbow),hand=V(arm.hand);
        b.Tube({shoulder,elbow,hand},{0.036f,0.027f,0.019f},7,Skin);
        // The palm carries separate finger roots instead of three tubes sharing one point.
        const Vector3 dir=Unit(hand-elbow);
        Vector3 across=Unit(Vector3::Cross(dir,Vector3::Up),right);
        const Vector3 palm=Unit(Vector3::Cross(across,dir));
        Ellipsoid(b,hand+dir*.022f,across,dir,palm,.028f,.041f,.014f,Skin,5,8);
        for(int f=0;f<3;++f) {
            const float spread=(static_cast<float>(f)-1.0f)*0.3f;
            const Vector3 root=hand+dir*.045f+across*(static_cast<float>(f)-1)*.018f;
            const Vector3 tip=root+Unit(dir+across*spread-palm*.08f)*(.075f+(f==1?.015f:0));
            b.Tube({root,(root+tip)*.5f-palm*.01f,tip},{0.011f,0.008f,0.005f},5,SkinDark);
        }
        b.Tube({hand,hand+Unit(dir*0.4f+across*0.8f+palm*0.2f)*0.06f},{0.011f,0.006f},5,SkinDark);
    }
    // ---- the head: an egg widest above the eyes, tapering to a small chin
    {
        constexpr int Rings=10,Segments=16;
        constexpr float HalfHeight=0.205f,HalfWidth=0.155f,HalfDepth=0.135f;
        const auto section=[](float t) {
            const float round=std::sqrt(std::max(0.0f,1-t*t));
            const float taper=.55f+.45f*std::clamp((t+1)*.62f,0.0f,1.0f);
            return std::array<float,2>{HalfWidth*round*taper,HalfDepth*round*(.8f+.2f*taper)};
        };
        const auto faceDepth=[&](float height,float x) {
            const auto radii=section(height/HalfHeight);
            const float fraction=x/std::max(.001f,radii[0]);
            return radii[1]*std::sqrt(std::max(0.0f,1-fraction*fraction));
        };
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(int i=0;i<Rings;++i) {
            const float t=-.99f+1.98f*static_cast<float>(i)/static_cast<float>(Rings-1);
            const auto radii=section(t);
            const Vector3 c=head+hu*(t*HalfHeight);
            std::vector<Vector3> ring;
            for(int k=0;k<Segments;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Segments;
                ring.push_back(c+hr*(std::cos(a)*radii[0])+hf*(std::sin(a)*radii[1]));
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
            Ellipsoid(b,head-hu*.08f+hr*(side*.012f)+hf*(faceDepth(-.08f,side*.012f)+.004f),
                      hr,hu,hf,.0035f,.0045f,.003f,Eye,4,6);
        std::vector<Vector3> mouth;
        for(const float x:{-.028f,0.0f,.028f})
            mouth.push_back(head-hu*.137f+hr*x+hf*(faceDepth(-.137f,x)+.003f));
        b.Tube(mouth,{.0018f,.0022f,.0018f},5,Mouth);
    }
}
}
