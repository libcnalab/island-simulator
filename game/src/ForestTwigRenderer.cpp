#include "IslandSimulator/ForestTwigRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
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
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
ForestTwigRenderer::ForestTwigRenderer(GraphicsDevice& device,const IslandTerrain& terrain,
                                       const std::span<const ForestTwigPlacement> twigs)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
    std::vector<VertexPositionColor> staging;
    staging.reserve(twigs.size()*static_cast<std::size_t>(VerticesPerTwig));
    for(const auto& twig:twigs) {
        if(!std::isfinite(twig.x)||!std::isfinite(twig.z)) continue;
        const Vector3 bark=twig.variant==0?Vector3(84,66,48):twig.variant==1?Vector3(96,80,58):Vector3(72,58,44);
        const Vector3 broken(150,128,96);
        const float c=std::cos(twig.yawRadians),s=std::sin(twig.yawRadians);
        const Vector3 along(c,0,s),across(-s,0,c);
        // Spine: three points along the yaw, the middle lifted and set over by the bend.
        std::array<Vector3,Segments+1> spine;
        for(int i=0;i<=Segments;++i) {
            const float t=static_cast<float>(i)/static_cast<float>(Segments);
            const float lift=std::sin(t*std::numbers::pi_v<float>)*twig.bend*twig.length*0.35f;
            const float side=std::sin(t*std::numbers::pi_v<float>)*twig.bend*twig.length*0.5f;
            const Vector3 flat=Vector3(twig.x,0,twig.z)+along*((t-0.5f)*twig.length)+across*side;
            spine[static_cast<std::size_t>(i)]=Vector3(flat.X,terrain.Height(flat.X,flat.Z)+twig.radius*0.9f+lift,flat.Z);
        }
        const auto ring=[&](int i,int side){
            const float a=static_cast<float>(side)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Sides);
            const float taper=1.0f-0.35f*static_cast<float>(i)/static_cast<float>(Segments);
            const Vector3 n=across*std::cos(a)+Vector3::Up*std::sin(a);
            return std::pair<Vector3,Vector3>(spine[static_cast<std::size_t>(i)]+n*(twig.radius*taper),n);
        };
        const auto shade=[&](const Vector3& pigment,const Vector3& normal){
            const float light=0.45f+0.55f*std::max(0.0f,Vector3::Dot(normal,sun));
            return Color(static_cast<int>(pigment.X*light),static_cast<int>(pigment.Y*light),static_cast<int>(pigment.Z*light),255);};
        for(int i=0;i<Segments;++i) for(int side=0;side<Sides;++side) {
            const auto [a,na]=ring(i,side);const auto [b,nb]=ring(i,(side+1)%Sides);
            const auto [c2,nc]=ring(i+1,(side+1)%Sides);const auto [d,nd]=ring(i+1,side);
            Vector3 n=na+nb+nc+nd;n.Normalize();
            const Color colour=shade(bark,n);
            staging.emplace_back(a,colour);staging.emplace_back(b,colour);staging.emplace_back(c2,colour);
            staging.emplace_back(a,colour);staging.emplace_back(c2,colour);staging.emplace_back(d,colour);
        }
        // The broken end: a pale cap on the thick end.
        for(int side=0;side<Sides;++side) {
            const auto [a,na]=ring(0,side);const auto [b,nb]=ring(0,(side+1)%Sides);
            const Color cap=shade(broken,along*-1.0f);
            staging.emplace_back(spine[0],cap);staging.emplace_back(b,cap);staging.emplace_back(a,cap);
        }
        ++twigs_;
    }
    if(!staging.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
            static_cast<int>(staging.size()),BufferUsage::WriteOnly);
        vertices_->SetData(staging.data(),static_cast<int>(staging.size()));
        primitives_=static_cast<int>(staging.size()/3);
    }
    std::cout<<"island-simulator: fallen twigs lie scattered on the forest floor under the crowns ("<<twigs_<<" twigs)\n";
    MirrorRuntimeDiagnosticToPlatformLog("island-simulator: fallen twigs lie scattered on the forest floor under the crowns");
}

void ForestTwigRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    (void)camera;
    drawCalls_=0;
    if(!vertices_||primitives_==0) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,primitives_);
    drawCalls_=1;
}
}
