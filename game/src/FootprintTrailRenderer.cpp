#include "IslandSimulator/FootprintTrailRenderer.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
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
FootprintTrailRenderer::FootprintTrailRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerPrint*static_cast<int>(FootprintTrail::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerPrint)*FootprintTrail::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: the walker's bare feet leave a trail of prints in the sand that the sand takes back over four minutes";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void FootprintTrailRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                  const std::span<const Footprint> prints) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    const auto fan=[&](float cx,float cz,float forwardX,float forwardZ,float forwardRadius,float sideRadius,int alpha){
        const float sideX=-forwardZ,sideZ=forwardX;
        const Vector3 centre(cx,DrawnOrAnalyticHeight(ground_,terrain_,cx,cz)+0.012f,cz);
        const Color core(0,0,0,alpha),rim(0,0,0,0);
        std::array<Vector3,Segments> edge;
        for(int segment=0;segment<Segments;++segment) {
            const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
            const float along=std::cos(angle)*forwardRadius,across=std::sin(angle)*sideRadius;
            const float ex=cx+forwardX*along+sideX*across,ez=cz+forwardZ*along+sideZ*across;
            edge[static_cast<std::size_t>(segment)]=Vector3(ex,DrawnOrAnalyticHeight(ground_,terrain_,ex,ez)+0.012f,ez);
        }
        for(int segment=0;segment<Segments;++segment) {
            staging_.emplace_back(centre,core);
            staging_.emplace_back(edge[static_cast<std::size_t>(segment)],rim);
            staging_.emplace_back(edge[static_cast<std::size_t>((segment+1)%Segments)],rim);
        }
    };
    for(const auto& print:prints) {
        if(!print.active||!std::isfinite(print.x)||!std::isfinite(print.z)) continue;
        const float distance=std::hypot(print.x-camera.X,print.z-camera.Z);
        if(distance>45.0f) continue;
        const int alpha=static_cast<int>(std::lround(78.0f*std::clamp(print.density,0.0f,1.0f)*(1.0f-std::clamp((distance-30.0f)/15.0f,0.0f,1.0f))));
        if(alpha<3) continue;
        const float fx=std::cos(print.headingRadians),fz=std::sin(print.headingRadians);
        if(print.kind==1U) {
            // A cloven hoof: two narrow toes side by side, splayed a little at the front.
            for(const float toe:{-1.0f,1.0f}) {
                const float across=toe*0.03f;
                const float dx=fx-fz*toe*0.18f,dz=fz+fx*toe*0.18f;
                const float length=std::hypot(dx,dz);
                fan(print.x-fz*across,print.z+fx*across,dx/length,dz/length,0.055f,0.021f,alpha);
            }
            ++visible_;
            continue;
        }
        // The sole behind, the toe pad ahead of it, a little to the outside.
        fan(print.x-fx*0.03f,print.z-fz*0.03f,fx,fz,0.10f,0.045f,alpha);
        const float toeSide=print.left?0.012f:-0.012f;
        fan(print.x+fx*0.105f-fz*toeSide,print.z+fz*0.105f+fx*toeSide,fx,fz,0.035f,0.05f,alpha);
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
