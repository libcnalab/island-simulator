#include "IslandSimulator/WindthrowRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <numbers>
#include <sstream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// the pigments, past full for the build's light
const Vector3 Bark(118,96,74),BarkDark(80,64,50),Moss(82,112,50),Lichen(168,166,140),Wood(198,170,126),
    Soil(110,84,60),Clay(178,134,88),Humus(62,48,36),Litter(88,70,48),Root(126,98,72),RootEnd(196,170,128),
    Stone(124,116,102),Fungus(214,200,170),FungusTop(128,100,74),Fern(74,122,50),Wet(60,48,38),Puddle(46,56,60);
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Smooth(const float a,const float b,const float x) {
    const float t=std::clamp((x-a)/(b-a),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}

// A grid surface lit at each corner by its own normal (finite differences over the grid, turned
// to `outward`), so the big rounded pieces shade smoothly instead of in facets.
struct SmoothGrid {
    int rows=0,cols=0;bool wrap=false;
    std::vector<Vector3> position,albedo;
    Vector3& P(const int r,const int c) {return position[static_cast<std::size_t>(r*cols+c)];}
    Vector3& A(const int r,const int c) {return albedo[static_cast<std::size_t>(r*cols+c)];}
    SmoothGrid(const int r,const int c,const bool w):rows(r),cols(c),wrap(w),
        position(static_cast<std::size_t>(r*c)),albedo(static_cast<std::size_t>(r*c)) {}
    void Emit(std::vector<VertexPositionColor>& mesh,const Vector3& sun,const std::function<Vector3(int,int)>& outward) {
        std::vector<Vector3> normal(position.size());
        for(int r=0;r<rows;++r) for(int c=0;c<cols;++c) {
            const int c0=wrap?(c+cols-1)%cols:std::max(c-1,0),c1=wrap?(c+1)%cols:std::min(c+1,cols-1);
            const Vector3 du=P(r,c1)-P(r,c0),dv=P(std::min(r+1,rows-1),c)-P(std::max(r-1,0),c);
            Vector3 n=Vector3::Cross(du,dv);
            const Vector3 hint=outward(r,c);
            if(n.LengthSquared()<1e-12f) n=hint;
            n.Normalize();
            if(Vector3::Dot(n,hint)<0.0f) n=-n;
            normal[static_cast<std::size_t>(r*cols+c)]=n;
        }
        const auto lit=[&](const int r,const int c) {
            const auto i=static_cast<std::size_t>(r*cols+c);
            const float light=0.46f+0.54f*std::max(0.0f,Vector3::Dot(normal[i],sun));
            const Vector3 v=albedo[i]*light;
            return VertexPositionColor(position[i],Color(static_cast<int>(std::min(255.0f,v.X)),static_cast<int>(std::min(255.0f,v.Y)),
                                                         static_cast<int>(std::min(255.0f,v.Z)),255));
        };
        const int last=wrap?cols:cols-1;
        for(int r=0;r+1<rows;++r) for(int c=0;c<last;++c) {
            const int cn=(c+1)%cols;
            const auto a=lit(r,c),b=lit(r,cn),d=lit(r+1,c),e=lit(r+1,cn);
            mesh.push_back(a);mesh.push_back(b);mesh.push_back(e);
            mesh.push_back(a);mesh.push_back(e);mesh.push_back(d);
        }
    }
};

class Builder {
public:
    Builder(const WindthrowPlacement& w,std::function<float(float,float)> groundAt)
        :w_(w),ground_(std::move(groundAt)),d_(w.dirX,0.0f,w.dirZ),s_(w.AcrossX(),0.0f,w.AcrossZ()) {
        b_.ground=ground_;b_.stainColour=Soil;b_.stainHeight=0.1f;b_.stainAmount=0.5f;
    }
    // the mesh, the pit's earth last: `casting` is how many vertices before it cast shadows
    std::vector<VertexPositionColor> Build(std::size_t& casting) {
        Trunk();BrokenEnd();Crown();Stubs();Fungi();Ferns();Plate();Pit();
        std::vector<VertexPositionColor> mesh=std::move(smooth_);
        mesh.insert(mesh.end(),b_.builder.mesh.begin(),b_.builder.mesh.end());
        casting=mesh.size();
        mesh.insert(mesh.end(),pit_.begin(),pit_.end());
        return mesh;
    }
private:
    float h(const std::uint32_t salt) const {return Hash(w_.seed*0x9e3779b9U^salt);}
    Vector3 Plan(const float along,const float across) const {
        const auto p=w_.At(along,across);
        return Vector3(p[0],ground_(p[0],p[1]),p[1]);
    }
    // the trunk's axis `along` it: resting on the ground's high points, held up at its foot by the plate
    Vector3 Axis(const float along) const {
        float sum=0.0f;int count=0;
        for(float k=-1.5f;k<=1.51f;k+=0.75f) {
            const float a=std::clamp(along+k,0.0f,WindthrowPlacement::TrunkLength);
            sum+=Plan(a,0.0f).Y+0.82f*WindthrowPlacement::RadiusAt(a);++count;
        }
        const Vector3 ground=Plan(along,0.0f);
        const float r=WindthrowPlacement::RadiusAt(along);
        float y=std::max(sum/static_cast<float>(count),ground.Y+0.6f*r);
        if(along<1.4f) y=std::max(y,w_.groundY+r+0.22f*(1.0f-std::max(along,0.0f)/1.4f));
        return Vector3(ground.X,y,ground.Z);
    }
    Vector3 Radial(const float theta) const {return s_*std::cos(theta)+Vector3::Up*std::sin(theta);}

    void Trunk() {
        constexpr int Sides=22;
        const float phase=2.0f*Pi*h(1U);
        std::vector<float> stations;
        for(float a=-0.3f;a<WindthrowPlacement::TrunkLength;a+=0.45f) stations.push_back(a);
        stations.push_back(WindthrowPlacement::TrunkLength);
        SmoothGrid grid(static_cast<int>(stations.size()),Sides,true);
        for(int i=0;i<grid.rows;++i) {
            const float a=stations[static_cast<std::size_t>(i)];
            const Vector3 axis=Axis(a);
            const float r=WindthrowPlacement::RadiusAt(a);
            for(int k=0;k<Sides;++k) {
                const float theta=2.0f*Pi*static_cast<float>(k)/Sides;
                const float furrow=std::sin(theta*11.0f+a*0.18f+phase);
                const Vector3 dir=Radial(theta);
                const Vector3 p=axis+dir*(r*(1.0f+0.03f*furrow+0.02f*std::sin(theta*5.0f-a*0.4f)));
                grid.P(i,k)=p;
                // furrowed bark in dark streaks, cushions of moss over the top, pale lichen patches
                // on the flanks, dark and earthy beneath
                const float streak=std::sin(theta*23.0f+std::sin(a*0.8f)*2.0f+phase);
                Vector3 c=Bark*(0.78f+0.16f*furrow+0.1f*std::sin(a*1.3f+theta*3.0f)-0.12f*Smooth(0.6f,1.0f,streak));
                const float lichen=Smooth(0.45f,0.85f,std::sin(a*2.1f+theta*4.0f+phase)*std::sin(a*0.7f-theta*2.0f));
                c=Vector3::Lerp(c,Lichen,0.75f*lichen*(1.0f-std::abs(dir.Y)));
                const float cushion=0.5f+0.5f*std::sin(a*1.7f+theta*3.0f+phase)*std::sin(a*0.6f-theta*5.0f+1.3f);
                const float moss=Smooth(0.05f,0.55f,dir.Y)*Smooth(0.25f,0.6f,cushion+0.25f*dir.Y);
                c=Vector3::Lerp(c,Moss*(0.8f+0.3f*std::sin(a*3.1f+theta*7.0f)),std::clamp(moss,0.0f,1.0f));
                c=Vector3::Lerp(c,BarkDark,Smooth(-0.2f,-0.7f,dir.Y)*0.6f);
                const float touch=1.0f-Smooth(0.0f,0.14f,p.Y-ground_(p.X,p.Z));
                grid.A(i,k)=Vector3::Lerp(c,Soil,0.6f*touch);
            }
        }
        grid.Emit(smooth_,b_.builder.sun,[&](const int i,const int k) {
            (void)i;return Radial(2.0f*Pi*static_cast<float>(k)/Sides);
        });
    }
    // the snapped end: pale splintered wood standing out of the break
    void BrokenEnd() {
        const float L=WindthrowPlacement::TrunkLength;
        const Vector3 axis=Axis(L);
        const float r=WindthrowPlacement::RadiusAt(L);
        b_.inside=axis-d_*0.4f;
        constexpr int Rim=14;
        for(int k=0;k<Rim;++k) {
            const float t0=2.0f*Pi*static_cast<float>(k)/Rim,t1=2.0f*Pi*static_cast<float>(k+1)/Rim;
            b_.Face(axis+d_*(0.06f+0.08f*h(40U)),axis+Radial(t0)*r,axis+Radial(t1)*r,Wood*(0.82f+0.12f*h(41U+static_cast<std::uint32_t>(k))));
        }
        for(int j=0;j<11;++j) {
            const auto salt=static_cast<std::uint32_t>(60+j*7);
            const float theta=2.0f*Pi*(static_cast<float>(j)+0.6f*h(salt))/11.0f;
            const float reach=0.25f+0.85f*h(salt^0x1U);
            const Vector3 b1=axis+Radial(theta-0.2f)*(r*0.97f),b2=axis+Radial(theta+0.2f)*(r*0.97f);
            const Vector3 b3=axis+Radial(theta)*(r*(0.45f+0.3f*h(salt^0x2U)));
            const Vector3 tip=axis+d_*reach+Radial(theta)*(r*(0.55f+0.3f*h(salt^0x3U)));
            b_.inside=(b1+b2+b3)/3.0f-Radial(theta)*0.05f;
            b_.Face(b1,b2,tip,Bark*0.9f);
            b_.inside=(b1+b2+b3)/3.0f+Radial(theta)*0.3f;
            b_.Face(b2,b3,tip,Wood);b_.Face(b3,b1,tip,Wood*0.9f);
        }
    }
    // the crown's broken limbs strewn beyond the break, grey and leafless
    void Crown() {
        const float L=WindthrowPlacement::TrunkLength;
        for(int j=0;j<4;++j) {
            const auto salt=static_cast<std::uint32_t>(100+j*13);
            const float turn=(j%2==0?1.0f:-1.0f)*(0.25f+0.6f*h(salt));
            const Vector3 dir=d_*std::cos(turn)+s_*std::sin(turn);
            const float length=2.8f+2.4f*h(salt^0x1U);
            const float start=L-0.6f+1.6f*h(salt^0x2U);
            const Vector3 origin=Plan(start,(h(salt^0x3U)-0.5f)*1.2f);
            std::vector<Vector3> points;std::vector<float> radii;
            for(int k=0;k<=4;++k) {
                const float t=static_cast<float>(k)/4.0f;
                Vector3 p=origin+dir*(length*t);
                const float radius=0.17f-0.12f*t;
                p.Y=ground_(p.X,p.Z)+radius*0.7f+0.25f*(1.0f-t)*h(salt^0x4U);
                points.push_back(p);radii.push_back(radius);
            }
            b_.Tube(points,radii,7,Bark*0.92f);
            for(int k=1;k<=2;++k) {
                const auto twig=salt^static_cast<std::uint32_t>(k*0x3fU);
                const Vector3 from=points[static_cast<std::size_t>(k+1)];
                const float side=(h(twig)<0.5f?-1.0f:1.0f)*(0.6f+0.5f*h(twig^0x1U));
                const Vector3 out=Vector3::Normalize(dir*std::cos(side)+Vector3::Cross(Vector3::Up,dir)*std::sin(side)+Vector3::Up*0.35f);
                const Vector3 to=from+out*(0.9f+0.7f*h(twig^0x2U));
                b_.Tube({from,(from+to)*0.5f+Vector3::Up*0.08f,to},{0.05f,0.035f,0.015f},5,Bark*0.95f);
            }
        }
    }
    // branch stubs snapped short along the trunk
    void Stubs() {
        for(int j=0;j<3;++j) {
            const auto salt=static_cast<std::uint32_t>(200+j*11);
            const float a=5.0f+3.6f*static_cast<float>(j)+1.2f*h(salt);
            const float theta=Pi*0.5f+(j%2==0?1.0f:-1.0f)*(0.5f+0.5f*h(salt^0x1U));
            const Vector3 dir=Radial(theta);
            const float r=WindthrowPlacement::RadiusAt(a);
            const Vector3 from=Axis(a)+dir*(r*0.8f),to=from+Vector3::Normalize(dir+d_*0.25f)*(0.35f+0.5f*h(salt^0x2U));
            b_.Tube({from,to},{0.12f,0.08f},7,Bark);
            b_.inside=from;
            const Vector3 n=Vector3::Normalize(to-from);
            Vector3 u=Vector3::Cross(n,Vector3::Up);
            if(u.LengthSquared()<1e-6f) u=s_;
            u.Normalize();
            const Vector3 v=Vector3::Cross(u,n);
            for(int k=0;k<6;++k) {
                const float t0=2.0f*Pi*static_cast<float>(k)/6.0f,t1=2.0f*Pi*static_cast<float>(k+1)/6.0f;
                b_.Face(to+n*0.01f,to+(u*std::cos(t0)+v*std::sin(t0))*0.08f,to+(u*std::cos(t1)+v*std::sin(t1))*0.08f,Wood*0.85f);
            }
        }
    }
    // shelf fungi in tiers up the trunk's flanks
    void Fungi() {
        for(int j=0;j<11;++j) {
            const auto salt=static_cast<std::uint32_t>(300+j*17);
            const float a=2.8f+11.0f*h(salt);
            const float side=h(salt^0x1U)<0.5f?1.0f:-1.0f;
            const float theta=(side>0.0f?0.0f:Pi)+side*(0.15f+0.45f*h(salt^0x2U));
            const int tiers=1+static_cast<int>(h(salt^0x3U)*2.99f);
            for(int t=0;t<tiers;++t) {
                const float r=WindthrowPlacement::RadiusAt(a);
                const Vector3 dir=Radial(theta-side*0.12f*static_cast<float>(t));
                const Vector3 root=Axis(a+0.05f*static_cast<float>(t))+dir*(r*0.98f);
                Vector3 out(dir.X,0.0f,dir.Z);
                if(out.LengthSquared()<1e-6f) out=s_*side;
                out.Normalize();
                const float width=0.12f+0.12f*h(salt^static_cast<std::uint32_t>(0x10+t));
                constexpr int Fan=7;
                for(int k=0;k<Fan;++k) {
                    const float f0=Pi*static_cast<float>(k)/Fan-Pi*0.5f,f1=Pi*static_cast<float>(k+1)/Fan-Pi*0.5f;
                    const Vector3 e0=root+(out*std::cos(f0)+d_*std::sin(f0))*width,e1=root+(out*std::cos(f1)+d_*std::sin(f1))*width;
                    b_.inside=root-Vector3::Up*0.05f;
                    b_.Face(root+Vector3::Up*0.012f,e0,e1,FungusTop*(0.92f+0.15f*h(salt^static_cast<std::uint32_t>(k))));
                    b_.inside=root+Vector3::Up*0.05f;
                    b_.Face(root-Vector3::Up*0.01f,e1-Vector3::Up*0.012f,e0-Vector3::Up*0.012f,Fungus);
                }
            }
        }
    }
    // a nurse log: ferns and seedlings rooting in the moss along its top
    void Ferns() {
        for(int j=0;j<13;++j) {
            const auto salt=static_cast<std::uint32_t>(400+j*19);
            const float a=1.8f+13.5f*h(salt);
            const float r=WindthrowPlacement::RadiusAt(a);
            const Vector3 base=Axis(a)+Radial(Pi*0.5f+(h(salt^0x1U)-0.5f)*0.7f)*(r*0.97f);
            const int fronds=4+static_cast<int>(h(salt^0x2U)*3.99f);
            for(int k=0;k<fronds;++k) {
                const auto f=salt^static_cast<std::uint32_t>(k*0x2bU);
                const float around=2.0f*Pi*(static_cast<float>(k)+0.4f*h(f))/static_cast<float>(fronds);
                const Vector3 flat=d_*std::cos(around)+s_*std::sin(around);
                const float length=0.32f+0.36f*h(f^0x1U);
                Vector3 at=base,dir=Vector3::Normalize(flat*0.55f+Vector3::Up*0.85f);
                const Vector3 side=Vector3::Normalize(Vector3::Cross(dir,Vector3::Up)+flat*0.01f);
                constexpr std::array<float,4> Width{{0.016f,0.042f,0.032f,0.0f}};
                b_.inside=base-Vector3::Up*0.2f;
                for(int seg=0;seg<3;++seg) {
                    const Vector3 bend=Vector3::Normalize(dir+flat*0.35f-Vector3::Up*(0.25f*static_cast<float>(seg)));
                    const Vector3 next=at+bend*(length/3.0f);
                    b_.Quad(at-side*Width[static_cast<std::size_t>(seg)],at+side*Width[static_cast<std::size_t>(seg)],
                            next+side*Width[static_cast<std::size_t>(seg+1)],next-side*Width[static_cast<std::size_t>(seg+1)],
                            Fern*(0.85f+0.3f*h(f^0x2U)));
                    at=next;dir=bend;
                }
            }
        }
    }
    // the root plate on edge at the foot, leaning a little back over its pit
    void Plate() {
        const float tilt=0.14f+0.06f*h(500U);
        const Vector3 v=Vector3::Up*std::cos(tilt)-d_*std::sin(tilt);
        const Vector3 front=Vector3::Normalize(-d_*std::cos(tilt)-Vector3::Up*std::sin(tilt));
        const Vector3 foot=Plan(-0.25f,0.0f);
        const Vector3 centre=Vector3(foot.X,w_.groundY-WindthrowPlacement::PlateSink,foot.Z)+v*(WindthrowPlacement::PlateHeight*0.5f);
        const float r1=2.0f*Pi*h(501U),r2=2.0f*Pi*h(502U),r3=2.0f*Pi*h(503U);
        const auto rim=[&](const float phi) {
            // ragged: broad lobes, and torn jags along the top where the roots snapped
            return 1.0f+0.1f*std::sin(3.0f*phi+r1)+0.07f*std::sin(7.0f*phi+r2)
                +(0.07f*std::sin(13.0f*phi+r3)+0.05f*std::sin(23.0f*phi+r1))*Smooth(-0.2f,0.6f,std::sin(phi));
        };
        const auto plane=[&](const float rho,const float phi) {
            const float k=rho*rim(phi);
            return centre+s_*(std::cos(phi)*WindthrowPlacement::PlateHalfWidth*k)+v*(std::sin(phi)*WindthrowPlacement::PlateHeight*0.5f*k);
        };
        // clods and hollows over the torn earth, at three scales
        const auto bump=[&](const float rho,const float phi) {
            return 0.07f*std::sin(phi*5.0f+rho*6.0f+r1)*std::sin(rho*8.0f-phi*2.0f+r2)
                +0.045f*std::sin(phi*13.0f+rho*15.0f+r3)*std::sin(rho*17.0f+phi*4.0f+r1)
                +0.02f*std::sin(phi*29.0f-rho*31.0f+r2);
        };
        constexpr int Rings=14,Around=64;
        SmoothGrid face(Rings+1,Around,true),back(Rings+1,Around,true);
        for(int i=0;i<=Rings;++i) {
            const float rho=std::max(0.02f,static_cast<float>(i)/Rings);
            for(int k=0;k<Around;++k) {
                const float phi=2.0f*Pi*static_cast<float>(k)/Around;
                const Vector3 p=plane(rho,phi);
                const float bulge=0.38f*(1.0f-rho*rho)+0.05f;
                face.P(i,k)=p+front*(bulge+bump(rho,phi));
                back.P(i,k)=p-front*(0.24f*(1.0f-rho*rho)+0.05f+0.5f*bump(rho,phi+1.0f));
                // the old underside: mineral soil and clay, dark humus round the taproot's hollow
                const float clay=Smooth(0.1f,0.7f,std::sin(phi*3.0f+rho*5.0f+r2)*std::sin(rho*4.0f-phi+r3)+0.2f);
                const float clod=bump(rho,phi)/0.135f;
                Vector3 c=Vector3::Lerp(Soil*(0.8f+0.2f*std::sin(phi*5.0f+rho*9.0f)+0.25f*clod),Clay,0.8f*clay);
                c=Vector3::Lerp(c,Humus,std::max(Smooth(0.3f,0.05f,rho)*0.7f,0.5f*Smooth(-0.2f,-0.8f,clod)));
                face.A(i,k)=c;
                // the old forest floor: dark litter, a little moss
                back.A(i,k)=Vector3::Lerp(Litter*(0.85f+0.25f*std::sin(phi*7.0f+rho*6.0f+r1)),Moss*0.8f,
                                          0.3f*Smooth(0.5f,0.9f,std::sin(phi*4.0f+r3)*std::sin(rho*6.0f+r1)));
            }
        }
        face.Emit(smooth_,b_.builder.sun,[&](int,int) {return front;});
        back.Emit(smooth_,b_.builder.sun,[&](int,int) {return -front;});
        // the rim: torn earth joining the two faces, pushed out a little
        SmoothGrid band(3,Around,true);
        for(int k=0;k<Around;++k) {
            const float phi=2.0f*Pi*static_cast<float>(k)/Around;
            band.P(0,k)=face.P(Rings,k);
            band.P(1,k)=plane(1.04f,phi)+front*0.02f;
            band.P(2,k)=back.P(Rings,k);
            band.A(0,k)=face.A(Rings,k);band.A(1,k)=Soil*0.85f;band.A(2,k)=back.A(Rings,k);
        }
        band.Emit(smooth_,b_.builder.sun,[&](const int,const int k) {
            const float phi=2.0f*Pi*static_cast<float>(k)/Around;
            return s_*std::cos(phi)+v*std::sin(phi);
        });
        // the broken roots in the old underside: curving out through the earth, weaving in and
        // out of it, the ones that reach the rim snapped off a little past it
        const auto onFace=[&](const float rho,const float phi) {
            return plane(rho,phi)+front*(0.38f*(1.0f-rho*rho)+0.05f+bump(rho,phi));
        };
        for(int j=0;j<30;++j) {
            const auto salt=static_cast<std::uint32_t>(600+j*23);
            const float phi0=2.0f*Pi*h(salt);
            const float rho0=j<5?0.06f+0.1f*h(salt^0x7U):0.1f+0.55f*h(salt^0x7U);
            const float rho1=std::min(1.0f,rho0+0.3f+0.8f*h(salt^0x8U));
            if(std::sin(phi0)*rho1<-0.55f) continue;   // the lower rim is in the ground
            const float bend=0.9f*(h(salt^0x1U)-0.5f),wiggle=2.0f*Pi*h(salt^0x9U);
            const float thick=j<5?0.075f+0.03f*h(salt^0x2U):0.02f+0.045f*h(salt^0x2U);
            std::vector<Vector3> points;std::vector<float> radii;
            constexpr int Steps=8;
            for(int k=0;k<=Steps;++k) {
                const float t=static_cast<float>(k)/Steps;
                const float rho=rho0+(rho1-rho0)*t;
                const float phi=phi0+bend*t+0.08f*std::sin(t*7.0f+wiggle);
                // in and out of the earth: half buried, now and then sunk out of sight
                const float rise=thick*(0.25f+0.55f*std::sin(t*5.0f+wiggle));
                points.push_back(onFace(rho,phi)+front*rise);
                radii.push_back(thick*(1.0f-0.4f*t));
            }
            if(rho1>=0.999f) {
                const float reach=0.06f+0.3f*h(salt^0x3U)*h(salt^0x4U);
                const Vector3 dir=Vector3::Normalize(points.back()-points[points.size()-3U]);
                const Vector3 out=points.back()+dir*reach-v*(0.04f*h(salt^0x5U));
                points.push_back(out);radii.push_back(thick*0.55f);
                b_.Tube(points,radii,6,Root*(0.8f+0.3f*h(salt^0x6U)));
                b_.Tube({out,out+dir*0.03f},{thick*0.55f,thick*0.5f},6,RootEnd);
            } else {
                b_.Tube(points,radii,6,Root*(0.8f+0.3f*h(salt^0x6U)));
            }
        }
        // the flare's big roots running from the trunk's foot into the old forest floor
        for(int j=0;j<5;++j) {
            const auto salt=static_cast<std::uint32_t>(700+j*29);
            const float phi=2.0f*Pi*(static_cast<float>(j)+0.4f*h(salt))/5.0f;
            const Vector3 from=Axis(0.9f)+Radial(phi)*(WindthrowPlacement::RadiusAt(0.9f)*0.8f);
            const Vector3 mid=plane(0.45f,phi)-front*0.32f,to=plane(0.9f,phi+0.1f)-front*0.12f;
            b_.Tube({from,mid,to},{0.2f,0.13f,0.05f},7,Root*0.8f);
        }
        // stones held in the earth, half sunk in it
        for(int j=0;j<9;++j) {
            const auto salt=static_cast<std::uint32_t>(800+j*31);
            const float rho=0.2f+0.72f*h(salt),phi=2.0f*Pi*h(salt^0x1U);
            if(std::sin(phi)*rho<-0.5f) continue;
            const float size=0.1f+0.14f*h(salt^0x2U);
            Lump(onFace(rho,phi)-front*(size*0.35f),size,salt);
        }
        // fine roots hanging off the upper rim
        for(int j=0;j<26;++j) {
            const auto salt=static_cast<std::uint32_t>(900+j*37);
            const float phi=0.15f+(Pi-0.3f)*h(salt);
            const Vector3 top=plane(1.0f,phi)+front*0.05f;
            const float length=0.15f+0.45f*h(salt^0x1U);
            const Vector3 bottom=top-Vector3::Up*length+front*(0.06f*h(salt^0x2U));
            const Vector3 across=s_*0.008f;
            b_.inside=top-front*0.3f;
            b_.Quad(top-across,top+across,bottom+across*0.3f,bottom-across*0.3f,Root*0.75f);
        }
    }
    // a rounded, knobbly stone
    void Lump(const Vector3& centre,const float size,const std::uint32_t salt) {
        b_.inside=centre;
        constexpr int Around=8,Bands=4;
        const Vector3 c=Stone*(0.68f+0.22f*h(salt^0x77U));
        std::array<std::array<Vector3,Around>,Bands+1> grid{};
        for(int i=0;i<=Bands;++i) {
            const float lat=-0.5f*Pi+Pi*static_cast<float>(i)/Bands;
            for(int k=0;k<Around;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Around;
                const float jag=0.8f+0.4f*h(salt^static_cast<std::uint32_t>(i*Around+k+10));
                grid[static_cast<std::size_t>(i)][static_cast<std::size_t>(k)]=centre+(s_*std::cos(a)+d_*std::sin(a))*(size*std::cos(lat)*jag)
                    +Vector3::Up*(size*0.75f*std::sin(lat)*jag);
            }
        }
        for(int i=0;i<Bands;++i) for(int k=0;k<Around;++k) {
            const auto i0=static_cast<std::size_t>(i),i1=i0+1U,k0=static_cast<std::size_t>(k),k1=static_cast<std::size_t>((k+1)%Around);
            b_.Quad(grid[i0][k0],grid[i0][k1],grid[i1][k1],grid[i1][k0],c);
        }
    }
    // the pit the plate tore out: wet dark earth, a puddle in its bottom, loose stones and roots
    void Pit() {
        constexpr int Rings=7,Around=30;
        const float r1=2.0f*Pi*h(1001U);
        SmoothGrid grid(Rings+1,Around,true);
        for(int i=0;i<=Rings;++i) {
            const float rho=std::max(0.02f,static_cast<float>(i)/Rings);
            for(int k=0;k<Around;++k) {
                const float phi=2.0f*Pi*static_cast<float>(k)/Around;
                const float ragged=1.0f+0.12f*std::sin(3.0f*phi+r1)+0.06f*std::sin(7.0f*phi-r1);
                const auto p=w_.At(-1.35f+std::cos(phi)*1.25f*rho*ragged,std::sin(phi)*2.0f*rho*ragged);
                grid.P(i,k)=Vector3(p[0],ground_(p[0],p[1])+0.045f,p[1]);
                const float puddle=Smooth(0.42f,0.3f,rho*(1.0f+0.15f*std::sin(2.0f*phi+r1)));
                // its rim the forest floor's litter, darkening to wet earth in the bottom
                Vector3 c=Vector3::Lerp(Litter*0.78f,Wet,Smooth(0.95f,0.45f,rho));
                c=Vector3::Lerp(c,Clay*0.8f,0.3f*Smooth(0.6f,0.9f,std::sin(phi*5.0f+rho*8.0f)));
                grid.A(i,k)=Vector3::Lerp(c,Puddle,puddle);
            }
        }
        grid.Emit(pit_,b_.builder.sun,[](int,int) {return Vector3::Up;});
        for(int j=0;j<6;++j) {
            const auto salt=static_cast<std::uint32_t>(1100+j*41);
            const auto p=w_.At(-1.35f+(h(salt)-0.5f)*2.0f,(h(salt^0x1U)-0.5f)*3.2f);
            const float size=0.05f+0.08f*h(salt^0x2U);
            Lump(Vector3(p[0],ground_(p[0],p[1])+size*0.3f,p[1]),size,salt);
        }
        for(int j=0;j<4;++j) {
            const auto salt=static_cast<std::uint32_t>(1200+j*43);
            const auto a=w_.At(-0.6f-1.6f*h(salt),(h(salt^0x1U)-0.5f)*3.0f);
            const float turn=2.0f*Pi*h(salt^0x2U);
            const Vector3 from(a[0],ground_(a[0],a[1])+0.04f,a[1]);
            const Vector3 to=from+(d_*std::cos(turn)+s_*std::sin(turn))*(0.4f+0.5f*h(salt^0x3U))+Vector3::Up*0.06f;
            b_.Tube({from,to},{0.035f,0.02f},5,Root*0.85f);
        }
    }

    const WindthrowPlacement& w_;
    std::function<float(float,float)> ground_;
    Vector3 d_,s_;
    SculptMeshBuilder b_;
    std::vector<VertexPositionColor> smooth_,pit_;
};
}

