#include "IslandSimulator/SeabedShadowRenderer.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SeabedShadowProjection.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
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
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
SeabedShadowRenderer::SeabedShadowRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerShadow*MaximumShadows,BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerShadow)*MaximumShadows);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: the fish, the stingrays, the sea turtles and the shark cast soft shadows on the sandy seabed below them, projected along the hour's sun refracted at the surface, as strong as its shadows";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void SeabedShadowRenderer::AppendShadow(const Vector3& camera,const float x,const float y,const float z,
                                        const float headingRadians,const float forwardRadius,const float sideRadius) noexcept {
    if(visible_>=MaximumShadows) return;
    const SeabedShadowPoint shadow=SeabedShadowProjection::Project(terrain_,x,y,z,slopeX_,slopeZ_);
    if(!shadow.valid||!std::isfinite(headingRadians)) return;
    const float distance=std::hypot(std::hypot(shadow.x-camera.X,shadow.z-camera.Z),DrawnOrAnalyticHeight(ground_,terrain_,shadow.x,shadow.z)-camera.Y);
    if(distance>MaximumDistance) return;
    // Darker and tighter close over the bottom, broad and faint for a high one; the water
    // takes it with distance.
    const float heightFade=1.0f-std::clamp((shadow.height-1.0f)/(SeabedShadowProjection::MaximumHeight-1.0f),0.0f,1.0f);
    const float distanceFade=1.0f-std::clamp((distance-18.0f)/16.0f,0.0f,1.0f);
    const int alpha=static_cast<int>(std::lround(84.0f*(0.3f+0.7f*heightFade)*distanceFade*strength_));
    if(alpha<4) return;
    const float spread=1.0f+shadow.height*0.10f;
    const float forwardX=std::cos(headingRadians),forwardZ=std::sin(headingRadians);
    const float sideX=-forwardZ,sideZ=forwardX;
    const Vector3 centre(shadow.x,DrawnOrAnalyticHeight(ground_,terrain_,shadow.x,shadow.z)+0.025f,shadow.z);
    const Color centreColor(0,0,0,alpha),edgeColor(0,0,0,0);
    std::array<Vector3,Segments> edge;
    for(int segment=0;segment<Segments;++segment) {
        const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
        const float along=std::cos(angle)*forwardRadius*spread,across=std::sin(angle)*sideRadius*spread;
        const float ex=shadow.x+forwardX*along+sideX*across,ez=shadow.z+forwardZ*along+sideZ*across;
        edge[static_cast<std::size_t>(segment)]=Vector3(ex,DrawnOrAnalyticHeight(ground_,terrain_,ex,ez)+0.025f,ez);
    }
    for(int segment=0;segment<Segments;++segment) {
        staging_.emplace_back(centre,centreColor);
        staging_.emplace_back(edge[static_cast<std::size_t>(segment)],edgeColor);
        staging_.emplace_back(edge[static_cast<std::size_t>((segment+1)%Segments)],edgeColor);
    }
    ++visible_;
}

void SeabedShadowRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                const std::span<const FishState> fish,const std::span<const StingrayState> rays,
                                const std::span<const SeaTurtleState> turtles,const SharkState* shark) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    // The hour's light (the sun by day, a bright moon's by night) bent toward the vertical at
    // the surface, and as strong as the shadows it casts: none under an overcast or a dark night.
    const auto& light=sun_light::Current();
    const auto run=SeabedShadowProjection::Refracted(light.shading.x,light.shading.y,light.shading.z);
    slopeX_=run[0];slopeZ_=run[1];
    strength_=std::clamp(light.lighting.shadowStrength,0.0f,1.0f);
    if(strength_<0.02f) return;
    for(const auto& one:fish) AppendShadow(camera,one.x,one.y,one.z,one.headingRadians,0.16f*one.scale,0.06f*one.scale);
    for(const auto& ray:rays) AppendShadow(camera,ray.x,ray.y,ray.z,ray.headingRadians,0.95f*ray.scale,1.25f*ray.scale);
    for(const auto& turtle:turtles) AppendShadow(camera,turtle.x,turtle.y,turtle.z,turtle.headingRadians,0.62f*turtle.scale,0.50f*turtle.scale);
    if(shark&&shark->present&&shark->visible)
        AppendShadow(camera,shark->x,shark->y,shark->z,shark->headingRadians,0.60f*shark->scale,0.16f*shark->scale);
    if(staging_.empty()) return;
    vertices_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3));
    drawCalls_=1;
}
}
