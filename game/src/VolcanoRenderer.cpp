#include "IslandSimulator/VolcanoRenderer.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SetDataOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <algorithm>
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
using V=VolcanoSite;
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Noise(const float x,const float y) {
    const float fx=std::floor(x),fy=std::floor(y);
    const auto at=[&](const float ix,const float iy) {
        return Hash(static_cast<std::uint32_t>(static_cast<int>(ix)*73856093)^static_cast<std::uint32_t>(static_cast<int>(iy)*19349663));
    };
    float tx=x-fx,ty=y-fy;tx=tx*tx*(3.0f-2.0f*tx);ty=ty*ty*(3.0f-2.0f*ty);
    const float a=at(fx,fy),b=at(fx+1.0f,fy),c=at(fx,fy+1.0f),d=at(fx+1.0f,fy+1.0f);
    return (a+(b-a)*tx)+((c+(d-c)*tx)-(a+(b-a)*tx))*ty;
}
const Vector3 Soil(112,90,70),Moss(84,104,58),Basalt(72,66,64),Ash(140,136,128),Flow(44,42,44),
    Sulphur(206,186,80),Scoria(138,70,52),Wall(64,54,50);

constexpr const char* LavaVertex=R"(
layout(location=0) in vec3 aPosition;
layout(location=1) in vec4 aColor;
uniform mat4 uViewProjection;
out vec3 vWorld;
void main(){
    vWorld=aPosition;
    gl_Position=uViewProjection*vec4(aPosition,1.0);
}
)";
// The lake's crust: dark plates drifting and churning between bright seams of molten rock,
// hot spots welling up through them, the whole glow breathing slowly. Emissive, unlit.
constexpr const char* LavaFragment=R"(
in vec3 vWorld;
uniform float uTime;
out vec4 fragColor;
float hash(vec2 p){p=fract(p*vec2(123.34,456.21));p+=dot(p,p+45.32);return fract(p.x*p.y);}
float noise(vec2 p){vec2 i=floor(p);vec2 f=fract(p);f=f*f*(3.0-2.0*f);
    return mix(mix(hash(i),hash(i+vec2(1.0,0.0)),f.x),mix(hash(i+vec2(0.0,1.0)),hash(i+vec2(1.0,1.0)),f.x),f.y);}
float fbm(vec2 p){float v=0.0;float a=0.5;for(int k=0;k<4;++k){v+=a*noise(p);p=p*2.03+vec2(1.7,9.2);a*=0.5;}return v;}
void main(){
    vec2 p=vWorld.xz*0.55;
    float t=uTime;
    vec2 q=p+vec2(fbm(p+t*0.05),fbm(p-t*0.04))*1.6;
    float n=fbm(q+vec2(t*0.03,-t*0.02));
    float seam=1.0-smoothstep(0.0,0.085,abs(n-0.5));
    float well=smoothstep(0.64,0.92,fbm(q*2.3-vec2(t*0.1,t*0.07)));
    float heat=clamp(seam+well*0.65,0.0,1.0);
    float pulse=0.86+0.14*sin(t*0.9+n*6.0);
    vec3 crust=vec3(0.11,0.045,0.03)+vec3(0.14,0.035,0.0)*n;
    vec3 hot=mix(vec3(0.95,0.26,0.04),vec3(1.0,0.80,0.32),heat*heat);
    fragColor=vec4(mix(crust,hot*pulse,heat),1.0);
}
)";
}

