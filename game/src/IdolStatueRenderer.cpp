#include "IslandSimulator/IdolStatueRenderer.hpp"
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
// Weathered basalt (pigment past full: the build lights faces 0.46 + 0.54 n.s), the darks of the
// carved hollows, moss and lichen.
const Vector3 Stone(132,127,118),Hollow(44,41,38),Moss(74,96,52),Lichen(168,162,122);

float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}

// Linear interpolation through (u, value) control points.
float Through(const std::vector<std::array<float,2>>& points,const float u) {
    if(u<=points.front()[0]) return points.front()[1];
    for(std::size_t i=0;i+1<points.size();++i)
        if(u<=points[i+1][0]) return points[i][1]+(points[i+1][1]-points[i][1])*(u-points[i][0])/(points[i+1][0]-points[i][0]);
    return points.back()[1];
}
}

IdolStatueRenderer::IdolStatueRenderer(GraphicsDevice& device,const IdolStatuePlacement& placement,
                                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    SculptMeshBuilder b;
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    b.ground=groundAt;b.stainColour=Vector3(86,78,62);b.stainHeight=0.18f;b.stainAmount=0.45f;
    // Moss on the faces turned up, lichen in patches, rain streaks down the sides.
    b.tint=[](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        const float patch=std::sin(centre.X*2.3f+centre.Y*1.7f)*std::sin(centre.Z*2.9f-centre.Y*2.1f);
        Vector3 colour=pigment;
        if(pigment.X>60.0f) {
            const float streak=0.88f+0.12f*std::sin(centre.X*9.0f+centre.Z*7.0f);
            colour=colour*(std::abs(normal.Y)<0.4f?streak:1.0f);
            colour=Vector3::Lerp(colour,Lichen,std::clamp(patch-0.45f,0.0f,1.0f)*0.8f);
            colour=Vector3::Lerp(colour,Moss,std::clamp((normal.Y-0.45f)*2.2f,0.0f,1.0f)*(0.45f+0.35f*patch));
        }
        return colour;
    };
    const Vector3 face(placement.faceX,0.0f,placement.faceZ),right(placement.faceZ,0.0f,-placement.faceX);
    const Vector3 base(placement.x,placement.TopY(),placement.z);
    const auto P=[&](const float r,const float u,const float f) {return base+right*r+Vector3::Up*u+face*f;};

    // The ahu: a core faced with two courses of fitted blocks, paved on top with slabs.
    float lowest=placement.groundY;
    for(const float r:{-1.0f,1.0f}) for(const float f:{-1.0f,1.0f}) {
        const Vector3 corner=P(r*IdolStatuePlacement::PlatformHalfWidth,0.0f,f*IdolStatuePlacement::PlatformHalfDepth);
        lowest=std::min(lowest,groundAt(corner.X,corner.Z));
    }
    const float bottom=lowest-0.25f-placement.TopY();   // in platform-top coordinates
    b.Box(P(0.0f,bottom*0.5f-0.02f,0.0f),right,Vector3::Up,face,IdolStatuePlacement::PlatformHalfWidth-0.04f,
          -bottom*0.5f,IdolStatuePlacement::PlatformHalfDepth-0.04f,Stone*0.8f,Stone*0.8f);
    std::uint32_t block=0;
    for(int course=0;course<2;++course) {
        const float top=course==0?0.0f:-IdolStatuePlacement::PlatformHeight*0.5f;
        const float down=course==0?-IdolStatuePlacement::PlatformHeight*0.5f:bottom;
        for(int sideIndex=0;sideIndex<4;++sideIndex) {
            const bool alongWidth=sideIndex<2;
            const float sign=sideIndex%2==0?1.0f:-1.0f;
            const float half=alongWidth?IdolStatuePlacement::PlatformHalfWidth:IdolStatuePlacement::PlatformHalfDepth;
            const float offset=alongWidth?IdolStatuePlacement::PlatformHalfDepth:IdolStatuePlacement::PlatformHalfWidth;
            float at=-half+(course==1?0.35f:0.0f)*0.5f;
            while(at<half-0.05f) {
                const float length=std::min(0.62f+0.5f*Hash(++block*2654435761U),half-at);
                const float mid=at+length*0.5f;
                const Vector3 centre=alongWidth?P(mid,(top+down)*0.5f,sign*offset):P(sign*offset,(top+down)*0.5f,mid);
                const Vector3 along=alongWidth?right:face,out=alongWidth?face*sign:right*sign;
                const Vector3 shade=Stone*(0.84f+0.22f*Hash(block*7919U+3U));
                b.Box(centre+out*0.02f,along,Vector3::Up,out,length*0.5f-0.015f,(top-down)*0.5f-0.012f,0.05f,shade,shade*1.04f);
                at+=length;
            }
        }
    }
    for(float r=-IdolStatuePlacement::PlatformHalfWidth+0.45f;r<IdolStatuePlacement::PlatformHalfWidth;r+=0.9f)
        for(float f=-IdolStatuePlacement::PlatformHalfDepth+0.45f;f<IdolStatuePlacement::PlatformHalfDepth;f+=0.85f) {
            const Vector3 shade=Stone*(0.86f+0.18f*Hash(++block*104729U));
            b.Box(P(r,0.015f+0.01f*Hash(block),f),right,Vector3::Up,face,0.43f,0.03f,0.40f,shade,shade);
        }

    // The god, standing on its carved base.
    const float stand=0.06f;
    b.Box(P(0.0f,stand*0.5f,0.0f),right,Vector3::Up,face,0.78f,stand*0.5f,0.58f,Stone*0.95f,Stone);
    const auto S=[&](const float r,const float u,const float f) {return P(r,u+stand,f);};
    // the body and the great head: superelliptic rings up the figure (half-widths and
    // half-depths), the belly and the face carried forward
    const std::vector<std::array<float,2>> width{{{0.55f,0.55f},{0.75f,0.58f},{0.95f,0.60f},{1.15f,0.60f},{1.35f,0.62f},
            {1.55f,0.64f},{1.70f,0.56f},{1.78f,0.50f},{1.90f,0.66f},{2.05f,0.74f},{2.25f,0.78f},{2.45f,0.80f},{2.85f,0.80f},
            {3.00f,0.78f},{3.12f,0.74f},{3.20f,0.68f},{3.27f,0.55f},{3.30f,0.30f}}};
    const std::vector<std::array<float,2>> depth{{{0.55f,0.42f},{0.75f,0.48f},{0.95f,0.52f},{1.15f,0.50f},{1.35f,0.44f},
            {1.55f,0.40f},{1.70f,0.36f},{1.78f,0.34f},{1.90f,0.50f},{2.05f,0.56f},{2.25f,0.60f},{2.45f,0.60f},{2.85f,0.58f},
            {3.00f,0.56f},{3.12f,0.53f},{3.20f,0.48f},{3.27f,0.38f},{3.30f,0.20f}}};
    const std::vector<std::array<float,2>> forward{{{0.55f,0.0f},{0.75f,0.04f},{0.95f,0.07f},{1.15f,0.06f},{1.35f,0.02f},
            {1.55f,0.0f},{1.90f,0.03f},{2.25f,0.06f},{2.85f,0.05f},{3.12f,0.03f},{3.30f,0.0f}}};
    const auto front=[&](const float u) {return Through(depth,u)+Through(forward,u);};
    {
        constexpr int Ring=24;
        std::vector<std::vector<Vector3>> rings;std::vector<Vector3> centres;
        for(float u=0.55f;u<=3.301f;u+=0.0625f) {
            const float w=Through(width,u),d=Through(depth,u),c=Through(forward,u);
            std::vector<Vector3> ring;
            for(int k=0;k<Ring;++k) {
                const float a=2.0f*Pi*static_cast<float>(k)/Ring;
                const float ca=std::cos(a),sa=std::sin(a);
                const float x=std::copysign(std::pow(std::abs(ca),0.78f),ca),y=std::copysign(std::pow(std::abs(sa),0.78f),sa);
                ring.push_back(S(x*w,u,c+y*d));
            }
            rings.push_back(ring);centres.push_back(S(0.0f,u,c));
        }
        b.Loft(rings,centres,[&](std::size_t,std::size_t) {return Stone;});
    }
    // a ringed hollow on the face (the eyes, the mouth): a raised rim round a dark recess
    const auto hollow=[&](const float r,const float u,const float halfWidth,const float halfHeight,const float rim) {
        constexpr int Segments=14;
        const float f0=front(u);
        b.inside=S(r,u,f0-0.3f);
        for(int k=0;k<Segments;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/Segments,a1=2.0f*Pi*static_cast<float>(k+1)/Segments;
            const auto at=[&](const float a,const float scale,const float lift) {
                return S(r+std::cos(a)*halfWidth*scale,u+std::sin(a)*halfHeight*scale,f0+lift);
            };
            b.Quad(at(a0,1.0f+rim,0.0f),at(a1,1.0f+rim,0.0f),at(a1,1.0f,0.045f),at(a0,1.0f,0.045f),Stone*1.04f);
            b.Quad(at(a0,1.0f,0.045f),at(a1,1.0f,0.045f),at(a1,0.92f,0.01f),at(a0,0.92f,0.01f),Stone*0.8f);
            b.Face(S(r,u,f0-0.01f),at(a0,0.92f,0.01f),at(a1,0.92f,0.01f),Hollow);
        }
    };
    hollow(-0.25f,2.70f,0.17f,0.115f,0.35f);
    hollow(0.25f,2.70f,0.17f,0.115f,0.35f);
    hollow(0.0f,2.20f,0.30f,0.085f,0.30f);
    // the heavy brow, the broad nose with its nostrils, the tongue out over the lower lip
    b.Box(S(0.0f,2.90f,front(2.90f)+0.02f),right,Vector3::Up,face,0.62f,0.055f,0.07f,Stone,Stone*1.05f);
    {
        const float f0=front(2.42f);
        const Vector3 bl=S(-0.17f,2.42f,f0),br=S(0.17f,2.42f,f0),top=S(0.0f,2.72f,front(2.72f)),tip=S(0.0f,2.45f,f0+0.16f);
        b.inside=S(0.0f,2.50f,f0-0.1f);
        b.Face(bl,tip,top,Stone);b.Face(tip,br,top,Stone);b.Face(bl,br,tip,Stone*0.9f);
        for(const float side:{-1.0f,1.0f}) {
            b.inside=S(side*0.07f,2.44f,f0);
            b.Quad(S(side*0.03f,2.432f,f0+0.10f),S(side*0.11f,2.432f,f0+0.06f),S(side*0.11f,2.432f,f0+0.02f),S(side*0.03f,2.432f,f0+0.05f),Hollow);
        }
    }
    b.Box(S(0.0f,2.14f,front(2.14f)+0.06f),right,Vector3::Up-face*0.35f,face,0.075f,0.028f,0.08f,Stone*0.92f,Stone*0.95f);
    // the ears and the band round the head, and the topknot
    for(const float side:{-1.0f,1.0f})
        b.Blade(S(side*0.78f,2.62f,0.05f),face,right*side,Vector3::Up,0.035f,0.09f,0.17f,0.10f,Stone*0.95f);
    {
        std::vector<Vector3> band;std::vector<float> radii;
        for(int k=0;k<=24;++k) {
            const float a=2.0f*Pi*static_cast<float>(k)/24.0f;
            const float ca=std::cos(a),sa=std::sin(a);
            const float x=std::copysign(std::pow(std::abs(ca),0.78f),ca),y=std::copysign(std::pow(std::abs(sa),0.78f),sa);
            band.push_back(S(x*0.80f,3.05f,0.04f+y*0.57f));radii.push_back(0.045f);
        }
        b.Tube(band,radii,6,Stone*0.97f);
    }
    b.Tube({S(0.0f,3.22f,0.02f),S(0.0f,3.44f,0.02f)},{0.30f,0.27f},16,Stone*0.98f);
    // the arms folded to the belly, the hands' fingers grooved
    for(const float side:{-1.0f,1.0f}) {
        b.Tube({S(side*0.66f,1.56f,0.04f),S(side*0.74f,1.20f,0.26f),S(side*0.34f,1.02f,front(1.02f)-0.02f)},{0.15f,0.13f,0.11f},10,Stone);
        const Vector3 hand=S(side*0.17f,1.00f,front(1.0f)+0.03f);
        b.Box(hand,right,Vector3::Up,face,0.15f,0.10f,0.055f,Stone,Stone*1.05f);
        b.inside=hand;
        for(int finger=0;finger<3;++finger) {
            const float u=0.94f+0.055f*static_cast<float>(finger);
            const float f=front(1.0f)+0.087f;
            b.Quad(S(side*0.03f,u,f),S(side*0.31f,u,f),S(side*0.31f,u+0.012f,f),S(side*0.03f,u+0.012f,f),Hollow);
        }
        // the bent legs and the feet
        b.Tube({S(side*0.30f,0.66f,0.0f),S(side*0.42f,0.36f,0.30f),S(side*0.38f,0.10f,0.16f)},{0.22f,0.19f,0.16f},10,Stone);
        b.Box(S(side*0.38f,0.05f,0.30f),right,Vector3::Up,face,0.14f,0.05f,0.23f,Stone*0.95f,Stone);
    }
    {
        // the navel
        const float f0=front(0.90f);
        b.inside=S(0.0f,0.90f,0.0f);
        for(int k=0;k<8;++k) {
            const float a0=2.0f*Pi*static_cast<float>(k)/8.0f,a1=2.0f*Pi*static_cast<float>(k+1)/8.0f;
            b.Face(S(0.0f,0.90f,f0+0.003f),S(std::cos(a0)*0.04f,0.90f+std::sin(a0)*0.04f,f0+0.003f),
                   S(std::cos(a1)*0.04f,0.90f+std::sin(a1)*0.04f,f0+0.003f),Hollow);
        }
    }

    auto& mesh=b.builder.mesh;
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
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"idol",false,300.0f,760.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: idol sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    std::ostringstream message;
    message<<"island-simulator: a weathered stone god, a tiki idol on its ahu platform, stands on a rise at ("
        <<static_cast<int>(std::lround(placement.x))<<", "<<static_cast<int>(std::lround(placement.z))
        <<") looking out to sea; "<<triangles_<<" triangles in one draw";
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

void IdolStatueRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
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

void IdolStatueRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(centre_.X,centre_.Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"idol");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: idol sun shadow caster unavailable: "<<error.what()<<'\n';
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
