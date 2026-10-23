#include "IslandSimulator/WeaverNestRenderer.hpp"
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
#include <cmath>
#include <iostream>
#include <numbers>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
using TV=WeaverNestRenderer::TemplateVertex;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
Color Mix(const Color& a,const Color& b,const float k) {
    const auto channel=[k](const std::uint8_t x,const std::uint8_t y) {
        return static_cast<int>(std::lround(static_cast<float>(x)+(static_cast<float>(y)-static_cast<float>(x))*k));
    };
    return Color(channel(a.getRProperty(),b.getRProperty()),channel(a.getGProperty(),b.getGProperty()),
                 channel(a.getBProperty(),b.getBProperty()));
}
Color Shade(const Color& c,const float k) {
    return Color(static_cast<int>(std::min(255.0f,c.getRProperty()*k)),static_cast<int>(std::min(255.0f,c.getGProperty()*k)),
                 static_cast<int>(std::min(255.0f,c.getBProperty()*k)));
}
void Tri(std::vector<TV>& out,const Vector3& a,const Vector3& b,const Vector3& c,const Color& colour) {
    const Color lit=wildlife_render::FacetColor(a,b,c,colour,true);
    for(const Vector3& p:{a,b,c}) out.push_back({p.X,p.Y,p.Z,lit.getRProperty(),lit.getGProperty(),lit.getBProperty()});
}
// a strand: a thin two-sided strip from a to b, `width` across, facing `normal`
void Strand(std::vector<TV>& out,const Vector3& a,const Vector3& b,const Vector3& normal,const float width,const Color& colour) {
    Vector3 across=Vector3::Cross(b-a,normal);
    if(across.LengthSquared()<1e-10f) across=Vector3::Cross(b-a,Vector3::UnitX);
    across=Vector3::Normalize(across)*(width*0.5f);
    Tri(out,a-across,b-across*0.4f,a+across,colour);
    Tri(out,a+across,b-across*0.4f,a-across,colour);
    Tri(out,a+across,b+across*0.4f,b-across*0.4f,colour);
    Tri(out,b-across*0.4f,b+across*0.4f,a+across,colour);
}

