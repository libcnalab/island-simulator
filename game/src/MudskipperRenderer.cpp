#include "IslandSimulator/MudskipperRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WildlifeRenderLighting.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr float FishLength=.16f;   // a mudskipper some 16 cm long
using TV=MudskipperRenderer::TemplateVertex;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
void Tri(std::vector<TV>& out,const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour,const std::uint8_t part=0) {
    const Color lit=wildlife_render::FacetColor(a,b,c,colour,true);
    for(const Vector3& p:{a,b,c}) out.push_back({p.X,p.Y,p.Z,lit.getRProperty(),lit.getGProperty(),lit.getBProperty(),part});
}
void Both(std::vector<TV>& out,const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour,const std::uint8_t part=0) {
    Tri(out,a,b,c,colour,part);Tri(out,c,b,a,colour,part);
}
// One fish a unit long in its own frame: x right, y up, z ahead; its belly on y = 0.
void BuildFish(std::vector<TV>& out) {
    const Color back(96,88,64),flank(122,112,82),belly(170,160,128),spot(150,186,196),fin(84,74,56),eye(40,38,30),eyeRing(196,170,96);
    // the body: rings from the tail's root to the blunt head, flat-bellied, deepest behind the head
    struct Ring {float z,w,h;};
    // (the head big and blunt, standing higher than the body, which tapers away to the tail)
    constexpr std::array<Ring,7> Rings{{{-.42f,.030f,.045f},{-.30f,.050f,.075f},{-.12f,.075f,.105f},{.04f,.092f,.130f},
                                        {.18f,.110f,.175f},{.30f,.108f,.185f},{.42f,.070f,.140f}}};
    constexpr int Sides=10;
    const auto at=[&](const std::size_t r,const int k) {
        const float a=2.0f*Pi*static_cast<float>(k)/Sides;
        const float s=std::sin(a);
        // the belly flattened on the mud
        return Vector3(std::cos(a)*Rings[r].w,Rings[r].h*(0.5f+0.5f*s)*(s<0.0f?0.55f:1.0f)+.004f,Rings[r].z);
    };
    for(std::size_t r=0;r+1<Rings.size();++r)
        for(int k=0;k<Sides;++k) {
            const float s=std::sin(2.0f*Pi*(static_cast<float>(k)+0.5f)/Sides);
            const Color c=s>0.55f?back:(s>-0.4f?flank:belly);
            Tri(out,at(r,k),at(r+1,k),at(r+1,k+1),c);
            Tri(out,at(r,k),at(r+1,k+1),at(r,k+1),c);
        }
    // the blunt snout's cap
    const Vector3 nose(0,.070f,.47f);
    for(int k=0;k<Sides;++k) Tri(out,at(Rings.size()-1,k),nose,at(Rings.size()-1,k+1),flank);
    // pale blue spots on the flanks
    for(int s=0;s<10;++s) {
        const float z=-.25f+.06f*static_cast<float>(s%5)+.02f*Hash(static_cast<std::uint32_t>(s));
        const float side=s<5?1.0f:-1.0f;
        const Vector3 c(side*.093f,.085f+.02f*Hash(static_cast<std::uint32_t>(s)+9U),z);
        Tri(out,c+Vector3(side*.004f,.012f,0),c+Vector3(side*.004f,-.010f,.012f),c+Vector3(side*.004f,-.010f,-.012f),spot);
    }
    // the eyes: two domes high on the head, bulging up and out, ringed
    for(const float side:{-1.0f,1.0f}) {
        const Vector3 base(side*.045f,.175f,.30f);
        constexpr int E=8;
        for(int k=0;k<E;++k) {
            const float a0=2.0f*Pi*k/E,a1=2.0f*Pi*(k+1)/E;
            const Vector3 r0(std::cos(a0)*.030f,0,std::sin(a0)*.030f),r1(std::cos(a1)*.030f,0,std::sin(a1)*.030f);
            const Vector3 m0=base+r0*.9f+Vector3(side*.008f,.028f,0),m1=base+r1*.9f+Vector3(side*.008f,.028f,0);
            Tri(out,base+r0,base+r1,m1,eyeRing);Tri(out,base+r0,m1,m0,eyeRing);
            Tri(out,m0,m1,base+Vector3(side*.012f,.052f,0),eye);
        }
        // the pectoral fins: planted on the mud like elbows, the fin's fan held out flat
        const Vector3 shoulder(side*.090f,.035f,.16f),elbow(side*.150f,.012f,.20f),foot(side*.165f,0.0f,.13f);
        Both(out,shoulder,elbow,foot,fin);
        Both(out,elbow,foot,elbow+Vector3(side*.035f,0,.020f),fin);
        // the pelvic fins under the belly
        Both(out,Vector3(side*.04f,.006f,.06f),Vector3(side*.07f,0,.02f),Vector3(side*.04f,.006f,-.02f),fin);
    }
    // the dorsal sail (part 1, raised in display) and a low second dorsal
    Both(out,Vector3(0,.125f,.10f),Vector3(0,.125f,-.06f),Vector3(0,.20f,-.01f),fin,1);
    Both(out,Vector3(0,.115f,-.08f),Vector3(0,.090f,-.30f),Vector3(0,.135f,-.20f),fin,1);
    // the tail fin (part 2), rounded, curled a little to one side at rest
    Both(out,Vector3(0,.055f,-.40f),Vector3(.010f,.12f,-.58f),Vector3(-.010f,0.005f,-.58f),fin,2);
}
}

