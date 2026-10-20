#include "IslandSimulator/StreamDriftRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
StreamDriftRenderer::StreamDriftRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerLeaf*static_cast<int>(StreamDriftField::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerLeaf)*StreamDriftField::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    const char* message="island-simulator: leaves drift down the stream on the current, hurry over the cascade lip, idle in the pool and go out to sea; out of the mouth they drift on across the lagoon downwind";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void StreamDriftRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                               const std::span<const DriftingLeaf> leaves) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    for(const auto& leaf:leaves) {
        if(!leaf.active||!std::isfinite(leaf.x)||!std::isfinite(leaf.y)||!std::isfinite(leaf.z)) continue;
        if((Vector3(leaf.x,leaf.y,leaf.z)-camera).LengthSquared()>70.0f*70.0f) continue;
        const Vector3 base=leaf.variant==0?Vector3(96,128,54):leaf.variant==1?Vector3(122,88,46)
            :leaf.variant==2?Vector3(176,150,58):Vector3(226,220,206);   // 3: a bird's pale feather
        const float c=std::cos(leaf.yawRadians),s=std::sin(leaf.yawRadians);
        const auto at=[&](float along,float across,float lift){
            return Vector3(leaf.x+c*along-s*across,leaf.y+lift,leaf.z+s*along+c*across);};
        // A pointed leaf: tip, two shoulders, stalk; the two lobes shade a little apart.
        const Vector3 tip=at(0.11f,0.0f,0.004f),stalk=at(-0.09f,0.0f,0.0f);
        const Vector3 left=at(0.0f,-0.045f,0.006f),right=at(0.0f,0.045f,0.006f),mid=at(0.0f,0.0f,0.002f);
        const auto colour=[&](float light){return Color(static_cast<int>(std::min(255.0f,base.X*light)),
            static_cast<int>(std::min(255.0f,base.Y*light)),static_cast<int>(std::min(255.0f,base.Z*light)),255);};
        const Color lit=colour(1.0f),shade=colour(0.82f);
        staging_.emplace_back(tip,lit);staging_.emplace_back(left,lit);staging_.emplace_back(mid,lit);
        staging_.emplace_back(tip,shade);staging_.emplace_back(mid,shade);staging_.emplace_back(right,shade);
        staging_.emplace_back(mid,lit);staging_.emplace_back(left,lit);staging_.emplace_back(stalk,lit);
        staging_.emplace_back(mid,shade);staging_.emplace_back(stalk,shade);staging_.emplace_back(right,shade);
        ++visible_;
    }
    if(staging_.empty()) return;
    vertices_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3));
    drawCalls_=1;
}
}
