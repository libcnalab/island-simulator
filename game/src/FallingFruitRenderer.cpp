#include "IslandSimulator/FallingFruitRenderer.hpp"
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
#include <numbers>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
FallingFruitRenderer::FallingFruitRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerFruit*static_cast<int>(FallingFruitField::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerFruit)*FallingFruitField::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    const char* message="island-simulator: fruit knocked loose by picking monkeys drops from the crown and lies under the tree";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void FallingFruitRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                const std::span<const FallingFruit> fruit) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    constexpr float Pi=std::numbers::pi_v<float>;
    Vector3 sun(environment::SunDirectionX,environment::SunDirectionY,environment::SunDirectionZ);sun.Normalize();
    for(const auto& f:fruit) {
        if(!f.active||!std::isfinite(f.x)||!std::isfinite(f.y)||!std::isfinite(f.z)) continue;
        if((Vector3(f.x,f.y,f.z)-camera).LengthSquared()>90.0f*90.0f) continue;
        const Vector3 base=f.variant==2?Vector3(216,152,54):Vector3(146,54,80);
        // Lying fruit darkens a little as it ages.
        const float age=f.restSeconds>0?std::min(1.0f,f.restSeconds/FallingFruitField::RestSeconds):0.0f;
        const float radius=0.09f;
        const Vector3 centre(f.x,f.y+radius*0.9f,f.z);
        const auto point=[&](int ring,int seg){
            const float v=-0.5f*Pi+Pi*static_cast<float>(ring)/4.0f;
            const float u=2*Pi*static_cast<float>(seg)/6.0f;
            return Vector3(std::cos(v)*std::cos(u),std::sin(v),std::cos(v)*std::sin(u));
        };
        for(int ring=0;ring<4;++ring) for(int seg=0;seg<6;++seg) {
            const Vector3 n00=point(ring,seg),n10=point(ring+1,seg),n11=point(ring+1,seg+1),n01=point(ring,seg+1);
            Vector3 normal=n00+n10+n11+n01;normal.Normalize();
            const float light=(0.42f+0.58f*std::max(0.0f,Vector3::Dot(normal,sun)))*(1.0f-0.35f*age);
            const Color color(static_cast<int>(std::min(255.0f,base.X*light)),
                static_cast<int>(std::min(255.0f,base.Y*light)),static_cast<int>(std::min(255.0f,base.Z*light)),255);
            staging_.emplace_back(centre+n00*radius,color);staging_.emplace_back(centre+n10*radius,color);staging_.emplace_back(centre+n11*radius,color);
            staging_.emplace_back(centre+n00*radius,color);staging_.emplace_back(centre+n11*radius,color);staging_.emplace_back(centre+n01*radius,color);
        }
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