// One nest in its own frame: x across, y up from the knot it hangs by, z the way its spout faces.
void BuildNest(std::vector<TV>& out,const WeaverNest& nest) {
    const float s=nest.size,hang=nest.hang;
    const auto seed=nest.seed;
    const Color fresh(104,146,56),straw(184,160,98),grey(150,134,96),leaf(58,124,52),dryLeaf(150,136,84),inside(26,22,14);
    const float age=nest.age;
    const Color grass=age<0.5f?Mix(fresh,straw,age*2.0f):Mix(straw,grey,(age-0.5f)*2.0f);
    // each facet a strand or two of its own shade: the criss-cross of the weave
    const auto weave=[&](const std::uint32_t cell,const float lean) {
        const float h=Hash(seed*17U+cell);
        const Color c=h<0.18f?Mix(grass,straw,0.5f):(h>0.86f?Mix(grass,fresh,age>0.5f?0.0f:0.5f):grass);
        return Shade(c,0.86f+0.20f*Hash(seed*29U+cell*3U)+lean);
    };
    // the strips of leaflet it is tied on by: torn down from the frond and knotted into the roof
    const Color strip=Mix(leaf,dryLeaf,std::clamp(age*1.2f,0.0f,1.0f));
    const int strips=3+static_cast<int>(Hash(seed+40U)*2.99f);
    for(int k=0;k<strips;++k) {
        const float a=2.0f*Pi*static_cast<float>(k)/static_cast<float>(strips)+Hash(seed+41U)*1.3f;
        const Vector3 top(std::cos(a)*0.035f*Hash(seed+42U+static_cast<std::uint32_t>(k)),0.01f,std::sin(a)*0.03f);
        const Vector3 bottom(std::cos(a)*0.05f*s,-hang-0.015f*s,std::sin(a)*0.05f*s);
        const Vector3 middle=(top+bottom)*0.5f+Vector3(0.008f*std::sin(a*3.0f),0.0f,0.006f);
        Strand(out,top,middle,Vector3(std::cos(a),0,std::sin(a)),0.016f,strip);
        Strand(out,middle,bottom,Vector3(std::cos(a),0,std::sin(a)),0.014f,Shade(strip,0.92f));
    }
    const Vector3 centre(0.0f,-hang-0.10f*s,0.0f);
    if(nest.building) {
        // the hoop: a ring of strands hanging across the frond, the roof just begun over it
        constexpr int Around=18,Sides=6;
        const float major=0.085f*s,minor=0.016f*s;
        const auto at=[&](const int i,const int j) {
            const float u=2.0f*Pi*static_cast<float>(i)/Around,v=2.0f*Pi*static_cast<float>(j)/Sides;
            const float r=major+std::cos(v)*minor*(1.0f+0.3f*Hash(seed+static_cast<std::uint32_t>(i*7+j)));
            return centre+Vector3(std::cos(u)*r,std::sin(u)*r,std::sin(v)*minor);
        };
        for(int i=0;i<Around;++i)
            for(int j=0;j<Sides;++j) {
                const Color c=weave(static_cast<std::uint32_t>(i*Sides+j),0.0f);
                Tri(out,at(i,j),at(i+1,j),at(i+1,j+1),c);
                Tri(out,at(i,j),at(i+1,j+1),at(i,j+1),c);
            }
        // the roof's start: a short hood over the hoop's top, toward the back
        constexpr int Hood=8;
        for(int i=0;i<Hood;++i) {
            const float u0=Pi*(0.25f+0.5f*static_cast<float>(i)/Hood),u1=Pi*(0.25f+0.5f*static_cast<float>(i+1)/Hood);
            const Vector3 a0=centre+Vector3(std::cos(u0)*major,std::sin(u0)*major,0),a1=centre+Vector3(std::cos(u1)*major,std::sin(u1)*major,0);
            const Vector3 b0=centre+Vector3(std::cos(u0)*major*0.8f,std::sin(u0)*major*0.8f,-0.07f*s);
            const Vector3 b1=centre+Vector3(std::cos(u1)*major*0.8f,std::sin(u1)*major*0.8f,-0.07f*s);
            const Color c=weave(200U+static_cast<std::uint32_t>(i),0.0f);
            Tri(out,a0,a1,b1,c);Tri(out,a0,b1,b0,c);
        }
    } else {
        // the chamber: a kidney of grass, bulging at the back over the egg chamber and drawn
        // down at the front into the spout
        constexpr int Around=20,Rings=14;
        const float rx=0.085f*s,ry=0.098f*s,rz=0.108f*s;
        const auto at=[&](const int ring,const int k) {
            const float v=Pi*static_cast<float>(ring)/Rings;          // 0 the top .. pi the bottom
            const float u=2.0f*Pi*static_cast<float>(k)/Around;       // 0 the front (+z)
            const float sv=std::sin(v),cv=std::cos(v);
            float x=std::sin(u)*sv*rx,y=cv*ry,z=std::cos(u)*sv*rz;
            // the front's bottom drawn down toward the spout, the back's bottom tucked up
            const float front=std::max(0.0f,std::cos(u));
            if(cv<0.0f) {y+=cv*front*0.035f*s;z+=front*(-cv)*0.02f*s;y-=(1.0f-front)*cv*0.015f*s;}
            // a shaggy surface: the strands bunch a little
            const float rough=1.0f+0.07f*(Hash(seed+static_cast<std::uint32_t>(ring*41+k*7))-0.5f);
            return centre+Vector3(x*rough,y*rough,z*rough);
        };
        for(int ring=0;ring<Rings;++ring)
            for(int k=0;k<Around;++k) {
                // the spout's root at the front of the bottom: left open
                if(ring>=Rings-2&&(k<=1||k>=Around-1)) continue;
                const Color c0=weave(static_cast<std::uint32_t>(ring*Around+k),0.0f);
                const Color c1=weave(static_cast<std::uint32_t>(ring*Around+k+500),-0.04f);
                Tri(out,at(ring,k),at(ring+1,k),at(ring+1,k+1),c0);
                Tri(out,at(ring,k),at(ring+1,k+1),at(ring,k+1),c1);
            }
        // the spout: a short woven tube from the bottom's front, its open mouth facing down
        constexpr int Sides=10,Steps=4;
        const Vector3 root=at(Rings-1,0)+Vector3(0,-0.004f*s,0);
        const Vector3 tip=centre+Vector3(0.0f,-0.25f*s+0.10f*s,0.085f*s);
        const float rootRadius=0.040f*s,tipRadius=0.034f*s;
        const auto ringPoint=[&](const int step,const int k) {
            const float t=static_cast<float>(step)/Steps;
            const Vector3 axis=root+(tip-root)*t;
            const float a=2.0f*Pi*static_cast<float>(k)/Sides;
            const float r=(rootRadius+(tipRadius-rootRadius)*t)*(1.0f+0.10f*(Hash(seed+300U+static_cast<std::uint32_t>(step*13+k))-0.5f));
            // round the tube's axis, which leans down and forward
            return axis+Vector3(std::cos(a)*r,0.0f,std::sin(a)*r*0.9f);
        };
        for(int step=0;step<Steps;++step)
            for(int k=0;k<Sides;++k) {
                const Color c=weave(900U+static_cast<std::uint32_t>(step*Sides+k),-0.06f);
                Tri(out,ringPoint(step,k),ringPoint(step+1,k),ringPoint(step+1,k+1),c);
                Tri(out,ringPoint(step,k),ringPoint(step+1,k+1),ringPoint(step,k+1),c);
            }
        // the dark of the inside, seen up the spout
        const Vector3 deep=root+Vector3(0,0.03f*s,-0.01f*s);
        for(int k=0;k<Sides;++k) Tri(out,ringPoint(Steps,k),ringPoint(Steps,k+1),deep,inside);
        // the weave itself: strands of grass laid over the chamber, criss-crossing on the
        // diagonals, each a little proud of the shell and bowed over it
        for(int k=0;k<150;++k) {
            const auto h=seed*313U+static_cast<std::uint32_t>(k)*29U;
            const float u=2.0f*Pi*Hash(h),v=Pi*(0.06f+0.86f*Hash(h+1U));
            const auto shell=[&](const float uu,const float vv,const float lift) {
                const float sv=std::sin(vv),cv=std::cos(vv);
                return centre+Vector3(std::sin(uu)*sv*rx*lift,cv*ry*lift,std::cos(uu)*sv*rz*lift);
            };
            const float slant=k%2==0?1.0f:-1.0f;
            const float length=(0.45f+0.35f*Hash(h+2U));
            const float du=slant*length*0.55f,dv=length*0.40f;
            const Vector3 a=shell(u-du*0.5f,v-dv*0.5f,1.035f);
            const Vector3 m=shell(u,v,1.055f);
            const Vector3 b=shell(u+du*0.5f,v+dv*0.5f,1.035f);
            const Vector3 normal=Vector3::Normalize(m-centre);
            const Color c=Shade(Mix(grass,Hash(h+3U)<0.5f?straw:fresh,0.15f+0.35f*Hash(h+4U)*(age>0.5f?0.4f:1.0f)),
                                0.84f+0.30f*Hash(h+5U));
            Strand(out,a,m,normal,0.010f*s,c);
            Strand(out,m,b,normal,0.009f*s,Shade(c,0.95f));
        }
    }
    // the loose ends: strands sticking out of the weave, a few hanging from the bottom
    for(int k=0;k<46;++k) {
        const auto h=seed*101U+static_cast<std::uint32_t>(k)*13U;
        const float u=2.0f*Pi*Hash(h),v=Pi*(0.15f+0.8f*Hash(h+1U));
        const Vector3 dir(std::sin(u)*std::sin(v),std::cos(v),std::cos(u)*std::sin(v));
        const Vector3 base=centre+Vector3(dir.X*0.083f*s,dir.Y*0.095f*s,dir.Z*0.10f*s);
        const bool dangling=k<6;
        const float length=dangling?0.08f+0.10f*Hash(h+2U):0.025f+0.05f*Hash(h+2U);
        const Vector3 tangent=Vector3::Normalize(Vector3::Cross(dir,Vector3(Hash(h+3U)-0.5f,0.4f,Hash(h+4U)-0.5f)));
        const Vector3 end=dangling?base+Vector3((Hash(h+5U)-0.5f)*0.03f,-length,(Hash(h+6U)-0.5f)*0.03f)
                                  :base+(dir*0.6f+tangent*0.8f)*length;
        Strand(out,base,end,dir,0.006f,Shade(Mix(grass,straw,Hash(h+7U)*0.6f),0.9f+0.2f*Hash(h+8U)));
    }
}
}

