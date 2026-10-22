#include "IslandSimulator/BeachLitterRenderer.hpp"
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
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Sides=12;
// The bottle's radius along it, base (0) to the neck's mouth; the cap from `cap` to the end.
struct Station {float u,r;};
struct Shape {std::vector<Station> body;float cap,capRadius,label0,label1;};
const Shape& ShapeOf(const BeachBottle::Kind kind) {
    // a litre and a half of water: grip ribs low on the body, a long shoulder, the neck's ring
    static const Shape water{{{0.0f,0.78f},{0.02f,0.96f},{0.05f,1.0f},{0.20f,1.0f},{0.23f,0.92f},{0.26f,1.0f},
        {0.29f,0.92f},{0.32f,1.0f},{0.60f,1.0f},{0.67f,0.95f},{0.74f,0.80f},{0.81f,0.56f},{0.86f,0.38f},
        {0.885f,0.37f},{0.89f,0.50f},{0.905f,0.50f},{0.91f,0.38f},{0.93f,0.38f}},0.925f,0.42f,0.36f,0.57f};
    static const Shape small{{{0.0f,0.80f},{0.03f,0.97f},{0.06f,1.0f},{0.62f,1.0f},{0.70f,0.92f},{0.78f,0.70f},
        {0.84f,0.50f},{0.865f,0.48f},{0.87f,0.62f},{0.885f,0.62f},{0.89f,0.48f},{0.905f,0.48f}},0.90f,0.53f,0.24f,0.56f};
    // two litres of soda: the five-lobed foot, a straight body, a short neck
    static const Shape soda{{{0.0f,0.70f},{0.03f,0.95f},{0.07f,1.0f},{0.60f,1.0f},{0.68f,0.92f},{0.76f,0.70f},
        {0.83f,0.45f},{0.88f,0.30f},{0.895f,0.30f},{0.90f,0.40f},{0.915f,0.40f},{0.92f,0.30f},{0.945f,0.30f}},
        0.94f,0.34f,0.22f,0.52f};
    switch(kind) {
    case BeachBottle::Kind::Small: return small;
    case BeachBottle::Kind::Soda: return soda;
    default: return water;
    }
}
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
// the plastic (clear, blue, green, frosted by the sun) and how much of what lies behind it each
// wall hides; the labels, sun-faded; the caps
const std::array<Vector3,4> Plastic{{Vector3(226,240,246),Vector3(168,206,238),Vector3(92,178,120),Vector3(230,228,212)}};
constexpr std::array<float,4> PlasticAlpha{{0.16f,0.24f,0.42f,0.50f}};
const std::array<Vector3,4> Labels{{Vector3(204,92,80),Vector3(88,130,196),Vector3(104,172,116),Vector3(236,216,128)}};
const Vector3 Bleached(240,234,218),Stripe(244,240,228);
const std::array<Vector3,4> Caps{{Vector3(56,104,200),Vector3(244,244,238),Vector3(208,56,46),Vector3(62,150,78)}};
}

