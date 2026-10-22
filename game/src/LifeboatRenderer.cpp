#include "IslandSimulator/LifeboatRenderer.hpp"
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
using L=LifeboatPlacement;
constexpr float Pi=std::numbers::pi_v<float>;
// the paint (its colour from where on the hull a face lies), the wood and the fittings
// marker pigments for the hull's three painted strakes: the antifouling, the white topsides and the
// band under the gunwale (the tint gives each its colour and its peeling)
const Vector3 PaintBottom(1,2,3),PaintWhite(1,2,4),PaintBand(1,2,5),White(240,236,224),Wood(134,122,104),Bottom(156,78,60),Band(212,104,60),
    Oar(150,116,80),RingOrange(226,104,52),RingWhite(236,232,222);
float Wave(const float x,const float y) {return std::sin(x*1.7f+std::sin(y*2.3f))*std::sin(y*1.3f-x*0.7f);}
}

LifeboatRenderer::LifeboatRenderer(GraphicsDevice& device,const LifeboatPlacement& boat,const IslandTerrain& terrain,
                                   const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    const Vector3 forward(boat.dirX,0.0f,boat.dirZ),side(-boat.dirZ,0.0f,boat.dirX);
    const float baseY=groundAt(boat.x,boat.z)-0.12f;
    // the hull's half breadth and depth along it, double-ended, the sheer rising to the ends
    const auto breadth=[](const float x) {return L::HalfBeam*std::pow(std::max(0.0f,1.0f-(2.0f*x/L::Length)*(2.0f*x/L::Length)),0.75f);};
    const auto depth=[](const float x) {return L::HullDepth(x);};
    // the keel's rocker, rising toward the ends (upright), and the stem and stern raked out
    const auto keelUp=[](const float x) {return L::KeelRocker(x);};
    const auto section=[&](const float a,const float t,float& along,float& up,float& across) {
        along=a*(0.80f+0.20f*t);
        up=keelUp(along)+(depth(along)-keelUp(along))*t;
        across=breadth(along)*std::pow(t,0.45f);
    };
    // a point of the upright hull (along, across, up from the keel) turned keel up and rolled
    const auto place=[&](const float along,const float across,const float up) {
        const float h=depth(along)-up;
        const float a=across*std::cos(boat.list)-h*std::sin(boat.list);
        const float v=across*std::sin(boat.list)+h*std::cos(boat.list);
        return Vector3(boat.x,baseY,boat.z)+forward*along+side*a+Vector3::Up*v;
    };
    SculptMeshBuilder b;
    b.tintPerVertex=true;
    b.ground=groundAt;b.stainColour=Vector3(200,182,142);b.stainHeight=0.18f;b.stainAmount=0.6f;
    // the tint is taken corner by corner, so the peeling runs smoothly across the planks; each
    // strake is built apart, so the edges between the paints stay sharp
    b.tint=[&](const Vector3& pigment,const Vector3&,const Vector3& corner) {
        const auto same=[&](const Vector3& other) {return pigment.X==other.X&&pigment.Y==other.Y&&pigment.Z==other.Z;};
        const bool bottom=same(PaintBottom);
        if(!bottom&&!same(PaintWhite)&&!same(PaintBand)) return pigment;
        const Vector3 d=corner-Vector3(boat.x,baseY,boat.z);
        const float along=Vector3::Dot(d,forward);
        Vector3 colour=bottom?Bottom:(same(PaintBand)?Band:White);
        // the paint peeling off to the bare grey planks in patches, more on the white
        const float peel=Wave(along*1.9f,d.Y*5.0f+Vector3::Dot(d,side)*1.3f);
        colour=Vector3::Lerp(colour,Wood,std::clamp((peel-0.55f)*3.0f,0.0f,1.0f)*(bottom?0.35f:0.65f));
        return colour*(0.96f+0.04f*Wave(along*4.0f,d.Y*7.0f));
    };
    // the hull's skin, outside and (a little in) inside, from one gunwale over the keel to the other
    constexpr int Stations=28,Rows=7;
    for(const float shell:{1.0f,0.95f}) {
        for(int i=0;i<Stations;++i) {
            const float a0=-L::Length*0.5f+L::Length*static_cast<float>(i)/Stations;
            const float a1=-L::Length*0.5f+L::Length*static_cast<float>(i+1)/Stations;
            for(const float s:{-1.0f,1.0f}) for(int k=0;k<Rows;++k) {
                // rows from the keel to the gunwale, their edges on the strakes' edges
                constexpr std::array<float,Rows+1> RowAt{{0.0f,0.13f,0.26f,0.38f,0.53f,0.69f,0.84f,1.0f}};
                const float t0=RowAt[static_cast<std::size_t>(k)],t1=RowAt[static_cast<std::size_t>(k+1)];
                const Vector3 strake=t1<=0.38f+1e-4f?PaintBottom:(t0>=0.84f-1e-4f?PaintBand:PaintWhite);
                const auto p=[&](const float a,const float t) {
                    float along=0,up=0,across=0;section(a,t,along,up,across);
                    return place(along,s*across*shell,up+(1.0f-shell)*0.05f);
                };
                const Vector3 q00=p(a0,t0),q10=p(a1,t0),q11=p(a1,t1),q01=p(a0,t1);
                // the outside faces away from the hull's hollow; the inside toward it
                const Vector3 hollow=place((a0+a1)*0.5f,0.0f,depth((a0+a1)*0.5f)*0.85f);
                const Vector3 mid=(q00+q10+q11+q01)*0.25f;
                b.inside=shell>0.99f?hollow:mid+(mid-hollow);
                b.Quad(q00,q10,q11,q01,shell>0.99f?strake:Wood*0.55f);
            }
        }
    }
    // the keel along the top, the stem and the sternpost, rubbing strakes along both gunwales
    {
        std::vector<Vector3> keel;std::vector<float> radii;
        for(int i=0;i<=16;++i) {
            const float a=(-L::Length*0.47f+L::Length*0.94f*static_cast<float>(i)/16.0f)*0.8f;
            keel.push_back(place(a,0.0f,keelUp(a)-0.04f));radii.push_back(0.055f);
        }
        b.Tube(keel,radii,6,Bottom*0.85f);
        for(const float end:{-1.0f,1.0f}) {
            std::vector<Vector3> post;std::vector<float> r;
            for(int k=0;k<=4;++k) {
                const float t=static_cast<float>(k)/4.0f;
                float along=0,up=0,across=0;section(end*L::Length*0.5f,t,along,up,across);
                post.push_back(place(along,0.0f,up));r.push_back(0.05f);
            }
            b.Tube(post,r,6,Wood*0.9f);
        }
        for(const float s:{-1.0f,1.0f}) {
            std::vector<Vector3> strake;std::vector<float> r;
            for(int i=0;i<=20;++i) {
                const float a=-L::Length*0.48f+L::Length*0.96f*static_cast<float>(i)/20.0f;
                strake.push_back(place(a,s*(breadth(a)+0.03f),depth(a)*0.97f));r.push_back(0.045f);
            }
            b.Tube(strake,r,5,Band*0.9f);
        }
    }
    // two oars thrown down on the sand beside it
    for(int k=0;k<2;++k) {
        const float along=-1.2f+1.6f*static_cast<float>(k),out=L::HalfBeam+0.9f+0.5f*static_cast<float>(k);
        const float turn=0.25f+0.4f*static_cast<float>(k);
        const Vector3 dir=forward*std::cos(turn)+side*std::sin(turn);
        const Vector3 centre=Vector3(boat.x,0.0f,boat.z)+forward*along+side*out;
        const float y=groundAt(centre.X,centre.Z)+0.025f;
        const Vector3 c(centre.X,y,centre.Z);
        const Vector3 across=Vector3::Cross(Vector3::Up,dir);
        b.Box(c,across,Vector3::Up,dir,0.03f,0.025f,1.3f,Oar,Oar*1.05f);
        b.Box(c+dir*1.55f,across,Vector3::Up,dir,0.08f,0.012f,0.3f,Oar*0.95f,Oar);
    }
    // a life ring, half sunk in the sand by the bow
    {
        const Vector3 at=Vector3(boat.x,0.0f,boat.z)+forward*(L::Length*0.5f+0.7f)-side*0.6f;
        const Vector3 c(at.X,groundAt(at.X,at.Z)+0.02f,at.Z);
        const Vector3 axis=Vector3::Normalize(Vector3::Up*0.9f+side*0.4f);
        Vector3 u=Vector3::Cross(axis,forward);u.Normalize();
        const Vector3 v=Vector3::Cross(axis,u);
        constexpr int Around=20,Tube=6;
        for(int i=0;i<Around;++i) for(int j=0;j<Tube;++j) {
            const auto point=[&](const int ii,const int jj) {
                const float a=2.0f*Pi*static_cast<float>(ii)/Around,t=2.0f*Pi*static_cast<float>(jj)/Tube;
                const Vector3 ring=u*std::cos(a)+v*std::sin(a);
                return c+ring*(0.33f+0.075f*std::cos(t))+axis*(0.075f*std::sin(t));
            };
            const float a=2.0f*Pi*(static_cast<float>(i)+0.5f)/Around;
            b.inside=c+(u*std::cos(a)+v*std::sin(a))*0.33f;
            b.Quad(point(i,j),point(i+1,j),point(i+1,j+1),point(i,j+1),(i/5)%2==0?RingOrange:RingWhite);
        }
    }

    auto& mesh=b.builder.mesh;
    triangles_=static_cast<int>(mesh.size()/3U);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& vtx:mesh) {low=Vector3::Min(low,vtx.Position);high=Vector3::Max(high,vtx.Position);}
    bounds_=BoundingBox(low-Vector3(0.2f,0.2f,0.2f),high+Vector3(0.2f,0.2f,0.2f));
    centre_=(low+high)*0.5f;
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"lifeboat",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: lifeboat sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: the coaster's lifeboat lies keel up on the beach at ("
        <<static_cast<int>(std::lround(boat.x))<<", "<<static_cast<int>(std::lround(boat.z))
        <<"), its oars and a life ring beside it; "<<triangles_<<" triangles";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void LifeboatRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>300*300
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

void LifeboatRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"lifeboat");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: lifeboat sun shadow caster unavailable: "<<error.what()<<'\n';
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