VolcanoRenderer::VolcanoRenderer(GraphicsDevice& device,const IslandTerrain& terrain,const RenderedTerrainSurface& ground)
    :device_(device),effect_(std::make_unique<BasicEffect>(device))
{
    const auto groundAt=[&](const float x,const float z) {return DrawnOrAnalyticHeight(ground,terrain,x,z);};
    const auto at=[&](const float r,const float theta) {
        const float x=V::X+std::cos(theta)*r,z=V::Z+std::sin(theta)*r;
        float y=V::BaseY+V::Profile(r,theta);
        // beyond the foot a short skirt runs down under the lowland
        if(r>V::BaseRadius-0.01f) y=std::min(groundAt(x,z),V::BaseY)-0.6f;
        return Vector3(x,y,z);
    };
    SculptMeshBuilder b;
    b.tintPerQuad=true;
    b.ground=groundAt;b.stainColour=Vector3(96,84,62);b.stainHeight=0.6f;b.stainAmount=0.5f;
    b.tint=[](const Vector3& pigment,const Vector3& normal,const Vector3& centre) {
        const float r=V::Radius(centre.X,centre.Z),theta=V::Bearing(centre.X,centre.Z);
        const float h=centre.Y-V::BaseY;
        if(pigment.X==Wall.X&&pigment.Y==Wall.Y&&pigment.Z==Wall.Z) {
            // the crater's wall: dark, with sulphur crusting its upper ledges
            const float sulphur=std::clamp((Noise(theta*24.0f,h*2.6f)-0.7f)*4.0f,0.0f,1.0f)*std::clamp((h-19.0f)/4.0f,0.0f,1.0f);
            return Vector3::Lerp(Wall*(0.9f+0.15f*Noise(theta*40.0f,h*3.0f)),Sulphur*0.8f,sulphur*0.45f);
        }
        const float n=Noise(centre.X*0.12f,centre.Z*0.12f),fine=Noise(centre.X*0.5f,centre.Z*0.5f);
        // low down weathered soil going green, then black basalt, grey ash high up
        Vector3 colour=Vector3::Lerp(Soil,Moss,std::clamp((n-0.45f)*2.5f,0.0f,1.0f)*std::clamp((7.0f-h)/5.0f,0.0f,1.0f));
        colour=Vector3::Lerp(colour,Basalt,std::clamp((h-3.0f)/6.0f,0.0f,1.0f));
        colour=Vector3::Lerp(colour,Ash,std::clamp((h-13.0f)/8.0f,0.0f,1.0f)*(0.75f+0.25f*n));
        // the old flow, black and blistered
        float d=std::remainder(theta-V::FlowAngle,2.0f*Pi);
        d+=0.05f*std::sin(r*0.21f);
        const float width=0.07f+0.05f*(1.0f-(V::BaseRadius-r)/(V::BaseRadius-V::RimRadius));
        colour=Vector3::Lerp(colour,Flow,std::exp(-(d*d)/(width*width*1.6f)));
        // the rim: red scoria and yellow sulphur
        const float rim=std::clamp((h-23.0f)/2.5f,0.0f,1.0f);
        colour=Vector3::Lerp(colour,Scoria,rim*std::clamp((n-0.4f)*3.0f,0.0f,1.0f)*0.7f);
        colour=Vector3::Lerp(colour,Sulphur,rim*std::clamp((fine-0.7f)*4.0f,0.0f,1.0f)*0.8f);
        // the fumaroles' crusts: bright sulphur round each vent, fading out over two metres
        for(int k=0;k<4;++k) {
            const float vx=V::X+std::cos(V::VentBearings[k])*V::VentRadii[k],vz=V::Z+std::sin(V::VentBearings[k])*V::VentRadii[k];
            const float reach=std::hypot(centre.X-vx,centre.Z-vz)/(1.6f+0.6f*n);
            colour=Vector3::Lerp(colour,Sulphur*1.15f,std::clamp(1.0f-reach,0.0f,1.0f)*0.85f);
        }
        // the gullies hold the darker wet grit
        const float gully=0.5f+0.5f*std::cos(11.0f*theta+2.0f*std::sin(3.0f*theta)+r*0.07f);
        colour*=1.0f-0.12f*std::pow(gully,4.0f)*std::sin(Pi*std::clamp((V::BaseRadius-r)/(V::BaseRadius-V::RimRadius),0.0f,1.0f));
        return colour*(0.96f+0.08f*fine)*(normal.Y>0.0f?1.0f:0.9f);
    };
    constexpr int Around=180;
    // the outer slope: rings closer together toward the rim, where it is steep
    std::vector<float> outer;
    constexpr int Rings=36;
    for(int k=0;k<=Rings;++k) outer.push_back(V::RimRadius+(V::BaseRadius-V::RimRadius)*std::pow(static_cast<float>(k)/Rings,1.35f));
    outer.push_back(V::BaseRadius+2.0f);
    std::vector<float> inner;
    for(int k=0;k<=14;++k) inner.push_back(V::LavaRadius-0.4f+(V::RimRadius-V::LavaRadius+0.4f)*static_cast<float>(k)/14.0f);
    for(int i=0;i<Around;++i) {
        const float t0=2.0f*Pi*static_cast<float>(i)/Around,t1=2.0f*Pi*static_cast<float>(i+1)/Around;
        for(std::size_t k=0;k+1<outer.size();++k) {
            const Vector3 p00=at(outer[k],t0),p10=at(outer[k],t1),p11=at(outer[k+1],t1),p01=at(outer[k+1],t0);
            b.inside=Vector3(V::X,(p00.Y+p11.Y)*0.5f-12.0f,V::Z);
            b.Quad(p00,p10,p11,p01,Basalt);
        }
        for(std::size_t k=0;k+1<inner.size();++k) {
            const Vector3 p00=at(inner[k],t0),p10=at(inner[k],t1),p11=at(inner[k+1],t1),p01=at(inner[k+1],t0);
            const float tm=(t0+t1)*0.5f;
            b.inside=Vector3(V::X+std::cos(tm)*(V::RimRadius+6.0f),(p00.Y+p11.Y)*0.5f,V::Z+std::sin(tm)*(V::RimRadius+6.0f));
            b.Quad(p00,p10,p11,p01,Wall);
        }
    }
    // the fumaroles' mouths: a dark hole ringed with crusted stones
    for(int k=0;k<4;++k) {
        const float vx=V::X+std::cos(V::VentBearings[k])*V::VentRadii[k],vz=V::Z+std::sin(V::VentBearings[k])*V::VentRadii[k];
        const Vector3 mouth(vx,V::SurfaceY(vx,vz)+0.03f,vz);
        constexpr int Rim=10;
        b.inside=mouth-Vector3::Up;
        for(int i=0;i<Rim;++i) {
            const float a0=2.0f*Pi*static_cast<float>(i)/Rim,a1=2.0f*Pi*static_cast<float>(i+1)/Rim;
            const Vector3 r0(std::cos(a0)*0.35f,0.0f,std::sin(a0)*0.35f),r1(std::cos(a1)*0.35f,0.0f,std::sin(a1)*0.35f);
            b.Face(mouth,mouth+r0+Vector3::Up*(V::SurfaceY(vx+r0.X,vz+r0.Z)-mouth.Y+0.02f),
                   mouth+r1+Vector3::Up*(V::SurfaceY(vx+r1.X,vz+r1.Z)-mouth.Y+0.02f),Vector3(30,26,22));
            const Vector3 stone=mouth+(r0+r1)*1.25f;
            const float size=0.12f+0.06f*static_cast<float>((i*7+k)%3);
            b.Box(Vector3(stone.X,V::SurfaceY(stone.X,stone.Z)+size*0.4f,stone.Z),Vector3::Normalize(r0),Vector3::Up,
                  Vector3::Normalize(Vector3::Cross(r0,Vector3::Up)),size,size*0.7f,size,Sulphur*0.95f,Sulphur*1.1f);
        }
    }
    // blocks of black lava and scoria scattered down the slopes, half bedded in the ash
    for(int k=0;k<70;++k) {
        const auto seed=static_cast<std::uint32_t>(k)*2654435761U+0x3c6ef372U;
        const float theta=2.0f*Pi*Hash(seed);
        const float r=V::RimRadius+2.0f+(V::BaseRadius*0.92f-V::RimRadius-2.0f)*std::sqrt(Hash(seed^0x9e37U));
        const float size=0.35f+1.1f*Hash(seed^0x51edU)*Hash(seed^0x1234U);
        const Vector3 base=at(r,theta);
        const Vector3 out(std::cos(theta),0.0f,std::sin(theta)),side(-std::sin(theta),0.0f,std::cos(theta));
        const float spin=Hash(seed^0x77U)*Pi;
        const Vector3 u=out*std::cos(spin)+side*std::sin(spin),w=side*std::cos(spin)-out*std::sin(spin);
        b.Box(base+Vector3::Up*(size*0.25f),u,Vector3::Up,w,size*(0.8f+0.4f*Hash(seed^0x5U)),size*0.55f,size*(0.6f+0.4f*Hash(seed^0x6U)),
              Flow*(0.9f+0.4f*Hash(seed^0x8U)),Flow*(1.1f+0.3f*Hash(seed^0x9U)));
    }
    auto& mesh=b.builder.mesh;
    coneTriangles_=static_cast<int>(mesh.size()/3U);
    Vector3 low(1e9f,1e9f,1e9f),high(-1e9f,-1e9f,-1e9f);
    for(const auto& v:mesh) {low=Vector3::Min(low,v.Position);high=Vector3::Max(high,v.Position);}
    bounds_=BoundingBox(low-Vector3(1,1,1),high+Vector3(1,30,1));
    cone_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(mesh.size()),BufferUsage::WriteOnly);
    cone_->SetData(mesh.data(),static_cast<int>(mesh.size()));

    // the lava's light on the crater's wall, warmest just over the lake
    {
        std::vector<VertexPositionColor> glow;
        const auto lit=[&](const Vector3& p) {
            const float above=std::max(0.0f,p.Y-V::LavaY());
            const float strength=1.7f/(1.0f+(above/3.4f)*(above/3.4f));
            const Vector3 c=Vector3(Wall.X*1.0f,Wall.Y*0.42f,Wall.Z*0.12f)*strength*1.4f;
            return Color(static_cast<int>(std::min(255.0f,c.X)),static_cast<int>(std::min(255.0f,c.Y)),static_cast<int>(std::min(255.0f,c.Z)));
        };
        for(int i=0;i<Around;++i) {
            const float t0=2.0f*Pi*static_cast<float>(i)/Around,t1=2.0f*Pi*static_cast<float>(i+1)/Around;
            std::vector<float> rings(inner);
            rings.push_back(V::RimRadius+0.6f);rings.push_back(V::RimRadius+1.4f);
            for(std::size_t k=0;k+1<rings.size();++k) {
                const Vector3 p00=at(rings[k],t0),p10=at(rings[k],t1),p11=at(rings[k+1],t1),p01=at(rings[k+1],t0);
                // a few centimetres off the wall toward the crater's axis and up, so the light
                // never loses the depth test to the wall it lies on
                for(Vector3 p:{p00,p10,p11,p00,p11,p01}) {
                    Vector3 in(V::X-p.X,0.0f,V::Z-p.Z);
                    if(in.LengthSquared()>1e-6f) in.Normalize();
                    glow.emplace_back(p+in*0.04f+Vector3::Up*0.04f,lit(p));
                }
            }
        }
        glowTriangles_=static_cast<int>(glow.size()/3U);
        glow_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(glow.size()),BufferUsage::WriteOnly);
        glow_->SetData(glow.data(),static_cast<int>(glow.size()));
    }
    // the lake: a disc tucked under the wall's foot
    {
        std::vector<VertexPositionColor> disc;
        constexpr int Segments=40,Bands=5;
        const float y=V::LavaY();
        for(int i=0;i<Segments;++i) {
            const float t0=2.0f*Pi*static_cast<float>(i)/Segments,t1=2.0f*Pi*static_cast<float>(i+1)/Segments;
            for(int k=0;k<Bands;++k) {
                const float r0=(V::LavaRadius+0.5f)*static_cast<float>(k)/Bands,r1=(V::LavaRadius+0.5f)*static_cast<float>(k+1)/Bands;
                const auto p=[&](const float r,const float t) {return Vector3(V::X+std::cos(t)*r,y,V::Z+std::sin(t)*r);};
                const Color white(255,255,255);
                for(const Vector3& q:{p(r0,t0),p(r0,t1),p(r1,t1),p(r0,t0),p(r1,t1),p(r1,t0)}) disc.emplace_back(q,white);
            }
        }
        lavaTriangles_=static_cast<int>(disc.size()/3U);
        lava_=std::make_unique<VertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),static_cast<int>(disc.size()),BufferUsage::WriteOnly);
        lava_->SetData(disc.data(),static_cast<int>(disc.size()));
        using CNA::Graphics::ShaderCodeEXT;
        const auto source=[](bool es,const char* body){
            return std::string(es?"#version 300 es\nprecision highp float;\n":"#version 330 core\n")+body;};
        try {
            const CNA::Graphics::ShaderPackageEXT package({
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","lava.es.vert",source(true,LavaVertex)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","lava.es.frag",source(true,LavaFragment)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","lava.vert",source(false,LavaVertex)),
                ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","lava.frag",source(false,LavaFragment))},
                {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment},{});
            auto effect=std::make_unique<ShaderEffect>(device,package);
            if(effect->IsEffectValid()) lavaEffect_=std::move(effect);
            else std::cout<<"island-simulator: lava shader unavailable: "<<effect->GetCompileErrorEXT()<<'\n';
        } catch(const std::exception& error) {
            std::cout<<"island-simulator: lava shader unavailable: "<<error.what()<<'\n';
        }
    }
    effect_->setVertexColorEnabledProperty(true);effect_->setLightingEnabledProperty(false);
    effect_->setFogEnabledProperty(true);
    effect_->setFogColorProperty(Vector3(environment::FogColorRed,environment::FogColorGreen,environment::FogColorBlue));
    effect_->setFogStartProperty(300.0f);effect_->setFogEndProperty(900.0f);
    try {receiver_=std::make_unique<StructureShadowReceiver>(device,"volcano",false,300.0f,900.0f,0.46f);}
    catch(const std::exception& error) {
        std::cout<<"island-simulator: volcano sun shadow receiver unavailable: "<<error.what()<<'\n';
    }
    halo_=std::make_unique<DynamicVertexBuffer>(device,VertexPositionColor::getVertexDeclarationStatic(),256,BufferUsage::WriteOnly);
    haloStaging_.reserve(256);
    plume_=std::make_unique<CascadeMistRenderer>(device,"island-simulator: steam rises out of the volcano's crater in a plume leaning downwind, and out of four sulphur-crusted fumaroles on its slopes",
                                                 static_cast<std::size_t>(PlumePuffs+4*VentPuffs));
    plume_->SetRange(260.0f,240.0f,1.5f);
    std::ostringstream message;
    message<<"island-simulator: a small volcano rises "<<static_cast<int>(std::lround(V::Height))<<" m from the eastern foothills at ("
        <<static_cast<int>(std::lround(V::X))<<", "<<static_cast<int>(std::lround(V::Z))<<"), lava glowing in its crater; "
        <<coneTriangles_<<" + "<<glowTriangles_<<" + "<<lavaTriangles_<<" triangles"<<(lavaEffect_?"":" (lava shader unavailable)");
    std::cout<<message.str()<<'\n';MirrorRuntimeDiagnosticToPlatformLog(message.str());
}

