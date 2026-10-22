#include "IslandSimulator/MineRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
// smooth value noise over the rock, for its bumps and its stains
float Noise(const float x,const float y) {
    const float fx=std::floor(x),fy=std::floor(y);
    const auto at=[&](const float ix,const float iy) {
        return Hash(static_cast<std::uint32_t>(static_cast<int>(ix)*73856093)^static_cast<std::uint32_t>(static_cast<int>(iy)*19349663));
    };
    float tx=x-fx,ty=y-fy;tx=tx*tx*(3.0f-2.0f*tx);ty=ty*ty*(3.0f-2.0f*ty);
    const float a=at(fx,fy),b=at(fx+1.0f,fy),c=at(fx,fy+1.0f),d=at(fx+1.0f,fy+1.0f);
    return (a+(b-a)*tx)+((c+(d-c)*tx)-(a+(b-a)*tx))*ty;
}
struct Lamp {Vector3 position,colour;float intensity,radius,reach;};
const Vector3 Rock(118,104,90),RockDark(84,74,64),Gravel(104,94,80),Timber(132,98,64),TimberDark(96,70,46),
    RailIron(104,86,70),Glass(60,48,30),CrystalBlue(80,214,255),CrystalViolet(186,120,255),CartIron(112,84,62),
    Ore(70,62,58);
}

