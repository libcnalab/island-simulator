#include "IslandSimulator/TyreTrackRenderer.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
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
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
TyreTrackRenderer::TyreTrackRenderer(GraphicsDevice& device,const IslandTerrain& terrain,
                                     const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerSegment*static_cast<int>(TyreTrackField::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerSegment)*TyreTrackField::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: the quad's knobby tyres leave ruts in the sand and soil that the ground takes back over five minutes (one on the wet sand)";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void TyreTrackRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                             const std::span<const TyreTrack> tracks) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    const auto on=[&](float x,float z){return Vector3(x,DrawnOrAnalyticHeight(ground_,terrain_,x,z)+0.013f,z);};
    // a quad of four ground points with their alphas (black, premultiplied: it darkens)
    const auto quad=[&](const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,int alphaA,int alphaB,int alphaC,int alphaD){
        const Color ca(0,0,0,alphaA),cb(0,0,0,alphaB),cc(0,0,0,alphaC),cd(0,0,0,alphaD);
        staging_.emplace_back(a,ca);staging_.emplace_back(b,cb);staging_.emplace_back(c,cc);
        staging_.emplace_back(a,ca);staging_.emplace_back(c,cc);staging_.emplace_back(d,cd);
    };
    for(const auto& track:tracks) {
        if(!track.active||!std::isfinite(track.x0)||!std::isfinite(track.z1)) continue;
        const float mx=(track.x0+track.x1)*0.5f,mz=(track.z0+track.z1)*0.5f;
        const float distance=std::hypot(mx-camera.X,mz-camera.Z);
        if(distance>DrawDistance) continue;
        const float fade=1.0f-std::clamp((distance-30.0f)/(DrawDistance-30.0f),0.0f,1.0f);
        const int alpha=static_cast<int>(std::lround(70.0f*std::clamp(track.density,0.0f,1.0f)*fade));
        if(alpha<3) continue;
        float dx=track.x1-track.x0,dz=track.z1-track.z0;
        const float length=std::hypot(dx,dz);
        if(length<1e-3f) continue;
        dx/=length;dz/=length;
        const float sx=-dz,sz=dx,half=track.width*0.5f;
        // the rut across: a soft outer lip, the pressed floor, a soft lip again
        const std::array<float,4> across{-half*1.25f,-half*0.8f,half*0.8f,half*1.25f};
        const std::array<int,4> alphas{0,alpha,alpha,0};
        for(int column=0;column<3;++column) {
            const float a0=across[static_cast<std::size_t>(column)],a1=across[static_cast<std::size_t>(column+1)];
            quad(on(track.x0+sx*a0,track.z0+sz*a0),on(track.x1+sx*a0,track.z1+sz*a0),
                 on(track.x1+sx*a1,track.z1+sz*a1),on(track.x0+sx*a1,track.z0+sz*a1),
                 alphas[static_cast<std::size_t>(column)],alphas[static_cast<std::size_t>(column)],
                 alphas[static_cast<std::size_t>(column+1)],alphas[static_cast<std::size_t>(column+1)]);
        }
        // the knobs' prints: short bars across each half of the tread, staggered
        if(distance<22.0f) {
            const int knob=std::min(255,alpha*2/3);
            for(int k=0;k<KnobsPerSegment;++k) {
                const float t0=(static_cast<float>(k)+0.15f)/KnobsPerSegment*length,t1=t0+0.4f/KnobsPerSegment*length;
                for(int side=0;side<2;++side) {
                    const float offset=(k+side)%2==0?0.0f:0.5f/KnobsPerSegment*length;
                    const float b0=side==0?-half*0.72f:half*0.08f,b1=side==0?-half*0.08f:half*0.72f;
                    const float u0=t0+offset,u1=std::min(t1+offset,length);
                    if(u1<=u0) continue;
                    quad(on(track.x0+dx*u0+sx*b0,track.z0+dz*u0+sz*b0),on(track.x0+dx*u1+sx*b0,track.z0+dz*u1+sz*b0),
                         on(track.x0+dx*u1+sx*b1,track.z0+dz*u1+sz*b1),on(track.x0+dx*u0+sx*b1,track.z0+dz*u0+sz*b1),
                         knob,knob,knob,knob);
                }
            }
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
    ++drawCalls_;
}
}
