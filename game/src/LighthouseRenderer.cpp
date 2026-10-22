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
    Frame(280,278,270),Dome(220,58,44),Hollow(30,28,26),Rust(170,110,72);
}

LighthouseRenderer::LighthouseRenderer(GraphicsDevice& device,const LighthousePlacement& placement,
                                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b;
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
    // the tower: whitewash with two red bands, rust bleeding down from under the gallery and the
    // windows, grime at its foot
    constexpr int Segments=32;
    const float towerTop=galleryY-0.22f;
    const auto radiusAt=[&](const float y) {
        const float t=std::clamp((y-baseY)/(towerTop-baseY),0.0f,1.0f);
        return LighthousePlacement::BaseRadius+(LighthousePlacement::TopRadius-LighthousePlacement::BaseRadius)*std::pow(t,0.9f);
    };
    {
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;std::vector<float> heights;
        const int stations=static_cast<int>((towerTop-baseY)/0.55f)+1;
        for(int i=0;i<=stations;++i) {
            const float y=baseY+(towerTop-baseY)*static_cast<float>(i)/static_cast<float>(stations);
            std::vector<Vector3> ring;
            for(int k=0;k<Segments;++k) ring.push_back(around(2.0f*Pi*static_cast<float>(k)/Segments,radiusAt(y),y));
            rings.push_back(ring);centres.push_back(axis+Vector3::Up*y);heights.push_back(y);
        }
        b.Loft(rings,centres,[&](const std::size_t i,const std::size_t k) {
            const float t=(heights[i]-baseY)/(towerTop-baseY);
            const bool red=(t>0.30f&&t<0.42f)||(t>0.66f&&t<0.78f);
            Vector3 colour=red?BandRed:Whitewash;
            // streaks: a few columns of rust from the top fading down, salt grime at the foot
            const float column=std::sin(static_cast<float>(k)*2.7f+1.3f)*std::sin(static_cast<float>(k)*1.1f);
            const float rust=std::clamp(column-0.35f,0.0f,1.0f)*std::clamp((t-0.55f)/0.45f,0.0f,1.0f);
            colour=Vector3::Lerp(colour,Rust,rust*0.55f);
            colour=Vector3::Lerp(colour,Vector3(150,146,136),std::clamp(1.0f-t*8.0f,0.0f,1.0f)*0.45f);
            return colour;
        },false,true);
    }
    // a hollow framed in white on the tower's face (the door, the windows)
    const auto opening=[&](const float angle,const float bottom,const float width,const float height) {
        const float y=bottom+height*0.5f,r=radiusAt(y)+0.03f;
        const Vector3 centre=around(angle,r,y);
        Vector3 out=centre-(axis+Vector3::Up*y);out.Y=0.0f;out.Normalize();
        const Vector3 across(out.Z,0.0f,-out.X);
        b.Box(centre,across,Vector3::Up,out,width*0.5f,height*0.5f,0.02f,Hollow,Hollow);
        b.Box(centre+Vector3::Up*(height*0.5f+0.05f)+out*0.03f,across,Vector3::Up,out,width*0.5f+0.08f,0.05f,0.05f,Frame,Frame);
        b.Box(centre-Vector3::Up*(height*0.5f+0.04f)+out*0.05f,across,Vector3::Up,out,width*0.5f+0.06f,0.04f,0.08f,Frame,Frame);
        for(const float s:{-1.0f,1.0f})
            b.Box(centre+across*(s*(width*0.5f+0.04f))+out*0.03f,across,Vector3::Up,out,0.04f,height*0.5f,0.05f,Frame,Frame);
    };
    opening(Pi,baseY,1.0f,2.05f);   // the door, inland
    constexpr std::array<std::array<float,2>,5> Windows{{{2.2f,3.6f},{0.6f,6.6f},{3.6f,9.6f},{1.9f,12.4f},{4.8f,14.6f}}};
    for(const auto& window:Windows) opening(window[0],baseY+window[1],0.42f,0.70f);
    // the gallery on its corbels, its railing
    {
        constexpr int Ring=32;
        const float inner=LighthousePlacement::TopRadius-0.15f,outer=LighthousePlacement::GalleryRadius,t=0.22f;
        b.inside=axis+Vector3::Up*(galleryY-t*0.5f);
        for(int k=0;k<Ring;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/Ring,a1=2.0f*Pi*static_cast<float>(k+1)/Ring;
            b.Quad(around(a0,inner,galleryY),around(a1,inner,galleryY),around(a1,outer,galleryY),around(a0,outer,galleryY),Stone*0.95f);
            b.Quad(around(a0,inner,galleryY-t),around(a0,outer,galleryY-t),around(a1,outer,galleryY-t),around(a1,inner,galleryY-t),Stone*0.8f);
            b.Quad(around(a0,outer,galleryY-t),around(a1,outer,galleryY-t),around(a1,outer,galleryY),around(a0,outer,galleryY),Stone*0.9f);
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
            top.push_back(around(a,outer-0.08f,galleryY+1.0f));mid.push_back(around(a,outer-0.08f,galleryY+0.5f));radii.push_back(0.035f);
            if(k<36&&k%2==0) b.Tube({around(a,outer-0.08f,galleryY),around(a,outer-0.08f,galleryY+1.0f)},{0.025f,0.025f},5,Iron);
        }
        b.Tube(top,radii,5,Iron);b.Tube(mid,radii,5,Iron);
    }
    // the lantern: a low wall, twelve glazed panes in white frames, the red dome, ball and rod
    {
        constexpr int Panes=12;
        const float r=LighthousePlacement::LanternRadius,wall=0.55f,glassTop=galleryY+LighthousePlacement::LanternHeight;
        std::vector<std::vector<Vector3>> rings(2,std::vector<Vector3>(Panes));
        for(int k=0;k<Panes;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/Panes;
            rings[0][static_cast<std::size_t>(k)]=around(a,r,galleryY);rings[1][static_cast<std::size_t>(k)]=around(a,r,galleryY+wall);
        }
        b.Loft(rings,{axis+Vector3::Up*galleryY,axis+Vector3::Up*(galleryY+wall)},[](std::size_t,std::size_t) {return Iron*1.4f;},false,false);
        for(int k=0;k<Panes;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/Panes,a1=2.0f*Pi*static_cast<float>(k+1)/Panes;
            b.inside=axis+Vector3::Up*(galleryY+1.2f);
            b.Quad(around(a0,r*0.98f,galleryY+wall),around(a1,r*0.98f,galleryY+wall),around(a1,r*0.98f,glassTop),around(a0,r*0.98f,glassTop),
                   Glass*(0.9f+0.25f*static_cast<float>(k%3)));
            b.Tube({around(a0,r,galleryY+wall),around(a0,r,glassTop)},{0.035f,0.035f},5,Frame);
        }
        for(const float y:{galleryY+wall,galleryY+wall+0.9f,glassTop}) {
            std::vector<Vector3> ring;std::vector<float> radii;
            for(int k=0;k<=Panes;++k) {ring.push_back(around(2.0f*Pi*static_cast<float>(k)/Panes,r,y));radii.push_back(0.04f);}
            b.Tube(ring,radii,5,Frame);
        }
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
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"lighthouse",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: lighthouse sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
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