bool VolcanoRenderer::Visible(const Matrix& view,const Matrix& projection,const Vector3& camera) const {
    return (Vector3(V::X,V::BaseY,V::Z)-camera).LengthSquared()<1500.0f*1500.0f
        &&BoundingFrustum(view*projection).Contains(bounds_)!=ContainmentType::Disjoint;
}

void VolcanoRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera) {
    drawCalls_=0;
    if(!Visible(view,projection,camera)) return;
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
    device_.SetVertexBuffer(cone_.get());device_.setIndicesProperty(nullptr);
    if(receiver_) receiver_->Apply(view*projection,camera,sunShadow_);
    else {
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    }
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,coneTriangles_);drawCalls_=1;
}

void VolcanoRenderer::DrawSunShadow(SunShadowMap& map) {
    if(sunShadowUnavailable_||!map.CastsInto(V::X,V::Z)) return;
    if(!sunShadowEffect_) {
        try {sunShadowEffect_=map.CreateCasterEffect(SunShadowMap::StructureCasterVertex,"volcano");}
        catch(const std::exception& error) {
            std::cout<<"island-simulator: volcano sun shadow caster unavailable: "<<error.what()<<'\n';
            sunShadowUnavailable_=true;return;
        }
    }
    float matrix[16];map.LightViewProjectionColumnMajor(matrix);
    device_.SetVertexBuffer(cone_.get());device_.setIndicesProperty(nullptr);
    sunShadowEffect_->Apply();sunShadowEffect_->SetUniformMat4("uSunViewProjection",matrix);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,coneTriangles_);
    map.CountCasters(1);
}