WindthrowRenderer::WindthrowRenderer(GraphicsDevice& device,const WindthrowPlacement& placement,
                                     const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    Builder builder(placement,[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);});
    std::size_t casting=0;
    const std::vector<VertexPositionColor> mesh=builder.Build(casting);
    castingTriangles_=static_cast<int>(casting/3U);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.5f,0.5f,0.5f),high+Vector3(0.5f,0.5f,0.5f));
    centre_=(low+high)*0.5f;
    triangles_=static_cast<int>(mesh.size()/3U);
    if(!mesh.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
        vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    }
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(220.0f);effect_->setFogEndProperty(520.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"windthrow",false,220.0f,520.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: windthrow sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a windthrown jungle giant lies "<<static_cast<int>(WindthrowPlacement::TrunkLength)
           <<" m along the forest floor, its root plate torn up on edge over the pit at its foot; "<<triangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void WindthrowRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!vertices_) return;
    const Vector3 nearest(std::clamp(camera.X,bounds_.Min.X,bounds_.Max.X),std::clamp(camera.Y,bounds_.Min.Y,bounds_.Max.Y),
                          std::clamp(camera.Z,bounds_.Min.Z,bounds_.Max.Z));
    if((nearest-camera).LengthSquared()>DrawDistance*DrawDistance) return;
    if(BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
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

void WindthrowRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!vertices_) return;
    bool casts=map.CastsInto(centre_.X,centre_.Z);
    for(const float x:{bounds_.Min.X,bounds_.Max.X}) for(const float z:{bounds_.Min.Z,bounds_.Max.Z}) casts=casts||map.CastsInto(x,z);
    if(!casts) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"windthrow");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: windthrow sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,castingTriangles_);
    map.CountCasters(1);
}
}
