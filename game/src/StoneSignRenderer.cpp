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
        <<"), reading from the sea; "<<triangles_<<" triangles in one draw";
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
