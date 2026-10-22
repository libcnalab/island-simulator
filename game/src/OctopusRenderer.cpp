#include "IslandSimulator/OctopusRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
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
using namespace Microsoft::Xna::Framework;using namespace Microsoft::Xna::Framework::Graphics;
namespace island{namespace{
constexpr int NearVertexCount=4326,MaxPer=4400;
constexpr int Max=static_cast<int>(OctopusGroup::Count)*MaxPer;constexpr float NearDistance=105;
constexpr const char* OctopusProfileDiagnostic="island-simulator: near octopuses use a rear-swept mantle, smooth analytic skin light, and thicker tapered arms";
constexpr const char* OctopusGeometryDiagnostic="island-simulator: octopus near/far geometry 4326/24 vertices in one bounded batch";
constexpr const char* OctopusEyeDiagnostic="island-simulator: near octopuses use raised ocular bulbs, horizontal pupils, and fitted eye highlights";
constexpr const char* OctopusAnatomyDiagnostic="island-simulator: near octopuses use a narrow rear mantle, hooded inset eyes, and pinched arm tips";
static_assert(MaxPer>=NearVertexCount);
struct Palette{Color mantle,highlight,shadow,arm,sucker,eye,iris;};
// The sand's tones an octopus lying still takes on: the seabed's pale grain, a shade for the
// undersides, the eye left as it is.
Color Toward(const Color&a,const Color&b,const float amount){const float t=std::clamp(amount,0.0f,1.0f);const auto channel=[&](const std::uint8_t x,const std::uint8_t y){return static_cast<int>(std::lround(x+(y-x)*t));};return Color(channel(a.getRProperty(),b.getRProperty()),channel(a.getGProperty(),b.getGProperty()),channel(a.getBProperty(),b.getBProperty()),255);}
Palette Camouflaged(const Palette&base,const float amount){const float t=std::clamp(amount,0.0f,1.0f)*.82f;return{Toward(base.mantle,Color(188,170,132,255),t),Toward(base.highlight,Color(214,198,160,255),t),Toward(base.shadow,Color(146,130,98,255),t),Toward(base.arm,Color(178,160,124,255),t),Toward(base.sucker,Color(206,190,156,255),t*.5f),base.eye,base.iris};}
const std::array<Palette,3> P{{{Color(137,66,45,255),Color(181,101,69,255),Color(91,43,37,255),Color(151,74,50,255),Color(213,177,142,255),Color(158,126,69,255),Color(25,18,13,255)},{Color(111,76,57,255),Color(157,112,81,255),Color(72,49,42,255),Color(126,88,65,255),Color(200,174,145,255),Color(145,123,79,255),Color(22,18,14,255)},{Color(126,82,48,255),Color(176,126,73,255),Color(80,51,34,255),Color(143,93,55,255),Color(220,183,133,255),Color(169,134,67,255),Color(25,18,12,255)}}};
void Tri(std::vector<VertexPositionColor>& o,const Vector3&a,const Vector3&b,const Vector3&c,Color x){o.emplace_back(a,x);o.emplace_back(b,x);o.emplace_back(c,x);}void DTri(std::vector<VertexPositionColor>&o,const Vector3&a,const Vector3&b,const Vector3&c,Color x){Tri(o,a,b,c,x);Tri(o,c,b,a,x);}
void TriColors(std::vector<VertexPositionColor>&o,const Vector3&a,const Color ca,
               const Vector3&b,const Color cb,const Vector3&c,const Color cc)
{o.emplace_back(a,ca);o.emplace_back(b,cb);o.emplace_back(c,cc);}
Color MixColor(const Color&a,const Color&b,const float amount){const float t=std::clamp(amount,0.0f,1.0f);const auto channel=[&](const std::uint8_t x,const std::uint8_t y){return static_cast<int>(std::lround(x+(y-x)*t));};return Color(channel(a.getRProperty(),b.getRProperty()),channel(a.getGProperty(),b.getGProperty()),channel(a.getBProperty(),b.getBProperty()),255);}
Color LitColor(const Color&base,const Vector3&normal,const float variation){const Vector3 sun=Vector3::Normalize(Vector3(.46f,.83f,-.31f));const float light=(.64f+.34f*std::max(0.0f,Vector3::Dot(normal,sun)))*variation;return Color(static_cast<int>(std::clamp(base.getRProperty()*light,0.0f,255.0f)),static_cast<int>(std::clamp(base.getGProperty()*light,0.0f,255.0f)),static_cast<int>(std::clamp(base.getBProperty()*light,0.0f,255.0f)),255);}
struct Basis{Vector3 c,r,u,f;float s;Vector3 point(float x,float y,float z)const{return c+r*(x*s)+u*(y*s)+f*(z*s);}};
Basis basis(const OctopusState&o){Vector3 f(std::cos(o.headingRadians),0,std::sin(o.headingRadians));return{{o.x,o.y,o.z},{-f.Z,0,f.X},Vector3::Up,f,o.scale};}
void Ellipsoid(std::vector<VertexPositionColor>&out,const Basis&b,Vector3 local,float rx,float ry,float rz,Color top,Color side){
    constexpr int rings=8,sides=16;
    std::array<std::array<Vector3,sides>,rings> positions{};
    std::array<std::array<Color,sides>,rings> colors{};
    for(int j=0;j<rings;++j){
        const float latitude=-std::numbers::pi_v<float>*.5f
            +std::numbers::pi_v<float>*static_cast<float>(j)/(rings-1);
        for(int i=0;i<sides;++i){
            const float angle=2*std::numbers::pi_v<float>*static_cast<float>(i)/sides;
            const float cosineLatitude=std::cos(latitude);
            positions[j][i]=b.point(local.X+std::cos(angle)*cosineLatitude*rx,
                local.Y+std::sin(latitude)*ry,
                local.Z+std::sin(angle)*cosineLatitude*rz);
            Vector3 normal=b.r*(std::cos(angle)*cosineLatitude/rx)
                +b.u*(std::sin(latitude)/ry)
                +b.f*(std::sin(angle)*cosineLatitude/rz);
            normal.Normalize();
            const float upperBlend=std::clamp((std::sin(latitude)+.18f)/.92f,0.0f,1.0f);
            Color pigment=MixColor(side,top,upperBlend);
            // a mottled skin: darker reticulated patches and a few pale spots over the mantle
            const float patch=std::sin(angle*3.0f+latitude*4.1f)*std::sin(angle*5.3f-latitude*2.7f);
            pigment=MixColor(pigment,MixColor(side,Color(40,26,20,255),.65f),std::clamp((patch+.05f)/.45f,0.0f,1.0f)*.80f);
            const float spot=std::sin(angle*7.1f+latitude*5.3f)*std::sin(angle*4.3f-latitude*6.1f);
            pigment=MixColor(pigment,MixColor(top,Color(226,204,170,255),.60f),std::clamp((spot-.40f)/.25f,0.0f,1.0f)*.70f);
            const float skinVariation=.95f+.05f*std::sin(angle*2.0f+latitude*1.7f)
                *std::sin(angle*3.0f-latitude*.8f);
            colors[j][i]=LitColor(pigment,normal,skinVariation);
        }
    }
    for(int j=0;j<rings-1;++j)for(int i=0;i<sides;++i){
        const int next=(i+1)%sides;
        TriColors(out,positions[j][i],colors[j][i],positions[j+1][i],colors[j+1][i],
                  positions[j+1][next],colors[j+1][next]);
        TriColors(out,positions[j][i],colors[j][i],positions[j+1][next],colors[j+1][next],
                  positions[j][next],colors[j][next]);
    }
}
void EyeSocket(std::vector<VertexPositionColor>&out,const Basis&b,float side,const Palette&p){
    constexpr int sides=8;
    constexpr std::array<float,3> heights{{-.58f,0.0f,.58f}};
    constexpr std::array<float,3> radii{{.815f,1.0f,.815f}};
    const auto point=[&](std::size_t ring,int segment){
        const float angle=2*std::numbers::pi_v<float>*segment/sides;
        return b.point(side*.185f+std::cos(angle)*radii[ring]*.050f,
            .225f+heights[ring]*.040f,.285f+std::sin(angle)*radii[ring]*.050f);
    };
    const Color upper=MixColor(p.mantle,p.highlight,.22f);
    const Color lower=MixColor(p.shadow,p.mantle,.35f);
    const Vector3 bottom=b.point(side*.185f,.185f,.285f);
    const Vector3 top=b.point(side*.185f,.265f,.285f);
    for(int segment=0;segment<sides;++segment){
        const int next=(segment+1)%sides;
        Tri(out,bottom,point(0,segment),point(0,next),lower);
        for(std::size_t ring=0;ring+1<heights.size();++ring){
            Tri(out,point(ring,segment),point(ring+1,segment),point(ring+1,next),
                ring==0?lower:upper);
            Tri(out,point(ring,segment),point(ring+1,next),point(ring,next),
                ring==0?lower:upper);
        }
        Tri(out,top,point(2,next),point(2,segment),upper);
    }
}
void EyeLens(std::vector<VertexPositionColor>&out,const Basis&b,float side,const Palette&p){constexpr int sides=8;const Vector3 centre=b.point(side*.185f,.226f,.329f);const Vector3 irisApex=centre+b.f*(.016f*b.s),pupilApex=centre+b.f*(.022f*b.s);for(int i=0;i<sides;++i){float a=2*std::numbers::pi_v<float>*i/sides,n=2*std::numbers::pi_v<float>*(i+1)/sides;Vector3 ra=b.r*(std::cos(a)*.031f*b.s)+b.u*(std::sin(a)*.017f*b.s),rn=b.r*(std::cos(n)*.031f*b.s)+b.u*(std::sin(n)*.017f*b.s);Tri(out,irisApex,centre+ra,centre+rn,p.eye);Vector3 pa=b.r*(std::cos(a)*.019f*b.s)+b.u*(std::sin(a)*.0065f*b.s),pn=b.r*(std::cos(n)*.019f*b.s)+b.u*(std::sin(n)*.0065f*b.s);Tri(out,pupilApex,centre+pa+b.f*(.019f*b.s),centre+pn+b.f*(.019f*b.s),p.iris);}Tri(out,centre+b.r*(-.007f*b.s)+b.u*(.009f*b.s)+b.f*(.039f*b.s),centre+b.r*(-.002f*b.s)+b.u*(.013f*b.s)+b.f*(.039f*b.s),centre+b.r*(.003f*b.s)+b.u*(.007f*b.s)+b.f*(.039f*b.s),p.sucker);}
// The arms: six-sided tubes, a little flattened, lit by their own normals, mottled along their
// length with dark patches and pale flecks, paler underneath, tapering to pinched tips; two rows
// of pale suckers run down each underside, shrinking toward the tip.
void Arms(std::vector<VertexPositionColor>&out,const OctopusState&o,const Basis&b,const Palette&p,const float hide,const float jet){
    constexpr int Sides=6,Segments=7;
    for(int arm=0;arm<8;++arm){
        const float angle=2*std::numbers::pi_v<float>*arm/8+.18f;
        std::array<Vector3,Segments+1> centre{},across{},along{};
        std::array<float,Segments+1> width{};
        centre[0]=b.point(std::cos(angle)*.17f,.075f,std::sin(angle)*.17f);
        width[0]=.094f*b.s;
        across[0]=b.f*std::cos(angle)-b.r*std::sin(angle);
        const float reach=(arm==o.probeArm?std::clamp(o.probe,0.f,1.f):0.f);
        for(int seg=1;seg<=Segments;++seg){
            const float t=seg/static_cast<float>(Segments);
            const float length=(.42f+.66f*(1-hide)+.12f*jet+.55f*reach)*t;
            const float curl=std::sin(o.armWave+arm*.93f+seg*.62f)*.17f*t*t*(1-.85f*jet)*(1-.9f*reach);
            const float a=angle+curl;
            centre[static_cast<std::size_t>(seg)]=b.point(std::cos(a)*length,.047f+.035f*std::sin(o.armWave+seg*.8f)*(1-t),std::sin(a)*length);
            width[static_cast<std::size_t>(seg)]=(.088f*(1-t)*(1-.15f*t)+.006f)*b.s;
            across[static_cast<std::size_t>(seg)]=b.f*std::cos(a)-b.r*std::sin(a);   // square to the arm's run
        }
        for(int i=0;i<=Segments;++i){
            const auto at=static_cast<std::size_t>(i);
            along[at]=centre[static_cast<std::size_t>(std::min(i+1,Segments))]-centre[static_cast<std::size_t>(std::max(i-1,0))];
            along[at].Normalize();
        }
        const auto ring=[&](const int i,const int k,Vector3&position,Color&color){
            const auto at=static_cast<std::size_t>(i);
            const float theta=2*std::numbers::pi_v<float>*k/Sides+std::numbers::pi_v<float>/Sides;
            const float c=std::cos(theta),sn=std::sin(theta);
            position=centre[at]+across[at]*(c*width[at])+b.u*(sn*width[at]*.65f);
            Vector3 normal=across[at]*c+b.u*(sn/.65f);
            normal.Normalize();
            // the upper side the arm's colour toward the highlight, the underside paler
            Color pigment=sn>0.0f?MixColor(p.arm,p.highlight,.25f*sn):MixColor(p.arm,p.sucker,.40f*-sn);
            const float patch=std::sin(i*2.3f+arm*1.7f+k*.9f)*std::sin(i*1.1f-arm*.6f+k*1.7f);
            pigment=MixColor(pigment,MixColor(p.shadow,Color(40,26,20,255),.4f),std::clamp((patch+.05f)/.45f,0.0f,1.0f)*.75f*std::max(sn,0.0f));
            const float fleck=std::sin(i*3.7f+arm*2.9f+k*2.3f);
            if(sn>0.0f&&fleck>.70f)pigment=MixColor(pigment,p.sucker,.55f);
            color=LitColor(pigment,normal,1.0f);
        };
        for(int i=0;i<Segments;++i){
            for(int k=0;k<Sides;++k){
                const int kn=(k+1)%Sides;
                Vector3 a,b2,c,d;Color ca,cb,cc,cd;
                ring(i,k,a,ca);ring(i+1,k,b2,cb);ring(i+1,kn,c,cc);ring(i,kn,d,cd);
                TriColors(out,a,ca,b2,cb,c,cc);
                TriColors(out,a,ca,c,cc,d,cd);
            }
        }
        // suckers: a pair on each station's underside but the tip's, each a small pale disc
        const Color suckerColor=LitColor(p.sucker,-b.u,.92f);
        for(int i=1;i<Segments;++i){
            const auto at=static_cast<std::size_t>(i);
            const float radius=width[at]*.30f;
            for(const float sideSign:{-1.0f,1.0f}){
                const Vector3 disc=centre[at]+across[at]*(sideSign*width[at]*.40f)-b.u*(width[at]*.60f+.003f*b.s);
                const Vector3 x=across[at]*radius,y=along[at]*radius;
                TriColors(out,disc-y,suckerColor,disc+x,suckerColor,disc+y,suckerColor);
                TriColors(out,disc-y,suckerColor,disc+y,suckerColor,disc-x,suckerColor);
            }
        }
    }
}
void Near(std::vector<VertexPositionColor>&out,const OctopusState&o){const std::size_t first=out.size();const Palette p=Camouflaged(P[o.colorVariant%P.size()],o.camouflageAmount);const Basis b=basis(o);float hide=std::clamp(o.hideAmount,0.f,1.f);float jet=std::clamp(o.jet,0.f,1.f);float pulse=(1+o.mantlePulse*.045f)*(1-.22f*jet);Ellipsoid(out,b,{0,.31f*(1-hide*.48f),-.34f},.19f*pulse,.33f*(1-hide*.45f),.55f*pulse,p.highlight,p.mantle);Ellipsoid(out,b,{0,.145f,.13f},.315f,.15f,.285f,MixColor(p.mantle,p.highlight,.30f),MixColor(p.shadow,p.mantle,.65f));
for(float side:{-1.f,1.f})EyeSocket(out,b,side,p);
for(float side:{-1.f,1.f})EyeLens(out,b,side,p);
Arms(out,o,b,p,hide,jet);
if(out.size()-first!=NearVertexCount)throw std::runtime_error("Octopus near mesh does not match its exact vertex budget");
}
void Far(std::vector<VertexPositionColor>&out,const OctopusState&o){const Palette p=Camouflaged(P[o.colorVariant%P.size()],o.camouflageAmount);const Basis b=basis(o);Vector3 top=b.point(0,.62f,0),left=b.point(-.32f,.08f,.05f),right=b.point(.32f,.08f,.05f),rear=b.point(0,.08f,-.30f);DTri(out,top,left,right,p.mantle);DTri(out,left,rear,right,p.shadow);for(float side:{-1.f,1.f})DTri(out,rear,b.point(side*.58f,.04f,-.20f),b.point(side*.85f,.02f,-.38f),p.arm);}
}
OctopusRenderer::OctopusRenderer(GraphicsDevice&d,float max):device_(d),effect_(std::make_unique<BasicEffect>(d)),vertices_(std::make_unique<DynamicVertexBuffer>(d,VertexPositionColor::getVertexDeclarationStatic(),Max,BufferUsage::WriteOnly)),maximumDistance_(max){if(!std::isfinite(max)||max<=0)throw std::invalid_argument("Octopus maximum distance must be finite and positive");effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);effect_->setFogEnabledProperty(true);effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));effect_->setFogStartProperty(145);effect_->setFogEndProperty(310);staging_.reserve(Max);constexpr auto marker="island-simulator: octopuses use pulsing mantles, expressive eyes, eight independently curled arms, and pale sucker rows in one draw";std::cout<<marker<<'\n';MirrorRuntimeDiagnosticToPlatformLog(marker);std::cout<<OctopusProfileDiagnostic<<'\n';MirrorRuntimeDiagnosticToPlatformLog(OctopusProfileDiagnostic);std::cout<<OctopusGeometryDiagnostic<<'\n';MirrorRuntimeDiagnosticToPlatformLog(OctopusGeometryDiagnostic);std::cout<<OctopusEyeDiagnostic<<'\n';MirrorRuntimeDiagnosticToPlatformLog(OctopusEyeDiagnostic);std::cout<<OctopusAnatomyDiagnostic<<'\n';MirrorRuntimeDiagnosticToPlatformLog(OctopusAnatomyDiagnostic);}OctopusRenderer::~OctopusRenderer()=default;
void OctopusRenderer::Draw(const Matrix&v,const Matrix&p,const Vector3&camera,std::span<const OctopusState> animals){staging_.clear();visible_=near_=far_=draws_=0;BoundingFrustum frustum(v*p);for(const auto&o:animals){Vector3 c(o.x,o.y+.3f*o.scale,o.z);float ds=Vector3::DistanceSquared(c,camera);if(ds>maximumDistance_*maximumDistance_||!frustum.Intersects(BoundingSphere(c,1.4f*o.scale)))continue;if(ds<=NearDistance*NearDistance){Near(staging_,o);++near_;}else{Far(staging_,o);++far_;}++visible_;}if(staging_.empty())return;device_.setRasterizerStateProperty(RasterizerState::CullNone);device_.setBlendStateProperty(BlendState::Opaque);device_.setDepthStencilStateProperty(DepthStencilState::Default);vertices_->SetData(staging_.data(),0,static_cast<int>(staging_.size()),SetDataOptions::Discard);device_.SetVertexBuffer(vertices_.get());device_.setIndicesProperty(nullptr);effect_->setWorldProperty(Matrix::getIdentityProperty());effect_->setViewProperty(v);effect_->setProjectionProperty(p);underwater_fog::Apply(*effect_,145.0f,310.0f);effect_->Apply();device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(staging_.size()/3));++draws_;}
}
