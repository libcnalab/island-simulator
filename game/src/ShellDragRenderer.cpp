#include "IslandSimulator/ShellDragRenderer.hpp"
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
#include <cmath>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
ShellDragRenderer::ShellDragRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),terrain_(terrain),ground_(ground),effect_(std::make_unique<BasicEffect>(device)),
     vertices_(std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        VerticesPerSegment*static_cast<int>(ShellDragTrail::Capacity),BufferUsage::WriteOnly))
{
    staging_.reserve(static_cast<std::size_t>(VerticesPerSegment)*ShellDragTrail::Capacity);
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    const char* message="island-simulator: the hermit crabs drag thin lines in the sand behind their shells that the sand levels within minutes";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void ShellDragRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                             const std::span<const DragSegment> segments) {
    drawCalls_=0;visible_=0;
    staging_.clear();
    for(const auto& segment:segments) {
        if(!segment.active||!std::isfinite(segment.x0)||!std::isfinite(segment.z0)||!std::isfinite(segment.x1)||!std::isfinite(segment.z1)) continue;
        const float distance=std::hypot(segment.x1-camera.X,segment.z1-camera.Z);
        if(distance>30.0f) continue;
        const int alpha=static_cast<int>(std::lround(64.0f*std::clamp(segment.density,0.0f,1.0f)*(1.0f-std::clamp((distance-20.0f)/10.0f,0.0f,1.0f))));
        if(alpha<3) continue;
        const float dx=segment.x1-segment.x0,dz=segment.z1-segment.z0;
        const float length=std::hypot(dx,dz);
        if(length<1e-4f) continue;
        const float nx=-dz/length*0.011f,nz=dx/length*0.011f;
        const Color ink(0,0,0,alpha);
        const auto at=[&](float x,float z){return Vector3(x,DrawnOrAnalyticHeight(ground_,terrain_,x,z)+0.008f,z);};
        const Vector3 a=at(segment.x0+nx,segment.z0+nz),b=at(segment.x0-nx,segment.z0-nz);
        const Vector3 c=at(segment.x1-nx,segment.z1-nz),d=at(segment.x1+nx,segment.z1+nz);
        staging_.emplace_back(a,ink);staging_.emplace_back(b,ink);staging_.emplace_back(c,ink);
        staging_.emplace_back(a,ink);staging_.emplace_back(c,ink);staging_.emplace_back(d,ink);
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
