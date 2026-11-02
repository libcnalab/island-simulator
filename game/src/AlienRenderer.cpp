#include "IslandSimulator/AlienRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/AlienBodyMesh.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr int Capacity=16000;
Vector3 V(const AlienPoint& p) {return {p.x,p.y,p.z};}

}

AlienRenderer::AlienRenderer(GraphicsDevice& device,const float maximumDistance)
    :device_(device),maximumDistance_(maximumDistance),effect_(std::make_unique<BasicEffect>(device)) {
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),Capacity,BufferUsage::WriteOnly);
    vertices_.reserve(Capacity);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);
    effect_->setFogEndProperty(760.0f);
}

AlienRenderer::~AlienRenderer()=default;

void AlienRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const AlienMotion& alien) {
    visible_=false;triangles_=0;
    const AlienState& s=alien.State();
    const Vector3 middle=V(s.pelvis);
    if(Vector3::DistanceSquared(middle,camera)>maximumDistance_*maximumDistance_) return;
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(middle,1.2f))) return;
    BuildAlienBodyMesh(s,vertices_);
    triangles_=static_cast<int>(vertices_.size()/3);
    if(vertices_.empty()||vertices_.size()>static_cast<std::size_t>(Capacity)) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(vertices_.data(),0,static_cast<int>(vertices_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    visible_=true;
}
}
