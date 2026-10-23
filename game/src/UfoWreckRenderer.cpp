#include "IslandSimulator/UfoWreckRenderer.hpp"
#include "IslandSimulator/LandmarkShapes.hpp"
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
#include <span>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// Dull silver hull (pigment past full: the build lights faces 0.46 + 0.54 n.s), its scorched and
// dark parts, the dome's cracked green glass, the burnt earth and the soil heaped by the furrow.
const Vector3 Hull(178,184,192),Scorch(48,42,40),Glass(70,124,128),Burnt(58,46,38),Soil(98,78,58),Dark(30,30,34);

float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

}

UfoWreckRenderer::UfoWreckRenderer(GraphicsDevice& device,const UfoWreckPlacement& placement,
                                   const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    using K=UfoWreckPlacement;
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(60,50,42);b.stainHeight=0.3f;b.stainAmount=0.45f;
    b.tintPerVertex=true;
    const Vector3 ahead(placement.headingX,0.0f,placement.headingZ),across(placement.headingZ,0.0f,-placement.headingX);
    const float cp=std::cos(K::Tilt),sp=std::sin(K::Tilt),cr=std::cos(K::Roll),sr=std::sin(K::Roll);
    constexpr float Lift=0.55f;
    // a point of the hull's own frame (forward, up from its underside, right) in the world: the
    // front pitched down, the whole rolled a little, set down on the ground at its middle
    const auto H=[&](const float f,const float u,const float r) {
        const float f1=f*cp+u*sp,u1=u*cp-f*sp;
        const float r2=r*cr+u1*sr,u2=u1*cr-r*sr;
        return Vector3(placement.x,placement.groundY+Lift,placement.z)+ahead*f1+across*r2+Vector3::Up*u2;
    };
    // a point of the plan on the ground, `u` metres up
    const auto G=[&](const float f,const float r,const float u=0.0f) {
        const auto p=placement.At(f,r);
        return Vector3(p[0],groundAt(p[0],p[1])+u,p[1]);
    };

    // The hull: rings of a profile (radius, height) round its axis, the metal dented at the rim,
    // burnt on the side it slid on, torn open on one side.
    {
        constexpr int Ring=28;
        const std::vector<std::array<float,2>> profile{{{0.9f,0.0f},{2.4f,0.1f},{3.9f,0.4f},{4.9f,0.85f},{5.3f,1.3f},
            {5.05f,1.7f},{4.2f,2.0f},{3.0f,2.3f},{2.1f,2.45f}}};
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(std::size_t i=0;i<profile.size();++i) {
            std::vector<Vector3> ring;
            for(int k=0;k<Ring;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Ring;
                float radius=profile[i][0];
                const float dent=Hash(static_cast<std::uint32_t>(k)*977U+static_cast<std::uint32_t>(i)*131U);
                if(i>=2&&i<=6) radius*=1.0f-0.045f*dent*static_cast<float>(i-1)/5.0f;
                // the gash torn in the rim at the back left
                if(i>=3&&i<=6&&k>=18&&k<=21) radius*=0.74f+0.06f*static_cast<float>(std::abs(k-19.5f)>1.0f);
                float u=profile[i][1];
                if(k>=3&&k<=6&&i>=3&&i<=5) u-=0.25f*dent;    // crumpled where it struck
                ring.push_back(H(std::cos(a)*radius,u,std::sin(a)*radius));
            }
            rings.push_back(ring);centres.push_back(H(0.0f,profile[i][1],0.0f));
        }
        b.tint=[&](const Vector3& pigment,const Vector3&,const Vector3& p) {
            if(pigment.X<100.0f) return pigment;
            // burnt on the leading side toward the underside, streaked with soot along the rim
            const Vector3 rel=p-Vector3(placement.x,0.0f,placement.z);
            const float lead=std::clamp((rel.X*ahead.X+rel.Z*ahead.Z)/(K::HullRadius*0.9f),0.0f,1.0f);
            const float low=std::clamp(1.0f-(p.Y-placement.groundY)/2.6f,0.0f,1.0f);
            const float streak=0.5f+0.5f*std::sin(p.X*3.1f+p.Z*2.3f)*std::sin(p.Z*4.1f-p.X*1.7f);
            return Vector3::Lerp(pigment*(0.9f+0.12f*streak),Scorch,std::clamp(lead*lead*0.5f+low*0.35f*lead+0.12f*streak,0.0f,0.85f));
        };
        b.Loft(rings,centres,[&](const std::size_t i,const std::size_t k) {
            if(i>=3&&i<=5&&k>=18&&k<=21) return Dark;   // the dark inside the gash
            return Hull*(((k+i)%2U==0U)?1.0f:0.93f)*(0.94f+0.1f*Hash(static_cast<std::uint32_t>(i*53U+k*7U)));
        });
        b.tint=nullptr;
        // the panel seams: thin dark ribs down the hull at every fourth segment
        for(int k=0;k<Ring;k+=4) {
            if(k>=16&&k<=22) continue;
            const float a=2.0f*Pi*static_cast<float>(k)/Ring,ca=std::cos(a),sa=std::sin(a);
            std::vector<Vector3> rib;std::vector<float> radii;
            for(const std::size_t i:{2U,3U,4U,5U,6U}) {
                const float radius=profile[i][0]*1.012f;
                rib.push_back(H(ca*radius,profile[i][1],sa*radius));radii.push_back(0.025f);
            }
            b.Tube(rib,radii,5,Dark*1.3f);
        }
    }
    // the dome: green glass, its apex cracked, fallen out at one side
    {
        constexpr int Ring=20;
        const std::vector<std::array<float,2>> profile{{{2.0f,2.4f},{1.9f,2.75f},{1.6f,3.1f},{1.1f,3.4f},{0.55f,3.58f},{0.0f,3.62f}}};
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(const auto& level:profile) {
            std::vector<Vector3> ring;
            for(int k=0;k<Ring;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Ring;
                ring.push_back(H(std::cos(a)*level[0]*1.0f,level[1],std::sin(a)*level[0]));
            }
            rings.push_back(ring);centres.push_back(H(0.0f,level[1],0.0f));
        }
        b.Loft(rings,centres,[&](const std::size_t i,const std::size_t k) {
            const float crack=Hash(static_cast<std::uint32_t>(i*37U+k*11U+5U));
            if(k>=12&&k<=15&&i>=2) return Dark*0.8f;   // the pane gone, the inside dark
            return Glass*(0.85f+0.3f*crack)*(crack>0.86f?0.55f:1.0f);
        },true,false);
        // cracks: bright splinters across the glass
        for(int k=0;k<7;++k) {
            const float a=2.0f*Pi*Hash(static_cast<std::uint32_t>(k)*733U+1U);
            const float r0=0.2f+0.5f*Hash(static_cast<std::uint32_t>(k)*733U+2U);
            b.Tube({H(std::cos(a)*r0,3.58f-r0*0.6f,std::sin(a)*r0),H(std::cos(a+0.25f)*(r0+0.8f),3.3f-r0*0.5f,std::sin(a+0.25f)*(r0+0.8f))},
                   {0.012f,0.01f},4,Vector3(204,222,226));
        }
    }
    // the portholes round the rim: dark cyan, a few smashed to black, a few lit pale
    for(int k=0;k<14;++k) {
        const float a=2.0f*Pi*(static_cast<float>(k)+0.5f)/14.0f;
        if(a>2.0f*Pi*18.0f/28.0f&&a<2.0f*Pi*22.0f/28.0f) continue;   // torn off with the rim
        const float ca=std::cos(a),sa=std::sin(a);
        const Vector3 centre=H(ca*5.16f,1.4f,sa*5.16f),out=Vector3::Normalize(H(ca,1.4f,sa)-H(0.0f,1.4f,0.0f));
        const float luck=Hash(static_cast<std::uint32_t>(k)*997U+11U);
        const Vector3 colour=luck<0.2f?Dark:luck>0.8f?Vector3(150,206,204):Vector3(46,102,110);
        b.Box(centre,Vector3::Normalize(Vector3::Cross(Vector3::Up,out)),Vector3::Up,out,0.2f,0.13f,0.05f,colour,colour);
    }
    // a landing strut bent out from the underside, its pad splayed on the ground
    {
        const Vector3 top=H(-2.7f,0.3f,-2.9f),knee=H(-3.5f,-0.4f,-3.5f),foot=H(-4.4f,-0.5f,-4.0f);
        b.Tube({top,knee,foot},{0.17f,0.13f,0.10f},8,Hull*0.9f);
        b.Tube({foot,foot+Vector3(0.0f,-0.05f,0.0f)},{0.5f,0.52f},10,Hull*0.8f,0.2f);
    }

    // (only the wreck itself stops the quad: it drives over the furrow, the skin and the cables)
    const std::size_t wreckVertices=b.builder.mesh.size();
    // The furrow ploughed behind it, its soil heaped on both sides, and the burnt earth round the
    // hull (laid just over the drawn ground).
    b.tint=nullptr;
    {
        const auto centreline=[&](const float f) {return 1.4f*std::sin(f*0.17f)+0.6f*std::sin(f*0.41f+1.0f);};
        const auto width=[&](const float f) {return 1.4f+1.1f*(1.0f-(-f-3.0f)/(K::FurrowLength-3.0f));};
        const float from=-3.0f,to=-K::FurrowLength;
        constexpr int Count=28,Strips=6;
        for(int k=0;k<Count;++k) {
            const float f0=from+(to-from)*static_cast<float>(k)/Count,f1=from+(to-from)*static_cast<float>(k+1)/Count;
            const float c0=centreline(f0),c1=centreline(f1),w0=width(f0),w1=width(f1);
            const float t=static_cast<float>(k)/Count;
            b.inside=G(f0,c0,-2.0f);
            for(int j=0;j<Strips;++j) {
                const float a0=-1.0f+2.0f*static_cast<float>(j)/Strips,a1=-1.0f+2.0f*static_cast<float>(j+1)/Strips;
                const float mid=0.5f*(a0+a1);
                const Vector3 colour=Vector3::Lerp(Burnt*0.8f,Vector3(92,74,56),t)
                    *(0.82f+0.3f*Hash(static_cast<std::uint32_t>(k*Strips+j)*31U+7U))*(1.0f-0.2f*mid*mid);
                b.Quad(G(f0,c0+w0*a0,0.06f),G(f1,c1+w1*a0,0.06f),G(f1,c1+w1*a1,0.06f),G(f0,c0+w0*a1,0.06f),colour);
            }
            for(const float side:{-1.0f,1.0f}) {
                const float e0=w0*side*1.45f,e1=w1*side*1.45f;
                b.Tube({G(f0,c0+e0,0.05f),G(f1,c1+e1,0.05f)},{0.32f+0.1f*Hash(static_cast<std::uint32_t>(k)*7U+3U),0.3f},7,
                       Soil*(0.85f+0.2f*Hash(static_cast<std::uint32_t>(k)*13U)),0.55f);
            }
        }
        // the burnt apron round the hull, a metre grid laid over the drawn ground
        for(int gx=-9;gx<9;++gx) for(int gz=-9;gz<9;++gz) {
            const float cx=static_cast<float>(gx)+0.5f,cz=static_cast<float>(gz)+0.5f;
            const float angle=std::atan2(cz,cx);
            const float edge=6.8f+1.2f*Hash(static_cast<std::uint32_t>(std::floor((angle+Pi)*26.0f/(2.0f*Pi))+1.0f)*91U);
            if(std::hypot(cx,cz)>edge) continue;
            const Vector3 colour=Burnt*(0.8f+0.3f*Hash(static_cast<std::uint32_t>((gx+20)*64+(gz+20))*17U));
            b.inside=G(0.0f,0.0f,-3.0f);
            b.Quad(G(static_cast<float>(gx),static_cast<float>(gz),0.06f),G(static_cast<float>(gx+1),static_cast<float>(gz),0.06f),
                   G(static_cast<float>(gx+1),static_cast<float>(gz+1),0.06f),G(static_cast<float>(gx),static_cast<float>(gz+1),0.06f),colour);
        }
    }
    // Its skin scattered: torn plates (the larger ones where the alien picks over them), slivers
    // along the furrow and twisted cables.
    b.tint=[&](const Vector3& pigment,const Vector3&,const Vector3& p) {
        if(pigment.X<100.0f) return pigment;
        return pigment*(0.82f+0.2f*(0.5f+0.5f*std::sin(p.X*6.1f+p.Z*4.7f)));
    };
    for(std::size_t k=0;k<K::DebrisCount;++k) {
        const auto spot=K::DebrisSpot(k);
        const float spin=Hash(static_cast<std::uint32_t>(k)*409U+3U)*Pi;
        const Vector3 along(std::cos(spin),0.0f,std::sin(spin)),side(-along.Z,0.0f,along.X);
        const Vector3 at=G(spot[0],spot[1]);
        const Vector3 tilt=Vector3::Normalize(Vector3::Up+side*(0.16f*(Hash(static_cast<std::uint32_t>(k)*61U)-0.5f)));
        b.Box(at+Vector3::Up*0.07f,along,tilt,Vector3::Cross(along,tilt),0.7f+0.2f*Hash(static_cast<std::uint32_t>(k)*17U),0.03f,0.5f,Hull*0.95f,Hull);
        // the torn edge, a darker rib on one side
        b.Box(at+Vector3::Up*0.09f+side*0.45f,along,Vector3::Up,side,0.7f,0.04f,0.03f,Dark*1.5f,Dark*1.5f);
    }
    for(int k=0;k<26;++k) {
        const auto seed=static_cast<std::uint32_t>(k)*2654435761U+0x77U;
        const float f=-4.0f-13.0f*Hash(seed),r=(Hash(seed^0x5U)-0.5f)*9.0f;
        const float size=0.1f+0.3f*Hash(seed^0x9U)*Hash(seed^0x9U);
        const float spin=Hash(seed^0x3U)*Pi;
        const Vector3 along(std::cos(spin),0.0f,std::sin(spin));
        b.Box(G(f,r,size*0.15f),along,Vector3::Up,Vector3(-along.Z,0.0f,along.X),size*1.6f,size*0.12f,size,Hull*(0.85f+0.2f*Hash(seed^0x11U)),Hull);
    }
    b.tint=nullptr;
    for(int k=0;k<4;++k) {
        const auto seed=static_cast<std::uint32_t>(k)*40503U+0x91U;
        const float f=-1.0f-6.0f*Hash(seed),r=3.0f+3.0f*Hash(seed^0x7U);
        const Vector3 a=G(f,r,0.04f),c=G(f-1.4f,r+1.2f*(Hash(seed^0x9U)-0.5f),0.04f);
        b.Tube({a,(a+c)*0.5f+Vector3::Up*0.3f+across*0.4f,c},{0.025f,0.025f,0.025f},5,k%2==0?Vector3(180,90,40):Dark*1.4f);
    }

    auto& mesh=b.builder.mesh;
    landmark_shapes::Note("ufo",std::span(mesh).first(wreckVertices));
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
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"ufo",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: ufo sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a crashed flying saucer lies in a clearing at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<") at the end of its scorched furrow; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void UfoWreckRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
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

void UfoWreckRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"ufo");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: ufo sun shadow caster unavailable: "<<error.what()<<'\n';
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
