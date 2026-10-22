#include "IslandSimulator/PlaneWreckRenderer.hpp"
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
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// the painted skin's marker (its colour comes from the face's facing and wear), and the other
// pigments past full for the build's light
const Vector3 Skin(1,2,3),Olive(124,146,94),Underside(196,196,188),Bare(214,214,208),Corroded(236,232,220),
    Disc(232,70,56),Glass(48,62,68),Frame(96,104,82),Engine(78,76,72),Prop(70,66,58),Rubber(40,38,36),
    Primer(118,132,104),Leather(98,72,50),Panel(34,34,32),Gauge(30,30,28);
// the cockpit's opening in the fuselage's top, forward from the wing roots
constexpr float CockpitAft=-0.3f,CockpitFore=1.5f;
// A frame for one piece of the wreck: origin and its forward, right and up axes.
struct Frame3 {
    Vector3 o,f,r,u;
    [[nodiscard]] Vector3 At(const float forward,const float right,const float up) const {return o+f*forward+r*right+u*up;}
    // turned about its own axes by yaw (about up), pitch (nose down positive) and roll (right wing down positive)
    [[nodiscard]] Frame3 Turned(const float yaw,const float pitch,const float roll) const {
        Vector3 nf=f*std::cos(yaw)+r*std::sin(yaw),nr=r*std::cos(yaw)-f*std::sin(yaw),nu=u;
        const Vector3 pf=nf*std::cos(pitch)-nu*std::sin(pitch),pu=nu*std::cos(pitch)+nf*std::sin(pitch);
        nf=pf;nu=pu;
        const Vector3 rr=nr*std::cos(roll)-nu*std::sin(roll),ru=nu*std::cos(roll)+nr*std::sin(roll);
        return {o,nf,rr,ru};
    }
};
float Profile(const std::vector<std::array<float,2>>& points,const float x) {
    if(x>=points.front()[0]) return points.front()[1];
    for(std::size_t i=0;i+1<points.size();++i)
        if(x>=points[i+1][0]) return points[i][1]+(points[i+1][1]-points[i][1])*(x-points[i][0])/(points[i+1][0]-points[i][0]);
    return points.back()[1];
}
}

