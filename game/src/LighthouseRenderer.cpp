#include "IslandSimulator/LighthouseRenderer.hpp"
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
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// pigments past full for the build's light (0.46 + 0.54 n.s)
const Vector3 Whitewash(300,296,286),BandRed(236,62,48),Stone(210,202,186),Iron(66,66,70),Glass(64,84,92),
    Frame(280,278,270),Dome(220,58,44),Hollow(30,28,26),Rust(170,110,72),Plaster(200,194,180),Tread(176,168,152),
    Daylight(320,330,336),Brass(196,158,84),Lens(150,176,170),RailGreen(52,92,64);
}

LighthouseRenderer::LighthouseRenderer(GraphicsDevice& device,const LighthousePlacement& placement,
                                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b,clear;   // the stone and the iron; the lantern's glass, drawn see-through
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(120,112,96);b.stainHeight=0.4f;b.stainAmount=0.5f;
    const Vector3 sea(placement.seaX,0.0f,placement.seaZ),side(placement.seaZ,0.0f,-placement.seaX);
    const float baseY=placement.TowerBaseY(),galleryY=placement.GalleryY();
    const auto around=[&](const float angle,const float radius,const float y) {
        return Vector3(placement.x+(sea.X*std::cos(angle)+side.X*std::sin(angle))*radius,y,
                       placement.z+(sea.Z*std::cos(angle)+side.Z*std::sin(angle))*radius);
    };
    const Vector3 axis(placement.x,0.0f,placement.z);
    // the plinth: an octagon of dressed stone, sunk below its lowest ground
    {
        float lowest=placement.groundY;
        for(int k=0;k<8;++k) {
            const Vector3 p=around(2.0f*Pi*static_cast<float>(k)/8.0f,LighthousePlacement::PlinthRadius,0.0f);
            lowest=std::min(lowest,groundAt(p.X,p.Z));
        }
        std::vector<std::vector<Vector3>> rings(2,std::vector<Vector3>(8));
        for(int k=0;k<8;++k) {
            const float a=2.0f*Pi*(static_cast<float>(k)+0.5f)/8.0f;
            rings[0][static_cast<std::size_t>(k)]=around(a,LighthousePlacement::PlinthRadius,lowest-0.3f);
            rings[1][static_cast<std::size_t>(k)]=around(a,LighthousePlacement::PlinthRadius,baseY);
        }
        b.Loft(rings,{axis+Vector3::Up*(lowest-0.3f),axis+Vector3::Up*baseY},
               [&](std::size_t,std::size_t k) {return Stone*(0.92f+0.08f*static_cast<float>(k%2));},false,true);
    }
    constexpr std::array<std::array<float,2>,5> Windows{{{2.2f,3.6f},{0.6f,6.6f},{3.6f,9.6f},{1.9f,12.4f},{4.8f,14.6f}}};
    // inside, the sun does not reach: the walls, the stair and the floors are lit by the daylight
    // in at the windows, the door and down from the lantern over a dim warm ambient, corner by
    // corner (the build's sun-turned light divided back out of each face)
    const auto interiorTint=[&](const Vector3& pigment,const Vector3& normal,const Vector3& p) {
        const float face=0.46f+0.54f*std::max(0.0f,Vector3::Dot(normal,b.builder.sun));
        float level=0.38f;
        for(const auto& window:Windows) {
            const float wy=baseY+window[1]+0.35f;
            const Vector3 centre=around(window[0],placement.InnerRadius(wy)-0.1f,wy);
            level+=0.8f*std::exp(-Vector3::DistanceSquared(p,centre)/1.6f);
        }
        level+=1.0f*std::exp(-Vector3::DistanceSquared(p,around(Pi,placement.InnerRadius(baseY+1.0f),baseY+1.0f))/2.6f);
        level+=0.6f*std::exp(-std::max(0.0f,galleryY-p.Y)/2.2f);
        level=std::min(level,1.3f);
        return Vector3(pigment.X*level,pigment.Y*level*0.97f,pigment.Z*level*0.92f)/face;
    };
    const auto interior=[&](const bool on) {
        b.tintPerVertex=on;
        if(on) b.tint=interiorTint; else b.tint=nullptr;
    };
    // the tower: whitewash with two red bands, rust bleeding down from under the gallery and the
    // windows, grime at its foot; a wall of stone a third of a metre thick, plastered inside, the
    // door through it at its foot
    constexpr int Segments=32;
    const float towerTop=placement.TowerTopY();
    const auto radiusAt=[&](const float y) {return placement.OuterRadius(y);};
    using L=LighthousePlacement;
    {
        std::vector<float> heights;
        const int stations=static_cast<int>((towerTop-baseY)/0.55f)+1;
        for(int i=0;i<=stations;++i) heights.push_back(baseY+(towerTop-baseY)*static_cast<float>(i)/static_cast<float>(stations));
        heights.push_back(baseY+L::DoorHeight);
        std::sort(heights.begin(),heights.end());
        // the segments' angles, the two either side of the door moved onto its jambs (the door's
        // half width at that face)
        const auto angles=[&](const float radius) {
            std::array<float,Segments+1> a{};
            for(int k=0;k<=Segments;++k) a[static_cast<std::size_t>(k)]=2.0f*Pi*static_cast<float>(k)/Segments;
            const float half=std::asin(L::DoorHalfWidth/radius);
            a[15]=Pi-half;a[17]=Pi+half;
            return a;
        };
        const auto outsideColour=[&](const float y,const int k) {
            const float t=(y-baseY)/(towerTop-baseY);
            const bool red=(t>0.30f&&t<0.42f)||(t>0.66f&&t<0.78f);
            Vector3 colour=red?BandRed:Whitewash;
            // streaks: a few columns of rust from the top fading down, salt grime at the foot
            const float column=std::sin(static_cast<float>(k)*2.7f+1.3f)*std::sin(static_cast<float>(k)*1.1f);
            const float rust=std::clamp(column-0.35f,0.0f,1.0f)*std::clamp((t-0.55f)/0.45f,0.0f,1.0f);
            colour=Vector3::Lerp(colour,Rust,rust*0.55f);
            return Vector3::Lerp(colour,Vector3(150,146,136),std::clamp(1.0f-t*8.0f,0.0f,1.0f)*0.45f);
        };
        const auto insideColour=[&](const float y,const int k) {
            // limewash gone grey with damp low down and soot from the keepers' lamps up the stair
            const float t=(y-baseY)/(towerTop-baseY);
            const float blotch=0.5f+0.5f*std::sin(static_cast<float>(k)*1.9f+y*0.8f)*std::sin(static_cast<float>(k)*0.7f-y*0.45f);
            return Plaster*(0.82f+0.12f*blotch)*(0.88f+0.12f*std::clamp(t*4.0f,0.0f,1.0f));
        };
        const float doorTop=baseY+L::DoorHeight;
        for(std::size_t i=0;i+1<heights.size();++i) {
            const float y0=heights[i],y1=heights[i+1];
            if(y1-y0<1e-3f) continue;
            const auto outer0=angles(radiusAt(y0));
            for(int k=0;k<Segments;++k) {
                const auto kk=static_cast<std::size_t>(k);
                if((k==15||k==16)&&y1<=doorTop+1e-3f) continue;
                const float ym=(y0+y1)*0.5f;
                b.inside=axis+Vector3::Up*ym;
                b.Quad(around(outer0[kk],radiusAt(y0),y0),around(outer0[kk+1],radiusAt(y0),y0),
                       around(outer0[kk+1],radiusAt(y1),y1),around(outer0[kk],radiusAt(y1),y1),outsideColour(ym,k));
            }
        }
        interior(true);
        for(std::size_t i=0;i+1<heights.size();++i) {
            const float y0=heights[i],y1=heights[i+1];
            if(y1-y0<1e-3f) continue;
            const auto inner0=angles(radiusAt(y0)-L::WallThickness);
            for(int k=0;k<Segments;++k) {
                const auto kk=static_cast<std::size_t>(k);
                if((k==15||k==16)&&y1<=doorTop+1e-3f) continue;
                const float ym=(y0+y1)*0.5f;
                b.inside=around((inner0[kk]+inner0[kk+1])*0.5f,radiusAt(ym)+0.5f,ym);
                b.Quad(around(inner0[kk],radiusAt(y0)-L::WallThickness,y0),around(inner0[kk+1],radiusAt(y0)-L::WallThickness,y0),
                       around(inner0[kk+1],radiusAt(y1)-L::WallThickness,y1),around(inner0[kk],radiusAt(y1)-L::WallThickness,y1),
                       insideColour(ym,k));
            }
        }
        interior(false);
        // the door's reveal: its jambs and the lintel's soffit through the wall's thickness
        {
            const auto jamb=[&](const float s,const float y) {
                const float outerR=radiusAt(y),innerR=outerR-L::WallThickness;
                return std::array<Vector3,2>{{around(Pi+s*std::asin(L::DoorHalfWidth/outerR),outerR,y),
                                              around(Pi+s*std::asin(L::DoorHalfWidth/innerR),innerR,y)}};
            };
            for(const float s:{-1.0f,1.0f}) {
                const auto low=jamb(s,baseY),high=jamb(s,doorTop);
                b.inside=around(Pi+s*0.6f,radiusAt(baseY)-L::WallThickness*0.5f,baseY+1.0f);
                b.Quad(low[0],low[1],high[1],high[0],Stone*0.9f);
            }
            const auto left=jamb(-1.0f,doorTop),right=jamb(1.0f,doorTop);
            b.inside=around(Pi,radiusAt(doorTop)-L::WallThickness*0.5f,doorTop+0.5f);
            b.Quad(left[0],right[0],right[1],left[1],Stone*0.8f);
        }
        // the wall's top under the gallery's slab is covered by it; the floor inside is the
        // plinth's top
    }
    // a hollow framed in white on the tower's face (the door, the windows)
    const auto opening=[&](const float angle,const float bottom,const float width,const float height,const bool hollow) {
        const float y=bottom+height*0.5f,r=radiusAt(y)+0.03f;
        const Vector3 centre=around(angle,r,y);
        Vector3 out=centre-(axis+Vector3::Up*y);out.Y=0.0f;out.Normalize();
        const Vector3 across(out.Z,0.0f,-out.X);
        if(hollow) b.Box(centre,across,Vector3::Up,out,width*0.5f,height*0.5f,0.02f,Hollow,Hollow);
        b.Box(centre+Vector3::Up*(height*0.5f+0.05f)+out*0.03f,across,Vector3::Up,out,width*0.5f+0.08f,0.05f,0.05f,Frame,Frame);
        b.Box(centre-Vector3::Up*(height*0.5f+0.04f)+out*0.05f,across,Vector3::Up,out,width*0.5f+0.06f,0.04f,0.08f,Frame,Frame);
        for(const float s:{-1.0f,1.0f})
            b.Box(centre+across*(s*(width*0.5f+0.04f))+out*0.03f,across,Vector3::Up,out,0.04f,height*0.5f,0.05f,Frame,Frame);
    };
    opening(Pi,baseY,1.0f,2.05f,false);   // the door, inland: open through the wall
    for(const auto& window:Windows) {
        opening(window[0],baseY+window[1],0.42f,0.70f,true);
        // and inside, the daylight in its deep embrasure
        const float y=baseY+window[1]+0.35f,r=radiusAt(y)-LighthousePlacement::WallThickness-0.01f;
        const Vector3 centre=around(window[0],r,y);
        Vector3 in=(axis+Vector3::Up*y)-centre;in.Y=0.0f;in.Normalize();
        b.Box(centre,Vector3(in.Z,0.0f,-in.X),Vector3::Up,in,0.21f,0.35f,0.01f,Daylight,Daylight);
    }
    // inside: the newel the stair winds round, rising into the lantern as the lamp's pedestal
    using L=LighthousePlacement;
    interior(true);
    b.Tube({axis+Vector3::Up*baseY,axis+Vector3::Up*(galleryY+0.6f)},{L::NewelRadius,L::NewelRadius*0.9f},10,Stone*0.9f);
    // the stair: a stone tread a sixteenth of a turn each, from the newel to the wall, climbing
    // clockwise from beside the door, its top where the walker's ramp runs at its middle
    {
        const float rise=placement.StairRisePerRadian(),step=2.0f*Pi/static_cast<float>(L::StepsPerTurn);
        const int treads=static_cast<int>(std::lround((galleryY-baseY)/(rise*step)));
        for(int i=0;i<treads;++i) {
            const float a0=L::StairStart-step*static_cast<float>(i),a1=a0-step*1.04f;
            const float top=baseY+rise*step*(static_cast<float>(i)+0.5f),bottom=top-0.08f;
            const float outer=placement.InnerRadius(top)-0.01f,inner=L::NewelRadius;
            const Vector3 tone=Tread*(0.88f+0.16f*std::sin(static_cast<float>(i)*2.3f));
            const std::array<Vector3,8> p{{around(a0,inner,bottom),around(a0,outer,bottom),around(a1,outer,bottom),around(a1,inner,bottom),
                                          around(a0,inner,top),around(a0,outer,top),around(a1,outer,top),around(a1,inner,top)}};
            b.inside=around((a0+a1)*0.5f,(inner+outer)*0.5f,(top+bottom)*0.5f);
            b.Quad(p[4],p[5],p[6],p[7],tone);b.Quad(p[0],p[3],p[2],p[1],tone*0.8f);
            b.Quad(p[0],p[1],p[5],p[4],tone*0.92f);b.Quad(p[2],p[3],p[7],p[6],tone*0.92f);
            b.Quad(p[1],p[2],p[6],p[5],tone*0.9f);
        }
    }
    // the handrail up the wall, iron painted the keepers' dark green, on brackets every few treads
    {
        const float rise=placement.StairRisePerRadian();
        std::vector<Vector3> rail;std::vector<float> radii;
        for(float phi=L::StairStart-0.15f;phi>placement.StairEnd()+0.3f;phi-=0.12f) {
            const float y=baseY+rise*(L::StairStart-phi)+0.92f;
            rail.push_back(around(phi,placement.InnerRadius(y)-0.07f,y));radii.push_back(0.022f);
        }
        b.Tube(rail,radii,6,RailGreen);
        for(std::size_t k=0;k<rail.size();k+=6) {
            const Vector3 at=rail[k];
            Vector3 out=at-(axis+Vector3::Up*at.Y);out.Y=0.0f;out.Normalize();
            b.Tube({at,at+out*0.07f},{0.012f,0.012f},4,RailGreen*0.8f);
        }
    }
    // the lantern's floor at the gallery's height, open over the stair's last turn
    {
        constexpr int Fan=20;
        const float from=placement.StairEnd()-1.35f,to=placement.StairEnd(),t=0.22f;
        for(int k=0;k<Fan;++k) {
            const float a0=from+(to-from)*static_cast<float>(k)/Fan,a1=from+(to-from)*static_cast<float>(k+1)/Fan;
            const float r0=L::NewelRadius,r1=L::LanternRadius-0.01f;
            b.inside=around((a0+a1)*0.5f,(r0+r1)*0.5f,galleryY-t*0.5f);
            b.Quad(around(a0,r0,galleryY),around(a1,r0,galleryY),around(a1,r1,galleryY),around(a0,r1,galleryY),Stone*0.92f);
            b.Quad(around(a0,r0,galleryY-t),around(a0,r1,galleryY-t),around(a1,r1,galleryY-t),around(a1,r0,galleryY-t),Plaster*0.7f);
        }
        for(const float a:{from,to}) {
            b.inside=around(a+(a==from?0.2f:-0.2f),0.7f,galleryY-t*0.5f);
            b.Quad(around(a,L::NewelRadius,galleryY-t),around(a,L::LanternRadius-0.01f,galleryY-t),
                   around(a,L::LanternRadius-0.01f,galleryY),around(a,L::NewelRadius,galleryY),Stone*0.8f);
        }
    }
    interior(false);
    // the stone steps up to the door from the ground inland
    {
        const float drop=baseY-placement.stepsFootY;
        const int count=std::max(1,static_cast<int>(std::ceil(drop/0.19f)));
        for(int k=0;k<count;++k) {
            const float from=L::PlinthRadius+placement.stepsRun*static_cast<float>(k)/static_cast<float>(count)-0.02f;
            const float to=L::PlinthRadius+placement.stepsRun*static_cast<float>(k+1)/static_cast<float>(count);
            const float top=baseY-drop*(static_cast<float>(k)+0.5f)/static_cast<float>(count);
            const Vector3 centre=around(Pi,(from+to)*0.5f,(top+placement.stepsFootY-0.4f)*0.5f);
            const Vector3 in=Vector3::Normalize(around(Pi,1.0f,0.0f)-axis);
            b.Box(centre,Vector3(in.Z,0.0f,-in.X),Vector3::Up,in,L::StepsHalfWidth,(top-placement.stepsFootY+0.4f)*0.5f,(to-from)*0.5f,
                  Stone*(0.86f+0.08f*static_cast<float>(k%2)),Stone*0.95f);
        }
    }
    // the gallery on its corbels, its railing
    {
        constexpr int Ring=32;
        // the slab reaches in to the lantern, over the wall's top
        const float inner=LighthousePlacement::LanternRadius-0.02f,outer=LighthousePlacement::GalleryRadius,t=0.22f;
        b.inside=axis+Vector3::Up*(galleryY-t*0.5f);
        for(int k=0;k<Ring;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/Ring,a1=2.0f*Pi*static_cast<float>(k+1)/Ring;
            b.Quad(around(a0,inner,galleryY),around(a1,inner,galleryY),around(a1,outer,galleryY),around(a0,outer,galleryY),Stone*0.95f);
            b.Quad(around(a0,inner,galleryY-t),around(a0,outer,galleryY-t),around(a1,outer,galleryY-t),around(a1,inner,galleryY-t),Stone*0.8f);
            b.Quad(around(a0,outer,galleryY-t),around(a1,outer,galleryY-t),around(a1,outer,galleryY),around(a0,outer,galleryY),Stone*0.9f);
            b.Quad(around(a0,inner,galleryY-t),around(a0,inner,galleryY),around(a1,inner,galleryY),around(a1,inner,galleryY-t),Stone*0.85f);
        }
        for(int k=0;k<16;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/16.0f;
            Vector3 out=around(a,1.0f,0.0f)-axis;out.Y=0.0f;out.Normalize();
            b.Blade(around(a,LighthousePlacement::TopRadius-0.05f,galleryY-t-0.45f),Vector3(out.Z,0,-out.X),out*0.55f+Vector3::Up*0.8f,
                    Vector3::Up,0.06f,0.62f,0.16f,0.07f,Stone*0.9f);
        }
        std::vector<Vector3> top,mid;std::vector<float> radii;
        for(int k=0;k<=36;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/36.0f;
            const float rail=outer-LighthousePlacement::RailInset;
            top.push_back(around(a,rail,galleryY+LighthousePlacement::RailHeight));
            mid.push_back(around(a,rail,galleryY+LighthousePlacement::RailHeight*0.5f));radii.push_back(LighthousePlacement::RailRadius);
            if(k<36&&k%2==0) b.Tube({around(a,rail,galleryY),around(a,rail,galleryY+LighthousePlacement::RailHeight)},{0.025f,0.025f},5,Iron);
        }
        b.Tube(top,radii,5,Iron);b.Tube(mid,radii,5,Iron);
    }
    // the lantern: a low wall, twelve glazed panes in white frames, the red dome, ball and rod
    {
        constexpr int Panes=12;
        const float r=LighthousePlacement::LanternRadius,wall=0.55f,glassTop=galleryY+LighthousePlacement::LanternHeight;
        // the door: the two panes either side of LanternDoorAngle (240 to 300 degrees), the iron
        // wall under them cut away with them; a white post either side, a lintel over it
        const auto door=[](const int k) {return k==8||k==9;};
        for(int k=0;k<Panes;++k) {
            if(door(k)) continue;
            const float a0=2.0f*Pi*static_cast<float>(k)/Panes,a1=2.0f*Pi*static_cast<float>(k+1)/Panes;
            b.inside=axis+Vector3::Up*(galleryY+wall*0.5f);
            b.Quad(around(a0,r,galleryY),around(a1,r,galleryY),around(a1,r,galleryY+wall),around(a0,r,galleryY+wall),Iron*1.4f);
            clear.inside=axis+Vector3::Up*(galleryY+1.2f);
            clear.Quad(around(a0,r*0.98f,galleryY+wall),around(a1,r*0.98f,galleryY+wall),around(a1,r*0.98f,glassTop),around(a0,r*0.98f,glassTop),
                       Glass*(0.9f+0.25f*static_cast<float>(k%3)));
        }
        for(int k=0;k<Panes;++k) {
            if(k==9) continue;
            const float a0=2.0f*Pi*static_cast<float>(k)/Panes;
            b.Tube({around(a0,r,(k==8||k==10)?galleryY:galleryY+wall),around(a0,r,glassTop)},{0.035f,0.035f},5,Frame);
        }
        for(const float y:{galleryY+wall,galleryY+wall+0.9f,glassTop}) {
            std::vector<Vector3> ring;std::vector<float> radii;
            const bool full=y>=galleryY+LighthousePlacement::LanternDoorHeight;
            for(int k=full?0:10;k<=(full?Panes:20);++k) {ring.push_back(around(2.0f*Pi*static_cast<float>(k)/Panes,r,y));radii.push_back(0.04f);}
            b.Tube(ring,radii,5,Frame);
        }
        {
            std::vector<Vector3> lintel;std::vector<float> radii;
            for(int k=0;k<=4;++k) {
                lintel.push_back(around(2.0f*Pi*(8.0f+0.5f*static_cast<float>(k))/Panes,r,galleryY+LighthousePlacement::LanternDoorHeight));
                radii.push_back(0.045f);
            }
            b.Tube(lintel,radii,5,Frame);
            // the door itself, swung open out onto the gallery against the rail: an iron panel
            // under a glazed one in a white frame
            const Vector3 hinge=around(2.0f*Pi*8.0f/Panes,r,galleryY),shut=around(2.0f*Pi*10.0f/Panes,r,galleryY);
            Vector3 leaf=shut-hinge;
            const float width=leaf.Length()*0.98f;
            leaf.Normalize();
            const float swing=1.75f,c=std::cos(swing),sn=std::sin(swing);
            Vector3 open(leaf.X*c-leaf.Z*sn,0.0f,leaf.X*sn+leaf.Z*c);
            if(Vector3::Dot(open,hinge-axis)<0.0f) open=Vector3(leaf.X*c+leaf.Z*sn,0.0f,-leaf.X*sn+leaf.Z*c);
            const Vector3 across(open.Z,0.0f,-open.X);
            const float h=LighthousePlacement::LanternDoorHeight-0.03f;
            b.Box(hinge+open*(width*0.5f)+Vector3::Up*(wall*0.5f+0.02f),open,Vector3::Up,across,width*0.5f,wall*0.5f,0.02f,Iron*1.4f,Iron*1.4f);
            clear.Box(hinge+open*(width*0.5f)+Vector3::Up*((wall+h)*0.5f+0.02f),open,Vector3::Up,across,width*0.5f-0.05f,(h-wall)*0.5f,0.012f,Glass*1.2f,Glass*1.2f);
            for(const float e:{0.03f,width-0.03f})
                b.Tube({hinge+open*e+Vector3::Up*0.02f,hinge+open*e+Vector3::Up*(h+0.02f)},{0.03f,0.03f},5,Frame);
            b.Tube({hinge+Vector3::Up*(h+0.02f),hinge+open*width+Vector3::Up*(h+0.02f)},{0.03f,0.03f},5,Frame);
        }
        // the lamp's lens on its pedestal: brass rings round tiers of glass prisms
        b.Tube({axis+Vector3::Up*(galleryY+0.55f),axis+Vector3::Up*(galleryY+0.65f)},{0.3f,0.3f},12,Brass);
        b.Tube({axis+Vector3::Up*(galleryY+0.65f),axis+Vector3::Up*(galleryY+1.05f),axis+Vector3::Up*(galleryY+1.42f)},
               {0.33f,0.36f,0.24f},12,Lens);
        for(const float y:{galleryY+0.8f,galleryY+1.05f,galleryY+1.28f})
            b.Tube({axis+Vector3::Up*(y-0.015f),axis+Vector3::Up*(y+0.015f)},{0.37f,0.37f},12,Brass);
        // the dome: a flattened bell from its eave to the vent, then the ball and the rod
        std::vector<std::vector<Vector3>> dome;std::vector<Vector3> centres;
        for(int i=0;i<=6;++i) {
            const float t=static_cast<float>(i)/6.0f;
            const float y=glassTop+0.95f*std::sin(t*Pi*0.5f),rr=(r+0.22f)*std::cos(t*Pi*0.5f)*0.92f+0.12f;
            std::vector<Vector3> ring;
            for(int k=0;k<24;++k) ring.push_back(around(2.0f*Pi*static_cast<float>(k)/24.0f,rr,y));
            dome.push_back(ring);centres.push_back(axis+Vector3::Up*y);
        }
        b.Loft(dome,centres,[](std::size_t,std::size_t) {return Dome;},true,true);
        b.Tube({axis+Vector3::Up*(glassTop+0.9f),axis+Vector3::Up*(glassTop+1.15f)},{0.08f,0.08f},8,Iron);
        b.Tube({axis+Vector3::Up*(glassTop+1.12f),axis+Vector3::Up*(glassTop+1.38f)},{0.16f,0.02f},10,Dome*0.95f);
        b.Tube({axis+Vector3::Up*(glassTop+1.3f),axis+Vector3::Up*(glassTop+2.1f)},{0.02f,0.012f},4,Iron);
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
    // the glass: its own faint green-grey over what lies behind (premultiplied), a glint where a
    // pane turns to the sun
    {
        auto& panes=clear.builder.mesh;
        Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
        for(std::size_t tri=0;tri<panes.size()/3U;++tri) {
            const Vector3 a=panes[tri*3U].Position,bb=panes[tri*3U+1U].Position,c=panes[tri*3U+2U].Position;
            Vector3 normal=Vector3::Cross(bb-a,c-a);
            if(normal.LengthSquared()<1e-14f) continue;
            normal.Normalize();
            const float glint=std::clamp((std::abs(Vector3::Dot(normal,sun))-0.7f)/0.2f,0.0f,1.0f);
            const float alpha=0.22f+0.45f*glint;
            for(std::size_t v=0;v<3U;++v) {
                const Color lit=panes[tri*3U+v].Color;
                const auto channel=[&](const int value) {return static_cast<int>(std::lround(std::min(255.0f,static_cast<float>(value)*alpha+255.0f*0.35f*glint)));};
                panes[tri*3U+v].Color=Color(channel(lit.getRProperty()),channel(lit.getGProperty()),channel(lit.getBProperty()),
                                            static_cast<int>(std::lround(alpha*255.0f)));
            }
        }
        glassTriangles_=static_cast<int>(panes.size()/3U);
        if(glassTriangles_>0) {
            glass_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
                static_cast<int>(panes.size()),BufferUsage::WriteOnly);
            glass_->SetData(panes.data(),static_cast<int>(panes.size()));
        }
    }
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"lighthouse",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: lighthouse sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    lamp_=Vector3(placement.Lamp()[0],placement.Lamp()[1],placement.Lamp()[2]);
    lightEffect_=std::make_unique<BasicEffect>(device);
    lightEffect_->setVertexColorEnabledProperty(true);lightEffect_->setLightingEnabledProperty(false);
    light_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),512,BufferUsage::WriteOnly);
    lightStaging_.reserve(512);
    std::cout<<"island-simulator: at night the lighthouse's lamp burns and its two beams sweep round over the sea every "
             <<static_cast<int>(BeamTurnSeconds)<<" seconds, flaring as they pass the eye\n";
    std::ostringstream message;
    message<<"island-simulator: a whitewashed lighthouse with two red bands stands over the sea at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<"), its glazed lantern "<<static_cast<int>(std::lround(placement.Lamp()[1]-placement.groundY))
        <<" m up under a red dome; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void LighthouseRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>700*700
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
    if(glass_) {
        // the lantern's glass over what lies behind it, without hiding what lies past it
        device_.setBlendStateProperty(BlendState::AlphaBlend);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
        device_.SetVertexBuffer(glass_.get());device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList,0,glassTriangles_);++drawCalls_;
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
    }
}