WeaverNestRenderer::WeaverNestRenderer(GraphicsDevice& device,const WeaverColony& colony)
    :device_(device),colony_(colony),effect_(std::make_unique<BasicEffect>(device)) {
    std::size_t total=0;
    for(const WeaverNest& nest:colony.Nests()) {
        nests_.emplace_back();
        BuildNest(nests_.back(),nest);
        total+=nests_.back().size();
    }
    if(total>0) {
        buffer_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
            static_cast<int>(total),BufferUsage::WriteOnly);
        staging_.reserve(total);
    }
    effect_->setVertexColorEnabledProperty(true);
    effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(250.0f);
    effect_->setFogEndProperty(660.0f);
    std::cout<<"island-simulator: "<<colony.Nests().size()<<" weavers' nests hang from the fronds of "
             <<WeaverColony::ColonyPalms<<" palms; "<<TriangleCount()<<" triangles of woven grass\n";
}

WeaverNestRenderer::~WeaverNestRenderer()=default;

std::size_t WeaverNestRenderer::TriangleCount() const noexcept {
    std::size_t total=0;
    for(const auto& nest:nests_) total+=nest.size()/3U;
    return total;
}

void WeaverNestRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,const float seconds,
                              const std::span<const std::array<float,3>> offsets) {
    staging_.clear();
    visible_=0;
    if(!buffer_) return;
    const BoundingFrustum frustum(view*projection);
    const auto nests=colony_.Nests();
    for(std::size_t i=0;i<nests.size();++i) {
        const WeaverNest& nest=nests[i];
        const std::array<float,3> offset=nest.site<offsets.size()?offsets[nest.site]:std::array<float,3>{0,0,0};
        const auto knot=colony_.NestPoint(i,seconds,offset,0,0,0);
        const Vector3 at(knot[0],knot[1]-nest.hang,knot[2]);
        if(Vector3::DistanceSquared(at,camera)>DrawDistance*DrawDistance) continue;
        if(!frustum.Intersects(BoundingSphere(at,nest.hang+0.4f))) continue;
        // the nest's frame this moment: its yaw, then its rock on the strips (as NestPoint)
        const auto origin=colony_.NestPoint(i,seconds,offset,0,0,0);
        const auto ex=colony_.NestPoint(i,seconds,offset,1,0,0);
        const auto ey=colony_.NestPoint(i,seconds,offset,0,1,0);
        const auto ez=colony_.NestPoint(i,seconds,offset,0,0,1);
        const Vector3 o(origin[0],origin[1],origin[2]);
        const Vector3 ax=Vector3(ex[0],ex[1],ex[2])-o,ay=Vector3(ey[0],ey[1],ey[2])-o,az=Vector3(ez[0],ez[1],ez[2])-o;
        for(const TV& v:nests_[i])
            staging_.emplace_back(o+ax*v.x+ay*v.y+az*v.z,Color(static_cast<int>(v.r),static_cast<int>(v.g),static_cast<int>(v.b)));
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