PlaneWreckRenderer::PlaneWreckRenderer(GraphicsDevice& device,const PlaneWreckPlacement& placement,
                                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b,clear;   // the metal; the canopy's glass, drawn see-through
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(124,110,84);b.stainHeight=0.6f;b.stainAmount=0.35f;
    // the painted skin: dark green on the faces turned up, grey under, worn through to aluminium
    // in patches and white with corrosion here and there
    // weathered corner by corner: tinted at each face's centre, the skin broke up into
    // triangles (or, per quad, tiles) of paint and bare metal
    b.tintPerVertex=true;
    b.tint=[](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        if(pigment.X!=Skin.X||pigment.Y!=Skin.Y||pigment.Z!=Skin.Z) return pigment;
        // the green blends into the grey round the sides, so the two halves of a quad never
        // step from one to the other
        const float top=std::clamp((normal.Y+0.25f)/0.7f,0.0f,1.0f);
        Vector3 colour=Vector3::Lerp(Underside,Olive,top*top*(3.0f-2.0f*top));
        const float wear=std::sin(centre.X*3.1f+centre.Z*1.7f)*std::sin(centre.Z*2.3f-centre.Y*4.1f+centre.X*0.7f);
        colour=Vector3::Lerp(colour,Bare,std::clamp((wear-0.30f)*1.8f,0.0f,1.0f)*0.7f);
        const float rot=std::sin(centre.X*11.0f+centre.Y*7.0f)*std::sin(centre.Z*9.0f-centre.X*5.0f);
        colour=Vector3::Lerp(colour,Corroded,std::clamp((rot-0.6f)*3.0f,0.0f,1.0f)*0.6f);
        // eighty years under the jungle: moss and lichen on the faces turned up
        const float moss=std::sin(centre.X*1.9f+centre.Z*1.3f+std::sin(centre.Y*2.7f))*std::sin(centre.Z*1.6f-centre.X*0.9f);
        colour=Vector3::Lerp(colour,Vector3(84,108,54),std::clamp((moss-0.15f)*2.2f,0.0f,1.0f)*std::clamp((normal.Y-0.35f)*2.0f,0.0f,1.0f)*0.7f);
        return colour*(0.94f+0.06f*std::sin(centre.X*5.3f+centre.Z*4.1f));
    };
    const Vector3 heading(placement.headingX,0.0f,placement.headingZ),right(placement.headingZ,0.0f,-placement.headingX);
    // the main piece lies on its belly, nose down a little and rolled onto its right wing
    const Frame3 main=Frame3{Vector3(placement.x,placement.groundY+0.52f,placement.z),heading,right,Vector3::Up}.Turned(0.0f,0.10f,0.07f);
    // the fuselage's radius along it (forward from the wing roots), and a lofted round section
    const std::vector<std::array<float,2>> fuselage{{{4.25f,0.10f},{4.05f,0.36f},{3.85f,0.56f},{3.55f,0.64f},{2.6f,0.65f},
        {1.6f,0.63f},{0.0f,0.59f},{-1.5f,0.49f},{-3.0f,0.33f},{-4.4f,0.17f},{-4.8f,0.08f}}};
    // the fuselage, lofted round; `open` leaves the cockpit's opening in its top
    const auto loftFuselage=[&](const Frame3& frame,const float from,const float to,const float shift,const bool open) {
        constexpr int Ring=18;
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;std::vector<float> stationsAt;
        const int stations=std::max(2,static_cast<int>(std::abs(from-to)/0.3f));
        for(int i=0;i<=stations;++i) {
            const float f=from+(to-from)*static_cast<float>(i)/static_cast<float>(stations);
            const float rr=Profile(fuselage,f);
            std::vector<Vector3> ring;
            for(int k=0;k<Ring;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Ring;
                ring.push_back(frame.At(f-shift,std::cos(a)*rr,std::sin(a)*rr*1.08f));
            }
            rings.push_back(ring);centres.push_back(frame.At(f-shift,0.0f,0.0f));stationsAt.push_back(f);
        }
        for(std::size_t i=0;i+1<rings.size();++i) {
            b.inside=(centres[i]+centres[i+1])*0.5f;
            const float fm=(stationsAt[i]+stationsAt[i+1])*0.5f;
            for(int k=0;k<Ring;++k) {
                const auto kk=static_cast<std::size_t>(k),k1=static_cast<std::size_t>((k+1)%Ring);
                const float am=2.0f*Pi*(static_cast<float>(k)+0.5f)/Ring;
                if(open&&fm>CockpitAft&&fm<CockpitFore&&std::sin(am)>0.7f) continue;
                b.Quad(rings[i][kk],rings[i+1][kk],rings[i+1][k1],rings[i][k1],Skin);
            }
        }
        for(std::size_t k=0;k<static_cast<std::size_t>(Ring);++k) {
            const std::size_t k1=(k+1)%static_cast<std::size_t>(Ring);
            b.inside=centres[1];b.Face(centres.front(),rings.front()[k1],rings.front()[k],Skin);
            b.inside=centres[centres.size()-2];b.Face(centres.back(),rings.back()[k],rings.back()[k1],Skin);
        }
    };
    loftFuselage(main,3.85f,-1.05f,0.0f,true);
    // the cockpit under its canopy: a tub painted the grey-green of the factory's primer, the
    // pilot's seat, the stick and the rudder pedals, the instrument panel with its dials, the
    // headrest on the bulkhead behind; a sill along the opening's edges
    {
        const auto box=[&](const float f0,const float f1,const float r0,const float r1,const float u0,const float u1,const Vector3& pigment) {
            b.Box(main.At((f0+f1)*0.5f,(r0+r1)*0.5f,(u0+u1)*0.5f),main.f,main.u,main.r,(f1-f0)*0.5f,(u1-u0)*0.5f,(r1-r0)*0.5f,pigment,pigment);
        };
        box(CockpitAft,CockpitFore,-0.42f,0.42f,-0.2f,-0.14f,Primer*0.8f);
        for(const float side:{-1.0f,1.0f}) {
            box(CockpitAft,CockpitFore,side>0.0f?0.4f:-0.44f,side>0.0f?0.44f:-0.4f,-0.2f,0.46f,Primer);
            // the sill between the opening's edge and the canopy's frame
            b.inside=main.At(0.6f,side*0.42f,0.2f);
            b.Quad(main.At(CockpitAft,side*0.36f,0.5f),main.At(CockpitFore,side*0.36f,0.5f),main.At(CockpitFore,side*0.49f,0.44f),
                   main.At(CockpitAft,side*0.49f,0.44f),Primer*0.9f);
        }
        box(CockpitAft-0.05f,CockpitAft,-0.42f,0.42f,-0.2f,0.5f,Primer*0.9f);
        b.Tube({main.At(CockpitAft+0.02f,-0.18f,0.5f),main.At(CockpitAft+0.02f,0.18f,0.5f)},{0.1f,0.1f},8,Leather*0.8f);
        // the seat, its pan and its back, leather gone dark and split
        box(0.1f,0.55f,-0.22f,0.22f,-0.02f,0.04f,Leather);
        b.Box(main.At(0.06f,0.0f,0.25f),main.u*0.96f+main.f*0.28f,main.r,main.f*0.96f-main.u*0.28f,0.24f,0.22f,0.03f,Leather*0.9f,Leather*0.9f);
        // the stick and the pedals
        b.Tube({main.At(0.85f,0.0f,-0.14f),main.At(0.82f,0.0f,0.18f),main.At(0.8f,0.0f,0.26f)},{0.022f,0.018f,0.03f},6,Rubber);
        for(const float side:{-1.0f,1.0f}) box(1.2f,1.28f,side*0.12f-0.05f,side*0.12f+0.05f,-0.14f,0.02f,Engine);
        // the instrument panel facing the seat: black, its dials pale rings with dark faces
        b.Box(main.At(CockpitFore-0.04f,0.0f,0.2f),main.f,main.u,main.r,0.03f,0.22f,0.36f,Panel,Panel);
        const std::array<std::array<float,2>,7> Dials{{{-0.22f,0.3f},{0.0f,0.32f},{0.22f,0.3f},{-0.24f,0.12f},{-0.08f,0.1f},{0.08f,0.1f},{0.24f,0.12f}}};
        for(const auto& dial:Dials) {
            const Vector3 centre=main.At(CockpitFore-0.075f,dial[0],dial[1]);
            b.inside=centre+main.f*0.2f;
            for(int k=0;k<10;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/10.0f,a1=2.0f*Pi*static_cast<float>(k+1)/10.0f;
                const Vector3 e0=main.r*std::cos(a0)+main.u*std::sin(a0),e1=main.r*std::cos(a1)+main.u*std::sin(a1);
                b.Face(centre,centre+e0*0.045f,centre+e1*0.045f,Gauge);
                b.Quad(centre+e0*0.045f,centre+e1*0.045f,centre+e1*0.06f,centre+e0*0.06f,Corroded);
            }
        }
    }
    // the tail section, broken off behind the cockpit, thrown aside and twisted onto its side
    const Frame3 tail=Frame3{main.At(-1.25f,0.85f,-0.12f),heading,right,Vector3::Up}.Turned(0.48f,-0.04f,-0.32f);
    loftFuselage(tail,-1.15f,-4.8f,-1.15f,false);
    {
        // its fin and rudder, and the tailplanes
        b.Blade(tail.At(-3.0f,0.0f,0.22f),tail.r,tail.u*0.94f-tail.f*0.34f,tail.f,0.045f,1.25f,0.62f,0.28f,Skin);
        for(const float s:{-1.0f,1.0f})
            b.Blade(tail.At(-3.25f,s*0.12f,0.02f),tail.u,tail.r*s-tail.f*0.18f,tail.f,0.035f,1.85f,0.48f,0.24f,Skin);
    }
    // the cowling's open front with the radial engine's cylinders, the spinner and the bent prop
    {
        for(int k=0;k<9;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/9.0f;
            b.Box(main.At(3.78f,std::cos(a)*0.38f,std::sin(a)*0.40f),main.r*std::cos(a)+main.u*std::sin(a),main.f,
                  main.u*std::cos(a)-main.r*std::sin(a),0.11f,0.08f,0.06f,Engine,Engine*1.1f);
        }
        b.Tube({main.At(3.80f,0.0f,0.0f),main.At(4.25f,0.0f,0.0f),main.At(4.45f,0.0f,0.0f)},{0.24f,0.15f,0.02f},12,Frame);
        for(int blade=0;blade<3;++blade) {
            const float a=2.0f*Pi*static_cast<float>(blade)/3.0f+0.4f;
            const Vector3 out=main.r*std::cos(a)+main.u*std::sin(a);
            // the blades that struck the ground bent back on themselves
            const float bend=blade==1?0.0f:0.9f;
            const Vector3 root=main.At(4.18f,0.0f,0.0f)+out*0.18f;
            const Vector3 knee=root+out*0.62f;
            Vector3 tipDir=out*std::cos(bend)-main.f*std::sin(bend);
            b.Blade(root,main.f,out,Vector3::Cross(out,main.f),0.03f,0.62f,0.12f,0.11f,Prop);
            b.Blade(knee,main.f,tipDir,Vector3::Cross(tipDir,main.f),0.025f,0.72f,0.11f,0.06f,Prop);
        }
    }
    // the canopy: a framed glasshouse over the cockpit, its rear panes gone
    {
        for(int i=0;i<=6;++i) {
            const float f=1.5f-1.8f*static_cast<float>(i)/6.0f;
            std::vector<Vector3> arch;std::vector<float> radii;
            for(int k=0;k<=8;++k) {
                const float a=Pi*static_cast<float>(k)/8.0f;
                arch.push_back(main.At(f,std::cos(a)*0.36f,0.50f+std::sin(a)*0.40f*(1.0f-0.25f*std::abs(f-0.6f))));
                radii.push_back(0.022f);
            }
            b.Tube(arch,radii,4,Frame);
            if(i<4&&i>0) {
                const float f1=1.5f-1.8f*static_cast<float>(i+1)/6.0f;
                for(int k=0;k<8;++k) {
                    const float a0=Pi*static_cast<float>(k)/8.0f,a1=Pi*static_cast<float>(k+1)/8.0f;
                    const auto at=[&](const float ff,const float a) {
                        return main.At(ff,std::cos(a)*0.35f,0.50f+std::sin(a)*0.39f*(1.0f-0.25f*std::abs(ff-0.6f)));
                    };
                    clear.inside=main.At((f+f1)*0.5f,0.0f,0.55f);
                    clear.Quad(at(f,a0),at(f1,a0),at(f1,a1),at(f,a1),Glass*(1.0f+0.3f*static_cast<float>(k%3)));
                }
            }
        }
    }
    // the wings: a lofted section, thick at the root and thin at the rounded tip, with the
    // fuselage's red disc near each tip; the right one crumpled at mid span, the left torn off
    const auto wing=[&](const Frame3& frame,const float side,const float droop,const bool flipped) {
        constexpr std::array<float,5> Chord{{0.0f,0.08f,0.30f,0.60f,1.0f}};
        const auto camber=[](const float x) {return 2.2f*std::sqrt(x)*(1.0f-x);};
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        constexpr int Stations=9;
        for(int i=0;i<=Stations;++i) {
            const float s=static_cast<float>(i)/Stations;
            const float span=0.55f+5.3f*s;
            const float chord=(2.2f-1.15f*s)*(i==Stations?0.55f:1.0f);
            const float lead=1.2f-0.55f*s-(i==Stations?0.25f:0.0f);
            const float thick=0.16f-0.10f*s;
            const float lift=span*0.10f-std::max(0.0f,s-0.45f)*droop;
            std::vector<Vector3> ring;
            for(int k=0;k<5;++k) ring.push_back(frame.At(lead-Chord[static_cast<std::size_t>(k)]*chord,side*span,lift+thick*0.5f*camber(Chord[static_cast<std::size_t>(k)])*(flipped?-1.0f:1.0f)));
            for(int k=3;k>=1;--k) ring.push_back(frame.At(lead-Chord[static_cast<std::size_t>(k)]*chord,side*span,lift-thick*0.2f*camber(Chord[static_cast<std::size_t>(k)])*(flipped?-1.0f:1.0f)));
            rings.push_back(ring);centres.push_back(frame.At(lead-0.4f*chord,side*span,lift));
        }
        b.Loft(rings,centres,[](std::size_t,std::size_t) {return Skin;});
        // the red disc on the upper face (on the lower once it lies upside down)
        const float s=0.72f,span=0.55f+5.3f*s,chord=2.2f-1.15f*s,lead=1.2f-0.55f*s,thick=0.16f-0.10f*s;
        const float lift=span*0.10f-std::max(0.0f,s-0.45f)*droop;
        const float face=(flipped?-1.0f:1.0f)*(thick*0.5f*camber(0.35f)+0.012f);
        const Vector3 centre=frame.At(lead-0.38f*chord,side*span,lift+face);
        b.inside=centre-frame.u*(flipped?-1.0f:1.0f);
        for(int k=0;k<14;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/14.0f,a1=2.0f*Pi*static_cast<float>(k+1)/14.0f;
            b.Face(centre,centre+frame.f*(std::cos(a0)*0.46f)+frame.r*(std::sin(a0)*0.46f),
                   centre+frame.f*(std::cos(a1)*0.46f)+frame.r*(std::sin(a1)*0.46f),Disc);
        }
    };
    wing(main,1.0f,0.75f,false);
    {
        const auto spot=placement.At(-0.8f,-6.8f);
        const float g=groundAt(spot[0],spot[1]);
        // the torn wing upside down on the ground, its root toward the fuselage
        const Frame3 torn=Frame3{Vector3(spot[0],g+0.10f,spot[1]),heading,right,Vector3::Up}.Turned(0.55f,0.0f,Pi);
        wing(Frame3{torn.o+torn.r*(-3.3f),torn.f,torn.r,torn.u},1.0f,0.0f,false);
    }
    // the red discs on the tail section's flanks
    for(const float s:{-1.0f,1.0f}) {
        const float rr=Profile(fuselage,-2.2f)+0.012f;
        const Vector3 centre=tail.At(-2.2f+1.15f,s*rr,0.0f);
        b.inside=tail.At(-2.2f+1.15f,0.0f,0.0f);
        for(int k=0;k<12;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/12.0f,a1=2.0f*Pi*static_cast<float>(k+1)/12.0f;
            b.Face(centre,centre+tail.f*(std::cos(a0)*0.30f)+tail.u*(std::sin(a0)*0.30f),
                   centre+tail.f*(std::cos(a1)*0.30f)+tail.u*(std::sin(a1)*0.30f),Disc*0.92f);
        }
    }
    // a landing leg sticking up from under the right wing, its wheel still on it
    {
        const Vector3 root=main.At(0.6f,2.0f,-0.05f),knee=root+main.u*0.5f+main.r*0.35f-main.f*0.2f;
        b.Tube({root,knee},{0.05f,0.045f},8,Frame);
        const Vector3 axle=knee+main.u*0.12f;
        b.Tube({axle-main.r*0.08f,axle+main.r*0.08f},{0.28f,0.28f},14,Rubber);
    }

    // the jungle taking it back: creepers climbing over the fuselage and the tail section, their
    // leaves along them, and ferns come up round its belly
    {
        const Vector3 vine(62,92,44),leaf(74,124,52),fern(68,116,50);
        const auto creeper=[&](const Frame3& frame,const float from,const float to,const float radius,const float drift,const std::uint32_t seed) {
            std::vector<Vector3> points;std::vector<float> radii;
            constexpr int Joints=16;
            for(int j=0;j<=Joints;++j) {
                const float t=static_cast<float>(j)/Joints;
                // up one flank from the ground, over the top and down the other
                const float a=-Pi*0.55f+Pi*1.1f*t;
                const float f=from+(to-from)*t+0.15f*std::sin(t*9.0f+static_cast<float>(seed));
                const float r=radius+0.03f;
                points.push_back(frame.At(f+drift*t,std::cos(a)*r*(t<0.5f?-1.0f:1.0f)*0.0f+std::sin(a)*r,std::cos(a)*r*1.08f));
                radii.push_back(0.018f);
            }
            b.Tube(points,radii,4,vine);
            for(int j=1;j<Joints;++j) {
                const Vector3 at=points[static_cast<std::size_t>(j)];
                Vector3 along=points[static_cast<std::size_t>(j+1)]-points[static_cast<std::size_t>(j-1)];along.Normalize();
                Vector3 out=at-frame.At(from+(to-from)*static_cast<float>(j)/Joints,0.0f,0.0f);out.Normalize();
                const float s=j%2==0?1.0f:-1.0f;
                const Vector3 side=Vector3::Normalize(Vector3::Cross(along,out))*s;
                const Vector3 tip=at+side*0.24f+out*0.06f+along*0.06f;
                const Vector3 w=along*0.09f;
                b.inside=at-out*0.2f;
                b.Quad(at,at+side*0.12f+w+out*0.04f,tip,at+side*0.12f-w+out*0.04f,leaf*(0.85f+0.3f*std::sin(static_cast<float>(j*3)+static_cast<float>(seed))));
            }
        };
        creeper(main,2.9f,2.5f,0.65f,0.3f,5U);
        creeper(main,2.2f,1.8f,0.62f,0.4f,1U);
        creeper(main,0.3f,0.6f,0.58f,-0.4f,6U);
        creeper(main,-0.4f,-0.9f,0.55f,-0.3f,2U);
        creeper(tail,-1.8f,-2.3f,0.45f,0.2f,3U);
        creeper(tail,-3.2f,-3.0f,0.3f,0.1f,4U);
        for(int k=0;k<6;++k) {
            const float side=k%2==0?1.0f:-1.0f;
            const auto spot=placement.At(-1.5f+1.2f*static_cast<float>(k/2),side*(1.2f+0.3f*static_cast<float>(k%3)));
            const Vector3 base(spot[0],groundAt(spot[0],spot[1]),spot[1]);
            for(int f=0;f<6;++f) {
                const float a=2.0f*Pi*static_cast<float>(f)/6.0f+static_cast<float>(k);
                const Vector3 out(std::cos(a),0.0f,std::sin(a));
                b.Blade(base,Vector3::Up,Vector3::Normalize(out+Vector3::Up*0.8f),Vector3::Cross(out,Vector3::Up),0.01f,0.7f,0.11f,0.02f,
                        fern*(0.85f+0.3f*std::sin(static_cast<float>(k*6+f))));
            }
        }
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
    // the canopy's glass: dirty and crazed, faintly green-grey over the cockpit (premultiplied),
    // a glint where a pane turns to the sun
    {
        auto& panes=clear.builder.mesh;
        Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
        for(std::size_t tri=0;tri<panes.size()/3U;++tri) {
            const Vector3 a=panes[tri*3U].Position,bb=panes[tri*3U+1U].Position,c=panes[tri*3U+2U].Position;
            Vector3 normal=Vector3::Cross(bb-a,c-a);
            if(normal.LengthSquared()<1e-14f) continue;
            normal.Normalize();
            const float glint=std::clamp((std::abs(Vector3::Dot(normal,sun))-0.75f)/0.2f,0.0f,1.0f);
            const float alpha=0.32f+0.4f*glint;
            for(std::size_t v=0;v<3U;++v) {
                const Color lit=panes[tri*3U+v].Color;
                const auto channel=[&](const int value) {return static_cast<int>(std::lround(std::min(255.0f,static_cast<float>(value)*alpha+255.0f*0.3f*glint)));};
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
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"plane-wreck",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: plane wreck sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a Second World War fighter's wreck lies nose-down in a clearing at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<"), its tail broken off and a wing torn away; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void PlaneWreckRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>320*320
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
        // the canopy's glass over the cockpit, without hiding it
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

void PlaneWreckRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"plane-wreck");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: plane wreck sun shadow caster unavailable: "<<error.what()<<'\n';
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
