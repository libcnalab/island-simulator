#include "IslandSimulator/TermiteNestRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
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
#include <sstream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Sectors=8;
// the pigments, past full for the build's light: the carton dark, its fresh repairs and the
// runways' mud lighter and redder
const Vector3 Carton(84,64,48),Repair(124,96,70),Runway(136,102,72);
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

// A covered runway up or down the bark from local height `from` to `to`, meandering round the
// trunk from `angle`: a low ridge of mud a few centimetres wide pressed onto the bark.
void BuildRunway(SculptMeshBuilder& b,const CanopyPlacement& tree,const float from,const float to,const float angle,
                 const float width,const std::uint32_t seed) {
    const float span=std::abs(to-from);
    const int steps=std::max(3,static_cast<int>(span/0.09f));
    const float phase=2.0f*Pi*Hash(seed),phase2=2.0f*Pi*Hash(seed^0x3U),drift=0.5f*(Hash(seed^0x5U)-0.5f);
    std::vector<Vector3> points,normals;
    for(int i=0;i<=steps;++i) {
        const float u=static_cast<float>(i)/static_cast<float>(steps);
        const float local=from+(to-from)*u;
        const float a=angle+drift*u*span+0.16f*std::sin(local*1.9f+phase)+0.06f*std::sin(local*5.3f+phase2);
        const auto bark=TermiteNestLayout::Bark(tree,local,a);
        points.emplace_back(bark[0],bark[1],bark[2]);
        Vector3 n(bark[3],bark[4],bark[5]);
        if(n.LengthSquared()<1e-8f) n=Vector3::UnitX;
        n.Normalize();
        normals.push_back(n);
    }
    for(int i=0;i<steps;++i) {
        const auto i0=static_cast<std::size_t>(i),i1=i0+1U;
        Vector3 along=points[i1]-points[i0];
        if(along.LengthSquared()<1e-10f) continue;
        along.Normalize();
        // the runway narrows toward its far end, and swells now and then where it was patched
        const auto ridge=[&](const std::size_t k) {
            const float u=static_cast<float>(k)/static_cast<float>(steps);
            const float w=width*(1.0f-0.35f*u)*(0.85f+0.3f*Hash(seed^static_cast<std::uint32_t>(k*7U+11U)));
            Vector3 side=Vector3::Cross(normals[k],along);
            if(side.LengthSquared()<1e-8f) side=Vector3::UnitX;
            side.Normalize();
            return std::array<Vector3,3>{{points[k]+side*w-normals[k]*0.006f,points[k]+normals[k]*(w*0.38f),points[k]-side*w-normals[k]*0.006f}};
        };
        const auto r0=ridge(i0),r1=ridge(i1);
        const Vector3 shade=Runway*(0.9f+0.18f*Hash(seed^static_cast<std::uint32_t>(i*13+5)));
        b.inside=(points[i0]+points[i1])*0.5f-(normals[i0]+normals[i1])*0.05f;
        b.Quad(r0[0],r1[0],r1[1],r0[1],shade);
        b.Quad(r0[1],r1[1],r1[2],r0[2],shade);
    }
}

