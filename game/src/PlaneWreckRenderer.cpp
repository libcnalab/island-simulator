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
    Disc(232,70,56),Glass(48,62,68),Frame(96,104,82),Engine(78,76,72),Prop(70,66,58),Rubber(40,38,36);
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
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(124,110,84);b.stainHeight=0.6f;b.stainAmount=0.35f;
    // the painted skin: dark green on the faces turned up, grey under, worn through to aluminium
    // in patches and white with corrosion here and there
    b.tint=[](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        if(pigment.X!=Skin.X||pigment.Y!=Skin.Y||pigment.Z!=Skin.Z) return pigment;
        // the green blends into the grey round the sides, so the two halves of a quad never
        // step from one to the other
        const float top=std::clamp((normal.Y+0.25f)/0.7f,0.0f,1.0f);
        Vector3 colour=Vector3::Lerp(Underside,Olive,top*top*(3.0f-2.0f*top));
        const float wear=std::sin(centre.X*3.1f+centre.Z*1.7f)*std::sin(centre.Z*2.3f-centre.Y*4.1f+centre.X*0.7f);
        colour=Vector3::Lerp(colour,Bare,std::clamp((wear-0.30f)*3.0f,0.0f,1.0f)*0.85f);
        const float rot=std::sin(centre.X*11.0f+centre.Y*7.0f)*std::sin(centre.Z*9.0f-centre.X*5.0f);
        colour=Vector3::Lerp(colour,Corroded,std::clamp((rot-0.6f)*3.0f,0.0f,1.0f)*0.6f);
        return colour*(0.94f+0.06f*std::sin(centre.X*5.3f+centre.Z*4.1f));
    };
    const Vector3 heading(placement.headingX,0.0f,placement.headingZ),right(placement.headingZ,0.0f,-placement.headingX);
    // the main piece lies on its belly, nose down a little and rolled onto its right wing
    const Frame3 main=Frame3{Vector3(placement.x,placement.groundY+0.52f,placement.z),heading,right,Vector3::Up}.Turned(0.0f,0.10f,0.07f);
    // the fuselage's radius along it (forward from the wing roots), and a lofted round section
    const std::vector<std::array<float,2>> fuselage{{{4.25f,0.10f},{4.05f,0.36f},{3.85f,0.56f},{3.55f,0.64f},{2.6f,0.65f},
        {1.6f,0.63f},{0.0f,0.59f},{-1.5f,0.49f},{-3.0f,0.33f},{-4.4f,0.17f},{-4.8f,0.08f}}};
    const auto loftFuselage=[&](const Frame3& frame,const float from,const float to,const float shift) {
        constexpr int Ring=18;
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        const int stations=std::max(2,static_cast<int>(std::abs(from-to)/0.3f));
        for(int i=0;i<=stations;++i) {
            const float f=from+(to-from)*static_cast<float>(i)/static_cast<float>(stations);
            const float rr=Profile(fuselage,f);
            std::vector<Vector3> ring;
            for(int k=0;k<Ring;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Ring;
                ring.push_back(frame.At(f-shift,std::cos(a)*rr,std::sin(a)*rr*1.08f));
            }
            rings.push_back(ring);centres.push_back(frame.At(f-shift,0.0f,0.0f));
        }
        b.Loft(rings,centres,[](std::size_t,std::size_t) {return Skin;});
    };
    loftFuselage(main,3.85f,-1.05f,0.0f);
    // the tail section, broken off behind the cockpit, thrown aside and twisted onto its side
    const Frame3 tail=Frame3{main.At(-1.25f,0.85f,-0.12f),heading,right,Vector3::Up}.Turned(0.48f,-0.04f,-0.32f);
    loftFuselage(tail,-1.15f,-4.8f,-1.15f);
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
                    b.inside=main.At((f+f1)*0.5f,0.0f,0.55f);
                    b.Quad(at(f,a0),at(f1,a0),at(f1,a1),at(f,a1),Glass*(1.0f+0.3f*static_cast<float>(k%3)));
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
