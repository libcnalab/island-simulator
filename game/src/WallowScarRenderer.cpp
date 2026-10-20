#include "IslandSimulator/WallowScarRenderer.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
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
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
WallowScarRenderer::WallowScarRenderer(GraphicsDevice& device,const IslandTerrain& terrain,
                                       const std::span<const BoarHerd::WallowSite> sites)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    std::vector<VertexPositionColor> staging;
    for(const auto& site:sites) {
        if(!std::isfinite(site.x)||!std::isfinite(site.z)) continue;
        // Long axis along the bank (across the facing), the churned centre darkest and wet.
        const float alongX=-site.faceZ,alongZ=site.faceX;
        const Vector3 centre(site.x,terrain.Height(site.x,site.z)+0.015f,site.z);
        const Color core(0,0,0,118),mid(0,0,0,60),rim(0,0,0,0);
        std::array<Vector3,Segments> edge;
        for(int segment=0;segment<Segments;++segment) {
            const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
            const float a=std::cos(angle)*AlongBank*(1.0f+0.08f*std::sin(angle*3.0f)),w=std::sin(angle)*TowardWater;
            const float ex=site.x+alongX*a+site.faceX*w,ez=site.z+alongZ*a+site.faceZ*w;
            edge[static_cast<std::size_t>(segment)]=Vector3(ex,terrain.Height(ex,ez)+0.015f,ez);
        }
        for(int segment=0;segment<Segments;++segment) {
            staging.emplace_back(centre,core);
            staging.emplace_back(edge[static_cast<std::size_t>(segment)],rim);
            staging.emplace_back(edge[static_cast<std::size_t>((segment+1)%Segments)],rim);
        }
        (void)mid;
        centres_.push_back(centre);
        ++scars_;
    }
    if(!staging.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
            static_cast<int>(staging.size()),BufferUsage::WriteOnly);
        vertices_->SetData(staging.data(),static_cast<int>(staging.size()));
    }
    std::cout<<"island-simulator: the boars' wallows show as dark churned mud at the stream bank ("<<scars_<<" wallow sites)\n";
    MirrorRuntimeDiagnosticToPlatformLog("island-simulator: the boars' wallows show as dark churned mud at the stream bank");
}

void WallowScarRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!vertices_||scars_==0) return;
    bool near=false;
    for(const auto& centre:centres_) if((centre-camera).LengthSquared()<90.0f*90.0f) near=true;
    if(!near) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,scars_*Segments);
    drawCalls_=1;
}
}
