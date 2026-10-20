#include "IslandSimulator/HatchlingRenderer.hpp"
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
HatchlingRenderer::HatchlingRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerHatchling*static_cast<int>(HatchlingField::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerHatchling)*HatchlingField::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    const char* message="island-simulator: turtle hatchlings break out of the nests in clutches and scramble down the sand to the surf";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void HatchlingRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                             const std::span<const Hatchling> hatchlings) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
    for(const auto& h:hatchlings) {
        if(!h.active||!std::isfinite(h.x)||!std::isfinite(h.y)||!std::isfinite(h.z)) continue;
        if((Vector3(h.x,h.y,h.z)-camera).LengthSquared()>40.0f*40.0f) continue;
        const float c=std::cos(h.headingRadians),s=std::sin(h.headingRadians);
        const auto at=[&](float along,float across,float lift){
            return Vector3(h.x+c*along-s*across,h.y+lift,h.z+s*along+c*across);};
        // Five centimetres of dark grey-green shell, lit from the sun's side, over a paler
        // rim; the flippers paddle alternately.
        const float lit=0.55f+0.45f*std::max(0.0f,sun.Y);
        const Color top(static_cast<int>(52*lit+20),static_cast<int>(60*lit+20),static_cast<int>(46*lit+16),255);
        const Color rim(78,84,64,255),flipper(44,50,40,255);
        const Vector3 crown=at(0.0f,0.0f,0.022f);
        const Vector3 nose=at(0.028f,0.0f,0.004f),tail=at(-0.026f,0.0f,0.004f);
        const Vector3 left=at(0.0f,-0.018f,0.004f),right=at(0.0f,0.018f,0.004f);
        staging_.emplace_back(crown,top);staging_.emplace_back(nose,rim);staging_.emplace_back(left,rim);
        staging_.emplace_back(crown,top);staging_.emplace_back(left,rim);staging_.emplace_back(tail,rim);
        staging_.emplace_back(crown,top);staging_.emplace_back(tail,rim);staging_.emplace_back(right,rim);
        staging_.emplace_back(crown,top);staging_.emplace_back(right,rim);staging_.emplace_back(nose,rim);
        const float paddle=std::sin(h.flipperPhase)*0.012f;
        const auto fin=[&](float along,float side,float sweep){
            staging_.emplace_back(at(along,side*0.016f,0.003f),flipper);
            staging_.emplace_back(at(along+0.012f+sweep,side*0.034f,0.003f),flipper);
            staging_.emplace_back(at(along-0.010f+sweep,side*0.030f,0.003f),flipper);
        };
        fin(0.016f,-1.0f,paddle);fin(0.016f,1.0f,-paddle);fin(-0.016f,-1.0f,-paddle*0.6f);fin(-0.016f,1.0f,paddle*0.6f);
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
