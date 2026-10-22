#include "IslandSimulator/StoneSignRenderer.hpp"
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
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// dark volcanic stones and a white lump of dead coral (pigment past full for the build's light)
const std::array<Vector3,5> Tones{{Vector3(118,114,108),Vector3(96,93,90),Vector3(132,124,112),Vector3(84,82,82),
                                   Vector3(300,292,272)}};
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

StoneSignRenderer::StoneSignRenderer(GraphicsDevice& device,const StoneSignPlacement& placement,
                                     const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(150,132,100);b.stainHeight=0.05f;b.stainAmount=0.5f;
    std::uint32_t index=0;
    for(const SignStone& stone:placement.Stones()) {
        ++index;
        // an irregular flattened stone: rings at four heights, each corner pushed in or out
        constexpr int Segments=7;
        constexpr std::array<float,4> Heights{{-0.55f,0.0f,0.45f,0.80f}},Widths{{0.80f,1.0f,0.80f,0.42f}};
        const float r=stone.radius,h=r*(0.55f+0.2f*Hash(index*5U+1U));
        const float y=groundAt(stone.x,stone.z)+h*0.15f;   // sunk to some forty per cent of its height
        const float stretch=0.8f+0.4f*Hash(index*5U+2U);
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(std::size_t level=0;level<Heights.size();++level) {
            std::vector<Vector3> ring;
            for(int k=0;k<Segments;++k) {
                const float a=stone.yaw+2.0f*Pi*static_cast<float>(k)/Segments;
                const float bump=0.82f+0.30f*Hash(index*97U+static_cast<std::uint32_t>(level*Segments+static_cast<std::size_t>(k)));
                const float w=r*Widths[level]*bump;
                ring.emplace_back(stone.x+std::cos(a)*w*stretch,y+Heights[level]*h,stone.z+std::sin(a)*w);
            }
            rings.push_back(ring);centres.emplace_back(stone.x,y+Heights[level]*h,stone.z);
        }
        centres.front().Y-=0.02f;centres.back().Y+=h*0.12f;
        const Vector3 tone=Tones[stone.tone%Tones.size()]*(0.9f+0.2f*Hash(index*31U));
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return tone;});
        ++stones_;
    }
    // past the sign's end: the signal fire stacked ready, driftwood poles leaning to a peak and
    // green palm fronds laid over them for smoke, and the ring of a fire burnt out, its charcoal
    // and grey ash, the ends of sticks it did not finish
    {
        const Vector3 drift(178,170,156),charred(46,42,40),ash(120,118,112),frond(80,118,56);
        const auto at=[&](const std::array<float,2>& p) {return Vector3(p[0],groundAt(p[0],p[1]),p[1]);};
        const Vector3 fire=at(placement.SignalFire());
        const Vector3 peak=fire+Vector3::Up*1.7f;
        for(int k=0;k<12;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/12.0f+0.2f*Hash(static_cast<std::uint32_t>(k)*11U);
            const float r=0.75f+0.15f*Hash(static_cast<std::uint32_t>(k)*13U);
            const Vector3 foot=fire+Vector3(std::cos(a)*r,-0.05f,std::sin(a)*r);
            const Vector3 top=peak+Vector3(std::cos(a)*0.12f,0.15f*Hash(static_cast<std::uint32_t>(k)*17U),std::sin(a)*0.12f);
            b.Tube({foot,(foot+top)*0.5f,top},{0.06f,0.05f,0.035f},6,drift*(0.85f+0.25f*Hash(static_cast<std::uint32_t>(k)*19U)));
        }
        // the fronds: a rib arching down over the poles, leaflets in pairs along it
        for(int k=0;k<5;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/5.0f+0.3f;
            const Vector3 out(std::cos(a),0.0f,std::sin(a)),across(-std::sin(a),0.0f,std::cos(a));
            const Vector3 tint=frond*(0.85f+0.3f*Hash(static_cast<std::uint32_t>(k)*23U));
            std::vector<Vector3> rib;std::vector<float> radii;
            constexpr int Joints=8;
            for(int j=0;j<=Joints;++j) {
                const float t=static_cast<float>(j)/Joints;
                rib.push_back(peak-Vector3::Up*0.05f+out*(1.5f*t)-Vector3::Up*(1.2f*t*t)+across*(0.12f*std::sin(t*2.0f+static_cast<float>(k))));
                radii.push_back(0.02f-0.012f*t);
            }
            b.Tube(rib,radii,4,tint*0.8f);
            for(int j=1;j<Joints;++j) {
                const Vector3 at=rib[static_cast<std::size_t>(j)];
                Vector3 along=rib[static_cast<std::size_t>(j+1)]-rib[static_cast<std::size_t>(j-1)];along.Normalize();
                const float length=0.42f*std::sin(Pi*(static_cast<float>(j)+0.5f)/Joints)+0.1f;
                for(const float s:{-1.0f,1.0f}) for(const float shift:{-0.06f,0.06f}) {
                    const Vector3 base=at+along*shift;
                    const Vector3 dir=Vector3::Normalize(across*s+along*0.55f-Vector3::Up*0.45f);
                    const Vector3 width=Vector3::Normalize(Vector3::Cross(dir,Vector3::Up))*0.025f;
                    const Vector3 tip=base+dir*length;
                    b.inside=base-Vector3::Up*0.3f;
                    b.Quad(base-width,base+width,tip+width*0.3f,tip-width*0.3f,tint*(0.9f+0.15f*Hash(static_cast<std::uint32_t>(j*7+k))));
                }
            }
        }
        const Vector3 pit=at(placement.BurntFire());
        for(int k=0;k<9;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/9.0f;
            const Vector3 c=pit+Vector3(std::cos(a)*0.62f,0.06f,std::sin(a)*0.62f);
            const Vector3 out(std::cos(a),0.0f,std::sin(a));
            b.Box(c,out,Vector3::Up,Vector3::Cross(out,Vector3::Up),0.13f,0.09f,0.16f,Tones[static_cast<std::size_t>(k)%4U]*0.75f,Tones[static_cast<std::size_t>(k)%4U]*0.7f);
        }
        {
            // the ash bed, a low grey disc inside the ring
            constexpr int Rim=12;
            b.inside=pit-Vector3::Up;
            for(int k=0;k<Rim;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/Rim,a1=2.0f*Pi*static_cast<float>(k+1)/Rim;
                b.Face(pit+Vector3::Up*0.035f,pit+Vector3(std::cos(a0)*0.5f,0.01f,std::sin(a0)*0.5f),
                       pit+Vector3(std::cos(a1)*0.5f,0.01f,std::sin(a1)*0.5f),ash);
            }
        }
        for(int k=0;k<7;++k) {
            const float a=2.0f*Pi*Hash(static_cast<std::uint32_t>(k)*29U),r=0.3f*Hash(static_cast<std::uint32_t>(k)*31U);
            const Vector3 c=pit+Vector3(std::cos(a)*r,0.05f,std::sin(a)*r);
            const Vector3 dir(std::cos(a*3.0f),0.0f,std::sin(a*3.0f));
            b.Box(c,dir,Vector3::Up,Vector3::Cross(dir,Vector3::Up),0.08f,0.035f,0.05f,charred,charred*1.1f);
        }
        for(int k=0;k<3;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/3.0f+0.6f;
            const Vector3 out(std::cos(a),0.0f,std::sin(a));
            b.Tube({pit+out*0.15f+Vector3::Up*0.06f,pit+out*0.55f+Vector3::Up*0.1f,pit+out*0.95f+Vector3::Up*0.05f},
                   {0.045f,0.05f,0.05f},6,charred*1.4f);
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
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"stone-sign",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: stone sign sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a castaway's HELP ME is laid out in "<<stones_<<" stones on the beach at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<"), reading from the sea, a signal fire stacked ready beside it by a fire burnt out; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void StoneSignRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>260*260
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

void StoneSignRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"stone-sign");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: stone sign sun shadow caster unavailable: "<<error.what()<<'\n';
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
