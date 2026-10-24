#include "IslandSimulator/BalloonShadowRenderer.hpp"
#include "IslandSimulator/BalloonShadowProjection.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {

BalloonShadowRenderer::BalloonShadowRenderer(GraphicsDevice& device,const IslandTerrain& terrain,
    const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,
        VertexPositionColor::getVertexDeclarationStatic(),VerticesPerShadow,BufferUsage::WriteOnly)) {
    staging_.reserve(VerticesPerShadow);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    constexpr std::string_view message=
        "island-simulator: the hot-air balloon casts one moving soft envelope shadow over land and sea, bounded outside the static sun-shadow map";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

BalloonShadowRenderer::~BalloonShadowRenderer()=default;

void BalloonShadowRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                  const BalloonFlight::State& balloon) {
    visible_=0;drawCalls_=0;staging_.clear();
    const auto& light=sun_light::Current();
    const float strength=std::clamp(light.lighting.shadowStrength,0.0f,1.0f);
    if(strength<.04f||light.shading.y<.14f)return;
    const float slopeX=light.shading.x/light.shading.y,slopeZ=light.shading.z/light.shading.y;
    const float casterY=balloon.y+BalloonFlight::EnvelopeHeight*.48f;
    const BalloonShadowPoint shadow=BalloonShadowProjection::Project(
        terrain_,balloon.x,casterY,balloon.z,slopeX,slopeZ);
    if(!shadow.valid)return;
    const float distance=std::hypot(std::hypot(shadow.x-camera.X,shadow.z-camera.Z),shadow.receiverY-camera.Y);
    if(distance>MaximumCameraDistance)return;
    const float distanceFade=1.0f-std::clamp((distance-190.0f)/(MaximumCameraDistance-190.0f),0.0f,1.0f);
    const float altitudeFade=1.0f-0.55f*std::clamp((shadow.height-70.0f)/260.0f,0.0f,1.0f);
    const int centreAlpha=static_cast<int>(std::lround(43.0f*strength*distanceFade*altitudeFade));
    if(centreAlpha<3)return;
    const float radius=BalloonFlight::EnvelopeRadius+std::clamp(shadow.height*.018f,1.0f,6.0f);
    const float stretch=1.0f+std::clamp(std::hypot(slopeX,slopeZ)*.08f,0.0f,.35f);
    const float alongX=std::hypot(slopeX,slopeZ)>1e-4f?slopeX/std::hypot(slopeX,slopeZ):1.0f;
    const float alongZ=std::hypot(slopeX,slopeZ)>1e-4f?slopeZ/std::hypot(slopeX,slopeZ):0.0f;
    const float sideX=-alongZ,sideZ=alongX;
    const auto point=[this,&shadow,alongX,alongZ,sideX,sideZ,radius,stretch](const float scale,const float angle) {
        const float along=std::cos(angle)*radius*stretch*scale,side=std::sin(angle)*radius*scale;
        const float x=shadow.x+alongX*along+sideX*side,z=shadow.z+alongZ*along+sideZ*side;
        const float analytic=terrain_.Height(x,z);
        const float y=analytic>0.0f?DrawnOrAnalyticHeight(ground_,terrain_,x,z)+.045f:.045f;
        return Vector3(x,y,z);
    };
    const Vector3 centre(point(0.0f,0.0f));
    const Color centreColour(0,0,0,centreAlpha),midColour(0,0,0,centreAlpha/2),edgeColour(0,0,0,0);
    std::array<Vector3,Segments> mid{},edge{};
    for(int i=0;i<Segments;++i) {
        const float angle=static_cast<float>(i)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
        mid[static_cast<std::size_t>(i)]=point(.62f,angle);edge[static_cast<std::size_t>(i)]=point(1.0f,angle);
    }
    for(int i=0;i<Segments;++i) {
        const std::size_t a=static_cast<std::size_t>(i),b=static_cast<std::size_t>((i+1)%Segments);
        staging_.emplace_back(centre,centreColour);staging_.emplace_back(mid[a],midColour);staging_.emplace_back(mid[b],midColour);
        staging_.emplace_back(mid[a],midColour);staging_.emplace_back(edge[a],edgeColour);staging_.emplace_back(edge[b],edgeColour);
        staging_.emplace_back(mid[a],midColour);staging_.emplace_back(edge[b],edgeColour);staging_.emplace_back(mid[b],midColour);
    }
    vertices_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);device_.SetVertexBuffer(vertices_.get());
    device_.setIndicesProperty(nullptr);effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3));
    visible_=1;drawCalls_=1;
}

}
