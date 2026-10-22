#include "IslandSimulator/PandanusRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
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
#include <functional>
#include <iostream>
#include <numbers>
#include <sstream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Sectors=8;
// the pigments, past full for the build's light
const Vector3 Trunk(138,128,112),Root(156,120,84),RootTip(176,110,62),Young(74,126,54),Leaf(92,130,58),Old(158,148,76),
    Dead(134,104,70),FruitRipe(226,124,44),FruitGreen(112,146,62);
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

void BuildTree(SculptMeshBuilder& b,const PandanusPlacement& tree,const std::function<float(float,float)>& groundAt) {
    const auto h=[&](const std::uint32_t salt) {return Hash(tree.seed*0x9e3779b9U^salt);};
    const Vector3 base(tree.x,groundAt(tree.x,tree.z),tree.z);
    const Vector3 sea(tree.seaX,0.0f,tree.seaZ);
    const Vector3 axis=Vector3::Normalize(Vector3::Up*std::cos(tree.lean)+sea*std::sin(tree.lean));
    Vector3 p=Vector3::Cross(axis,sea);
    if(p.LengthSquared()<1e-6f) p=Vector3::UnitX;
    p.Normalize();
    const Vector3 q=Vector3::Cross(p,axis);
    const float height=tree.height;
    // the stilt roots: a cone of them from the trunk's lower metre out to the ground
    const float rootTop=0.9f+0.6f*h(1U);
    const int roots=10+static_cast<int>(h(2U)*7.0f);
    for(int i=0;i<roots;++i) {
        const float phi=2.0f*Pi*static_cast<float>(i)/static_cast<float>(roots)+0.4f*(h(10U+static_cast<std::uint32_t>(i))-0.5f);
        const Vector3 out=p*std::cos(phi)+q*std::sin(phi);
        // stout and nearly straight, set at a steep slant like a tent's guys
        const Vector3 origin=base+axis*(rootTop*(0.35f+0.65f*h(30U+static_cast<std::uint32_t>(i))))+out*0.1f;
        const float reach=0.38f+0.42f*h(50U+static_cast<std::uint32_t>(i));
        Vector3 foot=base+Vector3(out.X,0.0f,out.Z)*reach;
        foot.Y=groundAt(foot.X,foot.Z)-0.06f;
        const Vector3 mid=(origin+foot)*0.5f+out*0.06f+Vector3::Up*0.03f;
        b.Tube({origin,mid,foot},{0.05f,0.056f,0.064f},6,i%3==0?RootTip:Root);
    }
    // the trunk, thin below the roots' tops, and the fork
    const Vector3 lower=base+axis*rootTop,fork=base+axis*(0.55f*height)+sea*(0.15f*h(3U));
    b.Tube({base-Vector3::Up*0.1f,base+axis*(rootTop*0.5f),lower,(lower+fork)*0.5f+p*(0.08f*(h(4U)-0.5f)),fork},
           {0.06f,0.08f,0.15f,0.13f,0.11f},8,Trunk*(0.92f+0.12f*h(5U)));
    const int forks=std::max(1,static_cast<int>(tree.forks));
    for(int j=0;j<forks;++j) {
        const float psi=2.0f*Pi*static_cast<float>(j)/static_cast<float>(forks)+2.0f*Pi*h(6U);
        const float tilt=forks==1?0.12f:0.45f+0.25f*h(70U+static_cast<std::uint32_t>(j));
        const Vector3 dir=Vector3::Normalize(axis*std::cos(tilt)+(p*std::cos(psi)+q*std::sin(psi))*std::sin(tilt));
        const float length=0.42f*height*(0.8f+0.4f*h(80U+static_cast<std::uint32_t>(j)));
        const Vector3 tip=fork+dir*length+Vector3::Up*(0.15f*length);
        b.Tube({fork,fork+dir*(length*0.5f)+Vector3::Up*0.05f,tip},{0.1f,0.085f,0.07f},7,Trunk);
        // the tuft: thirty leaves in a spiral, the old ones low and drooping, the young erect
        const Vector3 up=Vector3::Normalize(dir+Vector3::Up*1.2f);
        Vector3 u1=Vector3::Cross(up,Vector3::UnitX);
        if(u1.LengthSquared()<1e-6f) u1=Vector3::UnitZ;
        u1.Normalize();
        const Vector3 u2=Vector3::Cross(up,u1);
        b.inside=tip-up*1.2f;
        constexpr int Leaves=30;
        for(int k=0;k<Leaves;++k) {
            const auto salt=static_cast<std::uint32_t>(j*97+k);
            const float rank=static_cast<float>(k)/(Leaves-1);
            const float around=static_cast<float>(k)*2.39996f+h(salt);
            const float elevation=-0.55f+1.45f*rank;
            const Vector3 flat=u1*std::cos(around)+u2*std::sin(around);
            Vector3 direction=Vector3::Normalize(flat*std::cos(elevation)+up*std::sin(elevation));
            const float leafLength=1.05f+0.6f*h(salt^0x5U);
            const Vector3 colour=(rank<0.22f?Old:(rank>0.7f?Young:Leaf))*(0.9f+0.18f*h(salt^0x7U));
            Vector3 at=tip+up*(0.05f+0.25f*rank);
            Vector3 side=Vector3::Cross(direction,up);
            if(side.LengthSquared()<1e-6f) side=u1;
            side.Normalize();
            constexpr std::array<float,4> Width{{0.075f,0.065f,0.045f,0.0f}};
            for(int s=0;s<3;++s) {
                // each third bends further down, the old leaves the most
                const Vector3 bend=Vector3::Normalize(direction-Vector3::Up*(0.22f*static_cast<float>(s+1)*(1.1f-0.6f*rank)));
                const Vector3 next=at+bend*(leafLength/3.0f);
                b.Quad(at-side*Width[static_cast<std::size_t>(s)],at+side*Width[static_cast<std::size_t>(s)],
                       next+side*Width[static_cast<std::size_t>(s+1)],next-side*Width[static_cast<std::size_t>(s+1)],colour);
                at=next;direction=bend;
            }
        }
        // the dead leaves hanging in a skirt under the tuft
        for(int k=0;k<8;++k) {
            const auto salt=static_cast<std::uint32_t>(j*131+k+500);
            const float around=2.0f*Pi*static_cast<float>(k)/8.0f+h(salt);
            const Vector3 outward=u1*std::cos(around)+u2*std::sin(around);
            const Vector3 from=tip-up*0.12f+outward*0.06f;
            const Vector3 to=from+Vector3::Normalize(-Vector3::Up+outward*0.35f)*(0.85f+0.35f*h(salt^0x3U));
            Vector3 side=Vector3::Cross(to-from,Vector3::Up);
            if(side.LengthSquared()<1e-6f) side=u1;
            side.Normalize();
            b.inside=from-outward*0.4f;
            b.Quad(from-side*0.06f,from+side*0.06f,to+side*0.02f,to-side*0.02f,Dead*(0.85f+0.25f*h(salt^0x9U)));
        }
        // a fruit hanging under the first tuft: a pineapple-like head of wedges
        if(j==0&&tree.fruiting) {
            const Vector3 stalk=tip-up*0.18f+u1*0.1f,top=stalk-Vector3::Up*0.12f,bottom=top-Vector3::Up*0.34f;
            b.Tube({stalk,top},{0.02f,0.02f},4,Trunk);
            b.Tube({top,top-Vector3::Up*0.08f,(top+bottom)*0.5f,bottom-Vector3::Up*0.02f+Vector3::Up*0.06f,bottom},
                   {0.08f,0.15f,0.18f,0.14f,0.05f},10,h(9U)<0.5f?FruitRipe:FruitGreen);
        }
    }
}
}

