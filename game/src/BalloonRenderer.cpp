#include "IslandSimulator/BalloonRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
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
using Part=BalloonRenderer::Part;
constexpr int Gores=24,PerGore=3,Rings=34;
constexpr float Waist=11.8f;   // where the envelope is widest
constexpr float MouthRadius=1.5f;
constexpr int FlameSides=10;

// the envelope's radius at height y over its mouth: a cone flaring out to the waist, a dome over it
float Radius(const float y) {
    const float h=BalloonFlight::EnvelopeHeight,r=BalloonFlight::EnvelopeRadius;
    if(y>=Waist) {
        const float k=std::min(1.0f,(y-Waist)/(h-Waist));
        return r*std::sqrt(std::max(0.0f,1.0f-k*k));
    }
    return MouthRadius+(r-MouthRadius)*std::pow(std::sin(0.5f*Pi*y/Waist),1.2f);
}
// the height of the envelope's ring `ring`: closer together toward the dome's top
float RingHeight(const int ring) {
    const float k=static_cast<float>(ring)/Rings;
    return BalloonFlight::EnvelopeHeight*(1.0f-(1.0f-k)*(1.0f-k)*0.35f-0.65f*(1.0f-k));
}
void Add(std::vector<Part>& out,const Vector3& p,const Vector3& n,const Color& c,const bool fabric) {
    out.push_back({p,n,c.getRProperty(),c.getGProperty(),c.getBProperty(),fabric});
}
void Flat(std::vector<Part>& out,const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour) {
    Vector3 n=Vector3::Cross(b-a,c-a);
    n=n.LengthSquared()>1e-12f?Vector3::Normalize(n):Vector3::Up;
    Add(out,a,n,colour,false);Add(out,b,n,colour,false);Add(out,c,n,colour,false);
}
void Quad(std::vector<Part>& out,const Vector3& a,const Vector3& b,const Vector3& c,const Vector3& d,const Color& colour) {
    Flat(out,a,b,c,colour);Flat(out,a,c,d,colour);
}
// a thin four-sided rod from a to b
void Rod(std::vector<Part>& out,const Vector3& a,const Vector3& b,const float radius,const Color& colour) {
    const Vector3 axis=Vector3::Normalize(b-a);
    Vector3 u=Vector3::Cross(axis,std::abs(axis.Y)<0.9f?Vector3::Up:Vector3::UnitX);
    u=Vector3::Normalize(u)*radius;
    const Vector3 v=Vector3::Normalize(Vector3::Cross(axis,u))*radius;
    const std::array<Vector3,4> ring{u,v,-u,-v};
    for(std::size_t k=0;k<4;++k) {
        const Vector3& p=ring[k];const Vector3& q=ring[(k+1)%4];
        Quad(out,a+p,b+p,b+q,a+q,colour);
    }
}
void Ellipsoid(std::vector<Part>& out,const Vector3& centre,const Vector3& radii,const Color& colour,
               const Color& top=Color(0,0,0,0),const float topFrom=2.0f) {
    constexpr int Around=12,Up=8;
    const auto at=[&](const int i,const int j) {
        const float u=2.0f*Pi*static_cast<float>(i)/Around,v=Pi*static_cast<float>(j)/Up;
        const Vector3 n(std::cos(u)*std::sin(v),std::cos(v),std::sin(u)*std::sin(v));
        return std::pair{centre+Vector3(n.X*radii.X,n.Y*radii.Y,n.Z*radii.Z),n};
    };
    for(int j=0;j<Up;++j)
        for(int i=0;i<Around;++i) {
            const auto a=at(i,j),b=at(i+1,j),c=at(i+1,j+1),d=at(i,j+1);
            const bool capped=top.getAProperty()>0&&std::cos(Pi*(static_cast<float>(j)+0.5f)/Up)>topFrom;
            const Color c0=capped?top:colour;
            for(const auto& p:{a,c,b,a,d,c}) Add(out,p.first,p.second,c0,false);
        }
}

