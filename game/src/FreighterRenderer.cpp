#include "IslandSimulator/FreighterRenderer.hpp"
#include "IslandSimulator/HumanSkeleton.hpp"
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
#include <cstdint>
#include <iostream>
#include <numbers>
#include <set>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
using P=FreighterPlacement;
constexpr float Pi=std::numbers::pi_v<float>;
constexpr float Waterline=2.25f,BootTop=2.45f;
// the paints and the bare metal, past full for the build's light
const Vector3 AntiFouling(158,64,48),Boot(236,232,220),Topside(50,54,58),BulwarkInside(184,170,142),
    Deck(118,104,88),Hold(128,98,72),House(244,240,226),HouseInside(222,210,182),Glass(36,48,54),
    Funnel(222,174,74),FunnelTop(34,34,34),Mast(214,190,134),Coaming(88,116,98),Steel(146,134,116),
    Tread(104,96,86),Iron(66,68,70),Chain(84,70,60),Rust(150,78,42),Weed(70,88,52),Wood(146,112,78),
    Brass(180,140,72),Guano(236,234,224);
}

FreighterRenderer::FreighterRenderer(GraphicsDevice& device,const FreighterPlacement& ship,
                                     const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.stainColour=Vector3(196,178,140);b.stainHeight=0.5f;b.stainAmount=0.55f;
    // weathered corner by corner, so the paint fades and rusts smoothly across the plates
    b.tintPerVertex=true;
    // rust weeping down the topsides: from the scuppers along the deck's edge, from the fittings
    // on the bulwark's rail, and heaviest from the hawse pipes at the bow; each a thin streak of
    // its own strength and length, widening and fading as it runs down the black (the topsides
    // and the bulwarks are built fine enough to carry them)
    struct Streak {float along,top,length,width,strength,side;};
    std::vector<Streak> streaks,droppings;
    {
        std::uint32_t seed=0x51a7U;
        const auto next=[&seed]() {seed=seed*1664525U+1013904223U;return static_cast<float>(seed>>8U)*(1.0f/16777216.0f);};
        for(const float s:{-1.0f,1.0f}) {
            for(float a=2.6f+next();a<27.5f;a+=1.3f+1.2f*next())
                streaks.push_back({a,P::DeckUp(a)+0.03f,0.7f+1.9f*next(),0.04f+0.07f*next(),0.55f+0.4f*next(),s});
            for(float a=1.5f+2.0f*next();a<28.0f;a+=2.5f+3.0f*next())
                streaks.push_back({a,P::DeckUp(a)+P::Bulwark,0.35f+0.9f*next(),0.025f+0.03f*next(),0.3f+0.25f*next(),s});
            streaks.push_back({28.9f,P::DeckUp(28.9f)-0.45f,2.0f,0.15f,0.9f,s});
        }
        // each ends on the black, above the boot-top
        for(Streak& streak:streaks) streak.length=std::min(streak.length,streak.top-BootTop-0.08f);
        // and white down the port bow from the rail where the gulls stand (FreighterPlacement::
        // BirdPerches): the droppings of the birds
        for(const float a:{27.35f,27.6f,27.8f})
            droppings.push_back({a+0.06f*(next()-0.5f),P::DeckUp(a)+P::Bulwark+0.02f,0.5f+0.8f*next(),0.035f+0.02f*next(),0.75f+0.2f*next(),-1.0f});
    }
    const auto field=[](const std::vector<Streak>& list,const float along,const float up,const float across) {
        float rust=0.0f;
        for(const Streak& s:list) {
            if(s.side*across<=0.0f) continue;
            const float below=s.top-up;
            if(below<-0.02f||below>s.length) continue;
            const float run=std::clamp(below/s.length,0.0f,1.0f);
            const float wander=0.025f*std::sin(below*3.1f+s.along*7.0f);
            const float x=(along-s.along-wander)/(s.width*(0.6f+0.9f*run));
            const float start=std::clamp((below+0.02f)/0.05f,0.0f,1.0f);
            rust=std::max(rust,s.strength*std::exp(-x*x)*std::pow(1.0f-run,1.4f)*start);
        }
        return rust;
    };
    const auto weeping=[&streaks,field](const float along,const float up,const float across) {return field(streaks,along,up,across);};
    const auto whitewash=[&droppings,field](const float along,const float up,const float across) {return field(droppings,along,up,across);};
    // blistered rust patches, the black faded chalky by the sun, weed at the waterline; the glass
    // and the brass left as they are; the birds' droppings white under their perches
    b.tint=[&ship,weeping,whitewash](const Vector3& paint,const Vector3& normal,const Vector3& centre) {
        const FreighterPoint local=ship.Local({centre.X,centre.Y,centre.Z});
        const auto is=[](const Vector3& a,const Vector3& b) {return a.X==b.X&&a.Y==b.Y&&a.Z==b.Z;};
        const Vector3& pigment=paint;
        const auto same=[&](const Vector3& other) {return is(pigment,other);};
        if(same(Glass)||same(Brass)) return pigment;
        float rust=0.0f;
        const float patch=std::sin(local.x*0.83f+local.z*1.7f+local.y*0.6f)*std::sin(local.x*1.9f-local.y*1.1f+local.z*0.7f);
        rust+=std::clamp((patch-0.58f)*2.5f,0.0f,1.0f)*(normal.Y>0.6f?0.2f:0.32f);
        // the white paint holds better: only thin weeping under the fittings
        if(same(House)||same(HouseInside)||same(Boot)) rust*=0.35f;
        Vector3 colour=pigment;
        if(same(Topside)) {
            // the black faded chalky grey by years of sun, most high up and in patches
            const float high=std::clamp((local.y-BootTop)/(P::DeckUp(local.x)+P::Bulwark-BootTop),0.0f,1.0f);
            const float blotch=0.5f+0.5f*std::sin(local.x*0.41f+local.y*1.3f)*std::sin(local.x*0.23f-local.y*0.7f+1.3f);
            colour=Vector3::Lerp(colour,Vector3(104,106,108),0.14f+0.22f*high*blotch);
        }
        colour=Vector3::Lerp(colour,Rust,std::min(rust,0.75f));
        if(same(Topside)&&std::abs(normal.Y)<0.6f) {
            colour=Vector3::Lerp(colour,Rust*1.15f,std::min(0.9f,weeping(local.x,local.y,local.z)));
            colour=Vector3::Lerp(colour,Guano,std::min(0.9f,whitewash(local.x,local.y,local.z)));
        }
        // the mastheads whitewashed where the birds stand on the trucks
        if(same(Mast)) {
            const float a=std::abs(local.x-P::ForeMast)<std::abs(local.x-P::MainMast)?P::ForeMast:P::MainMast;
            const float top=P::DeckUp(a)+(a==P::ForeMast?8.0f:9.0f);
            if(std::abs(local.x-a)<0.4f&&std::abs(local.z)<0.4f)
                colour=Vector3::Lerp(colour,Guano,std::clamp(1.0f-(top-local.y)/0.55f,0.0f,1.0f)*0.85f);
        }
        const float weed=1.0f-std::clamp(std::abs(centre.Y+0.05f)/0.45f,0.0f,1.0f);
        colour=Vector3::Lerp(colour,Weed,weed*0.6f);
        return colour*(0.97f+0.03f*std::sin(local.x*3.1f+local.y*2.3f+local.z*1.7f));
    };
    const auto W=[&](const float a,const float u,const float c) {const auto p=ship.World(a,u,c);return Vector3(p.x,p.y,p.z);};
    const auto D=[](const float a) {return P::DeckUp(a);};
    const Vector3 fwd=W(1,0,0)-W(0,0,0),side=W(0,0,1)-W(0,0,0);
    // a box from eight corners (the bottom four, then the top four, in the same order)
    const auto hexa=[&](const std::array<Vector3,8>& p,const Vector3& pigment,const Vector3& top) {
        Vector3 centre(0,0,0);
        for(const auto& v:p) centre+=v;
        b.inside=centre/8.0f;
        b.Quad(p[4],p[5],p[6],p[7],top);b.Quad(p[0],p[1],p[2],p[3],pigment);
        for(int k=0;k<4;++k) b.Quad(p[k],p[(k+1)%4],p[4+(k+1)%4],p[4+k],pigment);
    };
    // a box in the ship's frame
    const auto box=[&](const float a0,const float a1,const float u0,const float u1,const float c0,const float c1,
                       const Vector3& pigment,const Vector3& top) {
        hexa({W(a0,u0,c0),W(a1,u0,c0),W(a1,u0,c1),W(a0,u0,c1),W(a0,u1,c0),W(a1,u1,c0),W(a1,u1,c1),W(a0,u1,c1)},pigment,top);
    };
    // one face of a wall or floor, turned toward `toward` (a point it faces)
    const auto face=[&](const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3,
                        const Vector3& toward,const Vector3& pigment) {
        const Vector3 centre=(p0+p1+p2+p3)/4.0f;
        b.inside=centre-(toward-centre)*0.01f;
        b.Quad(p0,p1,p2,p3,pigment);
    };
    // the hold's gloom: full light under the open hatch, falling off under the deck
    const auto holdShade=[](const float a,const float c) {
        const float da=std::max({P::HatchAft-a,0.0f,a-P::HatchFore}),dc=std::max(std::abs(c)-P::HatchHalf,0.0f);
        return 0.26f+0.74f*std::exp(-std::hypot(da,dc)/1.4f);
    };

    // ---- the hull's shell: red below the waterline's white boot-top, black above; sanded where
    // it meets the beach
    b.ground=groundAt;
    {
        const auto keelUp=[](const float a) {
            if(a<2.5f) return (2.5f-a)/2.5f*2.0f;
            if(a>27.0f) return (a-27.0f)/3.0f*P::DeckUp(P::Length)*0.96f;
            return 0.0f;
        };
        const auto breadth=[&](const float a,const float u) {
            const float k=keelUp(a),d=D(a);
            const float t=std::clamp((u-k)/std::max(d-k,1e-3f),0.0f,1.0f);
            // a flat bottom turning up through a tight bilge into the plumb side
            constexpr float Bilge=0.16f;
            float fraction=t>=Bilge?1.0f:0.72f+0.28f*std::sqrt(std::max(0.0f,1.0f-((Bilge-t)/Bilge)*((Bilge-t)/Bilge)));
            // the bow's fine entrance and the stern's run below the waterline
            const float fine=std::clamp((a-21.0f)/9.0f,0.0f,1.0f)*0.75f+std::clamp((3.5f-a)/3.5f,0.0f,1.0f)*0.35f;
            fraction*=1.0f-fine*std::pow(1.0f-t,1.5f);
            return P::HalfBreadth(a)*fraction;
        };
        // below the boot-top in quarter-metre stations; the black topsides, where the rust weeps,
        // in a fine grid, so a streak a few centimetres wide reads corner by corner
        const std::vector<float> lowRows{{0.0f,0.03f,0.12f,0.28f,0.5f,0.75f,1.1f,1.6f,Waterline,BootTop}};
        std::vector<float> highRows;
        for(float u=BootTop;u<P::DeckUp(P::Length)+0.15f;u+=0.15f) highRows.push_back(u);
        highRows.push_back(99.0f);
        std::vector<float> stations,fineStations;
        for(float a=0.0f;a<=P::Length+1e-3f;a+=0.25f) stations.push_back(a);
        for(int i=0;i<=500;++i) fineStations.push_back(P::Length*static_cast<float>(i)/500.0f);
        const auto shell=[&](const std::vector<float>& along,const std::vector<float>& rows) {
            const auto rowUp=[&](const float a,const std::size_t k) {return std::clamp(rows[k],keelUp(a),D(a));};
            for(std::size_t i=0;i+1<along.size();++i) {
                const float a0=along[i],a1=along[i+1];
                for(std::size_t k=0;k+1<rows.size();++k) {
                    const float u00=rowUp(a0,k),u01=rowUp(a0,k+1),u10=rowUp(a1,k),u11=rowUp(a1,k+1);
                    if(u01-u00<1e-4f&&u11-u10<1e-4f) continue;
                    const float middle=(u00+u01+u10+u11)*0.25f;
                    const Vector3 pigment=middle<Waterline?AntiFouling:(middle<BootTop?Boot:Topside);
                    b.inside=W((a0+a1)*0.5f,middle,0.0f);
                    for(const float s:{-1.0f,1.0f})
                        b.Quad(W(a0,u00,s*breadth(a0,u00)),W(a1,u10,s*breadth(a1,u10)),
                               W(a1,u11,s*breadth(a1,u11)),W(a0,u01,s*breadth(a0,u01)),pigment);
                }
            }
        };
        shell(stations,lowRows);
        shell(fineStations,highRows);
        // the flat bottom across the keel
        for(std::size_t i=0;i+1<stations.size();++i) {
            const float a0=stations[i],a1=stations[i+1];
            const float k0=keelUp(a0),k1=keelUp(a1);
            b.inside=W((a0+a1)*0.5f,(k0+k1)*0.5f+1.0f,0.0f);
            b.Quad(W(a0,k0,-breadth(a0,k0)),W(a1,k1,-breadth(a1,k1)),W(a1,k1,breadth(a1,k1)),W(a0,k0,breadth(a0,k0)),AntiFouling);
        }
        // the transom
        b.inside=W(1.0f,3.2f,0.0f);
        const std::array<float,13> rows{{0.0f,0.03f,0.12f,0.28f,0.5f,0.75f,1.1f,1.6f,Waterline,BootTop,3.1f,3.7f,99.0f}};
        for(std::size_t k=0;k+1<rows.size();++k) {
            const float u0=std::clamp(rows[k],keelUp(0.0f),D(0.0f)),u1=std::clamp(rows[k+1],keelUp(0.0f),D(0.0f));
            const Vector3 pigment=(u0+u1)*0.5f<BootTop?Boot:Topside;
            b.Quad(W(0.0f,u0,-breadth(0.0f,u0)),W(0.0f,u0,breadth(0.0f,u0)),W(0.0f,u1,breadth(0.0f,u1)),W(0.0f,u1,-breadth(0.0f,u1)),pigment);
        }
    }
    b.ground=nullptr;
    // ---- her name, painted in white on both bows and across the transom over her port of
    // registry: strokes laid on the plating, rusting with it
    {
        struct Stroke {float x0,y0,x1,y1;};
        const auto letter=[](const char c)->std::vector<Stroke> {
            switch(c) {
            case 'M': return {{0,0,0,1.5f},{0,1.5f,0.5f,0.8f},{0.5f,0.8f,1,1.5f},{1,1.5f,1,0}};
            case 'A': return {{0,0,0.5f,1.5f},{0.5f,1.5f,1,0},{0.22f,0.6f,0.78f,0.6f}};
            case 'R': return {{0,0,0,1.5f},{0,1.5f,0.75f,1.5f},{0.75f,1.5f,1,1.28f},{1,1.28f,1,0.97f},
                              {1,0.97f,0.75f,0.75f},{0.75f,0.75f,0,0.75f},{0.45f,0.75f,1,0}};
            case 'I': return {{0.5f,0,0.5f,1.5f}};
            case 'K': return {{0,0,0,1.5f},{1,1.5f,0,0.6f},{0.3f,0.85f,1,0}};
            case 'P': return {{0,0,0,1.5f},{0,1.5f,0.75f,1.5f},{0.75f,1.5f,1,1.28f},{1,1.28f,1,0.97f},
                              {1,0.97f,0.75f,0.75f},{0.75f,0.75f,0,0.75f}};
            case 'N': return {{0,0,0,1.5f},{0,1.5f,1,0},{1,0,1,1.5f}};
            default: return {};
            }
        };
        const Vector3 paint(236,232,218);
        // the text's strokes mapped onto a surface: (position along the text, height) to the world
        const auto write=[&](const char* text,const float height,const float width,const auto& onHull,const Vector3& hollow) {
            const float scale=height/1.5f,advance=(width+0.35f)*scale/0.6f*0.6f,stroke=0.12f*scale;
            float origin=0.0f;
            for(const char* c=text;*c;++c) {
                for(const Stroke& s:letter(*c)) {
                    const float ax=origin+s.x0*width*scale/0.6f*0.6f,ay=s.y0*scale,bx=origin+s.x1*width*scale/0.6f*0.6f,by=s.y1*scale;
                    float nx=-(by-ay),ny=bx-ax;
                    const float n=std::hypot(nx,ny);
                    if(n<1e-5f) continue;
                    nx*=stroke*0.5f/n;ny*=stroke*0.5f/n;
                    // reach a little past each end so the strokes join at the corners
                    const float ex=(bx-ax)/std::hypot(bx-ax,by-ay)*stroke*0.4f,ey=(by-ay)/std::hypot(bx-ax,by-ay)*stroke*0.4f;
                    b.inside=hollow;
                    b.Quad(onHull(ax-ex-nx,ay-ey-ny),onHull(bx+ex-nx,by+ey-ny),onHull(bx+ex+nx,by+ey+ny),onHull(ax-ex+nx,ay-ey+ny),paint);
                }
                origin+=*c==' '?advance*0.8f:advance;
            }
            return origin;
        };
        const char* name="MARIA K";
        const float letterHeight=0.75f,nameUp=D(26.0f)-1.05f;
        // the width of the name in metres, for starting the starboard side's text aft of the bow
        float nameLength=0.0f;
        {
            const float scale=letterHeight/1.5f,advance=(0.6f+0.35f)*scale;
            for(const char* c=name;*c;++c) nameLength+=*c==' '?advance*0.8f:advance;
        }
        for(const float side:{-1.0f,1.0f}) {
            // read left to right from outside: from the bow aft on the port side, toward the bow on the starboard
            const float start=side<0.0f?27.6f:27.6f-nameLength;
            const auto onSide=[&](const float x,const float y) {
                const float along=side<0.0f?start-x:start+x;
                return W(along,nameUp+y,side*(P::HalfBreadth(along)+0.012f));
            };
            write(name,letterHeight,0.6f,onSide,W(24.0f,nameUp,0.0f));
        }
        const auto onTransom=[&](const float y0,const float length) {
            return [=,&W](const float x,const float y) {return W(-0.012f,y0+y,-length*0.5f+x);};
        };
        // centred on the transom: six letters and a space, then six letters
        write(name,0.6f,0.6f,onTransom(D(0.0f)-0.95f,2.58f),W(1.5f,3.4f,0.0f));
        write("PANAMA",0.42f,0.6f,onTransom(D(0.0f)-1.65f,1.6f),W(1.5f,3.0f,0.0f));
    }
    // ---- the bulwarks: black outside, buff inside, a capping rail; the gangway's opening
    for(float a=0.0f;a<P::Length-1e-3f;a+=0.25f) {
        const float a1=a+0.25f;
        for(const float s:{-1.0f,1.0f}) {
            const float middle=a+0.125f;
            if(s>0.0f&&middle>P::GangwayAft&&middle<P::GangwayFore) continue;
            const float h0=P::HalfBreadth(a),h1=P::HalfBreadth(a1);
            const float i0=std::max(h0-0.1f,0.0f),i1=std::max(h1-0.1f,0.0f);
            const Vector3 outside=W(middle,D(middle)+0.5f,s*(P::HalfBreadth(middle)+2.0f));
            const Vector3 inboard=W(middle,D(middle)+0.5f,0.0f);
            // the outside in a fine grid, for the rust weeping down it from the rail's fittings
            for(int c=0;c<4;++c) for(int r=0;r<5;++r) {
                const float c0=a+0.0625f*static_cast<float>(c),c1=c0+0.0625f;
                const float r0=P::Bulwark*static_cast<float>(r)/5.0f,r1=P::Bulwark*static_cast<float>(r+1)/5.0f;
                face(W(c0,D(c0)+r0,s*P::HalfBreadth(c0)),W(c1,D(c1)+r0,s*P::HalfBreadth(c1)),
                     W(c1,D(c1)+r1,s*P::HalfBreadth(c1)),W(c0,D(c0)+r1,s*P::HalfBreadth(c0)),outside,Topside);
            }
            face(W(a,D(a),s*i0),W(a1,D(a1),s*i1),W(a1,D(a1)+P::Bulwark,s*i1),W(a,D(a)+P::Bulwark,s*i0),inboard,BulwarkInside);
            face(W(a,D(a)+P::Bulwark,s*i0),W(a1,D(a1)+P::Bulwark,s*i1),W(a1,D(a1)+P::Bulwark+0.02f,s*(h1+0.04f)),
                 W(a,D(a)+P::Bulwark+0.02f,s*(h0+0.04f)),W(middle,D(middle)+5.0f,s*h0),Boot*0.9f);
        }
    }
    {
        // across the stern
        const float h=P::HalfBreadth(0.0f);
        face(W(0.0f,D(0.0f),-h),W(0.0f,D(0.0f),h),W(0.0f,D(0.0f)+P::Bulwark,h),W(0.0f,D(0.0f)+P::Bulwark,-h),W(-3.0f,D(0.0f),0.0f),Topside);
        face(W(0.1f,D(0.1f),-h),W(0.1f,D(0.1f),h),W(0.1f,D(0.1f)+P::Bulwark,h),W(0.1f,D(0.1f)+P::Bulwark,-h),W(3.0f,D(0.0f),0.0f),BulwarkInside);
        box(0.0f,0.1f,D(0.0f)+P::Bulwark,D(0.0f)+P::Bulwark+0.03f,-h,h,Boot*0.9f,Boot*0.9f);
        // the gangway opening's ends
        for(const float a:{P::GangwayAft,P::GangwayFore})
            box(a-0.04f,a+0.04f,D(a),D(a)+P::Bulwark,P::HalfBeam-0.1f,P::HalfBeam,Topside,Boot*0.9f);
    }
    // ---- the deck: steel plate inside the bulwarks, open over the hatch and the stairwell, darker
    // under the deckhouse's roof; its underside over the hold
    {
        std::set<float> alongs,acrosses;
        for(float a=0.0f;a<=P::Length+1e-3f;a+=0.5f) alongs.insert(a);
        for(const float a:{P::HoldAft,P::HoldFore,P::HatchAft,P::HatchFore,P::StairFoot,P::StairTop,P::HouseAft,P::HouseFore}) alongs.insert(a);
        for(float c=-3.5f;c<=3.5f+1e-3f;c+=0.5f) acrosses.insert(c);
        for(const float c:{-P::HatchHalf,P::HatchHalf,P::StairInner,P::StairOuter,-P::HouseHalf,P::HouseHalf}) acrosses.insert(c);
        const std::vector<float> A(alongs.begin(),alongs.end()),C(acrosses.begin(),acrosses.end());
        for(std::size_t i=0;i+1<A.size();++i) for(std::size_t j=0;j+1<C.size();++j) {
            const float a0=A[i],a1=A[i+1],am=(a0+a1)*0.5f,cm=(C[j]+C[j+1])*0.5f;
            const float limit0=std::max(P::HalfBreadth(a0)-0.1f,0.0f),limit1=std::max(P::HalfBreadth(a1)-0.1f,0.0f);
            if(std::abs(cm)>=std::max(limit0,limit1)) continue;
            if(am>P::HatchAft&&am<P::HatchFore&&std::abs(cm)<P::HatchHalf) continue;
            if(am>P::StairFoot&&am<P::StairTop&&cm>P::StairInner) continue;
            const auto clampC=[&](const float c,const float limit) {return std::clamp(c,-limit,limit);};
            const float c00=clampC(C[j],limit0),c01=clampC(C[j+1],limit0),c10=clampC(C[j],limit1),c11=clampC(C[j+1],limit1);
            const bool roofed=am>P::HouseAft&&am<P::HouseFore&&std::abs(cm)<P::HouseHalf;
            face(W(a0,D(a0),c00),W(a1,D(a1),c10),W(a1,D(a1),c11),W(a0,D(a0),c01),W(am,D(am)+3.0f,cm),Deck*(roofed?0.8f:1.0f));
            if(am>P::HoldAft&&am<P::HoldFore) {
                const float under=-P::DeckThickness;
                face(W(a0,D(a0)+under,c00),W(a1,D(a1)+under,c10),W(a1,D(a1)+under,c11),W(a0,D(a0)+under,c01),
                     W(am,P::HoldFloor,cm),Hold*holdShade(am,cm)*0.8f);
            }
        }
        // the stairwell's edge in the deck plate
        face(W(P::StairFoot,D(P::StairFoot),P::StairInner),W(P::StairFoot,D(P::StairFoot),P::HalfBeam-0.1f),
             W(P::StairFoot,D(P::StairFoot)-P::DeckThickness,P::HalfBeam-0.1f),W(P::StairFoot,D(P::StairFoot)-P::DeckThickness,P::StairInner),
             W(P::StairFoot+1.0f,D(P::StairFoot),2.8f),Hold*0.7f);
    }
    // ---- the hatch: its coaming, green; the after covers still on, one of them knocked askew
    {
        const float top=P::Coaming;
        for(const float s:{-1.0f,1.0f}) {
            box(P::HatchAft,P::HatchFore,D(P::HatchAft),D(P::HatchAft)+top,s>0.0f?P::HatchHalf-0.08f:-P::HatchHalf,
                s>0.0f?P::HatchHalf:-P::HatchHalf+0.08f,Coaming,Coaming*1.05f);
            box(P::HatchAft,P::HatchFore,D(P::HatchAft)-0.6f,D(P::HatchAft),s>0.0f?P::HatchHalf-0.08f:-P::HatchHalf,
                s>0.0f?P::HatchHalf:-P::HatchHalf+0.08f,Hold*0.55f,Hold*0.55f);
        }
        for(const float a:{P::HatchAft,P::HatchFore}) {
            const float a0=a<15.0f?a:a-0.08f,a1=a<15.0f?a+0.08f:a;
            box(a0,a1,D(a),D(a)+top,-P::HatchHalf,P::HatchHalf,Coaming,Coaming*1.05f);
            box(a0,a1,D(a)-0.6f,D(a),-P::HatchHalf,P::HatchHalf,Hold*0.55f,Hold*0.55f);
        }
        // the covers: steel pontoons across the coaming, ribbed underneath
        for(int k=0;k<3;++k) {
            const float a0=P::HatchAft+0.05f+1.3f*static_cast<float>(k),a1=a0+1.25f;
            const float lift=k==2?0.18f:0.0f;   // the third lifted on one side
            hexa({W(a0,D(a0)+top,-P::HatchHalf-0.05f),W(a1,D(a1)+top,-P::HatchHalf-0.05f),W(a1,D(a1)+top+lift,P::HatchHalf+0.05f),
                  W(a0,D(a0)+top+lift,P::HatchHalf+0.05f),W(a0,D(a0)+top+0.22f,-P::HatchHalf-0.05f),W(a1,D(a1)+top+0.22f,-P::HatchHalf-0.05f),
                  W(a1,D(a1)+top+0.22f+lift,P::HatchHalf+0.05f),W(a0,D(a0)+top+0.22f+lift,P::HatchHalf+0.05f)},Coaming*0.85f,Coaming);
            // its underside, seen from the hold: rust-brown webs across it
            for(int web=0;web<3;++web) {
                const float wa=a0+0.2f+0.42f*static_cast<float>(web);
                box(wa,wa+0.08f,D(wa)+top-0.2f,D(wa)+top+0.01f,-P::HatchHalf+0.1f,P::HatchHalf-0.1f,Hold*0.45f,Hold*0.45f);
            }
            b.inside=W((a0+a1)*0.5f,D(a0)+top+0.5f,0.0f);
            b.Quad(W(a0,D(a0)+top-0.005f,-P::HatchHalf+0.08f),W(a1,D(a1)+top-0.005f,-P::HatchHalf+0.08f),
                   W(a1,D(a1)+top-0.005f+lift,P::HatchHalf-0.08f),W(a0,D(a0)+top-0.005f+lift,P::HatchHalf-0.08f),Hold*0.4f);
        }
    }
    // ---- the hold: its floor, the inner sides with their frames, the end bulkheads with
    // stiffeners, all dim away from the open hatch
    {
        const float inner=P::HalfBeam-0.15f;
        for(float a=P::HoldAft;a<P::HoldFore-1e-3f;a+=0.53f) {
            const float a1=std::min(a+0.53f,P::HoldFore),am=(a+a1)*0.5f;
            for(float c=-inner;c<inner-1e-3f;c+=0.67f) {
                const float c1=std::min(c+0.67f,inner),cm=(c+c1)*0.5f;
                face(W(a,P::HoldFloor,c),W(a1,P::HoldFloor,c),W(a1,P::HoldFloor,c1),W(a,P::HoldFloor,c1),W(am,3.0f,cm),Hold*holdShade(am,cm));
            }
            for(const float s:{-1.0f,1.0f}) {
                const float under=D(am)-P::DeckThickness;
                face(W(a,P::HoldFloor,s*inner),W(a1,P::HoldFloor,s*inner),W(a1,under,s*inner),W(a,under,s*inner),
                     W(am,2.8f,0.0f),Hold*holdShade(am,s*inner)*0.9f);
                // a frame every so often
                if(static_cast<int>((a-P::HoldAft)/0.53f)%2==0)
                    box(a,a+0.1f,P::HoldFloor,under,s*(inner-0.16f),s*inner,Hold*holdShade(am,s*inner)*0.75f,Hold*0.5f);
            }
        }
        for(const float a:{P::HoldAft,P::HoldFore}) {
            const float facing=a<15.0f?1.0f:-1.0f;
            const float under=D(a)-P::DeckThickness;
            for(float c=-inner;c<inner-1e-3f;c+=0.67f) {
                const float c1=std::min(c+0.67f,inner);
                face(W(a,P::HoldFloor,c),W(a,P::HoldFloor,c1),W(a,under,c1),W(a,under,c),W(a+facing,2.8f,(c+c1)*0.5f),
                     Hold*holdShade(a+facing*0.5f,(c+c1)*0.5f)*0.85f);
            }
            for(float c=-inner+0.5f;c<inner;c+=0.75f)
                box(facing>0.0f?a:a-0.14f,facing>0.0f?a+0.14f:a,P::HoldFloor,under,c-0.05f,c+0.05f,
                    Hold*holdShade(a,c)*0.7f,Hold*0.5f);
        }
    }
    // ---- the stair down into the hold: treads between stringers, a handrail on its open side
    {
        const float rise=D(P::StairTop)-P::HoldFloor,run=P::StairTop-P::StairFoot;
        const int steps=static_cast<int>(std::round(rise/0.2f));
        for(int k=0;k<steps;++k) {
            const float a1=P::StairFoot+run*static_cast<float>(k+1)/static_cast<float>(steps);
            const float a0=a1-run/static_cast<float>(steps)-0.04f;
            const float up=P::HoldFloor+rise*static_cast<float>(k+1)/static_cast<float>(steps);
            box(a0,a1,up-0.05f,up,P::StairInner+0.06f,P::StairOuter,Tread*holdShade(a1,2.8f),Tread);
        }
        for(const float c:{P::StairInner,P::StairOuter+0.03f}) {
            const float c0=c,c1=c+0.04f;
            hexa({W(P::StairFoot,P::HoldFloor,c0),W(P::StairTop,D(P::StairTop)-0.3f,c0),W(P::StairTop,D(P::StairTop)-0.3f,c1),
                  W(P::StairFoot,P::HoldFloor,c1),W(P::StairFoot,P::HoldFloor+0.3f,c0),W(P::StairTop,D(P::StairTop),c0),
                  W(P::StairTop,D(P::StairTop),c1),W(P::StairFoot,P::HoldFloor+0.3f,c1)},Steel*0.8f,Steel);
        }
        // the handrail along the open side, and the deck's rail round the stairwell
        std::vector<Vector3> rail;std::vector<float> radii;
        for(int k=0;k<=4;++k) {
            const float a=P::StairFoot+run*static_cast<float>(k)/4.0f;
            const float up=P::HoldFloor+rise*static_cast<float>(k)/4.0f;
            b.Tube({W(a,up,P::StairInner+0.02f),W(a,up+0.95f,P::StairInner+0.02f)},{0.025f,0.025f},6,Steel);
            rail.push_back(W(a,up+0.95f,P::StairInner+0.02f));radii.push_back(0.03f);
        }
        b.Tube(rail,radii,6,Steel);
        for(const float lift:{0.5f,1.0f}) {
            b.Tube({W(P::StairFoot,D(P::StairFoot)+lift,P::StairInner+0.02f),W(P::StairTop,D(P::StairTop)+lift,P::StairInner+0.02f)},{0.03f,0.03f},6,Steel);
            b.Tube({W(P::StairFoot,D(P::StairFoot)+lift,P::StairInner+0.02f),W(P::StairFoot,D(P::StairFoot)+lift,P::HalfBeam-0.1f)},{0.03f,0.03f},6,Steel);
        }
        for(const float a:{P::StairFoot,P::StairFoot+2.1f,P::StairTop})
            b.Tube({W(a,D(a),P::StairInner+0.02f),W(a,D(a)+1.0f,P::StairInner+0.02f)},{0.03f,0.03f},6,Steel);
    }
    // ---- the accommodation ladder down the starboard side to the sand
    {
        const float inner=P::HalfBeam+0.1f,outer=P::HalfBeam+0.1f+P::LadderWidth;
        const float head=D(P::GangwayFore),foot=ship.ladderFootUp,run=ship.ladderRun;
        box(P::GangwayAft,P::GangwayFore,head-0.08f,head,inner-0.1f,outer,Steel*0.85f,Tread);
        const int steps=std::max(3,static_cast<int>(std::round((head-foot)/0.2f)));
        for(int k=1;k<steps;++k) {
            const float t=static_cast<float>(k)/static_cast<float>(steps);
            const float a=P::GangwayFore+run*t,up=head+(foot-head)*t;
            box(a-0.13f,a+0.13f,up-0.04f,up,inner+0.04f,outer-0.04f,Tread,Tread*1.1f);
        }
        for(const float c:{inner,outer-0.04f})
            hexa({W(P::GangwayFore,head-0.3f,c),W(P::GangwayFore+run,foot-0.1f,c),W(P::GangwayFore+run,foot-0.1f,c+0.04f),
                  W(P::GangwayFore,head-0.3f,c+0.04f),W(P::GangwayFore,head+0.05f,c),W(P::GangwayFore+run,foot+0.2f,c),
                  W(P::GangwayFore+run,foot+0.2f,c+0.04f),W(P::GangwayFore,head+0.05f,c+0.04f)},Steel*0.8f,Steel);
        // the outer railing: stanchions, a handrail and a mid rail, round the landing too
        std::vector<Vector3> hand,mid;std::vector<float> radii;
        hand.push_back(W(P::GangwayAft,head+1.0f,inner-0.05f));mid.push_back(W(P::GangwayAft,head+0.5f,inner-0.05f));radii.push_back(0.025f);
        hand.push_back(W(P::GangwayAft,head+1.0f,outer));mid.push_back(W(P::GangwayAft,head+0.5f,outer));radii.push_back(0.025f);
        for(int k=0;k<=5;++k) {
            const float t=static_cast<float>(k)/5.0f;
            const float a=P::GangwayFore+run*t,up=head+(foot-head)*t;
            b.Tube({W(a,up,outer-0.02f),W(a,up+1.0f,outer-0.02f)},{0.025f,0.025f},6,Steel);
            hand.push_back(W(a,up+1.0f,outer-0.02f));mid.push_back(W(a,up+0.5f,outer-0.02f));radii.push_back(0.025f);
        }
        b.Tube({W(P::GangwayAft,head,outer-0.02f),W(P::GangwayAft,head+1.0f,outer-0.02f)},{0.025f,0.025f},6,Steel);
        b.Tube({W(P::GangwayAft,head,inner-0.05f),W(P::GangwayAft,head+1.0f,inner-0.05f)},{0.025f,0.025f},6,Steel);
        b.Tube(hand,radii,6,Steel);b.Tube(mid,radii,6,Steel);
    }
    // ---- the deckhouse: white plating, its door hanging open, portholes and windows; inside a
    // mess table and benches; the wheelhouse and the funnel on its roof, the empty davits
    {
        const float a0=P::HouseAft,a1=P::HouseFore,h=P::HouseHalf,t=0.08f;
        const auto u=[&](const float a,const float lift) {return D(a)+lift;};
        const float roof=P::HouseHeight;
        const Vector3 within=W(5.0f,u(5.0f,1.2f),0.0f);
        // a wall slab: its outer face white, its inner face cream and dim
        const auto slab=[&](const std::array<Vector3,8>& p) {
            Vector3 centre(0,0,0);
            for(const auto& v:p) centre+=v;
            centre/=8.0f;
            const auto quad=[&](const Vector3& q0,const Vector3& q1,const Vector3& q2,const Vector3& q3) {
                const Vector3 mid=(q0+q1+q2+q3)/4.0f;
                const bool inward=Vector3::Dot(mid-centre,within-centre)>0.0f
                    &&Vector3::DistanceSquared(mid,within)<Vector3::DistanceSquared(centre,within);
                b.inside=centre;b.Quad(q0,q1,q2,q3,inward?HouseInside*0.9f:House);
            };
            quad(p[4],p[5],p[6],p[7]);quad(p[0],p[1],p[2],p[3]);
            for(int k=0;k<4;++k) quad(p[k],p[(k+1)%4],p[4+(k+1)%4],p[4+k]);
        };
        const auto wall=[&](const float wa0,const float wa1,const float wc0,const float wc1,const float lo,const float hi) {
            slab({W(wa0,u(wa0,lo),wc0),W(wa1,u(wa1,lo),wc0),W(wa1,u(wa1,lo),wc1),W(wa0,u(wa0,lo),wc1),
                  W(wa0,u(wa0,hi),wc0),W(wa1,u(wa1,hi),wc0),W(wa1,u(wa1,hi),wc1),W(wa0,u(wa0,hi),wc1)});
        };
        wall(a0,a1,-h,-h+t,0.0f,roof);wall(a0,a1,h-t,h,0.0f,roof);
        wall(a0,a0+t,-h,h,0.0f,roof);
        wall(a1-t,a1,-h,P::DoorInner,0.0f,roof);wall(a1-t,a1,P::DoorOuter,h,0.0f,roof);
        wall(a1-t,a1,P::DoorInner,P::DoorOuter,P::DoorHeight,roof);
        wall(a0-0.1f,a1+0.1f,-h-0.1f,h+0.1f,roof,roof+0.12f);
        // the door, swung open against the forward face
        {
            const float hinge=P::DoorOuter+0.02f,swing=1.75f,width=P::DoorOuter-P::DoorInner;
            const float da=std::sin(swing)*width,dc=-std::cos(swing)*width;
            hexa({W(a1+0.02f,u(a1,0.03f),hinge),W(a1+0.02f+da,u(a1,0.03f),hinge+dc),W(a1+0.06f+da,u(a1,0.03f),hinge+dc),
                  W(a1+0.06f,u(a1,0.03f),hinge),W(a1+0.02f,u(a1,1.95f),hinge),W(a1+0.02f+da,u(a1,1.95f),hinge+dc),
                  W(a1+0.06f+da,u(a1,1.95f),hinge+dc),W(a1+0.06f,u(a1,1.95f),hinge)},House*0.9f,House*0.9f);
        }
        // portholes down the sides and the after face, windows either side of the door
        const auto porthole=[&](const Vector3& centre,const Vector3& out,const Vector3& side) {
            constexpr int Rim=10;
            b.inside=centre-out;
            for(int k=0;k<Rim;++k) {
                const float q0=2.0f*Pi*static_cast<float>(k)/Rim,q1=2.0f*Pi*static_cast<float>(k+1)/Rim;
                const Vector3 r0=side*std::cos(q0)+Vector3::Up*std::sin(q0),r1=side*std::cos(q1)+Vector3::Up*std::sin(q1);
                b.Face(centre+out*0.012f,centre+out*0.012f+r0*0.17f,centre+out*0.012f+r1*0.17f,Glass);
                b.Quad(centre+r0*0.17f+out*0.012f,centre+r0*0.22f+out*0.03f,centre+r1*0.22f+out*0.03f,centre+r1*0.17f+out*0.012f,Brass);
            }
        };
        for(const float a:{3.2f,5.0f,6.8f})
            for(const float s:{-1.0f,1.0f}) porthole(W(a,u(a,1.5f),s*h),side*s,fwd);
        for(const float c:{-1.2f,1.2f}) porthole(W(a0,u(a0,1.5f),c),-fwd,side);
        for(const float c:{-1.9f,-0.6f}) {
            b.inside=W(a1-1.0f,u(a1,1.4f),c);
            b.Quad(W(a1+0.01f,u(a1,1.1f),c-0.35f),W(a1+0.01f,u(a1,1.1f),c+0.35f),W(a1+0.01f,u(a1,1.7f),c+0.35f),W(a1+0.01f,u(a1,1.7f),c-0.35f),Glass);
        }
        // the mess: a table, benches either side, a locker on the after bulkhead
        box(3.4f,5.8f,D(4.6f)+0.72f,D(4.6f)+0.78f,-1.7f,-0.5f,Wood*0.7f,Wood*0.8f);
        for(const float a:{3.5f,5.7f}) for(const float c:{-1.6f,-0.6f}) box(a-0.04f,a+0.04f,D(a),D(a)+0.72f,c-0.04f,c+0.04f,Wood*0.55f,Wood*0.55f);
        box(3.4f,5.8f,D(4.6f),D(4.6f)+0.45f,-2.55f,-2.15f,Wood*0.5f,Wood*0.6f);
        box(2.1f,2.6f,D(2.3f),D(2.3f)+1.8f,0.8f,2.2f,HouseInside*0.45f,HouseInside*0.5f);
        // the wheelhouse: its windows all round under an overhanging roof
        const float w0=3.5f,w1=7.2f,wh=2.3f,base=roof+0.12f,band0=base+0.9f,band1=base+1.75f,cap=base+2.3f;
        wall(w0,w1,-wh,-wh+t,base,band0);wall(w0,w1,wh-t,wh,base,band0);
        wall(w0,w0+t,-wh,wh,base,band0);wall(w1-t,w1,-wh,wh,base,band0);
        wall(w0,w1,-wh,-wh+t,band1,cap);wall(w0,w1,wh-t,wh,band1,cap);
        wall(w0,w0+t,-wh,wh,band1,cap);wall(w1-t,w1,-wh,wh,band1,cap);
        for(int k=0;k<=5;++k) {
            const float c=-wh+2.0f*wh*static_cast<float>(k)/5.0f;
            wall(w1-t,w1,c-0.05f,c+0.05f,band0,band1);
        }
        for(const float s:{-1.0f,1.0f}) for(const float a:{w0,(w0+w1)*0.5f,w1-0.1f}) wall(a,a+0.1f,s*wh-(s>0?t:0.0f),s*wh+(s<0?t:0.0f),band0,band1);
        wall(w0,w0+t,-0.05f,0.05f,band0,band1);
        b.inside=W((w0+w1)*0.5f,u(5.0f,base+1.3f),0.0f);
        b.Quad(W(w1-0.04f,u(w1,band0),-wh),W(w1-0.04f,u(w1,band0),wh),W(w1-0.04f,u(w1,band1),wh),W(w1-0.04f,u(w1,band1),-wh),Glass);
        for(const float s:{-1.0f,1.0f})
            b.Quad(W(w0,u(w0,band0),s*(wh-0.04f)),W(w1,u(w1,band0),s*(wh-0.04f)),W(w1,u(w1,band1),s*(wh-0.04f)),W(w0,u(w0,band1),s*(wh-0.04f)),Glass);
        b.Quad(W(w0+0.04f,u(w0,band0),-wh),W(w0+0.04f,u(w0,band0),wh),W(w0+0.04f,u(w0,band1),wh),W(w0+0.04f,u(w0,band1),-wh),Glass);
        wall(w0-0.25f,w1+0.35f,-wh-0.25f,wh+0.25f,cap,cap+0.1f);
        // the funnel abaft the wheelhouse, raked, its black top
        {
            std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
            for(const float lift:{roof+0.1f,roof+2.0f,roof+2.6f,roof+3.6f}) {
                const float a=2.75f-(lift-roof)*0.08f;
                std::vector<Vector3> ring;
                for(int k=0;k<14;++k) {
                    const float q=2.0f*Pi*static_cast<float>(k)/14.0f;
                    ring.push_back(W(a+std::cos(q)*0.62f,u(2.75f,lift),std::sin(q)*0.48f));
                }
                rings.push_back(ring);centres.push_back(W(a,u(2.75f,lift),0.0f));
            }
            b.Loft(rings,centres,[&](const std::size_t ring,std::size_t) {return ring>=2U?FunnelTop:Funnel;});
        }
        // the lifeboat's davits, swung out over the side, the boat gone
        for(const float s:{-1.0f,1.0f}) for(const float a:{3.9f,6.4f})
            b.Tube({W(a,u(a,roof+0.12f),s*(h-0.15f)),W(a,u(a,roof+1.4f),s*(h+0.1f)),W(a,u(a,roof+1.9f),s*(h+0.7f)),W(a,u(a,roof+1.7f),s*(h+1.1f))},
                   {0.07f,0.06f,0.05f,0.04f},8,House*0.85f);
    }
    // ---- the masts with their derricks and stays
    {
        const auto mast=[&](const float a,const float height,const float boomTo,const float boomUp) {
            b.Tube({W(a,D(a)-0.1f,0.0f),W(a,D(a)+height*0.6f,0.0f),W(a,D(a)+height-0.5f,0.0f),W(a,D(a)+height,0.0f)},
                   {0.2f,0.16f,0.115f,0.11f},10,Mast);
            box(a-0.08f,a+0.08f,D(a)+height*0.7f,D(a)+height*0.7f+0.12f,-0.95f,0.95f,Mast,Mast);
            b.Tube({W(a+(boomTo>a?0.25f:-0.25f),D(a)+1.2f,0.0f),W(boomTo,D(boomTo)+boomUp,0.0f)},{0.13f,0.08f},8,Mast*0.95f);
            const Vector3 top=W(a,D(a)+height*0.95f,0.0f);
            for(const float s:{-1.0f,1.0f}) for(const float offset:{-0.9f,0.9f})
                b.Tube({top,W(a+offset,D(a+offset)+P::Bulwark,s*(P::HalfBreadth(a+offset)-0.12f))},{0.015f,0.015f},4,Iron);
            // the derrick's topping lift
            b.Tube({W(a,D(a)+height*0.7f,0.0f),W(boomTo,D(boomTo)+boomUp,0.0f)},{0.012f,0.012f},4,Iron);
        };
        mast(P::MainMast,9.0f,15.2f,P::Coaming+0.4f);
        mast(P::ForeMast,8.0f,18.6f,P::Coaming+1.9f);
        b.Tube({W(P::ForeMast,D(P::ForeMast)+7.6f,0.0f),W(29.6f,D(29.6f)+P::Bulwark,0.0f)},{0.015f,0.015f},4,Iron);
        b.Tube({W(P::MainMast,D(P::MainMast)+8.6f,0.0f),W(P::ForeMast,D(P::ForeMast)+7.6f,0.0f)},{0.015f,0.015f},4,Iron);
    }
    // ---- the windlass and its chains, the anchors: one stowed in its hawse, the other let go
    {
        const float a=P::Windlass;
        box(a-0.45f,a+0.45f,D(a),D(a)+0.35f,-0.85f,0.85f,Iron,Iron*1.1f);
        box(a-0.3f,a+0.3f,D(a)+0.35f,D(a)+0.85f,-0.3f,0.3f,Iron,Iron*1.1f);
        for(const float s:{-1.0f,1.0f})
            b.Tube({W(a,D(a)+0.62f,s*0.3f),W(a,D(a)+0.62f,s*0.85f)},{0.26f,0.26f},12,Iron*1.15f);
        b.Tube({W(a,D(a)+0.62f,-0.95f),W(a,D(a)+0.62f,0.95f)},{0.06f,0.06f},8,Iron);
        // the chains from the gypsies forward to the hawse pipes, link by link
        for(const float s:{-1.0f,1.0f}) {
            const Vector3 from=W(a+0.2f,D(a)+0.4f,s*0.58f),to=W(28.9f,D(28.9f)+0.05f,s*0.75f);
            const int links=12;
            for(int k=0;k<links;++k) {
                const Vector3 p=Vector3::Lerp(from,to,(static_cast<float>(k)+0.5f)/static_cast<float>(links));
                Vector3 along=to-from;along.Normalize();
                const Vector3 across=k%2==0?Vector3::Up:side*s;
                const Vector3 other=Vector3::Cross(along,across);
                hexa({p-along*0.07f-across*0.04f-other*0.012f,p+along*0.07f-across*0.04f-other*0.012f,p+along*0.07f+across*0.04f-other*0.012f,p-along*0.07f+across*0.04f-other*0.012f,
                      p-along*0.07f-across*0.04f+other*0.012f,p+along*0.07f-across*0.04f+other*0.012f,p+along*0.07f+across*0.04f+other*0.012f,p-along*0.07f+across*0.04f+other*0.012f},Chain,Chain);
            }
        }
        // the starboard anchor stowed against the bow
        {
            const float aa=28.6f,cc=P::HalfBreadth(aa)+0.12f;
            const Vector3 head=W(aa,D(aa)-0.4f,cc),crown=W(aa+0.2f,D(aa)-1.9f,cc+0.05f);
            b.Tube({head,crown},{0.09f,0.09f},6,Iron);
            for(const float s:{-1.0f,1.0f})
                b.Tube({crown,crown+fwd*(0.55f*s)+Vector3::Up*0.45f},{0.12f,0.04f},6,Iron);
        }
        // the port chain run out through its hawse down to the sand where the anchor went in
        {
            const Vector3 hawse=W(28.9f,D(28.9f)-0.6f,-P::HalfBreadth(28.9f)-0.08f);
            const FreighterPoint far=ship.World(34.5f,0.0f,-3.2f);
            const Vector3 end(far.x,groundAt(far.x,far.z)+0.05f,far.z);
            const Vector3 sag=(hawse+end)*0.5f-Vector3::Up*0.6f;
            std::vector<Vector3> points;
            for(int k=0;k<=24;++k) {
                const float t=static_cast<float>(k)/24.0f;
                points.push_back(hawse*(1.0f-t)*(1.0f-t)+sag*(2.0f*t*(1.0f-t))+end*(t*t));
            }
            for(std::size_t k=0;k+1<points.size();++k) {
                const Vector3 p=(points[k]+points[k+1])*0.5f;
                Vector3 along=points[k+1]-points[k];
                const float length=along.Length();along.Normalize();
                Vector3 across=k%2==0?side:Vector3::Cross(along,side);across.Normalize();
                const Vector3 other=Vector3::Cross(along,across);
                const float l=length*0.55f;
                hexa({p-along*l-across*0.05f-other*0.014f,p+along*l-across*0.05f-other*0.014f,p+along*l+across*0.05f-other*0.014f,p-along*l+across*0.05f-other*0.014f,
                      p-along*l-across*0.05f+other*0.014f,p+along*l-across*0.05f+other*0.014f,p+along*l+across*0.05f+other*0.014f,p-along*l+across*0.05f+other*0.014f},Chain*0.9f,Chain*0.9f);
            }
        }
    }
    // ---- bollards along the bulwarks, ventilators either side of the hatch
    for(const float a:{4.5f,11.0f,21.6f,25.6f}) for(const float s:{-1.0f,1.0f}) {
        if(s>0.0f&&a>P::StairFoot-0.5f&&a<P::StairTop+0.5f) continue;
        const float c=s*(P::HalfBreadth(a)-0.45f);
        for(const float offset:{-0.22f,0.22f})
            b.Tube({W(a+offset,D(a),c),W(a+offset,D(a)+0.42f,c)},{0.12f,0.12f},10,Iron);
        box(a-0.32f,a+0.32f,D(a),D(a)+0.06f,c-0.16f,c+0.16f,Iron,Iron);
    }
    for(const auto& [a,c]:std::array<std::array<float,2>,3>{{{9.0f,-1.6f},{22.4f,-1.1f},{22.4f,1.1f}}}) {
        const float d=D(a);
        b.Tube({W(a,d,c),W(a,d+1.1f,c),W(a+0.2f,d+1.45f,c),W(a+0.5f,d+1.55f,c)},{0.17f,0.17f,0.2f,0.3f},10,House*0.9f);
    }

    // ---- the crew, dead where they lay: one on the foredeck by the windlass, one sitting against
    // the hold's side, one fallen at the stair's foot, one slumped over the mess table, one curled
    // up on the after deck. Each pose is set out in the ship's frame and sheared onto its floors.
    int skeletonTriangles=0;
    {
        b.tint=nullptr;
        const auto toWorld=[&](SkeletonPose pose) {
            const auto point=[&](Vector3& v) {v=W(v.X,v.Y,v.Z);};
            const Vector3 origin=W(0.0f,0.0f,0.0f);
            const auto direction=[&](Vector3& v) {v=W(v.X,v.Y,v.Z)-origin;};
            point(pose.pelvis);point(pose.chest);point(pose.neck);point(pose.head);
            direction(pose.front);direction(pose.face);direction(pose.headUp);
            for(int i=0;i<2;++i) {
                point(pose.elbow[i]);point(pose.wrist[i]);point(pose.knee[i]);point(pose.ankle[i]);
                direction(pose.fingers[i]);direction(pose.toes[i]);
            }
            return pose;
        };
        const auto lay=[&](SkeletonPose pose,const bool boots,const bool cap,const float shade,const std::uint32_t seed) {
            pose.boots=boots;pose.cap=cap;
            skeletonTriangles+=BuildSkeleton(b,toWorld(pose),shade,seed);
        };
        lay(LyingSkeleton(Vector3(24.3f,D(24.3f),-1.75f),Vector3(1.0f,0.0f,0.08f),false,0.9f),true,true,1.0f,11U);
        lay(SittingSkeleton(Vector3(14.6f,P::HoldFloor,-P::HalfBeam+0.17f),Vector3(0.0f,0.0f,1.0f)),false,false,holdShade(14.6f,-2.9f),23U);
        lay(LyingSkeleton(Vector3(15.25f,P::HoldFloor,2.45f),Vector3(-1.0f,0.0f,-0.15f),true,1.2f),true,false,holdShade(15.6f,2.45f),37U);
        lay(SlumpedSkeleton(Vector3(4.6f,D(4.6f)+0.45f,-2.3f),Vector3(0.0f,0.0f,1.0f),D(4.6f),D(4.6f)+0.78f),false,true,0.95f,41U);
        lay(CurledSkeleton(Vector3(1.15f,D(1.15f),1.1f),Vector3(0.0f,0.0f,-1.0f)),true,false,0.95f,53U);
    }

    auto& mesh=b.builder.mesh;
    triangles_=static_cast<int>(mesh.size()/3);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.2f,0.2f,0.2f),high+Vector3(0.2f,0.2f,0.2f));
    centre_=(low+high)*0.5f;
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"freighter",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: freighter sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    const auto stern=ship.World(0.0f,0.0f,0.0f);
    message<<"island-simulator: a rusting 30 m coaster lies driven up the beach at ("
        <<static_cast<int>(std::lround(centre_.X))<<", "<<static_cast<int>(std::lround(centre_.Z))
        <<"), its stern in the surf at ("<<static_cast<int>(std::lround(stern.x))<<", "<<static_cast<int>(std::lround(stern.z))
        <<"); a ladder to its deck, a stair to its hold; five of its crew lie dead aboard ("<<skeletonTriangles
        <<" triangles of bone); "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void FreighterRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>600*600
        ||BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);drawCalls_=1;
}

void FreighterRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"freighter");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: freighter sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    map.CountCasters(1);
}
}