PandanusRenderer::PandanusRenderer(GraphicsDevice& device,const std::span<const PandanusPlacement> trees,
                                   const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    const std::function<float(float,float)> groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    std::vector<VertexPositionColor> mesh;
    int fruiting=0;
    for(int sector=0;sector<Sectors;++sector) {
        SculptMeshBuilder b;
        int count=0;
        for(const PandanusPlacement& tree:trees) {
            const float angle=std::atan2(tree.z,tree.x)+Pi;
            const int index=std::min(Sectors-1,static_cast<int>(angle/(2.0f*Pi)*Sectors));
            if(index!=sector) continue;
            BuildTree(b,tree,groundAt);
            ++count;
            fruiting+=tree.fruiting?1:0;
        }
        if(count==0) continue;
        Sector s;
        s.first=static_cast<int>(mesh.size());
        s.triangles=static_cast<int>(b.builder.mesh.size()/3U);
        Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
        for(const auto& v:b.builder.mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
        s.bounds=BoundingBox(low-Vector3(0.5f,0.5f,0.5f),high+Vector3(0.5f,0.5f,0.5f));
        s.centre=(low+high)*0.5f;
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
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"pandanus",false,220.0f,520.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: pandanus sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: "<<trees.size()<<" screw pines stand in groups on the coastal fringe on their stilt roots, "
           <<fruiting<<" of them fruiting; "<<mesh.size()/3U<<" triangles in "<<sectors_.size()<<" sectors";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void PandanusRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;visibleSectors_=0;
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
        ++drawCalls_;++visibleSectors_;
    }
}

void PandanusRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!vertices_) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"pandanus");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: pandanus sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    bool applied=false;
    for(const Sector& s:sectors_) {
        // the sector's trees reach the map wherever any corner of its box does
        bool casts=map.CastsInto(s.centre.X,s.centre.Z);
        for(const float x:{s.bounds.Min.X,s.bounds.Max.X}) for(const float z:{s.bounds.Min.Z,s.bounds.Max.Z}) casts=casts||map.CastsInto(x,z);
        if(!casts) continue;
        if(!applied) {
            device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
            sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
            applied=true;
        }
        device_.DrawPrimitives(PrimitiveType::TriangleList,s.first,s.triangles);
        map.CountCasters(1);
    }
}
}