void BuildEnvelope(std::vector<Part>& out) {
    const std::array<Color,4> palette{Color(206,42,38),Color(240,192,44),Color(36,84,176),Color(238,234,226)};
    const Color scoop(66,64,68),tape(96,88,82);
    const int around=Gores*PerGore;
    const auto point=[&](const int ring,const int column) {
        const float y=RingHeight(ring);
        const float theta=2.0f*Pi*static_cast<float>(column)/around;
        // each gore bulges a little between its tapes
        const float frac=static_cast<float>(column%PerGore)/PerGore;
        const float r=Radius(y)*(1.0f+0.014f*std::sin(Pi*frac));
        const float dy=0.05f;
        const float slope=(Radius(std::min(BalloonFlight::EnvelopeHeight,y+dy))-Radius(std::max(0.0f,y-dy)))/(2.0f*dy);
        Vector3 n(std::cos(theta),-slope,std::sin(theta));
        if(ring==Rings) n=Vector3::Up;
        return std::pair{Vector3(std::cos(theta)*r,y,std::sin(theta)*r),Vector3::Normalize(n)};
    };
    for(int ring=0;ring<Rings;++ring)
        for(int column=0;column<around;++column) {
            const int gore=column/PerGore;
            const float y=0.5f*(RingHeight(ring)+RingHeight(ring+1));
            Color c=palette[static_cast<std::size_t>(gore%4)];
            if(y>9.0f&&y<11.0f) c=palette[static_cast<std::size_t>((gore+2)%4)];          // the belt
            if(y>15.9f) c=palette[static_cast<std::size_t>(gore%2==0?3:0)];               // the crown
            if(y<1.7f) c=scoop;                                                          // the skirt
            const auto a=point(ring,column),b=point(ring,column+1),d=point(ring+1,column),e=point(ring+1,column+1);
            for(const auto& p:{a,d,b,b,d,e}) Add(out,p.first,p.second,c,true);
        }
    // the load tapes down the seams, a little proud of the fabric
    for(int gore=0;gore<Gores;++gore) {
        const float theta=2.0f*Pi*static_cast<float>(gore)/Gores;
        const Vector3 radial(std::cos(theta),0,std::sin(theta)),side(-std::sin(theta),0,std::cos(theta));
        for(int ring=0;ring<Rings;++ring) {
            const float y0=RingHeight(ring),y1=RingHeight(ring+1);
            const Vector3 p0=radial*(Radius(y0)*1.006f)+Vector3(0,y0,0),p1=radial*(Radius(y1)*1.006f)+Vector3(0,y1,0);
            const float w=0.045f;
            const auto a=point(ring,gore*PerGore),b=point(ring+1,gore*PerGore);
            for(const auto& p:{std::pair{p0-side*w,a.second},std::pair{p1-side*w,b.second},std::pair{p0+side*w,a.second},
                               std::pair{p0+side*w,a.second},std::pair{p1-side*w,b.second},std::pair{p1+side*w,b.second}})
                Add(out,p.first,p.second,tape,false);
        }
    }
    // the crown: the top's closing disc (the parachute vent's ring)
    const float top=RingHeight(Rings);
    for(int column=0;column<around;++column) {
        const auto a=point(Rings,column),b=point(Rings,column+1);
        Add(out,a.first,Vector3::Up,palette[3],true);Add(out,Vector3(0,top+0.02f,0),Vector3::Up,palette[3],true);
        Add(out,b.first,Vector3::Up,palette[3],true);
    }
}

