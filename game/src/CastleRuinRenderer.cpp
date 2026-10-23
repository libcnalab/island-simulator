#include "IslandSimulator/CastleRuinRenderer.hpp"
#include "IslandSimulator/LandmarkShapes.hpp"
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
#include <cstdint>
#include <iostream>
#include <numbers>
#include <sstream>
#include <vector>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
// Weathered limestone (pigment past full: the build lights faces 0.46 + 0.54 n.s), the darks of the
// openings, moss, lichen and old timber.
const Vector3 Stone(158,150,132),Hollow(36,34,32),Moss(74,100,52),Lichen(176,170,128),Timber(96,80,62);

float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

// Smooth value noise in 0..1 along a wall, one lattice point every `cell` metres.
float Noise(const std::uint32_t seed,const float u,const float cell) {
    const float s=u/cell,f=s-std::floor(s);
    const auto i=static_cast<std::uint32_t>(static_cast<int>(std::floor(s))+1000);
    const float a=Hash(seed*7919U+i),c=Hash(seed*7919U+i+1U),k=f*f*(3.0f-2.0f*f);
    return a+(c-a)*k;
}
}

CastleRuinRenderer::CastleRuinRenderer(GraphicsDevice& device,const CastleRuinPlacement& placement,
                                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    using K=CastleRuinPlacement;
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(92,84,66);b.stainHeight=0.25f;b.stainAmount=0.5f;
    b.tintPerVertex=true;
    // Lichen in patches, rain streaks, moss on the faces turned up and the damp foot, ivy up the
    // lower walls in places.
    b.tint=[&](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        if(pigment.X<60.0f) return pigment;
        const float patch=std::sin(centre.X*2.3f+centre.Y*1.7f)*std::sin(centre.Z*2.9f-centre.Y*2.1f);
        Vector3 colour=pigment;
        const float streak=0.9f+0.1f*std::sin(centre.X*8.0f+centre.Z*6.0f);
        colour=colour*(std::abs(normal.Y)<0.4f?streak:1.0f);
        colour=Vector3::Lerp(colour,Lichen,std::clamp(patch-0.4f,0.0f,1.0f)*0.7f);
        const float foot=std::clamp(1.0f-(centre.Y-groundAt(centre.X,centre.Z))/0.7f,0.0f,1.0f);
        colour=Vector3::Lerp(colour,Moss,std::min(0.85f,std::clamp((normal.Y-0.45f)*2.2f,0.0f,1.0f)*(0.5f+0.3f*patch)+foot*0.4f));
        const float ivy=std::sin(centre.X*0.9f+centre.Z*1.3f)*std::sin(centre.X*0.4f-centre.Z*0.7f);
        if(ivy>0.55f&&centre.Y-groundAt(centre.X,centre.Z)<4.5f)
            colour=Vector3::Lerp(colour,Vector3(52,86,40),std::clamp((ivy-0.55f)*4.0f,0.0f,0.75f));
        return colour;
    };
    const Vector3 face(placement.faceX,0.0f,placement.faceZ),right(placement.RightX(),0.0f,placement.RightZ());
    // a point of the castle's plan on the ground, `u` metres up
    const auto P=[&](const float r,const float f,const float u=0.0f) {
        const auto p=placement.At(r,f);
        return Vector3(p[0],groundAt(p[0],p[1])+u,p[1]);
    };
    const auto planar=[&](const float r,const float f) {const auto p=placement.At(r,f);return Vector3(p[0],0.0f,p[1]);};
    std::uint32_t blockCounter=0;
    constexpr float Course=0.6f,Foot=-0.5f;
    constexpr float Intact=6.2f;

    // One run of wall: a chain of columns (a prism of coursed blocks every ~0.45 m) along `pos(t)`
    // with `radial(t)` the horizontal direction across its thickness; `top(t)` its height over the
    // ground at each point, `opening(t)` the {sill, lintel} of any gap in it. The columns that
    // reach the intact height are battlemented in pairs.
    const auto chain=[&](const int count,const auto& pos,const auto& radial,const float thickness,
                         const auto& top,const auto& opening,const bool battlements,const std::uint32_t seed) {
        std::vector<float> heights(static_cast<std::size_t>(count)+1U);
        for(int k=0;k<=count;++k) {
            const float t=static_cast<float>(k)/static_cast<float>(count);
            float h=top(t);
            const float knocked=Hash(seed*977U+static_cast<std::uint32_t>(k))>0.9f?0.3f+0.5f*Hash(seed*31U+static_cast<std::uint32_t>(k)):0.0f;
            const bool whole=h>=Intact-0.05f;
            h=std::max(0.35f,h-knocked);
            if(battlements&&whole&&knocked==0.0f&&(k/2)%2==1) h-=0.8f;
            heights[static_cast<std::size_t>(k)]=h;
        }
        const auto column=[&](const Vector3& p0,const Vector3& p1,const Vector3& h0v,const Vector3& h1v,
                              const float u0,const float hA,const float hB,const std::uint32_t id) {
            if(std::max(hA,hB)-u0<0.04f) return;
            const Vector3 half0=h0v*(thickness*0.5f),half1=h1v*(thickness*0.5f);
            const auto at=[&](const Vector3& p,const Vector3& half,const float side,const float level) {
                return Vector3(p.X+half.X*side,groundAt(p.X,p.Z)+level,p.Z+half.Z*side);
            };
            b.inside=(at(p0,half0,0.0f,(u0+hA)*0.5f)+at(p1,half1,0.0f,(u0+hB)*0.5f))*0.5f;
            const float tallest=std::max(hA,hB);
            int course=0;
            for(float a=u0;a<tallest-0.03f;a+=Course,++course) {
                const float up=a+Course;
                const float t0=std::min(up,hA),t1=std::min(up,hB);
                if(t0<=a&&t1<=a) continue;
                const Vector3 shade=Stone*(0.84f+0.26f*Hash(id*2654435761U+static_cast<std::uint32_t>(course)*40503U+(++blockCounter&0U)));
                for(const float side:{-1.0f,1.0f})
                    b.Quad(at(p0,half0,side,a),at(p1,half1,side,a),at(p1,half1,side,std::max(t1,a)),at(p0,half0,side,std::max(t0,a)),
                           side>0.0f?shade:shade*0.96f);
            }
            const Vector3 cap=Stone*(0.9f+0.15f*Hash(id*40503U+7U));
            b.Quad(at(p0,half0,-1.0f,hA),at(p1,half1,-1.0f,hB),at(p1,half1,1.0f,hB),at(p0,half0,1.0f,hA),cap*1.04f);
            b.Quad(at(p0,half0,-1.0f,u0),at(p0,half0,1.0f,u0),at(p0,half0,1.0f,hA),at(p0,half0,-1.0f,hA),Stone*0.9f);
            b.Quad(at(p1,half1,1.0f,u0),at(p1,half1,-1.0f,u0),at(p1,half1,-1.0f,hB),at(p1,half1,1.0f,hB),Stone*0.9f);
        };
        for(int k=0;k<count;++k) {
            const float ta=static_cast<float>(k)/static_cast<float>(count),tb=static_cast<float>(k+1)/static_cast<float>(count);
            const Vector3 p0=pos(ta),p1=pos(tb),r0=radial(ta),r1=radial(tb);
            const float h0=heights[static_cast<std::size_t>(k)],h1=heights[static_cast<std::size_t>(k+1)];
            const auto gap=opening((ta+tb)*0.5f);
            const std::uint32_t id=seed*131U+static_cast<std::uint32_t>(k);
            if(gap[0]>=std::max(h0,h1)) {column(p0,p1,r0,r1,Foot,h0,h1,id);continue;}
            column(p0,p1,r0,r1,Foot,gap[0],gap[0],id);
            if(std::min(h0,h1)>gap[1]+0.1f) column(p0,p1,r0,r1,gap[1],h0,h1,id+977U);
        }
    };
    const auto none=[](float) {return std::array<float,2>{{99.0f,99.0f}};};
    // a straight run between two plan points, thick across `across`
    const auto straight=[&](const float r0,const float f0,const float r1,const float f1,const Vector3& across,const float thickness,
                            const auto& top,const auto& opening,const bool battlements,const std::uint32_t seed) {
        const Vector3 from=planar(r0,f0),to=planar(r1,f1);
        const int count=std::max(2,static_cast<int>(std::ceil((to-from).Length()/0.45f)));
        chain(count,[&](const float t) {return Vector3::Lerp(from,to,t);},[&](float) {return across;},thickness,top,opening,battlements,seed);
    };
    // the broken profile of a wall: intact, with stretches fallen to a stump
    const auto broken=[&](const std::uint32_t seed,const float damage,const float minimum) {
        return [=](const float t) {
            const float n=Noise(seed,t*20.0f,3.0f);
            return std::max(minimum,Intact-damage*9.0f*std::max(0.0f,n-0.25f)*1.4f);
        };
    };
    const float mid=K::Half-K::Wall*0.5f,reach=K::Half-1.9f;
    // slits in a wall run `from`..`to` (plan coordinate), where the wall still stands
    const auto slits=[&](const float from,const float to,const std::initializer_list<float> at) {
        return [=](const float t) {
            const float a=from+t*(to-from);
            for(const float s:at) if(std::abs(a-s)<0.2f) return std::array<float,2>{{2.4f,3.7f}};
            return std::array<float,2>{{99.0f,99.0f}};
        };
    };

    // the curtain wall: the front with its gate, the back and west sides, the east with its breach
    straight(-reach,mid,reach,mid,face,K::Wall,broken(11U,0.45f,1.4f),
             [&](const float t) {
                 const float a=-reach+t*2.0f*reach;
                 if(std::abs(a)>=K::GateHalf) return std::array<float,2>{{99.0f,99.0f}};
                 return a<0.0f?std::array<float,2>{{-0.5f,4.0f}}:std::array<float,2>{{-0.5f,99.0f}};   // the left half of the arch still spans, the right fallen
             },true,11U);
    straight(-reach,-mid,reach,-mid,face,K::Wall,broken(23U,0.3f,2.2f),slits(-reach,reach,{-5.0f,-1.5f,5.5f}),true,23U);
    straight(-mid,-reach,-mid,reach,right,K::Wall,broken(37U,0.5f,1.2f),slits(-reach,reach,{-4.5f,2.0f,5.0f}),true,37U);
    straight(mid,-reach,mid,reach,right,K::Wall,
             [&](const float t) {
                 const float f=-reach+t*2.0f*reach;
                 const float base=broken(41U,0.3f,2.0f)(t);
                 // the wall slopes down into the breach over 1.4 m and is a low heap across it
                 const float into=std::clamp(std::min(f-(K::BreachFrom-1.4f),(K::BreachTo+1.4f)-f)/1.4f,0.0f,1.0f);
                 const float heap=0.4f+0.3f*Noise(43U,f,0.8f);
                 return into>0.0f?std::min(base,heap+(base-heap)*(1.0f-into)):base;
             },slits(-reach,reach,{-5.5f,-2.0f}),true,41U);
    // the gatehouse's two stumps flanking the gate, standing out from the wall
    for(const float side:{-1.0f,1.0f}) {
        const float r=side*(K::GateHalf+0.9f);
        straight(r,mid-1.6f,r,mid+1.7f,right,1.7f,
                 [=](const float t) {return side<0.0f?4.8f-1.6f*t+0.6f*Noise(61U,t*9.0f,1.0f):2.6f+0.9f*Noise(67U,t*9.0f,1.0f);},none,false,side<0.0f?61U:67U);
    }
    // a round tower at each corner: two still tall and crowned, two stumps
    {
        int index=0;
        for(const float tr:{-1.0f,1.0f}) for(const float tf:{-1.0f,1.0f}) {
            const Vector3 centre=planar(tr*K::Half,tf*K::Half);
            const float radius=K::TowerRadius-0.45f;
            const bool tall=index==0||index==3;
            const auto seed=static_cast<std::uint32_t>(73+index*8);
            chain(28,[&](const float t) {return centre+Vector3(std::cos(2.0f*Pi*t),0.0f,std::sin(2.0f*Pi*t))*radius;},
                  [&](const float t) {return Vector3(std::cos(2.0f*Pi*t),0.0f,std::sin(2.0f*Pi*t));},0.9f,
                  [=](const float t) {
                      if(tall) return 8.4f-4.2f*std::max(0.0f,Noise(seed,t*14.0f,2.0f)-0.55f);
                      return 2.2f+2.4f*Noise(seed,t*14.0f,2.0f);
                  },
                  [=](const float t) {
                      // an arrow slit looking out over the walls on the tall towers
                      return tall&&std::abs(t-0.12f)<0.017f?std::array<float,2>{{4.0f,5.1f}}:std::array<float,2>{{99.0f,99.0f}};
                  },tall,seed);
            ++index;
        }
    }
    // the keep: a square block of thick walls, the back and west still high, the doorway in the
    // front wall, two windows in the back, the east fallen to a jagged stub
    {
        const float kh=K::KeepHalf,kw=K::KeepWall,km=kh-kw*0.5f,ki=kh-kw;
        const float kr=K::KeepRight,kf=K::KeepForward;
        const auto high=[](const std::uint32_t seed,const float low,const float peak) {
            return [=](const float t) {return low+(peak-low)*std::max(0.0f,1.0f-1.4f*Noise(seed,t*10.0f,2.0f)+0.2f);};
        };
        straight(kr-kh,kf-km,kr+kh,kf-km,face,kw,high(81U,5.0f,10.4f),
                 [&](const float t) {
                     const float a=-kh+t*2.0f*kh;
                     return std::abs(std::abs(a)-1.6f)<0.45f?std::array<float,2>{{3.4f,6.0f}}:std::array<float,2>{{99.0f,99.0f}};
                 },true,81U);
        straight(kr-km,kf-ki,kr-km,kf+ki,right,kw,high(89U,4.0f,10.0f),
                 [&](const float t) {
                     const float a=-ki+t*2.0f*ki;
                     return std::abs(a)<0.4f?std::array<float,2>{{3.6f,6.2f}}:std::array<float,2>{{99.0f,99.0f}};
                 },true,89U);
        straight(kr+km,kf-ki,kr+km,kf+ki,right,kw,[](const float t) {return 1.4f+3.4f*Noise(97U,t*9.0f,1.2f);},none,false,97U);
        straight(kr-kh,kf+km,kr+kh,kf+km,face,kw,high(105U,2.2f,7.0f),
                 [&](const float t) {
                     const float a=-kh+t*2.0f*kh;
                     return std::abs(a+0.3f)<0.85f?std::array<float,2>{{-0.5f,3.3f}}:std::array<float,2>{{99.0f,99.0f}};   // the doorway
                 },false,105U);
    }

    // The well in the courtyard: a ring of blocks round a dark shaft, its timber frame snapped.
    {
        const Vector3 centre=P(4.2f,3.0f);
        const float lipY=0.95f;
        for(int k=0;k<12;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/12.0f;
            const Vector3 out(std::cos(a),0.0f,std::sin(a)),around(-std::sin(a),0.0f,std::cos(a));
            const Vector3 shade=Stone*(0.88f+0.2f*Hash(static_cast<std::uint32_t>(k)*977U+5U));
            b.Box(centre+out*0.85f+Vector3::Up*(lipY*0.5f-0.1f),around,Vector3::Up,out,0.36f,lipY*0.5f+0.1f,0.2f,shade,shade*1.05f);
        }
        b.inside=centre+Vector3::Up*5.0f;
        for(int k=0;k<12;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/12.0f,a1=2.0f*Pi*static_cast<float>(k+1)/12.0f;
            b.Face(centre+Vector3::Up*0.55f,centre+Vector3(std::cos(a0),0.55f,std::sin(a0))*0.66f+Vector3::Up*0.0f,
                   centre+Vector3(std::cos(a1),0.55f,std::sin(a1))*0.66f,Hollow);
        }
        b.tint=nullptr;
        b.Tube({centre+right*0.95f,centre+right*0.95f+Vector3::Up*2.1f},{0.07f,0.06f},6,Timber);
        b.Tube({centre-right*0.95f,centre-right*0.95f+Vector3::Up*1.3f},{0.07f,0.04f},6,Timber*0.9f);
        b.Tube({centre+right*0.95f+Vector3::Up*2.0f,centre+right*0.1f+Vector3::Up*2.06f},{0.05f,0.045f},6,Timber);
        b.tint=nullptr;
    }

    // Fallen masonry: blocks and chunks along the walls' feet on both sides, many more at the
    // breach and by the fallen arch.
    b.tint=[&](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        const float lichenPatch=0.5f+0.5f*std::sin(centre.X*5.1f+centre.Z*3.7f);
        return Vector3::Lerp(pigment*(0.86f+0.18f*lichenPatch),Moss,std::clamp((normal.Y-0.6f)*1.6f,0.0f,1.0f)*0.5f);
    };
    for(int k=0;k<150;++k) {
        const auto seed=static_cast<std::uint32_t>(k)*2654435761U+0x4321U;
        const float along=-reach+2.0f*reach*Hash(seed);
        const float off=(Hash(seed^0x5U)<0.5f?-1.0f:1.0f)*(K::Wall*0.5f+0.2f+1.6f*Hash(seed^0x7U)*Hash(seed^0x7U));
        float r,f;
        switch(k%4) {
        case 0: r=along;f=mid+off;break;
        case 1: r=along;f=-mid+off;break;
        case 2: r=-mid+off;f=along;break;
        default: r=mid+off;f=along;break;
        }
        if(k%4==0&&std::abs(r)<K::GateHalf+0.3f) continue;
        if(k>=100) {   // heaped in the breach and in the gate
            if(k%2==0) {r=mid+off*1.4f;f=K::BreachFrom+(K::BreachTo-K::BreachFrom)*Hash(seed^0x9U);}
            else {r=(Hash(seed^0x9U)-0.5f)*2.0f*(K::GateHalf+0.6f);f=mid+off*0.9f;}
        }
        const float size=0.14f+0.34f*Hash(seed^0x11U)*Hash(seed^0x13U)+(k>=100?0.05f:0.0f);
        const Vector3 at=P(r,f);
        const float spin=Hash(seed^0x3U)*Pi;
        const Vector3 rr(std::cos(spin),0.0f,std::sin(spin));
        const Vector3 shade=Stone*(0.8f+0.3f*Hash(seed^0x17U));
        b.Box(at+Vector3::Up*size*0.4f,rr,Vector3::Up,Vector3(-rr.Z,0.0f,rr.X),size*1.4f,size*0.65f,size,shade,shade*1.05f);
    }
    b.tint=nullptr;

    auto& mesh=b.builder.mesh;
    landmark_shapes::Note("castle",mesh);
    triangles_=static_cast<int>(mesh.size()/3);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(0.2f,0.2f,0.2f),high+Vector3(0.2f,0.2f,0.2f));
    centre_=(low+high)*0.5f;
    vertices_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),
        static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    vertices_->SetData(mesh.data(),static_cast<int>(mesh.size()));
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(760.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"castle",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: castle sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a medieval castle ruin, curtain wall, corner towers and keep, stands inland at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<") its gate toward the sea; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void CastleRuinRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if((centre_-camera).LengthSquared()>320*320
        ||BoundingFrustum(view*projection).Contains(bounds_)==ContainmentType::Disjoint) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);drawCalls_=1;
}

void CastleRuinRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"castle");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: castle sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,triangles_);
    map.CountCasters(1);
}
}