MineRenderer::MineRenderer(GraphicsDevice& device,const MineLayout& layout,const IslandTerrain& terrain,
                           const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    // the frame: across, up from the floor at that depth, depth in
    const auto W=[&](const float a,const float u,const float d) {
        const auto p=layout.World(a,MineLayout::FloorRelative(d)+u,d);return Vector3(p.x,p.y,p.z);
    };
    const Vector3 origin=W(0,0,0);
    Vector3 acrossDir=W(1,0,0)-origin;acrossDir.Normalize();
    Vector3 inward=W(0,0,1)-W(0,0,0);inward.Y=0.0f;inward.Normalize();
    const float portal=layout.PortalDepth();
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();

    // the lanterns down the adit, hung from the posts on alternate sides
    std::vector<Lamp> lamps;
    std::vector<Vector3> lanternAt;
    std::vector<float> sets;
    for(float d=portal+0.6f;d<MineSite::AditLength-0.4f;d+=1.5f) sets.push_back(d);
    for(std::size_t k=0;k<sets.size();++k) {
        if(k%4U!=1U) continue;
        const float side=(k/4U)%2U==0U?1.0f:-1.0f;
        const Vector3 p=W(side*(MineSite::PostInner-0.13f),1.72f,sets[k]-0.18f);
        lanternAt.push_back(p);
        lamps.push_back({p,Vector3(1.0f,0.70f,0.36f),1.5f,1.35f,7.5f});
    }
    lanterns_=static_cast<int>(lanternAt.size());
    // the crystal clusters on the chamber's walls, each throwing its own cold light
    struct Cluster {float theta,up,lean;bool violet;};
    const std::array<Cluster,9> clusters{{{-0.2f,0.3f,0.2f,false},{0.35f,1.2f,-0.3f,false},{0.9f,0.15f,0.1f,true},
        {1.45f,1.9f,0.25f,false},{2.0f,0.5f,-0.15f,false},{2.55f,1.4f,0.3f,true},{3.1f,0.2f,0.0f,false},
        {3.7f,1.0f,-0.25f,false},{-0.75f,1.6f,0.15f,true}}};
    const auto chamberWall=[&](const float theta,const float up,float& a,float& d) {
        const float rise=std::clamp(up/MineSite::ChamberHeight,0.0f,1.0f);
        const float reach=std::pow(std::max(0.0f,1.0f-rise*rise),0.5f);
        a=MineSite::ChamberHalfWidth*std::cos(theta)*reach;d=MineSite::ChamberCentre+MineSite::ChamberHalfLength*std::sin(theta)*reach;
    };
    for(const Cluster& c:clusters) {
        float a=0,d=0;chamberWall(c.theta,c.up,a,d);
        const Vector3 p=W(a*0.88f,c.up+0.25f,MineSite::ChamberCentre+(d-MineSite::ChamberCentre)*0.88f);
        lamps.push_back({p,c.violet?Vector3(0.62f,0.40f,1.0f):Vector3(0.30f,0.78f,1.0f),1.25f,1.6f,6.5f});
    }

    const auto lampLight=[&](const Vector3& p,const Vector3& n) {
        Vector3 sum(0,0,0);
        for(const Lamp& lamp:lamps) {
            Vector3 v=lamp.position-p;
            const float distance=v.Length();
            if(distance>lamp.reach||distance<1e-4f) continue;
            v/=distance;
            const float facing=0.3f+0.7f*std::max(0.0f,Vector3::Dot(n,v));
            const float fade=1.0f-std::clamp((distance-lamp.reach*0.55f)/(lamp.reach*0.45f),0.0f,1.0f);
            const float r=distance/lamp.radius;
            sum+=lamp.colour*(lamp.intensity/(1.0f+r*r)*facing*fade*fade);
        }
        return sum;
    };
    const auto daylight=[&](const Vector3& p,const Vector3& n) {
        const float depth=layout.Local({p.X,p.Y,p.Z}).z;
        if(depth<portal) return 0.55f+0.45f*std::max(0.0f,Vector3::Dot(n,sun));
        // the day comes in at the portal and dies within a few metres
        return 0.06f+0.62f*std::exp(-(depth-portal)/2.4f)*(0.6f+0.4f*std::max(0.0f,-Vector3::Dot(n,inward)));
    };
    std::vector<VertexPositionColor> base,light;
    const auto colour=[](const Vector3& c) {
        return Color(static_cast<int>(std::clamp(c.X,0.0f,255.0f)),static_cast<int>(std::clamp(c.Y,0.0f,255.0f)),
                     static_cast<int>(std::clamp(c.Z,0.0f,255.0f)));
    };
    // one triangle: each corner lit for itself, the day in the first draw, the lamps in the second
    const auto tri=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& n,const Vector3& albedo,
                       const Vector3& emissive) {
        const Vector3 corners[3]={a,b,c};
        Color lit[3];bool any=false;
        for(int k=0;k<3;++k) {
            base.emplace_back(corners[k],colour(albedo*daylight(corners[k],n)));
            const Vector3 l=lampLight(corners[k],n);
            const Vector3 add=Vector3(albedo.X*l.X,albedo.Y*l.Y,albedo.Z*l.Z)+emissive;
            lit[k]=colour(add);
            any=any||add.X>2.0f||add.Y>2.0f||add.Z>2.0f;
        }
        if(any) for(int k=0;k<3;++k) light.emplace_back(corners[k],lit[k]);
    };
    const auto quad=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,const Vector3& n,
                        const Vector3& albedo,const Vector3& emissive=Vector3(0,0,0)) {
        tri(a,b,c,n,albedo,emissive);tri(a,c,d,n,albedo,emissive);
    };
    // a box in the frame, its faces lit by their own normals
    const auto box=[&](const float a0,const float a1,const float u0,const float u1,const float d0,const float d1,
                       const Vector3& albedo) {
        const Vector3 p[8]={W(a0,u0,d0),W(a1,u0,d0),W(a1,u0,d1),W(a0,u0,d1),W(a0,u1,d0),W(a1,u1,d0),W(a1,u1,d1),W(a0,u1,d1)};
        const Vector3 up(0,1,0);
        quad(p[4],p[5],p[6],p[7],up,albedo);quad(p[0],p[1],p[2],p[3],-up,albedo*0.8f);
        quad(p[0],p[1],p[5],p[4],-inward,albedo);quad(p[3],p[2],p[6],p[7],inward,albedo);
        quad(p[0],p[3],p[7],p[4],-acrossDir,albedo);quad(p[1],p[2],p[6],p[5],acrossDir,albedo);
    };
    const auto stain=[&](const float a,const float u,const float d) {
        return 0.82f+0.3f*Noise(d*1.3f+a*0.7f,u*1.9f+a*2.1f)+0.08f*Noise(d*5.1f,u*4.7f);
    };

    // ---- the cutting: gravel underfoot, log cribbing holding back the slope either side
    for(float d=-0.75f;d<portal+0.2f;d+=0.5f) {
        const float d1=std::min(d+0.5f,portal+0.2f);
        for(float a=-MineSite::HalfWidth;a<MineSite::HalfWidth-1e-3f;a+=0.65f) {
            const float a1=a+0.65f;
            quad(W(a,0,d),W(a1,0,d),W(a1,0,d1),W(a,0,d1),Vector3(0,1,0),Gravel*stain(a,0,d));
        }
    }
    for(const float s:{-1.0f,1.0f}) {
        // logs stacked against the cut face beside the cutting, each row reaching out only as far
        // as the hillside stands above it, so the cribbing steps down with the slope
        const auto bank=[&](const float d) {
            const auto side=W(s*1.65f,0,d);
            return DrawnOrAnalyticHeight(ground,terrain,side.X,side.Z)-side.Y;
        };
        for(float u=0.11f;u<MineSite::Height+0.4f;u+=0.22f) {
            float from=portal+0.15f;
            for(float d=portal;d>=-0.8f;d-=0.1f) {
                if(bank(d)<u-0.05f) break;
                from=d;
            }
            if(from>portal) continue;
            std::vector<Vector3> points;
            for(int k=0;k<=3;++k) {
                const float d=from-0.12f+(portal+0.27f-from)*static_cast<float>(k)/3.0f;
                points.push_back(W(s*(MineSite::HalfWidth+0.13f),u,d));
            }
            const float r=0.10f+0.02f*Hash(static_cast<std::uint32_t>(u*100.0f)+(s>0?7U:3U));
            // the log as an eight-sided tube, lit face by face
            for(std::size_t k=0;k+1<points.size();++k)
                for(int f=0;f<8;++f) {
                    const float q0=2.0f*Pi*static_cast<float>(f)/8.0f,q1=2.0f*Pi*static_cast<float>(f+1)/8.0f;
                    const Vector3 o0=acrossDir*std::cos(q0)+Vector3(0,1,0)*std::sin(q0),o1=acrossDir*std::cos(q1)+Vector3(0,1,0)*std::sin(q1);
                    Vector3 n=o0+o1;n.Normalize();
                    quad(points[k]+o0*r,points[k+1]+o0*r,points[k+1]+o1*r,points[k]+o1*r,n,Timber*(0.85f+0.15f*std::sin(u*13.0f)));
                }
        }
    }
    // ---- the adit: its rock walls and roof, the gravel floor with puddles
    const float aditEnd=48.0f;
    for(float d=portal-0.1f;d<aditEnd-1e-3f;d+=0.5f) {
        const float d1=std::min(d+0.5f,aditEnd);
        for(float a=-MineSite::HalfWidth-0.2f;a<MineSite::HalfWidth+0.2f-1e-3f;a+=0.75f) {
            const float a1=a+0.75f;
            const float wet=Noise(d*0.35f,a*0.9f)>0.72f?0.62f:1.0f;
            quad(W(a,0,d),W(a1,0,d),W(a1,0,d1),W(a,0,d1),Vector3(0,1,0),Gravel*stain(a,0,d)*wet);
            // the rock roof above the lagging
            quad(W(a,2.68f,d),W(a1,2.68f,d),W(a1,2.68f,d1),W(a,2.68f,d1),Vector3(0,-1,0),RockDark*stain(a,2.7f,d));
        }
        for(const float s:{-1.0f,1.0f}) {
            const auto bump=[&](const float u,const float dd) {return 0.14f*Noise(dd*0.9f+(s>0?11.0f:0.0f),u*1.4f);};
            const std::array<float,6> ups{{0.0f,0.5f,1.0f,1.5f,2.1f,2.68f}};
            for(std::size_t k=0;k+1<ups.size();++k) {
                const float u0=ups[k],u1=ups[k+1];
                const Vector3 n=acrossDir*(-s);
                quad(W(s*(MineSite::HalfWidth+bump(u0,d)),u0,d),W(s*(MineSite::HalfWidth+bump(u0,d1)),u0,d1),
                     W(s*(MineSite::HalfWidth+bump(u1,d1)),u1,d1),W(s*(MineSite::HalfWidth+bump(u1,d)),u1,d),n,
                     Rock*stain(s,(u0+u1)*0.5f,d));
            }
        }
    }
    // ---- the timber sets: two posts and a cap, the lagging laid over the caps between them;
    // the portal's set heavier
    for(std::size_t k=0;k<sets.size();++k) {
        const float d=sets[k];
        const float tone=0.85f+0.3f*Hash(static_cast<std::uint32_t>(k)*977U);
        for(const float s:{-1.0f,1.0f})
            box(s>0?MineSite::PostInner:-MineSite::HalfWidth-0.02f,s>0?MineSite::HalfWidth+0.02f:-MineSite::PostInner,
                0.0f,2.3f,d-0.1f,d+0.1f,Timber*tone);
        box(-MineSite::HalfWidth-0.05f,MineSite::HalfWidth+0.05f,2.3f,2.52f,d-0.11f,d+0.11f,Timber*tone*0.95f);
        if(k+1<sets.size())
            for(int plank=0;plank<9;++plank) {
                const float a=-MineSite::HalfWidth+0.05f+static_cast<float>(plank)*0.29f;
                const float shade=0.75f+0.35f*Hash(static_cast<std::uint32_t>(k*31U+static_cast<std::size_t>(plank)));
                quad(W(a,2.53f,d),W(a+0.26f,2.53f,d),W(a+0.26f,2.53f,sets[k+1]),W(a,2.53f,sets[k+1]),Vector3(0,-1,0),TimberDark*shade);
            }
    }
    {
        const float d=portal;
        for(const float s:{-1.0f,1.0f})
            box(s>0?MineSite::PostInner:-MineSite::HalfWidth-0.15f,s>0?MineSite::HalfWidth+0.15f:-MineSite::PostInner,
                0.0f,2.3f,d-0.16f,d+0.16f,Timber*0.9f);
        box(-MineSite::HalfWidth-0.4f,MineSite::HalfWidth+0.4f,2.3f,2.66f,d-0.18f,d+0.18f,Timber*0.85f);
        // a weathered board nailed over the lintel
        box(-0.75f,0.75f,2.68f,3.02f,d-0.22f,d-0.18f,Timber*1.05f);
    }
    // ---- the track: two rails on sleepers, from the portal to the chamber
    for(float d=-0.4f;d<51.0f;d+=0.65f) box(-0.55f,0.55f,0.0f,0.07f,d-0.08f,d+0.08f,TimberDark*0.9f);
    for(const float a:{-0.3f,0.3f}) box(a-0.025f,a+0.025f,0.07f,0.14f,-0.45f,50.4f,RailIron);
    // ---- the lanterns: frame, glowing glass and flame, a wire handle on a nail in the post
    for(const Vector3& p:lanternAt) {
        const float a=layout.Local({p.X,p.Y,p.Z}).x;
        const float d=layout.Local({p.X,p.Y,p.Z}).z;
        const float s=a>0.0f?1.0f:-1.0f;
        const float u=1.72f;
        box(a-0.07f,a+0.07f,u-0.12f,u-0.10f,d-0.07f,d+0.07f,Glass*1.2f);
        box(a-0.06f,a+0.06f,u+0.10f,u+0.13f,d-0.06f,d+0.06f,Glass*1.2f);
        box(a-0.04f,a+0.04f,u+0.13f,u+0.17f,d-0.04f,d+0.04f,Glass*1.2f);
        for(const float da:{-0.065f,0.065f}) for(const float dd:{-0.065f,0.065f})
            box(a+da-0.008f,a+da+0.008f,u-0.10f,u+0.10f,d+dd-0.008f,d+dd+0.008f,Glass);
        // the glass, lit from within
        const Vector3 glow(255,186,96);
        for(const float dd:{-0.055f,0.055f})
            quad(W(a-0.055f,u-0.09f,d+dd),W(a+0.055f,u-0.09f,d+dd),W(a+0.055f,u+0.09f,d+dd),W(a-0.055f,u+0.09f,d+dd),
                 inward*(dd>0?1.0f:-1.0f),Glass,glow);
        for(const float da:{-0.055f,0.055f})
            quad(W(a+da,u-0.09f,d-0.055f),W(a+da,u-0.09f,d+0.055f),W(a+da,u+0.09f,d+0.055f),W(a+da,u+0.09f,d-0.055f),
                 acrossDir*(da>0?1.0f:-1.0f),Glass,glow);
        quad(W(a-0.012f,u-0.05f,d),W(a+0.012f,u-0.05f,d),W(a+0.008f,u+0.04f,d),W(a-0.008f,u+0.04f,d),-inward,Glass,Vector3(255,236,170));
        // the handle up to the nail in the post
        box(a-0.006f,a+0.006f,u+0.17f,u+0.26f,d-0.006f,d+0.006f,Glass);
        box(std::min(a,s*MineSite::PostInner)-0.006f,std::max(a,s*MineSite::PostInner)+0.006f,u+0.25f,u+0.27f,d-0.006f,d+0.006f,Glass);
    }
    // ---- the chamber: a rough dome round an oval floor, open where the adit comes in
    {
        constexpr int Around=28,Rise=8;
        const auto wallPoint=[&](const float theta,const float rise) {
            const float up=MineSite::ChamberHeight*std::sin(rise*Pi*0.5f);
            float a=0,d=0;chamberWall(theta,up,a,d);
            // periodic round the oval, so the seam at a full turn closes
            const float rough=1.0f+0.08f*(Noise(std::cos(theta)*2.6f+7.0f,std::sin(theta)*2.6f+7.0f+rise*4.3f)-0.5f);
            return W(a*rough,up,MineSite::ChamberCentre+(d-MineSite::ChamberCentre)*rough);
        };
        const Vector3 middle=W(0.0f,1.2f,MineSite::ChamberCentre);
        for(int i=0;i<Around;++i) for(int j=0;j<Rise;++j) {
            const float t0=2.0f*Pi*static_cast<float>(i)/Around,t1=2.0f*Pi*static_cast<float>(i+1)/Around;
            const float r0=static_cast<float>(j)/Rise,r1=static_cast<float>(j+1)/Rise;
            const Vector3 p00=wallPoint(t0,r0),p10=wallPoint(t1,r0),p11=wallPoint(t1,r1),p01=wallPoint(t0,r1);
            const Vector3 c=(p00+p10+p11+p01)*0.25f;
            const auto local=layout.Local({c.X,c.Y,c.Z});
            if(std::abs(local.x)<MineSite::HalfWidth+0.15f&&local.z<48.6f&&local.y-MineLayout::FloorRelative(local.z)<2.75f) continue;
            Vector3 n=middle-c;n.Normalize();
            quad(p00,p10,p11,p01,n,Rock*stain(t0,r0*3.0f,52.0f)*0.95f);
        }
        // the headwall round the adit's mouth, closing the dome down to it
        {
            const float d=47.98f,top=MineSite::ChamberHeight+1.2f,side=4.2f,roof=2.68f;
            const Vector3 n=inward;
            quad(W(-side,0.0f,d),W(-MineSite::HalfWidth-0.15f,0.0f,d),W(-MineSite::HalfWidth-0.15f,top,d),W(-side,top,d),n,RockDark);
            quad(W(MineSite::HalfWidth+0.15f,0.0f,d),W(side,0.0f,d),W(side,top,d),W(MineSite::HalfWidth+0.15f,top,d),n,RockDark);
            quad(W(-MineSite::HalfWidth-0.15f,roof,d),W(MineSite::HalfWidth+0.15f,roof,d),W(MineSite::HalfWidth+0.15f,top,d),W(-MineSite::HalfWidth-0.15f,top,d),n,RockDark);
        }
        // the floor
        for(int i=0;i<Around;++i) {
            const float t0=2.0f*Pi*static_cast<float>(i)/Around,t1=2.0f*Pi*static_cast<float>(i+1)/Around;
            for(int ring=0;ring<3;++ring) {
                const float f0=static_cast<float>(ring)/3.0f,f1=static_cast<float>(ring+1)/3.0f;
                const auto at=[&](const float t,const float f) {
                    return W(MineSite::ChamberHalfWidth*std::cos(t)*f*1.07f,0.0f,MineSite::ChamberCentre+MineSite::ChamberHalfLength*std::sin(t)*f*1.07f);
                };
                quad(at(t0,f0),at(t1,f0),at(t1,f1),at(t0,f1),Vector3(0,1,0),Gravel*stain(t0,0.0f,f0*4.0f));
            }
        }
        // the crystals: hexagonal prisms with pointed tips growing from the wall, glowing
        for(std::size_t ci=0;ci<clusters.size();++ci) {
            const Cluster& c=clusters[ci];
            const Vector3 colourOf=c.violet?CrystalViolet:CrystalBlue;
            float a=0,d=0;chamberWall(c.theta,c.up,a,d);
            const Vector3 root=W(a,c.up,d);
            Vector3 out=middle-root;out.Normalize();
            const int count=4+static_cast<int>(Hash(static_cast<std::uint32_t>(ci)*131U)*4.0f);
            for(int k=0;k<count;++k) {
                const auto seed=static_cast<std::uint32_t>(ci*17U)+static_cast<std::uint32_t>(k)*977U;
                Vector3 axis=out+Vector3((Hash(seed)-0.5f)*1.1f,(Hash(seed^0x55U)-0.4f)*0.9f+c.lean,(Hash(seed^0xaaU)-0.5f)*1.1f);
                axis.Normalize();
                const float length=0.25f+0.6f*Hash(seed^0x1234U),radius=0.04f+0.07f*Hash(seed^0x4321U);
                const Vector3 foot=root+Vector3((Hash(seed^0x77U)-0.5f)*0.35f,(Hash(seed^0x99U)-0.5f)*0.3f,(Hash(seed^0x33U)-0.5f)*0.35f);
                Vector3 side=Vector3::Cross(axis,Vector3::Up);
                if(side.LengthSquared()<1e-4f) side=acrossDir;
                side.Normalize();
                const Vector3 other=Vector3::Cross(axis,side);
                const Vector3 tip=foot+axis*(length+radius*1.6f);
                const Vector3 glow=colourOf*(0.55f+0.35f*Hash(seed^0x5aU));
                for(int f=0;f<6;++f) {
                    const float q0=2.0f*Pi*static_cast<float>(f)/6.0f,q1=2.0f*Pi*static_cast<float>(f+1)/6.0f;
                    const Vector3 o0=side*std::cos(q0)+other*std::sin(q0),o1=side*std::cos(q1)+other*std::sin(q1);
                    Vector3 n=o0+o1;n.Normalize();
                    quad(foot+o0*radius,foot+o1*radius,foot+axis*length+o1*radius,foot+axis*length+o0*radius,n,colourOf*0.35f,glow);
                    tri(foot+axis*length+o0*radius,foot+axis*length+o1*radius,tip,n,colourOf*0.4f,glow*1.15f);
                }
            }
        }
        // the ore cart, tipped over on its side, its load spilled
        {
            const float ca=MineLayout::CartAcross,cd=MineLayout::CartDepth;
            // lying on its side: the tub's floor stands up toward the wall, its mouth to the adit
            box(ca+0.3f,ca+0.36f,0.0f,0.7f,cd-0.55f,cd+0.55f,CartIron);
            box(ca-0.3f,ca+0.36f,0.0f,0.05f,cd-0.55f,cd+0.55f,CartIron*0.9f);
            box(ca-0.3f,ca+0.36f,0.65f,0.7f,cd-0.55f,cd+0.55f,CartIron);
            for(const float e:{-0.55f,0.55f}) box(ca-0.3f,ca+0.36f,0.0f,0.7f,e>0?cd+0.5f:cd-0.55f,e>0?cd+0.55f:cd-0.5f,CartIron*0.95f);
            for(const float w:{-0.35f,0.35f}) for(const float h:{0.12f,0.58f})
                box(ca+0.36f,ca+0.42f,h-0.12f,h+0.12f,cd+w-0.12f,cd+w+0.12f,RailIron*0.8f);
            for(int k=0;k<14;++k) {
                const auto seed=static_cast<std::uint32_t>(k)*2654435761U;
                const float oa=ca-0.4f-0.9f*Hash(seed),od=cd-0.6f+1.2f*Hash(seed^0x9U),size=0.06f+0.08f*Hash(seed^0x3U);
                box(oa-size,oa+size,0.0f,size*1.3f,od-size,od+size,Ore*(0.8f+0.4f*Hash(seed^0x5U)));
            }
        }
    }
    // ---- the lanterns' halos: soft fans, crossed, added in the second draw
    std::vector<VertexPositionColor> halo;
    for(const Vector3& p:lanternAt) {
        const Color centre(110,72,34),rim(0,0,0);
        const Vector3 axes[3][2]={{acrossDir,Vector3::Up},{inward,Vector3::Up},{acrossDir,inward}};
        for(const auto& plane:axes) {
            constexpr int Rim=10;
            for(int k=0;k<Rim;++k) {
                const float q0=2.0f*Pi*static_cast<float>(k)/Rim,q1=2.0f*Pi*static_cast<float>(k+1)/Rim;
                halo.emplace_back(p,centre);
                halo.emplace_back(p+(plane[0]*std::cos(q0)+plane[1]*std::sin(q0))*0.42f,rim);
                halo.emplace_back(p+(plane[0]*std::cos(q1)+plane[1]*std::sin(q1))*0.42f,rim);
            }
        }
    }
    light.insert(light.end(),halo.begin(),halo.end());

    baseTriangles_=static_cast<int>(base.size()/3U);lightTriangles_=static_cast<int>(light.size()/3U);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:base) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.5f,0.5f,0.5f),high+Vector3(0.5f,0.5f,0.5f));
    entrance_=origin;
    base_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(base.size()),BufferUsage::WriteOnly);
    base_->SetData(base.data(),static_cast<int>(base.size()));
    light_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(light.size()),BufferUsage::WriteOnly);
    light_->SetData(light.data(),static_cast<int>(light.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    std::ostringstream message;
    message<<"island-simulator: an old mine is driven into the main mountain's eastern foot at ("
        <<static_cast<int>(std::lround(origin.X))<<", "<<static_cast<int>(std::lround(origin.Z))<<"): a "
        <<static_cast<int>(std::lround(MineSite::AditLength))<<" m timbered adit lit by "<<lanterns_
        <<" lanterns to a chamber of glowing crystals; "<<baseTriangles_<<" + "<<lightTriangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

bool MineRenderer::Visible(const Matrix& view,const Matrix& projection,const Vector3& camera) const {
    return (entrance_-camera).LengthSquared()<(DrawDistance+60.0f)*(DrawDistance+60.0f)
        &&BoundingFrustum(view*projection).Contains(bounds_)!=ContainmentType::Disjoint;
}

void MineRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!Visible(view,projection,camera)) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    effect_->setDiffuseColorProperty(Vector3(1.0f,1.0f,1.0f));
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.SetVertexBuffer(base_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,baseTriangles_);drawCalls_=1;
}

void MineRenderer::DrawLight(const Matrix& view,const Matrix& projection,const Vector3& camera,const float seconds) {
    if(lightTriangles_==0||!Visible(view,projection,camera)) return;
    // the flames breathe a little, all together
    const float flicker=0.93f+0.045f*std::sin(seconds*7.3f)+0.025f*std::sin(seconds*13.7f+1.1f);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    effect_->setDiffuseColorProperty(Vector3(flicker,flicker,flicker));
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.SetVertexBuffer(light_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,lightTriangles_);++drawCalls_;
    effect_->setDiffuseColorProperty(Vector3(1.0f,1.0f,1.0f));
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}
}