BeachLitterRenderer::BeachLitterRenderer(GraphicsDevice& device,const std::span<const BeachBottle> bottles,
                                         const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
    Vector3 shadeAway(-sun.X,0.0f,-sun.Z);shadeAway.Normalize();
    // the bottles in groups along the shore, each drawn only near the eye
    std::vector<std::size_t> order(bottles.size());
    for(std::size_t i=0;i<order.size();++i) order[i]=i;
    std::sort(order.begin(),order.end(),[&](const std::size_t a,const std::size_t b) {
        return std::atan2(bottles[a].z,bottles[a].x)<std::atan2(bottles[b].z,bottles[b].x);
    });
    std::vector<std::vector<std::size_t>> members;
    for(const std::size_t index:order) {
        const BeachBottle& bottle=bottles[index];
        if(members.empty()||std::hypot(bottles[members.back().front()].x-bottle.x,
                                        bottles[members.back().front()].z-bottle.z)>24.0f)
            members.emplace_back();
        members.back().push_back(index);
    }
    std::vector<VertexPositionColor> mesh;
    for(const auto& group:members) {
        SculptMeshBuilder solid,clear;
        std::vector<VertexPositionColor> shade;
        std::vector<int> clearAlpha;   // each clear triangle's plastic
        for(const std::size_t index:group) {
            const BeachBottle& bottle=bottles[index];
            const Shape& shape=ShapeOf(bottle.kind);
            const bool crushed=bottle.kind==BeachBottle::Kind::Crushed;
            const float length=bottle.Length(),radius=bottle.Radius();
            const Vector3 heading(std::cos(bottle.yaw),0.0f,std::sin(bottle.yaw));
            // flattened, a crushed bottle lies lower; its axis follows the sand from end to end
            const float lift=radius*(crushed?0.42f:1.0f)*(1.0f-bottle.sink);
            const float baseX=bottle.x-heading.X*length*0.5f,baseZ=bottle.z-heading.Z*length*0.5f;
            const float capX=bottle.x+heading.X*length*0.5f,capZ=bottle.z+heading.Z*length*0.5f;
            const Vector3 base(baseX,groundAt(baseX,baseZ)+lift,baseZ),tip(capX,groundAt(capX,capZ)+lift,capZ);
            Vector3 axis=tip-base;axis.Normalize();
            Vector3 side=Vector3::Cross(axis,Vector3::Up);side.Normalize();
            Vector3 up=Vector3::Cross(side,axis);
            const float roll=crushed?0.0f:bottle.roll;
            const Vector3 s=side*std::cos(roll)+up*std::sin(roll),t=up*std::cos(roll)-side*std::sin(roll);
            const auto seed=static_cast<std::uint32_t>(index)*0x9e3779b9U+0x632be5abU;
            const auto at=[&](const float u,const float a,float r,const int station,const int k) {
                float squash=1.0f;
                if(crushed&&u>0.08f&&u<0.80f) {
                    // trodden flat in the middle, crumpled into creases
                    const float middle=std::sin((u-0.08f)/0.72f*Pi);
                    squash=1.0f-0.62f*middle;
                    r*=1.0f+0.22f*middle*(Hash(seed^static_cast<std::uint32_t>(station*131+k))-0.5f);
                    r*=1.0f+0.25f*middle;
                }
                if(bottle.kind==BeachBottle::Kind::Soda&&u<0.07f)
                    r*=0.82f+0.18f*std::max(0.0f,std::cos(5.0f*a+bottle.roll));
                return base+axis*(u*length)+(s*std::cos(a)+t*(std::sin(a)*squash))*(r*radius);
            };
            // the plastic: lofted from the base to the neck's mouth, open there
            {
                std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
                int station=0;
                for(const Station& st:shape.body) {
                    std::vector<Vector3> ring;
                    for(int k=0;k<Sides;++k)
                        ring.push_back(at(st.u,2.0f*Pi*static_cast<float>(k)/Sides,st.r,station,k));
                    rings.push_back(ring);centres.push_back(base+axis*(st.u*length));
                    ++station;
                }
                const std::size_t before=clear.builder.mesh.size();
                clear.Loft(rings,centres,[&](std::size_t,std::size_t) {return Plastic[bottle.tint];},true,false);
                clearAlpha.insert(clearAlpha.end(),(clear.builder.mesh.size()-before)/3U,bottle.tint);
            }
            // the cap, ribbed round its rim, closed on top
            if(bottle.cap) {
                std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
                for(const float u:{shape.cap,shape.cap+0.01f,0.985f,1.0f}) {
                    std::vector<Vector3> ring;
                    for(int k=0;k<Sides;++k) {
                        const float r=shape.capRadius*(u>=0.985f?0.92f:(k%2==0?1.0f:0.94f));
                        ring.push_back(at(u,2.0f*Pi*static_cast<float>(k)/Sides,r,0,k));
                    }
                    rings.push_back(ring);centres.push_back(base+axis*(u*length));
                }
                solid.Loft(rings,centres,[&](std::size_t,std::size_t) {return Caps[bottle.capColour];});
            }
            // what is left of the label: a band round the body, torn away round part of it
            if(bottle.label>0.0f) {
                const auto covered=static_cast<int>(std::lround(bottle.label*Sides));
                const int start=static_cast<int>(Hash(seed^0x51ed270bU)*Sides);
                const Vector3 colour=Vector3::Lerp(Labels[bottle.labelColour],Bleached,0.35f);
                const std::array<float,4> bands{{shape.label0,shape.label0+(shape.label1-shape.label0)*0.38f,
                                                 shape.label0+(shape.label1-shape.label0)*0.62f,shape.label1}};
                for(int band=0;band<3;++band) {
                    const float u0=bands[static_cast<std::size_t>(band)],u1=bands[static_cast<std::size_t>(band+1)];
                    solid.inside=base+axis*((u0+u1)*0.5f*length);
                    const Vector3 pigment=band==1&&bottle.labelColour!=3?Vector3::Lerp(Stripe,colour,0.15f):colour;
                    for(int c=0;c<covered;++c) {
                        const int k=(start+c)%Sides;
                        const float a0=2.0f*Pi*static_cast<float>(k)/Sides,a1=2.0f*Pi*static_cast<float>(k+1)/Sides;
                        solid.Quad(at(u0,a0,1.012f,3,k),at(u1,a0,1.012f,3,k),at(u1,a1,1.012f,3,k+1),at(u0,a1,1.012f,3,k+1),pigment);
                    }
                }
            }
            // a soft shade on the sand under it, a little away from the sun
            {
                const Vector3 centre=Vector3(bottle.x,0.0f,bottle.z)+shadeAway*(radius*1.0f);
                const Vector3 across(-heading.Z,0.0f,heading.X);
                const auto ground=[&](const Vector3& p) {return Vector3(p.X,groundAt(p.X,p.Z)+0.012f,p.Z);};
                const Color dark(0,0,0,crushed?72:104),none(0,0,0,0);
                constexpr int Rim=14;
                for(int k=0;k<Rim;++k) {
                    const float a0=2.0f*Pi*static_cast<float>(k)/Rim,a1=2.0f*Pi*static_cast<float>(k+1)/Rim;
                    const Vector3 r0=centre+heading*(std::cos(a0)*length*0.62f)+across*(std::sin(a0)*radius*2.3f);
                    const Vector3 r1=centre+heading*(std::cos(a1)*length*0.62f)+across*(std::sin(a1)*radius*2.3f);
                    shade.emplace_back(ground(centre),dark);shade.emplace_back(ground(r0),none);shade.emplace_back(ground(r1),none);
                }
            }
        }
        // the plastic's own colour shows faintly over what lies behind: a wall hides its share of
        // it (premultiplied), more where it turns to the sun and the glint runs along it
        auto& glass=clear.builder.mesh;
        for(std::size_t tri=0;tri<glass.size()/3U;++tri) {
            const Vector3 a=glass[tri*3U].Position,b=glass[tri*3U+1U].Position,c=glass[tri*3U+2U].Position;
            Vector3 normal=Vector3::Cross(b-a,c-a);
            if(normal.LengthSquared()<1e-14f) continue;
            normal.Normalize();
            const float glint=std::clamp((Vector3::Dot(normal,sun)-0.74f)/0.14f,0.0f,1.0f);
            const auto plastic=static_cast<std::size_t>(clearAlpha[tri]);
            const float alpha=std::min(0.92f,PlasticAlpha[plastic]+0.50f*glint);
            for(std::size_t v=0;v<3U;++v) {
                const Color lit=glass[tri*3U+v].Color;
                const auto channel=[&](const int value) {
                    const float bright=static_cast<float>(value)+(255.0f-static_cast<float>(value))*0.85f*glint;
                    return static_cast<int>(std::lround(bright*alpha));
                };
                glass[tri*3U+v].Color=Color(channel(lit.getRProperty()),channel(lit.getGProperty()),
                                            channel(lit.getBProperty()),static_cast<int>(std::lround(alpha*255.0f)));
            }
        }
        Group drawn;
        Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
        const auto take=[&](const std::vector<VertexPositionColor>& part,int& start,int& count) {
            start=static_cast<int>(mesh.size());count=static_cast<int>(part.size()/3U);
            for(const auto& v:part) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
            mesh.insert(mesh.end(),part.begin(),part.end());
        };
        take(solid.builder.mesh,drawn.solidStart,drawn.solidTriangles);
        take(shade,drawn.shadeStart,drawn.shadeTriangles);
        take(glass,drawn.clearStart,drawn.clearTriangles);
        drawn.bounds=BoundingBox(low-Vector3(0.1f,0.1f,0.1f),high+Vector3(0.1f,0.1f,0.1f));
        drawn.centre=(low+high)*0.5f;
        groups_.push_back(drawn);
    }
    triangles_=static_cast<int>(mesh.size()/3U);
    if(!mesh.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
            static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
        vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    }
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"beach-litter",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: beach litter sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: washed-up plastic: "<<bottles.size()<<" PET bottles along the strandline in "
        <<groups_.size()<<" stretches of beach; "<<triangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void BeachLitterRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!vertices_) return;
    const BoundingFrustum frustum(view*projection);
    std::vector<const Group*> visible;
    for(const Group& group:groups_)
        if((group.centre-camera).LengthSquared()<(DrawDistance+14.0f)*(DrawDistance+14.0f)
            &&frustum.Contains(group.bounds)!=ContainmentType::Disjoint) visible.push_back(&group);
    if(visible.empty()) return;
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    const auto apply=[&] {
        if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
        else {
            effect_->setWorldProperty(Matrix::getIdentityProperty());
            effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
        }
    };
    const auto pass=[&](const auto start,const auto count) {
        for(const Group* group:visible) {
            if(group->*count==0) continue;
            device_.DrawPrimitives(PrimitiveType::TriangleList,group->*start,group->*count);++drawCalls_;
        }
    };
    // the caps and the labels; the shade on the sand; the bottles' far walls, then their near ones
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    apply();pass(&Group::solidStart,&Group::solidTriangles);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    apply();pass(&Group::shadeStart,&Group::shadeTriangles);
    device_.setRasterizerStateProperty(RasterizerState::CullClockwise);
    apply();pass(&Group::clearStart,&Group::clearTriangles);
    device_.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    apply();pass(&Group::clearStart,&Group::clearTriangles);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
}
}
