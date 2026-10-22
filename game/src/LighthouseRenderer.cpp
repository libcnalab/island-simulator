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
