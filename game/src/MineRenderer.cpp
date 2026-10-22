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
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
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
float SmoothStep(const float edge0,const float edge1,const float x) {
    const float t=std::clamp((x-edge0)/(edge1-edge0),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
struct Lamp {Vector3 position,colour;float intensity,radius,reach;};
const Vector3 Rock(118,104,90),RockDark(84,74,64),Gravel(104,94,80),Timber(132,98,64),TimberDark(96,70,46),
    RailIron(104,86,70),Glass(60,48,30),CrystalBlue(80,214,255),CrystalViolet(186,120,255),CartIron(112,84,62),
    Ore(70,62,58),Water(34,32,30);
}

std::vector<Vector3> MineRenderer::HangingLanterns(const MineLayout& layout) {
    // on every fourth timber set down the adit, hung from the posts on alternate sides
    std::vector<Vector3> lanterns;
    std::size_t k=0;
    for(float d=layout.PortalDepth()+0.6f;d<MineSite::AditLength-0.4f;d+=1.5f,++k) {
        if(k%4U!=1U) continue;
        const float side=(k/4U)%2U==0U?1.0f:-1.0f;
        const auto p=layout.World(side*(MineSite::PostInner-0.13f),MineLayout::FloorRelative(d-0.18f)+1.72f,d-0.18f);
        lanterns.emplace_back(p.x,p.y,p.z);
    }
    return lanterns;
}

std::vector<MineRenderer::Puddle> MineRenderer::Puddles(const MineLayout& layout) {
    // water standing in the low places down the adit: beside the track, where the floor dips
    // toward the walls, and between its sleepers; each an irregular oval
    std::vector<Puddle> puddles;
    std::uint32_t seed=0x7d1e5U;
    const auto next=[&seed]() {seed=seed*1664525U+1013904223U;return static_cast<float>(seed>>8U)*(1.0f/16777216.0f);};
    for(float d=layout.PortalDepth()+2.5f+2.0f*next();d<46.5f;d+=2.2f+3.2f*next()) {
        const float pick=next();
        Puddle puddle;
        const bool beside=pick<0.8f;
        puddle.across=beside?(pick<0.4f?-1.0f:1.0f)*(0.66f+0.18f*next()):0.0f;
        puddle.depth=d;
        puddle.halfAcross=beside?0.26f+0.14f*next():0.22f+0.14f*next();
        puddle.halfAlong=0.35f+0.55f*next();
        for(float& r:puddle.outline) r=0.82f+0.3f*next();
        puddles.push_back(puddle);
    }
    return puddles;
}

bool MineRenderer::ReflectionView(const MineLayout& layout,const float eyeHeight,Vector3& eye,Vector3& look) {
    // a puddle beside the track with a lantern on its side of the adit a few metres past it:
    // standing as far short of the puddle as the lantern is past it, at the lantern's height,
    // the flame shows in the water's middle
    const auto lanterns=HangingLanterns(layout);
    for(const Puddle& puddle:Puddles(layout)) {
        if(std::abs(puddle.across)<0.1f) continue;
        for(const Vector3& lamp:lanterns) {
            const auto local=layout.Local({lamp.X,lamp.Y,lamp.Z});
            if(local.x*puddle.across<=0.0f||local.z<puddle.depth+1.2f||local.z>puddle.depth+3.2f) continue;
            const float across=2.0f*puddle.across-local.x,depth=2.0f*puddle.depth-local.z;
            if(std::abs(across)>MineSite::PostInner-0.3f||depth<layout.PortalDepth()+0.5f) continue;
            const auto at=layout.World(across,MineLayout::FloorRelative(depth)+eyeHeight,depth);
            const auto down=layout.World(-local.x*0.25f,MineLayout::FloorRelative(local.z)+0.65f,local.z+0.5f);
            eye=Vector3(at.x,at.y,at.z);look=Vector3(down.x,down.y,down.z);
            return true;
        }
    }
    return false;
}

MineRenderer::MineRenderer(GraphicsDevice& device,const MineLayout& layout,const IslandTerrain& terrain,
                           const RenderedTerrainSurface& ground)
    :device_(device),layout_(layout),
     reflections_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
                                                        static_cast<int>(ReflectionCapacity),BufferUsage::WriteOnly)),
     effect_(std::make_unique<BasicEffect>(device))
{
    reflectionStaging_.reserve(ReflectionCapacity);
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
    for(const Vector3& p:HangingLanterns(layout)) {
        lanternAt.push_back(p);
        lamps.push_back({p,Vector3(1.0f,0.70f,0.36f),1.5f,1.35f,7.5f});
    }
    // one more set down on the chamber's floor by the cart, still burning
    struct LanternSpot {float a,d,u;bool hanging;};
    std::vector<LanternSpot> spots;
    for(const Vector3& p:lanternAt) {
        const auto local=layout.Local({p.X,p.Y,p.Z});
        spots.push_back({local.x,local.z,1.72f,true});
    }
    {
        const float a=0.85f,d=50.1f;
        spots.push_back({a,d,0.12f,false});
        const Vector3 p=W(a,0.12f,d);
        lanternAt.push_back(p);
        lamps.push_back({p,Vector3(1.0f,0.70f,0.36f),1.3f,1.3f,7.0f});
    }
    lanterns_=static_cast<int>(lanternAt.size());
    lanternPositions_=lanternAt;
    puddles_=Puddles(layout);
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
    // one triangle with its own normal and albedo at each corner, so rough rock shades smoothly
    // across its faces instead of in flat facets
    const auto smoothTri=[&](const std::array<Vector3,3>& p,const std::array<Vector3,3>& n,const std::array<Vector3,3>& albedo) {
        Color lit[3];bool any=false;
        for(std::size_t k=0;k<3;++k) {
            base.emplace_back(p[k],colour(albedo[k]*daylight(p[k],n[k])));
            const Vector3 l=lampLight(p[k],n[k]);
            const Vector3 add(albedo[k].X*l.X,albedo[k].Y*l.Y,albedo[k].Z*l.Z);
            lit[k]=colour(add);
            any=any||add.X>2.0f||add.Y>2.0f||add.Z>2.0f;
        }
        if(any) for(std::size_t k=0;k<3;++k) light.emplace_back(p[k],lit[k]);
    };
    // a lump of rock: a flattened, knobbly polyhedron round `centre`, each face lit by its own normal
    const auto lump=[&](const Vector3& centre,const float size,const Vector3& albedo,const std::uint32_t seed) {
        constexpr int Around=6,Bands=3;
        const auto point=[&](const int band,const int around) {
            const float lat=Pi*(static_cast<float>(band)/Bands-0.5f),lon=2.0f*Pi*static_cast<float>(around%Around)/Around;
            const float bump=band==0||band==Bands?1.0f
                :0.75f+0.45f*Hash(seed^static_cast<std::uint32_t>(band*31+(around%Around)*7+1));
            return centre+(acrossDir*(std::cos(lat)*std::cos(lon))+inward*(std::cos(lat)*std::sin(lon)))*(size*bump)
                +Vector3::Up*(std::sin(lat)*size*0.65f*bump);
        };
        const auto face=[&](const Vector3& a,const Vector3& b,const Vector3& c) {
            Vector3 n=Vector3::Cross(b-a,c-a);
            if(n.LengthSquared()<1e-12f) return;
            n.Normalize();
            if(Vector3::Dot(n,(a+b+c)/3.0f-centre)<0.0f) n=-n;
            tri(a,b,c,n,albedo*(0.9f+0.2f*Hash(seed^static_cast<std::uint32_t>(static_cast<int>(n.X*7.0f+n.Z*13.0f)+40))),Vector3(0,0,0));
        };
        for(int band=0;band<Bands;++band) for(int a=0;a<Around;++a) {
            const Vector3 p00=point(band,a),p01=point(band,a+1),p10=point(band+1,a),p11=point(band+1,a+1);
            if(band==0) face(p00,p11,p10);
            else if(band==Bands-1) face(p00,p01,p10);
            else {face(p00,p01,p11);face(p00,p11,p10);}
        }
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

    // ---- the cutting: gravel underfoot, log cribbing holding back the slope either side. The
    // gravel spans the whole width the hillside is cut away to (the terrain is not drawn there
    // above the floor), following the slope down in front of the mouth where it falls below the
    // floor; behind the cribbing an earth bank rises to the hillside on either side. (The gravel
    // was the adit's width only and stood level out over the falling slope: the cut showed
    // the sky through the island beside and below it.)
    constexpr float CutHalf=MineLayout::CutHalfWidth+0.05f;
    const auto cutFloor=[&](const float a,const float d) {
        Vector3 p=W(a,0,d);
        if(d<portal) p.Y=std::min(p.Y,DrawnOrAnalyticHeight(ground,terrain,p.X,p.Z)+0.05f);
        return p;
    };
    for(float d=MineLayout::CutStart-0.05f;d<portal+0.2f;d+=0.25f) {
        const float d1=std::min(d+0.25f,portal+0.2f);
        for(int k=0;k<6;++k) {
            const float a=-CutHalf+2.0f*CutHalf*static_cast<float>(k)/6.0f,a1=-CutHalf+2.0f*CutHalf*static_cast<float>(k+1)/6.0f;
            quad(cutFloor(a,d),cutFloor(a1,d),cutFloor(a1,d1),cutFloor(a,d1),Vector3(0,1,0),Gravel*stain(a,0,d));
        }
    }
    for(const float s:{-1.0f,1.0f}) {
        // the bank's top: the hillside just outside the cut, no higher than the roof's line
        const auto bankTop=[&](const float d) {
            const auto o=W(s*(CutHalf+0.05f),0,d);
            return std::min(DrawnOrAnalyticHeight(ground,terrain,o.X,o.Z)+0.05f,W(0,MineSite::Height+0.12f,d).Y);
        };
        for(float d=MineLayout::CutStart-0.05f;d<portal+0.2f;d+=0.25f) {
            const float d1=std::min(d+0.25f,portal+0.2f);
            const Vector3 b0=cutFloor(s*CutHalf,d)-Vector3::Up*0.05f,b1=cutFloor(s*CutHalf,d1)-Vector3::Up*0.05f;
            const float t0=bankTop(d),t1=bankTop(d1);
            if(t0<=b0.Y+0.06f&&t1<=b1.Y+0.06f) continue;
            const Vector3 u0(b0.X,std::max(t0,b0.Y),b0.Z),u1(b1.X,std::max(t1,b1.Y),b1.Z);
            quad(b0,b1,u1,u0,acrossDir*(-s),Vector3(96,80,62)*stain(s,d,d));
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
    // the gravel floor in a fine grid, tinted corner by corner: darker and wet round the puddles
    {
        const auto wetness=[&](const float a,const float d) {
            float wet=0.0f;
            for(const Puddle& puddle:puddles_) {
                const float x=(a-puddle.across)/(puddle.halfAcross*1.9f),z=(d-puddle.depth)/(puddle.halfAlong*1.6f);
                wet=std::max(wet,1.0f-std::clamp(x*x+z*z,0.0f,1.0f));
            }
            return wet;
        };
        const auto floorTone=[&](const float a,const float d) {
            return Gravel*((0.84f+0.24f*Noise(d*1.3f+a*0.7f,a*2.1f)+0.08f*Noise(d*5.1f,a*4.7f))*(1.0f-0.42f*wetness(a,d)));
        };
        constexpr float Cell=0.25f;
        const Vector3 up(0,1,0);
        for(float d=portal-0.1f;d<aditEnd-1e-3f;d+=Cell) {
            const float d1=std::min(d+Cell,aditEnd);
            for(float a=-MineSite::HalfWidth-0.2f;a<MineSite::HalfWidth+0.2f-1e-3f;a+=Cell) {
                const float a1=a+Cell;
                const std::array<Vector3,4> p{{W(a,0,d),W(a1,0,d),W(a1,0,d1),W(a,0,d1)}};
                const std::array<Vector3,4> t{{floorTone(a,d),floorTone(a1,d),floorTone(a1,d1),floorTone(a,d1)}};
                smoothTri({{p[0],p[1],p[2]}},{{up,up,up}},{{t[0],t[1],t[2]}});
                smoothTri({{p[0],p[2],p[3]}},{{up,up,up}},{{t[0],t[2],t[3]}});
            }
        }
        // the water: dark, still, the roof's gloom in it (the lanterns' reflections are drawn
        // with the light, from where the eye is)
        for(const Puddle& puddle:puddles_) {
            const Vector3 centre=W(puddle.across,0.012f,puddle.depth);
            const auto rim=[&](const std::size_t k) {
                const float angle=2.0f*Pi*static_cast<float>(k)/static_cast<float>(puddle.outline.size());
                const float r=puddle.outline[k%puddle.outline.size()];
                return W(puddle.across+std::cos(angle)*puddle.halfAcross*r,0.012f,puddle.depth+std::sin(angle)*puddle.halfAlong*r);
            };
            for(std::size_t k=0;k<puddle.outline.size();++k)
                smoothTri({{centre,rim(k),rim(k+1)}},{{up,up,up}},{{Water,Water*1.1f,Water*1.1f}});
        }
    }
    for(float d=portal-0.1f;d<aditEnd-1e-3f;d+=0.5f) {
        const float d1=std::min(d+0.5f,aditEnd);
        for(float a=-MineSite::HalfWidth-0.2f;a<MineSite::HalfWidth+0.2f-1e-3f;a+=0.75f) {
            const float a1=a+0.75f;
            // the rock roof above the lagging
            quad(W(a,2.68f,d),W(a1,2.68f,d),W(a1,2.68f,d1),W(a,2.68f,d1),Vector3(0,-1,0),RockDark*stain(a,2.7f,d));
        }
    }
    // the rock walls, hewn rough: broad swellings and smaller knobs, each corner lit by the
    // wall's own slope there and toned for itself, so the rock reads round, not in flat panels
    for(const float s:{-1.0f,1.0f}) {
        const auto bump=[&](const float u,const float dd) {
            return 0.14f*Noise(dd*0.9f+(s>0?11.0f:0.0f),u*1.4f)+0.05f*Noise(dd*3.1f+(s>0?5.0f:2.0f),u*3.7f+1.0f);
        };
        const auto wall=[&](const float u,const float dd) {return W(s*(MineSite::HalfWidth+bump(u,dd)),u,dd);};
        const auto wallNormal=[&](const float u,const float dd) {
            Vector3 n=Vector3::Cross(wall(u+0.04f,dd)-wall(u-0.04f,dd),wall(u,dd+0.04f)-wall(u,dd-0.04f));
            if(Vector3::Dot(n,acrossDir*(-s))<0.0f) n=-n;
            n.Normalize();
            return n;
        };
        const auto wallTone=[&](const float u,const float dd) {
            const float blotch=Noise(dd*1.3f+s*0.7f,u*1.9f+s*2.1f),grain=Noise(dd*5.1f+s,u*4.7f);
            // damp darkening the foot of the wall
            return Rock*((0.82f+0.3f*blotch+0.08f*grain)*(0.86f+0.14f*std::clamp(u/0.9f,0.0f,1.0f)));
        };
        constexpr int Rows=10;
        for(float d=portal-0.1f;d<aditEnd-1e-3f;d+=0.25f) {
            const float d1=std::min(d+0.25f,aditEnd);
            for(int k=0;k<Rows;++k) {
                const float u0=2.68f*static_cast<float>(k)/Rows,u1=2.68f*static_cast<float>(k+1)/Rows;
                const std::array<Vector3,4> p{{wall(u0,d),wall(u0,d1),wall(u1,d1),wall(u1,d)}};
                const std::array<Vector3,4> n{{wallNormal(u0,d),wallNormal(u0,d1),wallNormal(u1,d1),wallNormal(u1,d)}};
                const std::array<Vector3,4> t{{wallTone(u0,d),wallTone(u0,d1),wallTone(u1,d1),wallTone(u1,d)}};
                smoothTri({{p[0],p[1],p[2]}},{{n[0],n[1],n[2]}},{{t[0],t[1],t[2]}});
                smoothTri({{p[0],p[2],p[3]}},{{n[0],n[2],n[3]}},{{t[0],t[2],t[3]}});
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
    for(const LanternSpot& spot:spots) {
        const float a=spot.a,d=spot.d,u=spot.u;
        const float s=a>0.0f?1.0f:-1.0f;
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
        // the handle up to the nail in the post, or the bail standing over it on the floor
        box(a-0.006f,a+0.006f,u+0.17f,u+0.26f,d-0.006f,d+0.006f,Glass);
        if(spot.hanging)
            box(std::min(a,s*MineSite::PostInner)-0.006f,std::max(a,s*MineSite::PostInner)+0.006f,u+0.25f,u+0.27f,d-0.006f,d+0.006f,Glass);
        else box(a-0.05f,a+0.05f,u+0.25f,u+0.262f,d-0.006f,d+0.006f,Glass);
    }
    // ---- what the miners left: crates against the left wall, a pickaxe and a shovel against a
    // post, a fall of rubble with a broken cap beam against the right wall, a smashed lantern
    {
        const float c0=MineLayout::CrateFrom,c1=MineLayout::CrateTo,inner=MineLayout::CrateInner;
        const Vector3 crate(150,116,76);
        box(-MineSite::HalfWidth+0.05f,inner,0.0f,0.52f,c0+0.05f,c0+0.6f,crate*0.92f);
        box(-MineSite::HalfWidth+0.05f,inner,0.0f,0.5f,c0+0.62f,c1-0.05f,crate*1.02f);
        box(-MineSite::HalfWidth+0.08f,inner-0.04f,0.52f,1.0f,c0+0.12f,c0+0.62f,crate*0.85f);
        for(const float d:{c0+0.06f,c0+0.58f,c0+0.64f,c1-0.07f})
            box(inner-0.005f,inner+0.012f,0.03f,0.48f,d-0.03f,d+0.03f,TimberDark);
        // the tools leaning on the set nearest twenty metres in
        float toolSet=sets.front();
        for(const float d:sets) if(std::abs(d-20.0f)<std::abs(toolSet-20.0f)) toolSet=d;
        {
            const float d=toolSet+0.16f,a=MineSite::PostInner-0.05f;
            // a pickaxe: its haft leaning to the post, its iron head across the top
            const Vector3 foot=W(a-0.35f,0.0f,d),top=W(a-0.02f,0.86f,d);
            Vector3 up=top-foot;const float length=up.Length();up.Normalize();
            const Vector3 across=Vector3::Cross(up,inward);
            for(int k=0;k<6;++k) {
                const Vector3 p0=foot+up*(length*static_cast<float>(k)/6.0f),p1=foot+up*(length*static_cast<float>(k+1)/6.0f);
                quad(p0-across*0.018f,p1-across*0.018f,p1+across*0.018f,p0+across*0.018f,-acrossDir,Timber*1.1f);
                quad(p0-inward*0.018f,p1-inward*0.018f,p1+inward*0.018f,p0+inward*0.018f,-inward,Timber*1.05f);
            }
            box(a-0.06f,a+0.0f,0.84f,0.9f,d-0.28f,d+0.28f,RailIron*0.9f);
            // a shovel beside it
            const float d2=d+0.32f;
            box(a-0.42f,a-0.38f,0.0f,1.0f,d2-0.015f,d2+0.015f,Timber);
            box(a-0.46f,a-0.36f,0.02f,0.32f,d2-0.11f,d2+0.11f,RailIron*0.75f);
        }
        // the fall: blocks of rock heaped against the right wall, a cap beam come down across it
        const float r0=MineLayout::RubbleFrom,r1=MineLayout::RubbleTo,rin=MineLayout::RubbleInner;
        for(int k=0;k<16;++k) {
            const float t=static_cast<float>(k)/15.0f;
            const float d=r0+0.15f+(r1-r0-0.3f)*Noise(static_cast<float>(k)*1.7f,3.1f);
            const float across=rin+0.1f+(MineSite::HalfWidth-rin-0.15f)*Noise(static_cast<float>(k)*2.3f,7.7f);
            const float size=0.14f+0.18f*Noise(static_cast<float>(k)*3.1f,1.3f);
            const float lift=std::max(0.0f,0.55f*(1.0f-std::abs(across-MineSite::HalfWidth)/0.9f))*(0.4f+0.6f*t);
            box(across-size,across+size,lift,lift+size*1.4f,d-size,d+size,Rock*(0.7f+0.4f*Noise(static_cast<float>(k),9.0f)));
        }
        {
            const Vector3 high=W(MineSite::HalfWidth-0.05f,1.9f,r0+0.3f),low=W(rin+0.05f,0.12f,r1-0.2f);
            Vector3 along=low-high;const float length=along.Length();along.Normalize();
            Vector3 side=Vector3::Cross(along,Vector3::Up);side.Normalize();
            const Vector3 up=Vector3::Cross(side,along);
            for(int k=0;k<4;++k) {
                const Vector3 p0=high+along*(length*static_cast<float>(k)/4.0f),p1=high+along*(length*static_cast<float>(k+1)/4.0f);
                quad(p0+up*0.1f-side*0.1f,p1+up*0.1f-side*0.1f,p1+up*0.1f+side*0.1f,p0+up*0.1f+side*0.1f,up,Timber*0.8f);
                quad(p0-side*0.1f-up*0.1f,p1-side*0.1f-up*0.1f,p1-side*0.1f+up*0.1f,p0-side*0.1f+up*0.1f,-side,Timber*0.75f);
                quad(p0+side*0.1f-up*0.1f,p1+side*0.1f-up*0.1f,p1+side*0.1f+up*0.1f,p0+side*0.1f+up*0.1f,side,Timber*0.75f);
            }
        }
        // a lantern dropped and smashed, lying on its side by the track
        {
            const float a=-0.75f,d=36.2f;
            box(a-0.11f,a+0.09f,0.0f,0.12f,d-0.07f,d+0.07f,Glass*1.1f);
            box(a+0.09f,a+0.13f,0.01f,0.11f,d-0.06f,d+0.06f,Glass*1.3f);
            for(int k=0;k<6;++k) {
                const float sx=a+0.25f*(Noise(static_cast<float>(k)*4.1f,2.2f)-0.3f),sz=d+0.3f*(Noise(static_cast<float>(k)*5.3f,6.6f)-0.5f);
                quad(W(sx,0.005f,sz),W(sx+0.03f,0.005f,sz+0.01f),W(sx+0.02f,0.005f,sz+0.035f),W(sx-0.01f,0.005f,sz+0.02f),Vector3(0,1,0),Vector3(120,124,118));
            }
        }
    }
    // ---- the chamber: a rough dome round an oval floor, open where the adit comes in
    {
        constexpr int Around=64,Rise=16;
        // the rock's surface: hewn out rough, broad bulges and smaller knobs, the wall standing
        // back from the oval in recesses (never in on the walker's floor) and gentler toward the
        // crown, where the bats hang; periodic round the oval, so the seam at a full turn closes
        const auto wallPoint=[&](const float theta,const float rise) {
            const float up=MineSite::ChamberHeight*std::sin(rise*Pi*0.5f);
            float a=0,d=0;chamberWall(theta,up,a,d);
            const float c=std::cos(theta),s=std::sin(theta);
            const float broad=Noise(c*2.6f+7.0f,s*2.6f+7.0f+rise*4.3f)-0.5f;
            const float recess=std::max(0.0f,Noise(c*4.1f+11.0f,s*4.1f+5.0f+rise*6.7f)-0.4f);
            const float knobs=Noise(c*7.0f+3.0f,s*7.0f+1.0f+rise*11.0f);
            const float low=(1.0f-rise)*(1.0f-rise);
            const float rough=1.0f+0.08f*broad+(0.2f*recess+0.06f*knobs)*low;
            return W(a*rough,up,MineSite::ChamberCentre+(d-MineSite::ChamberCentre)*rough);
        };
        const Vector3 middle=W(0.0f,1.2f,MineSite::ChamberCentre);
        const auto wallNormal=[&](const float theta,const float rise) {
            const Vector3 p=wallPoint(theta,rise);
            const float r0=std::max(0.0f,rise-0.02f),r1=std::min(1.0f,rise+0.02f);
            Vector3 n=Vector3::Cross(wallPoint(theta+0.02f,rise)-wallPoint(theta-0.02f,rise),wallPoint(theta,r1)-wallPoint(theta,r0));
            if(n.LengthSquared()<1e-10f) n=middle-p;
            n.Normalize();
            if(Vector3::Dot(n,middle-p)<0.0f) n=-n;
            // the crown's segments all meet at one point: one normal there, straight down, so no
            // star of shading shows round it
            n=Vector3::Lerp(n,-Vector3::Up,std::clamp((rise-0.8f)/0.2f,0.0f,1.0f));
            n.Normalize();
            return n;
        };
        // the rock's tone: blotched and banded, a little darker low down where the damp gets in
        const auto wallTone=[&](const float theta,const float rise) {
            // read across the plan (shrinking to the crown), so the segments agree where they meet
            const float reach=std::cos(rise*Pi*0.5f);
            const float c=std::cos(theta)*reach,s=std::sin(theta)*reach;
            const float blotch=Noise(c*3.1f+2.0f,s*3.1f+rise*5.2f),grain=Noise(c*9.0f+1.0f,s*9.0f+rise*13.0f);
            const float band=0.94f+0.06f*std::sin(rise*19.0f+blotch*4.0f);
            return Rock*((0.8f+0.28f*blotch+0.1f*grain)*band*(0.86f+0.14f*rise)*0.95f);
        };
        for(int i=0;i<Around;++i) for(int j=0;j<Rise;++j) {
            const float t0=2.0f*Pi*static_cast<float>(i)/Around,t1=2.0f*Pi*static_cast<float>(i+1)/Around;
            const float r0=static_cast<float>(j)/Rise,r1=static_cast<float>(j+1)/Rise;
            const Vector3 p00=wallPoint(t0,r0),p10=wallPoint(t1,r0),p11=wallPoint(t1,r1),p01=wallPoint(t0,r1);
            const Vector3 c=(p00+p10+p11+p01)*0.25f;
            const auto local=layout.Local({c.X,c.Y,c.Z});
            if(std::abs(local.x)<MineSite::HalfWidth+0.15f&&local.z<48.6f&&local.y-MineLayout::FloorRelative(local.z)<2.75f) continue;
            const Vector3 n00=wallNormal(t0,r0),n10=wallNormal(t1,r0),n11=wallNormal(t1,r1),n01=wallNormal(t0,r1);
            const Vector3 c00=wallTone(t0,r0),c10=wallTone(t1,r0),c11=wallTone(t1,r1),c01=wallTone(t0,r1);
            smoothTri({{p00,p10,p11}},{{n00,n10,n11}},{{c00,c10,c11}});
            smoothTri({{p00,p11,p01}},{{n00,n11,n01}},{{c00,c11,c01}});
        }
        // the headwall round the adit's mouth, closing the dome down to it
        {
            const float d=47.98f,top=MineSite::ChamberHeight+1.2f,side=4.2f,roof=2.68f;
            const Vector3 n=inward;
            quad(W(-side,0.0f,d),W(-MineSite::HalfWidth-0.15f,0.0f,d),W(-MineSite::HalfWidth-0.15f,top,d),W(-side,top,d),n,RockDark);
            quad(W(MineSite::HalfWidth+0.15f,0.0f,d),W(side,0.0f,d),W(side,top,d),W(MineSite::HalfWidth+0.15f,top,d),n,RockDark);
            quad(W(-MineSite::HalfWidth-0.15f,roof,d),W(MineSite::HalfWidth+0.15f,roof,d),W(MineSite::HalfWidth+0.15f,top,d),W(-MineSite::HalfWidth-0.15f,top,d),n,RockDark);
        }
        // the floor: gravel over uneven rock, a little hummocky, in a grid reaching under the
        // walls' foot (it starts where the adit's floor ends)
        {
            constexpr float Cell=0.35f;
            const float reachA=MineSite::ChamberHalfWidth*1.32f,reachD=MineSite::ChamberHalfLength*1.32f;
            const auto bump=[](const float a,const float d) {
                return 0.025f*(Noise(a*2.3f+4.0f,d*2.3f)-0.5f)+0.012f*(Noise(a*6.1f,d*6.1f+2.0f)-0.5f);
            };
            const auto floorAt=[&](const float a,const float d) {return W(a,bump(a,d),d);};
            const auto floorNormal=[&](const float a,const float d) {
                Vector3 n(-(bump(a+0.05f,d)-bump(a-0.05f,d))/0.1f,1.0f,-(bump(a,d+0.05f)-bump(a,d-0.05f))/0.1f);
                n.Normalize();
                return n;
            };
            const auto floorTone=[&](const float a,const float d) {
                const float damp=std::clamp((Noise(a*0.9f+8.0f,d*0.9f)-0.62f)*3.0f,0.0f,1.0f);
                return Gravel*((0.84f+0.24f*Noise(a*1.7f,d*1.7f+3.0f)+0.08f*Noise(a*7.3f,d*7.3f))*(1.0f-0.25f*damp));
            };
            for(float d=48.0f;d<MineSite::ChamberCentre+reachD;d+=Cell)
                for(float a=-reachA;a<reachA;a+=Cell) {
                    const float ca=(a+Cell*0.5f)/reachA,cd=(d+Cell*0.5f-MineSite::ChamberCentre)/reachD;
                    if(ca*ca+cd*cd>1.0f) continue;
                    const float a1=a+Cell,d1=d+Cell;
                    const std::array<Vector3,4> p{{floorAt(a,d),floorAt(a1,d),floorAt(a1,d1),floorAt(a,d1)}};
                    const std::array<Vector3,4> n{{floorNormal(a,d),floorNormal(a1,d),floorNormal(a1,d1),floorNormal(a,d1)}};
                    const std::array<Vector3,4> t{{floorTone(a,d),floorTone(a1,d),floorTone(a1,d1),floorTone(a,d1)}};
                    smoothTri({{p[0],p[1],p[2]}},{{n[0],n[1],n[2]}},{{t[0],t[1],t[2]}});
                    smoothTri({{p[0],p[2],p[3]}},{{n[0],n[2],n[3]}},{{t[0],t[2],t[3]}});
                }
        }
        // the crystals: hexagonal prisms with pointed tips growing from the wall, glowing
        for(std::size_t ci=0;ci<clusters.size();++ci) {
            const Cluster& c=clusters[ci];
            const Vector3 colourOf=c.violet?CrystalViolet:CrystalBlue;
            // rooted in the rock's real surface, a little into it
            const Vector3 surface=wallPoint(c.theta,std::asin(std::clamp(c.up/MineSite::ChamberHeight,0.0f,1.0f))/(Pi*0.5f));
            const Vector3 root=surface+(surface-middle)*0.03f;
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
            // the spilled ore: knobbly lumps strewn out of the tub's mouth, half sunk in the gravel
            for(int k=0;k<14;++k) {
                const auto seed=static_cast<std::uint32_t>(k)*2654435761U;
                const float oa=ca-0.4f-0.9f*Hash(seed),od=cd-0.6f+1.2f*Hash(seed^0x9U),size=0.06f+0.08f*Hash(seed^0x3U);
                lump(W(oa,size*0.3f,od),size,Ore*(0.8f+0.4f*Hash(seed^0x5U)),seed);
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
    // the lanterns in the puddles: each lantern's flame mirrored in the water where the line
    // from the eye to its image under the floor crosses the puddle, a soft glint drawn out toward
    // the eye; it slides over the water as the walker moves and leaves it at the edge
    reflectionStaging_.clear();
    glints_=0;
    for(const Puddle& puddle:puddles_) {
        const auto centre=layout_.World(puddle.across,MineLayout::FloorRelative(puddle.depth)+0.012f,puddle.depth);
        const Vector3 middle(centre.x,centre.y,centre.z);
        if(Vector3::DistanceSquared(middle,camera)>30.0f*30.0f||camera.Y<=middle.Y) continue;
        for(const Vector3& lamp:lanternPositions_) {
            if(Vector3::DistanceSquared(lamp,middle)>7.0f*7.0f) continue;
            const Vector3 image(lamp.X,2.0f*middle.Y-lamp.Y,lamp.Z);
            const float t=(camera.Y-middle.Y)/(camera.Y-image.Y);
            const Vector3 glint=camera+(image-camera)*t;
            const auto local=layout_.Local({glint.X,glint.Y,glint.Z});
            const float x=(local.x-puddle.across)/puddle.halfAcross,z=(local.z-puddle.depth)/puddle.halfAlong;
            const float inside=1.0f-SmoothStep(0.55f,0.85f,std::sqrt(x*x+z*z));
            if(inside<=0.0f||reflectionStaging_.size()+60U>ReflectionCapacity) continue;
            // still water at a low angle gives back most of the light: the flame nearly as bright
            // in it as in the lantern
            const float strength=0.9f*flicker*inside;
            Vector3 toEye=camera-glint;toEye.Y=0.0f;
            if(toEye.LengthSquared()<1e-6f) toEye=Vector3::UnitZ;
            toEye.Normalize();
            const Vector3 across(-toEye.Z,0.0f,toEye.X);
            const float grazing=std::clamp((camera.Y-middle.Y)/std::max(0.1f,Vector3::Distance(camera,glint)),0.1f,1.0f);
            // the lantern's image: as wide as its glass seen from twice as far, drawn out toward the
            // eye by the low angle; round it, its halo
            const Vector3 at=glint+Vector3::Up*0.003f;
            ++glints_;
            const auto oval=[&](const float width,const float length,const float r,const float g,const float b) {
                const Color core(static_cast<int>(255.0f*std::min(1.0f,r)),static_cast<int>(255.0f*std::min(1.0f,g)),
                                 static_cast<int>(255.0f*std::min(1.0f,b))),edge(0,0,0);
                constexpr int Segments=10;
                for(int k=0;k<Segments;++k) {
                    const float a0=2.0f*Pi*static_cast<float>(k)/Segments,a1=2.0f*Pi*static_cast<float>(k+1)/Segments;
                    reflectionStaging_.emplace_back(at,core);
                    reflectionStaging_.emplace_back(at+across*(std::cos(a0)*width)+toEye*(std::sin(a0)*length),edge);
                    reflectionStaging_.emplace_back(at+across*(std::cos(a1)*width)+toEye*(std::sin(a1)*length),edge);
                }
            };
            oval(0.07f,0.12f/grazing,strength,0.82f*strength,0.5f*strength);
            oval(0.22f,0.3f/grazing,0.3f*strength,0.2f*strength,0.09f*strength);
        }
    }
    // the dust hanging in the lanterns' light: specks drifting slowly round each flame near the
    // eye, lit by how close they float to it, fading out at the light's edge (each to itself, so
    // nothing pops)
    {
        for(std::size_t li=0;li<lanternPositions_.size();++li) {
            const Vector3& lamp=lanternPositions_[li];
            const float away=Vector3::Distance(lamp,camera);
            if(away>14.0f) continue;
            const float nearFade=1.0f-SmoothStep(9.0f,14.0f,away);
            Vector3 toEye=camera-lamp;toEye.Normalize();
            Vector3 right=Vector3::Cross(toEye,Vector3::Up);
            if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
            right.Normalize();
            const Vector3 up=Vector3::Cross(right,toEye);
            constexpr int Motes=36;
            for(int k=0;k<Motes&&reflectionStaging_.size()+12U<=ReflectionCapacity;++k) {
                const auto seed=static_cast<std::uint32_t>(li*977U+static_cast<std::size_t>(k)*131U);
                const float phase=6.2831853f*Hash(seed),rate=0.05f+0.07f*Hash(seed^0x5U);
                const float t=seconds*rate+phase;
                const Vector3 offset(1.1f*std::sin(t*1.3f+Hash(seed^0x9U)*6.0f),0.55f*std::sin(t*0.7f+phase)-0.1f,
                                     1.1f*std::cos(t*1.1f+Hash(seed^0x3U)*6.0f));
                const Vector3 at=lamp+offset;
                // only in the adit's air, not in its rock
                const auto local=layout_.Local({at.X,at.Y,at.Z});
                if(std::abs(local.x)>MineSite::PostInner-0.05f||local.y-MineLayout::FloorRelative(local.z)<0.2f
                   ||local.y-MineLayout::FloorRelative(local.z)>2.5f) continue;
                const float glow=std::max(0.0f,1.0f-offset.Length()/1.3f);
                const float strength=glow*glow*nearFade*flicker*(0.5f+0.5f*Hash(seed^0x7U));
                if(strength<0.02f) continue;
                const Color core(static_cast<int>(std::min(255.0f,330.0f*strength)),static_cast<int>(std::min(255.0f,250.0f*strength)),
                                 static_cast<int>(std::min(255.0f,150.0f*strength))),edge(0,0,0);
                const float size=0.014f+0.01f*Hash(seed^0x11U);
                for(int q=0;q<4;++q) {
                    const float a0=1.5707963f*static_cast<float>(q),a1=a0+1.5707963f;
                    reflectionStaging_.emplace_back(at,core);
                    reflectionStaging_.emplace_back(at+(right*std::cos(a0)+up*std::sin(a0))*size,edge);
                    reflectionStaging_.emplace_back(at+(right*std::cos(a1)+up*std::sin(a1))*size,edge);
                }
            }
        }
    }
    if(!reflectionStaging_.empty()) {
        effect_->setDiffuseColorProperty(Vector3(1.0f,1.0f,1.0f));effect_->Apply();
        reflections_->SetData(reflectionStaging_.data(),0,static_cast<int>(reflectionStaging_.size()),SetDataOptions::Discard);
        device_.SetVertexBuffer(reflections_.get());device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(reflectionStaging_.size()/3U));++drawCalls_;
    }
    effect_->setDiffuseColorProperty(Vector3(1.0f,1.0f,1.0f));
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}
}