void BuildBasket(std::vector<Part>& out) {
    const Color wicker(156,114,64),wickerDark(118,84,46),rim(84,54,32),steel(128,128,134),cable(52,50,48);
    const float floor=-BalloonFlight::BasketBelow,rimY=floor+1.1f,half=0.66f;
    // the walls: woven in rows, each strand a shade of its own
    constexpr int Columns=8,Rows=7;
    const std::array<Vector3,4> corners{Vector3(-half,0,-half),Vector3(half,0,-half),Vector3(half,0,half),Vector3(-half,0,half)};
    for(std::size_t side=0;side<4;++side) {
        const Vector3 a=corners[side],b=corners[(side+1)%4];
        for(int row=0;row<Rows;++row)
            for(int column=0;column<Columns;++column) {
                const float u0=static_cast<float>(column)/Columns,u1=static_cast<float>(column+1)/Columns;
                const float v0=floor+(rimY-floor)*static_cast<float>(row)/Rows,v1=floor+(rimY-floor)*static_cast<float>(row+1)/Rows;
                const Vector3 p0=a+(b-a)*u0,p1=a+(b-a)*u1;
                const Color c=(row+column)%2==0?wicker:wickerDark;
                Quad(out,p0+Vector3(0,v0,0),p1+Vector3(0,v0,0),p1+Vector3(0,v1,0),p0+Vector3(0,v1,0),c);
            }
        // the padded leather rim round the top
        const Vector3 inward=Vector3::Normalize(-(a+b)*0.5f)*0.09f;
        Quad(out,a+Vector3(0,rimY,0),b+Vector3(0,rimY,0),b+Vector3(0,rimY+0.09f,0),a+Vector3(0,rimY+0.09f,0),rim);
        Quad(out,a+Vector3(0,rimY+0.09f,0),b+Vector3(0,rimY+0.09f,0),b+inward+Vector3(0,rimY+0.09f,0),a+inward+Vector3(0,rimY+0.09f,0),rim);
    }
    Quad(out,corners[0]+Vector3(0,floor,0),corners[1]+Vector3(0,floor,0),corners[2]+Vector3(0,floor,0),corners[3]+Vector3(0,floor,0),wickerDark);
    // the burner frame: uprights from the basket's corners to the burner's square, the burner
    const float frameY=-1.55f,frameHalf=0.34f;
    const std::array<Vector3,4> frame{Vector3(-frameHalf,frameY,-frameHalf),Vector3(frameHalf,frameY,-frameHalf),
                                      Vector3(frameHalf,frameY,frameHalf),Vector3(-frameHalf,frameY,frameHalf)};
    for(std::size_t k=0;k<4;++k) {
        Rod(out,corners[k]*0.94f+Vector3(0,rimY+0.09f,0),frame[k],0.022f,steel);
        Rod(out,frame[k],frame[(k+1)%4],0.02f,steel);
    }
    Ellipsoid(out,Vector3(0,-1.32f,0),Vector3(0.22f,0.20f,0.22f),Color(150,150,156));
    Rod(out,Vector3(0,-1.52f,0),Vector3(0,-1.08f,0),0.11f,Color(96,96,102));
    // the flying wires: three from each frame corner up to the envelope's mouth
    for(int k=0;k<12;++k) {
        const float theta=2.0f*Pi*(static_cast<float>(k)+0.5f)/12.0f;
        const Vector3 mouth(std::cos(theta)*MouthRadius,0.0f,std::sin(theta)*MouthRadius);
        std::size_t corner=0;
        for(std::size_t c=1;c<4;++c)
            if(frame[c].X*mouth.X+frame[c].Z*mouth.Z>frame[corner].X*mouth.X+frame[corner].Z*mouth.Z) corner=c;
        Rod(out,frame[corner],mouth,0.012f,cable);
    }
    // two people in it: the pilot in red at the burner's side, a passenger in blue
    const std::array<std::array<float,2>,2> places{{{0.30f,0.22f},{-0.28f,-0.24f}}};
    const std::array<Color,2> shirts{Color(184,52,44),Color(52,84,140)};
    const std::array<Color,2> hair{Color(54,40,30),Color(176,148,96)};
    for(std::size_t p=0;p<2;++p) {
        const Vector3 at(places[p][0],0,places[p][1]);
        Ellipsoid(out,at+Vector3(0,rimY-0.05f,0),Vector3(0.21f,0.30f,0.15f),shirts[p]);
        Ellipsoid(out,at+Vector3(0,rimY+0.42f,0),Vector3(0.10f,0.12f,0.11f),Color(200,156,124),hair[p],0.35f);
    }
}
}