void VolcanoRenderer::DrawPlume(const Matrix& view,const Matrix& projection,const Vector3& camera,const float seconds) {
    if(!std::isfinite(seconds)) return;
    // each puff born in turn over the lake, rising, swelling and leaning away on the wind
    constexpr float Life=18.0f;
    const Vector3 wind(wind::DirectionX,0.0f,wind::DirectionZ),side(-wind::DirectionZ,0.0f,wind::DirectionX);
    for(int k=0;k<PlumePuffs;++k) {
        const float phase=std::fmod(seconds/Life+static_cast<float>(k)/PlumePuffs,1.0f);
        const float age=phase*Life;
        const Vector3 p=Vector3(V::X,V::LavaY()+1.5f,V::Z)+Vector3::Up*(1.5f*age+0.03f*age*age)
            +wind*(0.15f*age*age*0.5f+0.4f*age)+side*(1.3f*std::sin(static_cast<float>(k)*1.7f+age*0.35f));
        MistPuff& puff=puffs_[static_cast<std::size_t>(k)];
        puff.x=p.X;puff.y=p.Y;puff.z=p.Z;
        puff.size=2.6f+0.6f*age;puff.age=age;
        puff.density=std::clamp(age/2.0f,0.0f,1.0f)*std::pow(1.0f-phase,1.4f);
        puff.active=true;
    }
    // the fumaroles: thin wisps out of the slope, a few metres up before they thin away
    constexpr float VentLife=6.0f;
    for(int v=0;v<4;++v) {
        const float vx=V::X+std::cos(V::VentBearings[v])*V::VentRadii[v],vz=V::Z+std::sin(V::VentBearings[v])*V::VentRadii[v];
        const Vector3 vent(vx,V::SurfaceY(vx,vz)+0.2f,vz);
        for(int k=0;k<VentPuffs;++k) {
            const float phase=std::fmod(seconds/VentLife+static_cast<float>(k)/VentPuffs+static_cast<float>(v)*0.37f,1.0f);
            const float age=phase*VentLife;
            const Vector3 p=vent+Vector3::Up*(0.9f*age)+wind*(0.18f*age*age);
            MistPuff& puff=puffs_[static_cast<std::size_t>(PlumePuffs+v*VentPuffs+k)];
            puff.x=p.X;puff.y=p.Y;puff.z=p.Z;
            puff.size=0.5f+0.35f*age;puff.age=age;
            puff.density=0.55f*std::clamp(age/0.8f,0.0f,1.0f)*std::pow(1.0f-phase,1.6f);
            puff.active=true;
        }
    }
    plume_->Draw(view,projection,camera,puffs_);
    drawCalls_+=plume_->DrawCallCount();
}