void BuildNest(SculptMeshBuilder& b,const TermiteNest& nest,const CanopyPlacement& tree) {
    const auto h=[&](const std::uint32_t salt) {return Hash(nest.seed*0x9e3779b9U^salt);};
    const Vector3 bark(nest.bark[0],nest.bark[1],nest.bark[2]);
    const Vector3 n=Vector3::Normalize(Vector3(nest.normal[0],0.0f,nest.normal[2]));
    const Vector3 t=Vector3::Cross(Vector3::Up,n);
    // an egg hugging the bark: its front swelling out, its back sunk into the trunk to past the
    // axis, so where it is wider than the trunk it wraps round it
    const float half=nest.height*0.5f,side=nest.width*0.5f;
    const float front=nest.width*0.48f,back=nest.trunkRadius+nest.width*0.18f;
    const Vector3 centre=bark+n*(nest.width*0.2f);
    // the lobes: a few broad swellings where the colony built on, and many small knobs over them
    std::array<Vector3,6> lobeAt{};std::array<float,6> lobeSize{};
    for(std::size_t k=0;k<lobeAt.size();++k) {
        const auto salt=static_cast<std::uint32_t>(k*31U+100U);
        const float theta=Pi*(h(salt)-0.5f)*1.3f,phi=Pi*(h(salt^0x1U)-0.5f)*0.95f;
        lobeAt[k]=Vector3(std::sin(theta)*std::cos(phi),std::sin(phi),std::cos(theta)*std::cos(phi));
        lobeSize[k]=0.14f+0.22f*h(salt^0x2U);
    }
    std::array<Vector3,44> knobAt{};
    for(std::size_t k=0;k<knobAt.size();++k) {
        const auto salt=static_cast<std::uint32_t>(k*17U+300U);
        knobAt[k]=Vector3::Normalize(Vector3(h(salt)-0.5f,h(salt^0x1U)-0.5f,h(salt^0x2U)-0.15f));
    }
    const float r1=2.0f*Pi*h(1U),r2=2.0f*Pi*h(2U),r3=2.0f*Pi*h(3U);
    // the knobbly relief, positive on a knob's crown and negative in the crevices between
    const auto relief=[&](const Vector3& dir) {
        float knobs=0.0f;
        for(const Vector3& k:knobAt) knobs=std::max(knobs,std::exp(-(1.0f-Vector3::Dot(dir,k))*38.0f));
        const float fine=std::sin(dir.X*17.0f+r1)*std::sin(dir.Y*15.0f+r2)*std::sin(dir.Z*16.0f+r3);
        return 0.6f*knobs+0.4f*fine;
    };
    constexpr int Around=32,Bands=26;
    const auto index=[](const int j,const int i) {return static_cast<std::size_t>(j*Around+(i+Around)%Around);};
    std::vector<Vector3> grid(static_cast<std::size_t>((Bands+1)*Around)),albedo(grid.size());
    for(int j=0;j<=Bands;++j) {
        const float phi=-0.5f*Pi+Pi*static_cast<float>(j)/static_cast<float>(Bands);
        for(int i=0;i<Around;++i) {
            const float theta=2.0f*Pi*static_cast<float>(i)/static_cast<float>(Around)-Pi;
            const Vector3 dir(std::sin(theta)*std::cos(phi),std::sin(phi),std::cos(theta)*std::cos(phi));
            float bump=1.0f+0.06f*std::sin(3.0f*theta+r1)*std::sin(2.0f*phi+r2);
            for(std::size_t k=0;k<lobeAt.size();++k) bump+=lobeSize[k]*std::exp(-(1.0f-Vector3::Dot(dir,lobeAt[k]))*5.0f);
            const float r=relief(dir);
            bump+=0.09f*r;
            // pear-shaped, broader and longer below, slumped where the wet carton sagged
            const float below=std::max(0.0f,-std::sin(phi));
            const float broad=1.0f+0.18f*below-0.12f*std::max(0.0f,std::sin(phi));
            const float vertical=std::sin(phi)<0.0f?half*1.12f:half*0.9f;
            const float depth=dir.Z>0.0f?front:back;
            grid[index(j,i)]=centre+t*(dir.X*side*broad*bump)+n*(dir.Z*depth*broad*bump)+Vector3::Up*(dir.Y*vertical*bump);
            // the crevices darker, the knobs' crowns lighter, a fresh repair here and there paler
            const float repair=std::clamp((std::sin(dir.X*4.0f+dir.Z*3.0f+r2)+std::sin(dir.Y*5.0f+r1))*1.3f-1.7f,0.0f,1.0f);
            albedo[index(j,i)]=Vector3::Lerp(Carton*(0.78f+0.62f*r),Repair,repair*0.7f);
        }
    }
    // lit at each corner by the grid's own normal, so the carton shades smoothly over its knobs
    const auto lit=[&](const int j,const int i) {
        const Vector3 du=grid[index(j,i+1)]-grid[index(j,i-1)];
        const Vector3 dv=grid[index(std::min(j+1,Bands),i)]-grid[index(std::max(j-1,0),i)];
        Vector3 normal=Vector3::Cross(du,dv);
        const Vector3 p=grid[index(j,i)];
        if(normal.LengthSquared()<1e-12f) normal=p-centre;
        normal.Normalize();
        if(Vector3::Dot(normal,p-centre)<0.0f) normal=-normal;
        const float light=0.46f+0.54f*std::max(0.0f,Vector3::Dot(normal,b.builder.sun));
        const Vector3 c=albedo[index(j,i)]*light;
        return VertexPositionColor(p,Color(static_cast<int>(std::min(255.0f,c.X)),static_cast<int>(std::min(255.0f,c.Y)),
                                           static_cast<int>(std::min(255.0f,c.Z)),255));
    };
    auto& mesh=b.builder.mesh;
    for(int j=0;j<Bands;++j) for(int i=0;i<Around;++i) {
        const auto v00=lit(j,i),v01=lit(j,i+1),v10=lit(j+1,i),v11=lit(j+1,i+1);
        mesh.push_back(v00);mesh.push_back(v11);mesh.push_back(v01);
        mesh.push_back(v00);mesh.push_back(v10);mesh.push_back(v11);
    }
    // the runways: two down the trunk from the nest's foot to the ground, one up toward the crown
    const float foot=nest.localHeight-half*0.9f/tree.scale,crown=nest.localHeight+half*0.8f/tree.scale;
    const float spread=0.5f*side/std::max(0.15f,nest.trunkRadius);
    BuildRunway(b,tree,foot,0.0f,nest.angle-spread*(0.3f+0.4f*h(4U)),0.03f,nest.seed^0x51U);
    BuildRunway(b,tree,foot,0.0f,nest.angle+spread*(0.3f+0.4f*h(5U)),0.024f,nest.seed^0x52U);
    BuildRunway(b,tree,crown,crown+(2.4f+1.4f*h(6U))/tree.scale,nest.angle+0.4f*(h(7U)-0.5f),0.022f,nest.seed^0x53U);
}
}

