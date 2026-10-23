#include "IslandSimulator/SeabirdIsletRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numbers>
#include <sstream>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Noise(const float x,const float z) {
    const float fx=std::floor(x),fz=std::floor(z),tx=x-fx,tz=z-fz;
    const auto corner=[](const float cx,const float cz) {
        return Hash(static_cast<std::uint32_t>(static_cast<std::int32_t>(cx)*73856093)^static_cast<std::uint32_t>(static_cast<std::int32_t>(cz)*19349663));
    };
    const float sx=tx*tx*(3.0f-2.0f*tx),sz=tz*tz*(3.0f-2.0f*tz);
    const float a=corner(fx,fz),b=corner(fx+1,fz),c=corner(fx,fz+1),d=corner(fx+1,fz+1);
    return (a+(b-a)*sx)+((c+(d-c)*sx)-(a+(b-a)*sx))*sz;
}
const Vector3 Basalt(70,66,62),WetRock(36,36,36),Weed(52,74,40),Guano(226,222,208);
}

SeabirdIsletRenderer::SeabirdIsletRenderer(GraphicsDevice& device,const SeabirdIslet& islet)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)) {
    SculptMeshBuilder b;
    b.tintPerVertex=true;
    b.tint=[&islet](const Vector3&,const Vector3& normal,const Vector3& p) {
        const float grain=0.88f+0.24f*Noise(p.X*1.7f,p.Z*1.7f+p.Y*1.3f);
        Vector3 colour=Basalt*grain;
        // the tide's band: weed low down, black wet rock above it
        colour=Vector3::Lerp(colour,WetRock,std::clamp((1.1f-p.Y)/0.6f,0.0f,1.0f));
        colour=Vector3::Lerp(colour,Weed*grain,std::clamp((0.35f-p.Y)/0.4f,0.0f,1.0f)*std::clamp((p.Y+0.6f)/0.4f,0.0f,1.0f));
        // guano: white over the level tops and ledges, in streaks down the faces below them
        const float level=std::clamp((normal.Y-0.55f)/0.3f,0.0f,1.0f)*std::clamp((p.Y-1.4f)/0.8f,0.0f,1.0f);
        const float angle=std::atan2(p.Z-islet.z,p.X-islet.x);
        const float streak=std::clamp((Noise(angle*26.0f,p.Y*0.25f)-0.55f)*3.0f,0.0f,1.0f)
            *std::clamp((p.Y-0.8f)/2.0f,0.0f,1.0f)*(1.0f-std::clamp((normal.Y-0.3f)/0.4f,0.0f,1.0f));
        const float speckle=0.7f+0.3f*Noise(p.X*3.1f,p.Z*3.1f);
        colour=Vector3::Lerp(colour,Guano*speckle,std::max(level*0.9f,streak*0.75f));
        return colour;
    };
    for(const IsletRock& rock:islet.rocks) {
        constexpr int Around=72,Rings=34;
        const auto at=[&](const float t,const float a) {
            // rings out to the rock's lumpy edge and down its skirt into the sea's floor
            const float reach=rock.radius*1.65f;
            const float r=reach*std::pow(t,0.8f);
            const float x=rock.x+std::cos(a)*r,z=rock.z+std::sin(a)*r;
            float y=islet.SurfaceY(x,z);
            if(t>=1.0f) y=SeabirdIsletLayout::SeaFloor-0.5f;
            return Vector3(x,y,z);
        };
        b.inside=Vector3(rock.x,SeabirdIsletLayout::SeaFloor-6.0f,rock.z);
        for(int i=0;i<Around;++i) {
            const float a0=2.0f*Pi*static_cast<float>(i)/Around,a1=2.0f*Pi*static_cast<float>(i+1)/Around;
            for(int k=0;k<Rings;++k) {
                const float t0=static_cast<float>(k)/Rings,t1=static_cast<float>(k+1)/Rings;
                b.Quad(at(t0,a0),at(t0,a1),at(t1,a1),at(t1,a0),Basalt);
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
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"seabird-islet",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: seabird islet sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a rocky islet of "<<islet.rocks.size()<<" basalt stacks stands out of the sea at ("
           <<static_cast<int>(std::lround(islet.x))<<", "<<static_cast<int>(std::lround(islet.z))
           <<"), white with the boobies' guano; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void SeabirdIsletRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
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
}