BalloonRenderer::BalloonRenderer(GraphicsDevice& device)
    :device_(device),effect_(std::make_unique<BasicEffect>(device)) {
    BuildEnvelope(parts_);
    BuildBasket(parts_);
    buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
                                                  static_cast<int>(parts_.size()),BufferUsage::WriteOnly);
    staging_.reserve(parts_.size());
    constexpr int FlameVertices=FlameSides*3*2;
    flameBuffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
                                                       FlameVertices,BufferUsage::WriteOnly);
    flame_.reserve(FlameVertices);
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(420.0f);
    effect_->setFogEndProperty(1900.0f);
    std::cout<<"island-simulator: a hot-air balloon drifts over the island; "<<TriangleCount()<<" triangles\n";
}

BalloonRenderer::~BalloonRenderer()=default;

void BalloonRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const BalloonFlight::State& state) {
    visible_=false;
    const Vector3 origin(state.x,state.y,state.z);
    if(!BoundingFrustum(view*projection).Intersects(BoundingSphere(origin+Vector3(0,6.0f,0),14.0f))) return;
    visible_=true;
    const Matrix rotation=Matrix::CreateRotationY(state.spin)*Matrix::CreateRotationX(state.tiltX)*Matrix::CreateRotationZ(state.tiltZ);
    const Vector3 sun=wildlife_render::FixedSunDirection;
    staging_.clear();
    for(const Part& part:parts_) {
        const Vector3 p=Vector3::Transform(part.position,rotation)+origin;
        Vector3 n=Vector3::TransformNormal(part.normal,rotation);
        // the fabric seen from inside, through the mouth: the inner face, lit through the cloth
        const bool inner=part.fabric&&Vector3::Dot(n,camera-p)<0.0f;
        if(inner) n=-n;
        const float facing=Vector3::Dot(n,sun);
        float light=0.72f+0.30f*std::max(0.0f,facing)+0.06f*std::max(0.0f,n.Y);
        // the cloth lets the sun through: a glow where the sun is behind it
        if(part.fabric) light+=0.16f*std::max(0.0f,-facing)*std::clamp(sun.Y*4.0f,0.0f,1.0f);
        if(inner) light*=0.82f;
        const Color lit=wildlife_render::ScaleColor(Color(static_cast<int>(part.r),static_cast<int>(part.g),static_cast<int>(part.b)),light);
        staging_.emplace_back(p,lit);
    }
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
    if(state.burner<=0.01f) return;
    // the flame: an orange cone roaring up into the mouth, a yellow-white core inside it
    flame_.clear();
    const Vector3 base=Vector3::Transform(Vector3(0,-1.08f,0),rotation)+origin;
    const Vector3 up=Vector3::TransformNormal(Vector3::Up,rotation);
    const float length=(1.6f+1.4f*state.burner);
    const auto cone=[&](const float radius,const float tall,const Color& root,const Color& tip) {
        for(int k=0;k<FlameSides;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/FlameSides,a1=2.0f*Pi*static_cast<float>(k+1)/FlameSides;
            const Vector3 r0(std::cos(a0)*radius,0,std::sin(a0)*radius),r1(std::cos(a1)*radius,0,std::sin(a1)*radius);
            flame_.emplace_back(base+r0,root);flame_.emplace_back(base+r1,root);flame_.emplace_back(base+up*tall,tip);
        }
    };
    const float k=std::clamp(state.burner,0.0f,1.0f);
    const auto scaled=[k](const int r,const int g,const int b) {
        return Color(static_cast<int>(static_cast<float>(r)*k),static_cast<int>(static_cast<float>(g)*k),static_cast<int>(static_cast<float>(b)*k));
    };
    cone(0.26f,length,scaled(255,130,40),scaled(120,30,0));
    cone(0.13f,length*0.6f,scaled(255,236,170),scaled(255,150,40));
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    flameBuffer_->SetData(flame_.data(),0,static_cast<int>(flame_.size()),SetDataOptions::Discard);
    device_.SetVertexBuffer(flameBuffer_.get());
    effect_->setFogEnabledProperty(false);
    effect_->Apply();
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(flame_.size()/3U));
    effect_->setFogEnabledProperty(true);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}

}