Vector3 LighthouseRenderer::BeamDirection(const float seconds) noexcept {
    const float a=(std::isfinite(seconds)?std::fmod(seconds,BeamTurnSeconds):0.0f)/BeamTurnSeconds*2.0f*Pi;
    // a little below the level, so the beam reaches the sea at the horizon's distance
    return Vector3::Normalize(Vector3(std::cos(a),-0.025f,std::sin(a)));
}

void LighthouseRenderer::DrawLight(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                   const float night,const float seconds) {
    flare_=0;
    if(!(night>0.02f)||(lamp_-camera).LengthSquared()>900.0f*900.0f) return;
    lightStaging_.clear();
    const auto colour=[](const float r,const float g,const float b) {
        const auto c=[](const float v){return static_cast<int>(std::lround(std::clamp(v,0.0f,1.0f)*255.0f));};
        return Color(c(r),c(g),c(b),255);
    };
    Vector3 toEye=camera-lamp_;
    const float eyeDistance=std::max(toEye.Length(),0.1f);
    toEye/=eyeDistance;
    // the two beams: each three crossed fans from the lamp widening out to their length, bright at
    // the lamp and gone at the end
    for(const float sign:{1.0f,-1.0f}) {
        const Vector3 dir=BeamDirection(seconds)*sign;
        flare_=std::max(flare_,std::pow(std::max(0.0f,Vector3::Dot(dir,toEye)),60.0f));
        Vector3 across=Vector3::Cross(dir,Vector3::Up);across.Normalize();
        const Vector3 up=Vector3::Cross(across,dir);
        for(int fan=0;fan<3;++fan) {
            const float a=Pi*static_cast<float>(fan)/3.0f;
            const Vector3 spread=across*std::cos(a)+up*std::sin(a);
            // the beam's core along its axis, fading out to nothing at its edges and its end
            const std::array<float,4> along{{0.0f,0.12f,0.40f,1.0f}},width{{0.30f,1.9f,5.2f,12.0f}},glow{{0.16f,0.12f,0.05f,0.0f}};
            for(std::size_t i=0;i+1<along.size();++i) {
                const Vector3 c0=lamp_+dir*(BeamLength*along[i]),c1=lamp_+dir*(BeamLength*along[i+1]);
                const Color k0=colour(glow[i]*night,glow[i]*0.94f*night,glow[i]*0.76f*night);
                const Color k1=colour(glow[i+1]*night,glow[i+1]*0.94f*night,glow[i+1]*0.76f*night);
                const Color none(0,0,0,255);
                for(const float edge:{1.0f,-1.0f}) {
                    const Vector3 e0=c0+spread*(edge*width[i]),e1=c1+spread*(edge*width[i+1]);
                    const VertexPositionColor q[]={{c0,k0},{e0,none},{e1,none},{c0,k0},{e1,none},{c1,k1}};
                    for(const auto& v:q) lightStaging_.push_back(v);
                }
            }
        }
    }
    // the lamp's glow, a disc turned to the eye, flaring as a beam swings past
    {
        Vector3 right=Vector3::Cross(toEye,Vector3::Up);
        if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
        right.Normalize();
        const Vector3 up=Vector3::Cross(right,toEye);
        const float radius=1.2f+7.0f*flare_;
        const Vector3 centre=lamp_+toEye*1.4f;
        const Color core=colour((0.85f+0.15f*flare_)*night,(0.78f+0.2f*flare_)*night,(0.55f+0.3f*flare_)*night),rim(0,0,0,255);
        constexpr int Segments=20;
        for(int k=0;k<Segments;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/Segments,a1=2.0f*Pi*static_cast<float>(k+1)/Segments;
            lightStaging_.emplace_back(centre,core);
            lightStaging_.emplace_back(centre+(right*std::cos(a0)+up*std::sin(a0))*radius,rim);
            lightStaging_.emplace_back(centre+(right*std::cos(a1)+up*std::sin(a1))*radius,rim);
        }
    }
    light_->SetData(lightStaging_.data(),0,static_cast<int>(lightStaging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(light_.get());device_.setIndicesProperty(nullptr);
    lightEffect_->setWorldProperty(Matrix::getIdentityProperty());
    lightEffect_->setViewProperty(view);lightEffect_->setProjectionProperty(projection);lightEffect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(lightStaging_.size()/3));
    ++drawCalls_;
}

void LighthouseRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"lighthouse");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: lighthouse sun shadow caster unavailable: "<<error.what()<<'\n';
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
