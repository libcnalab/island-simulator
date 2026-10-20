#include "IslandSimulator/BirdShadowRenderer.hpp"
#include "IslandSimulator/BirdShadowProjection.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
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
BirdShadowRenderer::BirdShadowRenderer(GraphicsDevice& device,const IslandTerrain& terrain)
    :device_(device),terrain_(terrain),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerShadow*MaximumShadows,BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerShadow)*MaximumShadows);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: flying birds cast soft moving shadows on the ground below them, projected along the fixed sun";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void BirdShadowRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                              const std::span<const BirdState> birds) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    for(const auto& bird:birds) {
        if(visible_>=MaximumShadows) break;
        if(bird.behavior==BirdBehavior::Perched||bird.behavior==BirdBehavior::GroundForage) continue;
        const BirdShadowPoint shadow=BirdShadowProjection::Project(terrain_,bird.x,bird.y,bird.z);
        if(!shadow.valid) continue;
        const float distance=std::hypot(shadow.x-camera.X,shadow.z-camera.Z);
        if(distance>MaximumDistance) continue;
        // Darker and tighter close under the bird, broad and faint for a high one; gone with
        // distance. The ellipse lies along the heading with the wings across it.
        const float heightFade=1.0f-std::clamp((shadow.height-4.0f)/(BirdShadowProjection::MaximumHeight-4.0f),0.0f,1.0f);
        const float distanceFade=1.0f-std::clamp((distance-40.0f)/30.0f,0.0f,1.0f);
        const int alpha=static_cast<int>(std::lround(96.0f*(0.25f+0.75f*heightFade)*distanceFade));
        if(alpha<4) continue;
        const float spread=1.0f+shadow.height*0.06f;
        const float forwardRadius=0.26f*bird.scale*spread,sideRadius=0.48f*bird.scale*spread;
        const float forwardX=std::cos(bird.headingRadians),forwardZ=std::sin(bird.headingRadians);
        const float sideX=-forwardZ,sideZ=forwardX;
        const Vector3 centre(shadow.x,terrain_.Height(shadow.x,shadow.z)+0.03f,shadow.z);
        const Color centreColor(0,0,0,alpha),edgeColor(0,0,0,0);
        std::array<Vector3,Segments> edge;
        for(int segment=0;segment<Segments;++segment) {
            const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
            const float along=std::cos(angle)*forwardRadius,across=std::sin(angle)*sideRadius;
            const float ex=shadow.x+forwardX*along+sideX*across,ez=shadow.z+forwardZ*along+sideZ*across;
            edge[static_cast<std::size_t>(segment)]=Vector3(ex,terrain_.Height(ex,ez)+0.03f,ez);
        }
        for(int segment=0;segment<Segments;++segment) {
            staging_.emplace_back(centre,centreColor);
            staging_.emplace_back(edge[static_cast<std::size_t>(segment)],edgeColor);
            staging_.emplace_back(edge[static_cast<std::size_t>((segment+1)%Segments)],edgeColor);
        }
        ++visible_;
    }
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
