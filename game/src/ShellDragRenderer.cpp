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
        static_cast<int>(std::max(VerticesPerSegment*ShellDragTrail::Capacity,3*VerticesPerSegment*CrocodileDragTrail::Capacity)),
        BufferUsage::WriteOnly))
{
    // (a crocodile's groove is three strips across, a shell's line one)
    staging_.reserve(std::max(VerticesPerSegment*ShellDragTrail::Capacity,3*VerticesPerSegment*CrocodileDragTrail::Capacity));
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
        // a wide groove (a crocodile's) reads from further than a shell's thin line
        const float wide=std::clamp((segment.halfWidth-0.011f)/0.06f,0.0f,1.0f);
        const float fadeStart=20.0f+30.0f*wide;
        if(distance>fadeStart+10.0f) continue;
        const int alpha=static_cast<int>(std::lround((64.0f-14.0f*wide)*std::clamp(segment.density,0.0f,1.0f)
            *(1.0f-std::clamp((distance-fadeStart)/10.0f,0.0f,1.0f))));
        if(alpha<3) continue;
        const float dx=segment.x1-segment.x0,dz=segment.z1-segment.z0;
        const float length=std::hypot(dx,dz);
        if(length<1e-4f) continue;
        const float sx=-dz/length,sz=dx/length;
        const auto at=[&](float x,float z){return Vector3(x,DrawnOrAnalyticHeight(ground_,terrain_,x,z)+0.008f,z);};
        // across the line: a soft lip, the pressed floor, a soft lip (the thin line is all floor)
        const float half=segment.halfWidth;
        const std::array<float,4> across=wide>0.0f?std::array<float,4>{-half,-half*0.45f,half*0.45f,half}
                                                  :std::array<float,4>{-half,-half,half,half};
        constexpr std::array<int,4> floorWeight{0,1,1,0};
        for(int column=0;column<3;++column) {
            const float a0=across[static_cast<std::size_t>(column)],a1=across[static_cast<std::size_t>(column+1)];
            if(a1-a0<1e-5f) continue;
            const int alphaA=wide>0.0f?floorWeight[static_cast<std::size_t>(column)]*alpha:alpha;
            const int alphaB=wide>0.0f?floorWeight[static_cast<std::size_t>(column+1)]*alpha:alpha;
            const Vector3 p0=at(segment.x0+sx*a0,segment.z0+sz*a0),p1=at(segment.x1+sx*a0,segment.z1+sz*a0);
            const Vector3 p2=at(segment.x1+sx*a1,segment.z1+sz*a1),p3=at(segment.x0+sx*a1,segment.z0+sz*a1);
            staging_.emplace_back(p0,Color(0,0,0,alphaA));staging_.emplace_back(p1,Color(0,0,0,alphaA));
            staging_.emplace_back(p2,Color(0,0,0,alphaB));
            staging_.emplace_back(p0,Color(0,0,0,alphaA));staging_.emplace_back(p2,Color(0,0,0,alphaB));
            staging_.emplace_back(p3,Color(0,0,0,alphaB));
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
