#include "IslandSimulator/StreamFishRenderer.hpp"
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
StreamFishRenderer::StreamFishRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerFish*static_cast<int>(StreamFishField::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerFish)*StreamFishField::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    const char* message="island-simulator: small fish hold station in the plunge pool facing upstream, dart now and then and scatter to the deep when the walker comes near; a leaf drifting over a fish draws it up in a dart";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void StreamFishRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                              const std::span<const StreamFish> fish) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    for(const auto& f:fish) {
        if(!f.active||!std::isfinite(f.x)||!std::isfinite(f.y)||!std::isfinite(f.z)) continue;
        if((Vector3(f.x,f.y,f.z)-camera).LengthSquared()>45.0f*45.0f) continue;
        const float c=std::cos(f.yawRadians),s=std::sin(f.yawRadians);
        const float flick=std::sin(f.tailPhase)*0.018f;
        const auto at=[&](float along,float across,float lift){
            return Vector3(f.x+c*along-s*across,f.y+lift,f.z+s*along+c*across);};
        // A dark olive sliver seen from above: nose, a widest point, and a forked tail that
        // flicks across the heading.
        // Fourteen centimetres nose to tail, a pale olive flank so it reads through the
        // water's tint as a flicker, a darker back and fin.
        const Color body(96,112,78,255),back(62,76,52,255),fin(48,60,42,255);
        const Vector3 nose=at(0.075f,0.0f,0.0f),midL=at(0.0f,-0.016f,0.0f),midR=at(0.0f,0.016f,0.0f);
        const Vector3 tailBase=at(-0.055f,flick*0.4f,0.0f);
        staging_.emplace_back(nose,back);staging_.emplace_back(midL,body);staging_.emplace_back(midR,body);
        staging_.emplace_back(midL,body);staging_.emplace_back(tailBase,body);staging_.emplace_back(midR,body);
        staging_.emplace_back(tailBase,fin);staging_.emplace_back(at(-0.095f,-0.024f+flick,0.0f),fin);staging_.emplace_back(at(-0.095f,0.024f+flick,0.0f),fin);
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
