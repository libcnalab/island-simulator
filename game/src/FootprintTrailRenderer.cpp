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
#include <limits>
#include <numbers>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
FootprintTrailRenderer::FootprintTrailRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        BufferVertices,BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(BufferVertices));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: the walker's bare feet leave a trail of prints in the sand that the sand takes back over four minutes";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void FootprintTrailRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                                  const std::span<const Footprint> prints) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    // a print on planks lies on the deck at its own height, not on the ground under it
    float deck=std::numeric_limits<float>::quiet_NaN();
    const auto heightAt=[&](const float x,const float z){
        return std::isfinite(deck)?deck+0.006f:DrawnOrAnalyticHeight(ground_,terrain_,x,z)+0.012f;
    };
    const auto fan=[&](float cx,float cz,float forwardX,float forwardZ,float forwardRadius,float sideRadius,int alpha){
        if(staging_.size()+static_cast<std::size_t>(Segments*3)>static_cast<std::size_t>(BufferVertices)) return;
        const float sideX=-forwardZ,sideZ=forwardX;
        const Vector3 centre(cx,heightAt(cx,cz),cz);
        const Color core(0,0,0,alpha),rim(0,0,0,0);
        std::array<Vector3,Segments> edge;
        for(int segment=0;segment<Segments;++segment) {
            const float angle=static_cast<float>(segment)*2.0f*std::numbers::pi_v<float>/static_cast<float>(Segments);
            const float along=std::cos(angle)*forwardRadius,across=std::sin(angle)*sideRadius;
            const float ex=cx+forwardX*along+sideX*across,ez=cz+forwardZ*along+sideZ*across;
            edge[static_cast<std::size_t>(segment)]=Vector3(ex,heightAt(ex,ez),ez);
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
        deck=print.y;
        if(print.kind==2U) {
            // A tyrannosaur's three-toed print, some 0.7 m long: the broad heel pad and three
            // long toes splayed forward from it, the middle one longest, each ending in a claw mark.
            const int deep=std::min(255,alpha*2);
            fan(print.x-fx*0.12f,print.z-fz*0.12f,fx,fz,0.20f,0.17f,deep);
            for(int toe=-1;toe<=1;++toe) {
                const float splay=static_cast<float>(toe)*0.38f;
                const float tx=std::cos(print.headingRadians+splay),tz=std::sin(print.headingRadians+splay);
                const float length=toe==0?0.46f:0.38f;
                fan(print.x+tx*length*0.55f,print.z+tz*length*0.55f,tx,tz,length*0.52f,0.075f,deep);
            }
            ++visible_;
            continue;
        }
        if(print.kind==3U||print.kind==4U) {
            // An elephant's print: a broad round dish where the forefoot's sole pressed (an oval,
            // longer than broad, behind), and the toenails' crescent along its front edge.
            const bool fore=print.kind==3U;
            const int deep=std::min(255,alpha*3/2);
            fan(print.x,print.z,fx,fz,fore?0.25f:0.28f,fore?0.24f:0.19f,deep);
            fan(print.x+fx*(fore?0.20f:0.22f),print.z+fz*(fore?0.20f:0.22f),fx,fz,0.05f,fore?0.17f:0.13f,deep);
            ++visible_;
            continue;
        }
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
