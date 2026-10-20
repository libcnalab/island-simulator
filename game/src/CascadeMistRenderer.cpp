#include "IslandSimulator/CascadeMistRenderer.hpp"
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
CascadeMistRenderer::CascadeMistRenderer(GraphicsDevice& device,const char* message,const std::size_t capacity)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerPuff*static_cast<int>(std::max<std::size_t>(capacity,1)),BufferUsage::WriteOnly)),
     capacity_(std::max<std::size_t>(capacity,1))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerPuff)*capacity_);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    if(!message) message="island-simulator: a mist of spray hangs at the cascade's foot, rising and drifting on the breeze and laid over by the gust fronts";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void CascadeMistRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                               const std::span<const MistPuff> puffs) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    for(const auto& puff:puffs) {
        if(static_cast<std::size_t>(visible_)>=capacity_) break;   // never past the buffer
        if(!puff.active||puff.density<=0.02f||!std::isfinite(puff.x)||!std::isfinite(puff.y)||!std::isfinite(puff.z)) continue;
        const Vector3 centre(puff.x,puff.y,puff.z);
        Vector3 toCamera=camera-centre;
        const float distance=toCamera.Length();
        if(distance>60.0f||distance<0.05f) continue;
        toCamera/=distance;
        // A camera-facing fan: right and up from the view direction.
        Vector3 right=Vector3::Cross(Vector3::Up,toCamera);
        if(right.LengthSquared()<1e-6f) right=Vector3::Right; else right.Normalize();
        Vector3 up=Vector3::Cross(toCamera,right);up.Normalize();
        const int alpha=static_cast<int>(std::lround(58.0f*puff.density*(1.0f-std::clamp((distance-40.0f)/20.0f,0.0f,1.0f))));
        if(alpha<2) continue;
        // The blend state takes premultiplied colour: the rim is clear black, the core the
        // pale spray scaled by its own alpha, or the fan reads as a solid white disc.
        const Color core(236*alpha/255,244*alpha/255,248*alpha/255,alpha),rim(0,0,0,0);
        std::array<Vector3,Segments> edge;
        for(int segment=0;segment<Segments;++segment) {
            const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
            edge[static_cast<std::size_t>(segment)]=centre+right*(std::cos(angle)*puff.size)+up*(std::sin(angle)*puff.size*0.8f);
        }
        for(int segment=0;segment<Segments;++segment) {
            staging_.emplace_back(centre,core);
            staging_.emplace_back(edge[static_cast<std::size_t>(segment)],rim);
            staging_.emplace_back(edge[static_cast<std::size_t>((segment+1)%Segments)],rim);
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