void VolcanoRenderer::DrawGlow(const Matrix& view,const Matrix& projection,const Vector3& camera,const float seconds,const float night) {
    if(!Visible(view,projection,camera)) return;
    const float pulse=0.88f+0.08f*std::sin(seconds*0.9f)+0.04f*std::sin(seconds*2.3f+0.7f);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);
    device_.setBlendStateProperty(BlendState::Additive);
    device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
    effect_->setDiffuseColorProperty(Vector3(pulse,pulse,pulse));
    effect_->setWorldProperty(Matrix::getIdentityProperty());
    effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
    device_.SetVertexBuffer(glow_.get());device_.setIndicesProperty(nullptr);
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,glowTriangles_);++drawCalls_;
    effect_->setDiffuseColorProperty(Vector3(1.0f,1.0f,1.0f));
    if(lavaEffect_) {
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        float matrix[16];(view*projection).ToColumnMajor(matrix);
        lavaEffect_->Apply();
        lavaEffect_->SetUniformMat4("uViewProjection",matrix);
        lavaEffect_->SetUniformFloat("uTime",std::remainder(seconds,600.0f)+600.0f);
        device_.SetVertexBuffer(lava_.get());device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList,0,lavaTriangles_);++drawCalls_;
    }
    // at night the lake lights the steam over the crater: a soft glow standing over the rim
    if(night>0.01f) {
        haloStaging_.clear();
        Vector3 toEye=camera-Vector3(V::X,V::LavaY()+8.0f,V::Z);
        if(toEye.LengthSquared()<1e-4f) toEye=Vector3::UnitZ;
        toEye.Normalize();
        Vector3 right=Vector3::Cross(toEye,Vector3::Up);
        if(right.LengthSquared()<1e-6f) right=Vector3::UnitX;
        right.Normalize();
        const Vector3 up=Vector3::Cross(right,toEye);
        const auto fan=[&](const Vector3& centre,const float radius,const Vector3& colour) {
            const Color core(static_cast<int>(std::min(255.0f,colour.X*255.0f)),static_cast<int>(std::min(255.0f,colour.Y*255.0f)),
                             static_cast<int>(std::min(255.0f,colour.Z*255.0f))),rim(0,0,0);
            constexpr int Segments=20;
            for(int k=0;k<Segments;++k) {
                const float a0=2.0f*Pi*static_cast<float>(k)/Segments,a1=2.0f*Pi*static_cast<float>(k+1)/Segments;
                haloStaging_.emplace_back(centre,core);
                haloStaging_.emplace_back(centre+(right*std::cos(a0)+up*std::sin(a0)*1.3f)*radius,rim);
                haloStaging_.emplace_back(centre+(right*std::cos(a1)+up*std::sin(a1)*1.3f)*radius,rim);
            }
        };
        const float glow=night*pulse;
        fan(Vector3(V::X,V::LavaY()+8.0f,V::Z),15.0f,Vector3(0.55f,0.20f,0.05f)*glow);
        fan(Vector3(V::X,V::LavaY()+14.0f,V::Z),28.0f,Vector3(0.20f,0.07f,0.02f)*glow);
        halo_->SetData(haloStaging_.data(),0,static_cast<int>(haloStaging_.size()),SetDataOptions::Discard);
        device_.setBlendStateProperty(BlendState::Additive);
        device_.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);effect_->setProjectionProperty(projection);effect_->Apply();
        device_.SetVertexBuffer(halo_.get());device_.setIndicesProperty(nullptr);
        device_.DrawPrimitives(PrimitiveType::TriangleList,0,static_cast<int>(haloStaging_.size()/3U));++drawCalls_;
    }
    device_.setBlendStateProperty(BlendState::Opaque);
    device_.setDepthStencilStateProperty(DepthStencilState::Default);
}
}
