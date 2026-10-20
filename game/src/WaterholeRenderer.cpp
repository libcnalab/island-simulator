#include "IslandSimulator/WaterholeRenderer.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
Color Premultiplied(int r,int g,int b,int a){return Color(r*a/255,g*a/255,b*a/255,a);}
}
WaterholeRenderer::WaterholeRenderer(GraphicsDevice& device,const IslandTerrain& terrain,
                                     const float x,const float z,const float radius)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)),centre_(x,terrain.Height(x,z),z) {
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    std::vector<VertexPositionColor> mesh;
    const auto at=[&](float r,float a,float lift){
        const float px=x+std::cos(a)*r,pz=z+std::sin(a)*r;
        return Vector3(px,terrain.Height(px,pz)+lift,pz);
    };
    const float wet=radius*1.45f;
    for(int i=0;i<Segments;++i) {
        const float a0=static_cast<float>(i)*2.0f*std::numbers::pi_v<float>/Segments;
        const float a1=static_cast<float>(i+1)*2.0f*std::numbers::pi_v<float>/Segments;
        // The water: dark and still at the middle, catching the sky toward the rim.
        const float sky0=.5f+.5f*std::cos(a0-2.4f),sky1=.5f+.5f*std::cos(a1-2.4f);
        const Color middle=Premultiplied(38,52,58,236);
        const Color rim0=Premultiplied(static_cast<int>(58+60*sky0),static_cast<int>(72+60*sky0),static_cast<int>(74+66*sky0),226);
        const Color rim1=Premultiplied(static_cast<int>(58+60*sky1),static_cast<int>(72+60*sky1),static_cast<int>(74+66*sky1),226);
        mesh.emplace_back(Vector3(x,terrain.Height(x,z)+.035f,z),middle);
        mesh.emplace_back(at(radius,a0,.035f),rim0);
        mesh.emplace_back(at(radius,a1,.035f),rim1);
        // The wet soil around it, dark at the water and fading to the dry slope.
        const Color mud=Premultiplied(56,44,32,170),clear(0,0,0,0);
        const auto i0=at(radius,a0,.03f),i1=at(radius,a1,.03f),o0=at(wet,a0,.02f),o1=at(wet,a1,.02f);
        mesh.emplace_back(i0,mud);mesh.emplace_back(o0,clear);mesh.emplace_back(o1,clear);
        mesh.emplace_back(i0,mud);mesh.emplace_back(o1,clear);mesh.emplace_back(i1,mud);
    }
    count_=static_cast<int>(mesh.size());
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),count_,BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),count_);
    const char* message="island-simulator: the goats' slope holds a rain-filled rock basin, a still dark pool in a ring of wet soil where they drink";
    std::cout<<message<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message);
}

void WaterholeRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(count_==0||Vector3::Distance(camera,centre_)>90.0f) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,count_/3);
    drawCalls_=1;
}
}