TermiteNestRenderer::TermiteNestRenderer(GraphicsDevice& device,const std::span<const TermiteNest> nests,
                                         const std::span<const CanopyPlacement> canopy)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    std::vector<VertexPositionColor> mesh;
    for(int sector=0;sector<Sectors;++sector) {
        SculptMeshBuilder b;
        int count=0;
        for(const TermiteNest& nest:nests) {
            if(nest.tree>=canopy.size()) continue;
            const float angle=std::atan2(nest.bark[2],nest.bark[0])+Pi;
            const int index=std::min(Sectors-1,static_cast<int>(angle/(2.0f*Pi)*Sectors));
            if(index!=sector) continue;
            BuildNest(b,nest,canopy[nest.tree]);
            ++count;
        }
        if(count==0) continue;
        Sector s;
        s.first=static_cast<int>(mesh.size());
        s.triangles=static_cast<int>(b.builder.mesh.size()/3U);
        Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
        for(const auto& v:b.builder.mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
        s.bounds=BoundingBox(low-Vector3(0.5f,0.5f,0.5f),high+Vector3(0.5f,0.5f,0.5f));
        mesh.insert(mesh.end(),b.builder.mesh.begin(),b.builder.mesh.end());
        sectors_.push_back(s);
    }
    if(!mesh.empty()) {
        vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
        vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    }
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(220.0f);effect_->setFogEndProperty(520.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"termite-nests",false,220.0f,520.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: termite nest sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: "<<nests.size()<<" arboreal termite nests hug jungle trunks, their mud runways down the bark; "
           <<mesh.size()/3U<<" triangles in "<<sectors_.size()<<" sectors";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void TermiteNestRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!vertices_) return;
    const BoundingFrustum frustum(view*projection);
    bool applied=false;
    for(const Sector& s:sectors_) {
        const Vector3 nearest(std::clamp(camera.X,s.bounds.Min.X,s.bounds.Max.X),std::clamp(camera.Y,s.bounds.Min.Y,s.bounds.Max.Y),
                              std::clamp(camera.Z,s.bounds.Min.Z,s.bounds.Max.Z));
        if((nearest-camera).LengthSquared()>DrawDistance*DrawDistance||frustum.Contains(s.bounds)==ContainmentType::Disjoint) continue;
        if(!applied) {
            device_.setRasterizerStateProperty(RasterizerState::CullNone);
            device_.setBlendStateProperty(BlendState::Opaque);
            device_.setDepthStencilStateProperty(DepthStencilState::Default);
            device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
            if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
            else {
                effect_->setWorldProperty(Matrix::getIdentityProperty());
                effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
            }
            applied=true;
        }
        device_.DrawPrimitives(PrimitiveType::TriangleList,s.first,s.triangles);
        ++drawCalls_;
    }
}
}