MudskipperRenderer::MudskipperRenderer(GraphicsDevice& device,const MudskipperColony& colony,const IslandTerrain& terrain,
                                       const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)) {
    BuildFish(fish_);
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(fish_.size()*MaximumSkippers),BufferUsage::WriteOnly);
    staging_.reserve(fish_.size()*MaximumSkippers);
    // The mud: a disc of rings over each patch, a few centimetres over the drawn ground, wet
    // grey-brown at its middle and blended to the sand round its ragged edge (premultiplied).
    std::vector<VertexPositionColor> mud;
    Vector3 sum(0,0,0);
    for(std::size_t p=0;p<colony.Patches().size();++p) {
        const auto& patch=colony.Patches()[p];
        constexpr int Around=28,Rings=6;
        const auto at=[&](const float t,const float a) {
            const float edge=1.0f+.22f*std::sin(a*3.0f+static_cast<float>(p))+.12f*std::sin(a*7.0f+1.3f*static_cast<float>(p));
            const float r=patch.radius*t*edge;
            const float x=patch.x+std::cos(a)*r,z=patch.z+std::sin(a)*r;
            return Vector3(x,DrawnOrAnalyticHeight(ground,terrain,x,z)+.025f,z);
        };
        const auto colour=[&](const float t,const Vector3& point) {
            const float alpha=std::clamp((1.0f-t)/.45f,0.0f,1.0f)*.92f;
            // darker and glossy where it is wettest, toward the water's level
            const float wet=std::clamp((0.6f-point.Y)/0.8f,0.0f,1.0f);
            const float shade=(.80f+.25f*Hash(static_cast<std::uint32_t>(std::floor(point.X*4.0f)*73.0f+std::floor(point.Z*4.0f)*19.0f)));
            const Vector3 base=Vector3::Lerp(Vector3(104,92,74),Vector3(62,58,50),wet)*shade;
            return Color(static_cast<int>(base.X*alpha),static_cast<int>(base.Y*alpha),static_cast<int>(base.Z*alpha),static_cast<int>(255*alpha));
        };
        for(int k=0;k<Around;++k) {
            const float a0=2.0f*Pi*k/Around,a1=2.0f*Pi*(k+1)/Around;
            for(int r=0;r<Rings;++r) {
                const float t0=static_cast<float>(r)/Rings,t1=static_cast<float>(r+1)/Rings;
                const Vector3 p00=at(t0,a0),p01=at(t0,a1),p10=at(t1,a0),p11=at(t1,a1);
                mud.emplace_back(p00,colour(t0,p00));mud.emplace_back(p10,colour(t1,p10));mud.emplace_back(p11,colour(t1,p11));
                if(r>0) {mud.emplace_back(p00,colour(t0,p00));mud.emplace_back(p11,colour(t1,p11));mud.emplace_back(p01,colour(t0,p01));}
            }
        }
        sum+=Vector3(patch.x,0,patch.z);
    }
    mudTriangles_=static_cast<int>(mud.size()/3);
    if(!mud.empty()) {
        mudCentre_=sum/static_cast<float>(colony.Patches().size());
        mud_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mud.size()),BufferUsage::WriteOnly);
        mud_->SetData(mud.data(),static_cast<int>(mud.size()));
    }
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(250.0f);
    effect_->setFogEndProperty(660.0f);
    std::cout<<"island-simulator: "<<colony.Count()<<" mudskippers live on "<<colony.Patches().size()
             <<" patches of mud under the mangroves; a "<<fish_.size()/3<<"-triangle fish\n";
}

MudskipperRenderer::~MudskipperRenderer()=default;

void MudskipperRenderer::DrawMud(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    if(!mud_||(mudCentre_-Vector3(camera.X,0,camera.Z)).LengthSquared()>260.0f*260.0f) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::AlphaBlend);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    device_.SetVertexBuffer(mud_.get());device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,mudTriangles_);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}

void MudskipperRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,
                              const std::span<const Mudskipper> skippers) {
    staging_.clear();
    visible_=0;
    const BoundingFrustum frustum(view*projection);
    static constexpr std::array<std::array<float,3>,3> Tones{{{1.0f,1.0f,1.0f},{.86f,.92f,.84f},{1.08f,1.0f,.90f}}};
    for(const Mudskipper& fish:skippers) {
        if(static_cast<std::size_t>(visible_)>=MaximumSkippers) break;
        const Vector3 at(fish.x,fish.y,fish.z);
        if(Vector3::DistanceSquared(at,camera)>30.0f*30.0f) continue;
        const float size=FishLength*fish.scale;
        if(!frustum.Intersects(BoundingSphere(at,size))) continue;
        const float c=std::cos(fish.heading),s=std::sin(fish.heading);
        const auto& tone=Tones[fish.tone%Tones.size()];
        // in the hop the body arcs: the head up as it leaves, down as it lands
        const float pitch=std::sin(fish.hop*Pi*2.0f)*.35f;
        const float pc=std::cos(pitch),ps=std::sin(pitch);
        for(const TV& v:fish_) {
            float x=v.x,y=v.y,z=v.z;
            if(v.part==1) y=(y-.11f)*(.35f+.65f*fish.display)+.11f;          // the sail folded flat but in display
            if(v.part==2) x+=(.06f+.10f*std::sin(fish.hop*Pi))*(-z-.40f);     // the tail curled, flicked straight in a hop
            const float py=y*pc+z*ps,pz=z*pc-y*ps;
            const Vector3 p=at+Vector3((x*s+pz*c)*size,py*size,(-x*c+pz*s)*size);
            staging_.emplace_back(p,Color(static_cast<int>(std::min(255.0f,v.r*tone[0])),static_cast<int>(std::min(255.0f,v.g*tone[1])),
                                          static_cast<int>(std::min(255.0f,v.b*tone[2]))));
        }
        ++visible_;
    }
    if(staging_.empty()) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    buffer_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(buffer_.get());
    device_.setIndicesProperty(nullptr);
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);
    effect_->setProjectionProperty(projection);
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3U));
}

}
