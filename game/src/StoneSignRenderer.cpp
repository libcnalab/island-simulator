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
    // an irregular flattened stone: rings at four heights, each corner pushed in or out, sunk to
    // some forty per cent of its height
    const auto stoneAt=[&](const float x,const float z,const float r,const float yaw,const Vector3& tone,const std::uint32_t index) {
        constexpr int Segments=7;
        constexpr std::array<float,4> Heights{{-0.55f,0.0f,0.45f,0.80f}},Widths{{0.80f,1.0f,0.80f,0.42f}};
        const float h=r*(0.55f+0.2f*Hash(index*5U+1U));
        const float y=groundAt(x,z)+h*0.15f;
        const float stretch=0.8f+0.4f*Hash(index*5U+2U);
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(std::size_t level=0;level<Heights.size();++level) {
            std::vector<Vector3> ring;
            for(int k=0;k<Segments;++k) {
                const float a=yaw+2.0f*Pi*static_cast<float>(k)/Segments;
                const float bump=0.82f+0.30f*Hash(index*97U+static_cast<std::uint32_t>(level*Segments+static_cast<std::size_t>(k)));
                const float w=r*Widths[level]*bump;
                ring.emplace_back(x+std::cos(a)*w*stretch,y+Heights[level]*h,z+std::sin(a)*w);
            }
            rings.push_back(ring);centres.emplace_back(x,y+Heights[level]*h,z);
        }
        centres.front().Y-=0.02f;centres.back().Y+=h*0.12f;
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return tone;});
    };
    std::uint32_t index=0;
    for(const SignStone& stone:placement.Stones()) {
        ++index;
        stoneAt(stone.x,stone.z,stone.radius,stone.yaw,Tones[stone.tone%Tones.size()]*(0.9f+0.2f*Hash(index*31U)),index);
        ++stones_;
    }
    // past the sign's end: the signal fire stacked ready, driftwood and branches leaning to a
    // peak and green palm fronds laid over them for smoke, coconut husks for tinder at its foot;
    // and the ring of a fire burnt out, its charcoal and grey ash, the ends of sticks it did not
    // finish
    {
        const Vector3 drift(178,170,156),bark(112,88,66),charred(46,42,40),ash(120,118,112),frond(80,118,56),husk(122,92,60);
        const auto at=[&](const std::array<float,2>& p) {return Vector3(p[0],groundAt(p[0],p[1]),p[1]);};
        // a crooked stick from `from` to `to`: bent at random along it, tapering
        const auto stick=[&](const Vector3& from,const Vector3& to,const float r0,const float r1,const Vector3& tone,
                             const std::uint32_t seed,const float crook) {
            Vector3 along=to-from;const float length=along.Length();along/=length;
            Vector3 side=Vector3::Cross(along,Vector3::Up);
            if(side.LengthSquared()<1e-6f) side=Vector3::UnitX;
            side.Normalize();
            const Vector3 other=Vector3::Cross(side,along);
            std::vector<Vector3> points;std::vector<float> radii;
            constexpr int Joints=5;
            // one gentle bow the whole way along, and a little unevenness on it
            const float bowSide=2.0f*Hash(seed*7U)-1.0f,bowOther=2.0f*Hash(seed*13U)-1.0f;
            for(int j=0;j<=Joints;++j) {
                const float t=static_cast<float>(j)/Joints;
                const float arc=std::sin(Pi*t)*crook*length;
                const float wobble=j==0||j==Joints?0.0f:0.012f*length;
                points.push_back(from+along*(length*t)
                                 +side*(arc*bowSide+wobble*(Hash(seed*23U+static_cast<std::uint32_t>(j))-0.5f))
                                 +other*(arc*bowOther*0.6f+wobble*(Hash(seed*29U+static_cast<std::uint32_t>(j))-0.5f)));
                radii.push_back((r0+(r1-r0)*t)*(0.9f+0.2f*Hash(seed*17U+static_cast<std::uint32_t>(j))));
            }
            b.Tube(points,radii,7,tone);
            return points;
        };
        const Vector3 fire=at(placement.SignalFire());
        const Vector3 peak=fire+Vector3::Up*1.7f;
        for(int k=0;k<12;++k) {
            const auto seed=static_cast<std::uint32_t>(k);
            const float a=2.0f*Pi*static_cast<float>(k)/12.0f+0.2f*Hash(seed*11U);
            const float r=0.75f+0.15f*Hash(seed*13U);
            const Vector3 foot=fire+Vector3(std::cos(a)*r,-0.05f,std::sin(a)*r);
            // crossing at the peak and standing on past it, as the poles of a stack do
            Vector3 lean=peak+Vector3(std::cos(a)*0.1f,0.15f*Hash(seed*17U),std::sin(a)*0.1f)-foot;lean.Normalize();
            const Vector3 top=peak+lean*(0.15f+0.35f*Hash(seed*37U))+Vector3(std::cos(a)*0.1f,0.0f,std::sin(a)*0.1f);
            // most of it the sea's driftwood, bleached silver-grey; the rest fresh branches in their bark
            const bool fresh=Hash(seed*41U)<0.4f;
            const Vector3 tone=(fresh?bark:drift)*(0.82f+0.28f*Hash(seed*19U));
            const auto points=stick(foot,top,0.065f+0.02f*Hash(seed*43U),0.03f,tone,seed+100U,0.04f);
            // the stub of a snapped side branch on the fresh ones
            if(fresh) {
                const Vector3 root=points[2];
                Vector3 out=Vector3::Normalize(Vector3(std::cos(a+0.8f),0.5f,std::sin(a+0.8f)));
                b.Tube({root,root+out*0.16f},{0.025f,0.014f},5,tone*0.92f);
            }
        }
        // coconut husks for tinder, heaped round the stack's foot inside
        for(int k=0;k<6;++k) {
            const auto seed=static_cast<std::uint32_t>(k)+300U;
            const float a=2.0f*Pi*Hash(seed*3U),r=0.25f+0.35f*Hash(seed*5U);
            const Vector3 c=fire+Vector3(std::cos(a)*r,0.0f,std::sin(a)*r);
            stoneAt(c.X,c.Z,0.1f+0.04f*Hash(seed*7U),a,husk*(0.85f+0.3f*Hash(seed*11U)),seed);
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
        // the ring's stones, blackened by the fire on their inner faces (sooted all over here)
        for(int k=0;k<9;++k) {
            const auto seed=static_cast<std::uint32_t>(k)+200U;
            const float a=2.0f*Pi*static_cast<float>(k)/9.0f+0.15f*(Hash(seed*3U)-0.5f);
            const float r=0.62f+0.06f*(Hash(seed*5U)-0.5f);
            stoneAt(pit.X+std::cos(a)*r,pit.Z+std::sin(a)*r,0.15f+0.04f*Hash(seed*7U),a,
                    Tones[static_cast<std::size_t>(k)%4U]*(0.62f+0.12f*Hash(seed*11U)),seed);
        }
        {
            // the ash bed, a low grey mound inside the ring, patchy with charcoal dust
            constexpr int Rim=14;
            b.inside=pit-Vector3::Up;
            for(int k=0;k<Rim;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/Rim,a1=2.0f*Pi*static_cast<float>(k+1)/Rim;
                const float r0=0.5f*(0.85f+0.3f*Hash(static_cast<std::uint32_t>(k)*3U+500U));
                const float r1=0.5f*(0.85f+0.3f*Hash(static_cast<std::uint32_t>((k+1)%Rim)*3U+500U));
                b.Face(pit+Vector3::Up*0.045f,pit+Vector3(std::cos(a0)*r0,0.01f,std::sin(a0)*r0),
                       pit+Vector3(std::cos(a1)*r1,0.01f,std::sin(a1)*r1),
                       Vector3::Lerp(ash,charred,0.35f*Hash(static_cast<std::uint32_t>(k)*7U+600U)));
            }
        }
        // the charcoal: knobbly black lumps in the ash
        for(int k=0;k<7;++k) {
            const auto seed=static_cast<std::uint32_t>(k)+400U;
            const float a=2.0f*Pi*Hash(static_cast<std::uint32_t>(k)*29U),r=0.3f*Hash(static_cast<std::uint32_t>(k)*31U);
            stoneAt(pit.X+std::cos(a)*r,pit.Z+std::sin(a)*r,0.05f+0.03f*Hash(seed*3U),a*3.0f,charred*(0.9f+0.25f*Hash(seed*5U)),seed);
        }
        // the sticks it did not finish: burnt black at the middle, the wood showing at their ends
        for(int k=0;k<3;++k) {
            const auto seed=static_cast<std::uint32_t>(k)+700U;
            const float a=2.0f*Pi*static_cast<float>(k)/3.0f+0.6f;
            const Vector3 out(std::cos(a),0.0f,std::sin(a));
            const Vector3 inner=pit+out*0.12f+Vector3::Up*0.06f,middle=pit+out*0.5f+Vector3::Up*0.1f,end=pit+out*0.95f+Vector3::Up*0.05f;
            stick(inner,middle,0.035f,0.05f,charred*1.3f,seed,0.05f);
            stick(middle,end,0.05f,0.05f,(k==1?bark:drift)*0.7f,seed+10U,0.04f);
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
