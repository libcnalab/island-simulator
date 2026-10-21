#include "IslandSimulator/CampfireRenderer.hpp"
#include "IslandSimulator/CampfireLighting.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/WindGust.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColorTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
namespace island {
namespace {
constexpr const char* vertex=R"(
layout(location=0) in vec3 aPosition;
layout(location=1) in vec4 aColor;
layout(location=2) in vec2 aTexCoord;
uniform mat4 uViewProjection;
uniform vec3 uRight;uniform vec3 uForward;
uniform float uMoving;
uniform float uGust;
uniform float uTend;
uniform float uDouse;
out vec3 world;out vec4 pigment;out vec2 uv;out float life;out float plumeHeight;
void main(){
    pigment=aColor;uv=aTexCoord;life=1.0;world=aPosition;plumeHeight=aPosition.y;
    if(uMoving>.5){
        vec3 local=aPosition;
        if(aColor.a>.6){
            float age=fract(uCampfire.w*.23+aColor.r)*4.35;
            local.y+=age*.45;local.x+=sin(age*3.0+aColor.r*37.0)*.10-age*.035;
            // a downpour puts the sparks out
            life=(1.0-smoothstep(2.6,4.35,age))*(1.0-uDouse);
        }else if(aColor.a>.35){
            // The upper plume's strips (spread below 255) sway and lift by their height
            // alone, so where two strips overlap they move as one column.
            bool plume=aColor.b<.99;
            float altitude=plume?.6*smoothstep(0.0,13.0,aPosition.y):1.0-aColor.g;
            float phase=plume?0.0:aColor.r;
            // A tended fire stands taller: the tongues and the plume stretched up a third.
            local.y*=1.0+uTend*.35;
            local.x+=sin(uCampfire.w*(.47-altitude*.16)+phase*13.0+local.y*.31)
                *(.12+altitude*.42)-local.y*.032;
            local.z+=sin(uCampfire.w*.19+phase*29.0+local.y*.17)*altitude*.26;
        }else{
            // the rain beats the tongues down towards the embers
            local.y*=1.0-.5*uDouse;
        }
        world=uCampfire.xyz-vec3(0,.45,0)+uRight*local.x+vec3(0,local.y,0)+uForward*local.z;
        if(aColor.a>.35&&aColor.a<.6){
            bool plume=aColor.b<.99;
            float altitude=plume?.6*smoothstep(0.0,13.0,aPosition.y):1.0-aColor.g;
            float phase=plume?0.0:aColor.r;
            float rise=max(local.y-.35,0.0);
            // The shared easterly trade wind bends the high, cool plume west in
            // world space rather than in camera-facing billboard space; a passing gust
            // front lays it over further and tears it sideways as the grass leans.
            world.x-=altitude*(rise*(.34+uGust*.55)+sin(uCampfire.w*.11+phase*19.0+local.y*.12)*.38);
            world.z+=altitude*rise*uGust*.30*sin(uCampfire.w*.9+phase*23.0+local.y*.6);
        }
    }
    gl_Position=uViewProjection*vec4(world,1);
    gl_Position.z=2.0*gl_Position.z-gl_Position.w;
}
)";
constexpr const char* fragment=R"(
in vec3 world;in vec4 pigment;in vec2 uv;in float life;in float plumeHeight;
uniform float uMoving;
uniform vec3 uCamera;
uniform float uDouse;
out vec4 fragColor;
float hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
float noise(vec2 p){vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y);}
float fbm(vec2 p){return noise(p)*.57+noise(p*2.13+13.1)*.28+noise(p*4.1-7.4)*.15;}
void main(){
    float t=uCampfire.w;
    if(uMoving<.5){
        int tag=int(pigment.a*255.0/36.0+.5);
        vec3 color;
        if(tag==0){color=mix(vec3(.27,.28,.27),vec3(.48,.46,.41),fbm(world.xz*7.0+world.y*3.0));}
        else if(tag==1){
            float fissure=pow(.5+.5*sin(uv.x*85.0+noise(uv*11.0)*2.0),12.0);
            color=mix(vec3(.035,.027,.024),vec3(.115,.071,.038),noise(uv*23.0));
            color+=vec3(.28,.035,.002)*fissure*campfirePulse(t);
        }else if(tag==2){
            float bark=noise(vec2(uv.x*81.0,uv.y*4.0));
            color=mix(vec3(.13,.072,.032),vec3(.36,.24,.12),bark);
        }else if(tag==3){
            float rings=.5+.5*sin(length(uv)*53.0+noise(uv*12.0)*2.0);
            color=mix(vec3(.37,.23,.11),vec3(.62,.46,.28),rings*.65);
        }else if(tag==4){
            float radius=length(world.xz-uCampfire.xz);
            float ash=1.0-smoothstep(.72,1.08,radius);
            float mottle=.62*noise(world.xz*8.0)+.38*noise(world.xz*27.0+7.3);
            color=mix(vec3(.095,.075,.058),mix(vec3(.27,.255,.23),vec3(.48,.455,.40),mottle),ash);
            color*=1.0-.40*uDouse;   // rain-soaked ash goes dark
        }else if(tag==5){
            color=mix(vec3(.30,.022,.002),vec3(1,.25,.008),noise(world.xz*31.0))*campfirePulse(t)*(1.0-.45*uDouse);
        }else{
            float grain=noise(world.xz*24.0+world.y*13.0);
            float fissure=pow(.5+.5*sin(world.x*81.0+world.z*69.0+world.y*37.0),18.0);
            color=mix(vec3(.012,.010,.009),vec3(.075,.050,.036),grain);
            color+=vec3(.40,.052,.002)*fissure*campfirePulse(t);
        }
        // The ring stones, the ash and the stacked wood take the flames' light on the faces
        // turned to the fire (the facet's own normal; they are flat-faceted): lit all round,
        // the stones glowed like coals.
        float facing=pigment.g;
        if(tag==1||tag>5)facing=1.0;   // the burning logs sit in the flames
        if(tag!=5)color=applyCampfireLightFacing(color*pigment.rrr,world,facing);
        fragColor=vec4(color,1);return;
    }
    vec2 p=vec2((uv.x-.5)*2.0,uv.y);
    float alpha;vec3 color;
    if(pigment.a<.35){
        float turbulence=fbm(vec2(p.x*3.7,p.y*5.2-t*1.9)+pigment.r*9.0);
        float bend=sin(p.y*7.0-t*2.3+pigment.r*11.0)*.10;
        float height=.88+.10*sin(t*2.7+pigment.r*23.0);
        float q=p.y/height;
        float width=(1.0-q)*(.72+.45*turbulence);
        float fine=noise(p*vec2(16.0,11.0)-vec2(t*.4,t*3.0)+pigment.r*17.0);
        float offset=p.x+bend+(turbulence-.5)*.28;
        float edge=width-abs(offset)+(fine-.5)*.09;
        alpha=smoothstep(-.03,.14,edge)*smoothstep(0.0,.07,p.y)*(1.0-smoothstep(.82,1.0,q))
            *smoothstep(.20,.66,turbulence)*.66;
        // Narrow moving gaps stop the six planes merging into one opaque orange cut-out.
        float breakup=noise(vec2(p.x*24.0-t*1.1,p.y*13.0-t*3.7)+pigment.r*31.0);
        float strand=.5+.5*sin(p.x*18.0+p.y*9.0-t*4.2+pigment.r*41.0);
        alpha*=mix(.58,1.0,smoothstep(.24,.80,breakup)*smoothstep(.08,.72,strand));
        float heat=clamp((1.0-p.y)*.95+(turbulence-.5)*.20,0.0,1.0);
        color=mix(vec3(.88,.055,.003),vec3(1,.65,.08),smoothstep(.15,.75,heat));
        float core=smoothstep(.02,.24,edge)*(1.0-smoothstep(.28,.88,abs(offset)/max(width,.04)));
        color=mix(color,vec3(1,.95,.58),smoothstep(.66,1.0,heat)*core);
        float blueBase=(1.0-smoothstep(.025,.17,p.y))*core;
        color=mix(color,vec3(.30,.48,1.0),blueBase*.46);
        // beaten down by the rain the flames thin and sputter
        alpha*=1.0-uDouse*(.30+.25*noise(vec2(t*6.0,pigment.r*13.0)));
    }else if(pigment.a<.6&&pigment.b<.99){
        // The upper plume: strips twice their spacing tall, each weighed by a tent over its
        // height, so the two over any point sum to one. Width, cloud and thinning follow the
        // height alone and the column reads as one widening, paling haze, not a string of
        // separate puffs.
        float h=plumeHeight;
        float x=p.x*pigment.b*255.0/40.0;   // metres across
        float radius=.45+.17*h;
        float sheet=pigment.r*2.0;
        float cloud=fbm(vec2(x*.85+sheet*5.0,h*.62-t*.43));
        float body=1.0-smoothstep(.25,1.0,abs(x+(cloud-.5)*.5*radius)/radius);
        float tent=1.0-abs(2.0*uv.y-1.0);
        float thin=smoothstep(1.0,13.0,h);
        alpha=body*tent*smoothstep(.18,.70,cloud)*mix(.30,.13,thin)*(1.0-smoothstep(10.5,15.5,h));
        alpha*=.74+.26*noise(vec2(x*1.9+t*.13,h*2.2-t*.31)+sheet*17.0);
        // thin wood smoke in daylight: a pale warm grey low down, a bluish haze aloft
        color=mix(vec3(.60,.59,.56),vec3(.78,.80,.83),thin)*(.86+.14*cloud);
        color=mix(color,vec3(.62,.63,.62)*(.85+.15*cloud),uDouse*.75*(1.0-thin*.6));
        alpha*=1.0+uDouse*(.55*(1.0-thin)-.55*thin);
        vec3 nightGrade=1.0/max(uCampfireNight.xyz,vec3(1.0));
        color*=mix(nightGrade,vec3(1.0,.62,.32),(1.0-smoothstep(.0,.35,thin))*uCampfireNight.w*.4);
        // a strip passing right by the eye fades rather than sweeping a grey sheet over it
        alpha*=smoothstep(1.0,7.0,length(world-uCamera));
    }else if(pigment.a<.6){
        float cloud=fbm(vec2(p.x*2.6,p.y*5.0-t*.43)+pigment.r*8.0);
        float altitude=1.0-pigment.g;
        alpha=(1.0-smoothstep(.12,1.0,length(vec2(p.x,p.y*2.0-1.0))))
            *smoothstep(.08,.54,cloud)*.31*mix(1.0,.78,altitude);
        // Keep a coherent, readable body at landmark distance while the second
        // noise field tears soft holes through it.  The two sheets remain far
        // below opaque even where they overlap.
        alpha*=.74+.26*noise(vec2(p.x*8.0+t*.13,p.y*11.0-t*.31)+pigment.r*17.0);
        color=mix(mix(vec3(.34,.32,.29),vec3(.50,.48,.44),cloud),
            vec3(.25,.27,.27),altitude*.52);
        // In a downpour the fire steams: a whiter, denser cloud low down, and the rain
        // knocks the high plume thin.
        color=mix(color,vec3(.58,.59,.58)*(.85+.15*cloud),uDouse*.75*(1.0-altitude*.6));
        alpha*=1.0+uDouse*(.55*(1.0-altitude)-.55*altitude);
        // Drawn after the night grade: the smoke takes the hour's darkness itself, glowing
        // a little orange low down where the flames light it at night.
        vec3 nightGrade=1.0/max(uCampfireNight.xyz,vec3(1.0));
        color*=mix(nightGrade,vec3(1.0,.62,.32),(1.0-smoothstep(.0,.35,altitude))*uCampfireNight.w*.4);
    }else{alpha=(1.0-smoothstep(.05,.8,length(uv*2.0-1.0)))*life*.85;color=vec3(1,.45,.04);}
    fragColor=vec4(color*alpha,alpha);
}
)";
}
CampfireRenderer::CampfireRenderer(GraphicsDevice& device,const CampfireLayout& layout):device_(device),centre_(layout.Centre()) {
    using CNA::Graphics::ShaderCodeEXT;
    const auto source=[](bool es,const char* body){return std::string(es?"#version 300 es\nprecision highp float;\nprecision highp int;\n":"#version 330 core\n")+CampfireLighting::Shader+body;};
    const CNA::Graphics::ShaderPackageEXT package({
        ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Vertex,"main","fire.es.vert",source(true,vertex)),
        ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs,CNA::ShaderStageEXT::Fragment,"main","fire.es.frag",source(true,fragment)),
        ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Vertex,"main","fire.vert",source(false,vertex)),
        ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop,CNA::ShaderStageEXT::Fragment,"main","fire.frag",source(false,fragment))},
        {CNA::ShaderStageEXT::Vertex,CNA::ShaderStageEXT::Fragment},{});
    effect_=std::make_unique<ShaderEffect>(device,package);
    if(!effect_->IsEffectValid())throw std::runtime_error("Campfire shader failed: "+effect_->GetCompileErrorEXT());
    std::vector<VertexPositionColorTexture> vertices;
    // The fire stands still, so how squarely a surface faces it is baked with the sun's light:
    // red and blue carry the sun, green the facing (from the layout's own normals, smooth on the
    // rounded stones; per facet in the shader it cut every stone into gem facets).
    const Vector3 fire(layout.Centre().x,layout.Centre().y+.45f,layout.Centre().z);
    for(const auto v:layout.Vertices()) {
        const float light=.43f+.57f*std::max(0.0f,v.normal.x*environment::SunDirectionX
            +v.normal.y*environment::SunDirectionY+v.normal.z*environment::SunDirectionZ);
        const int value=static_cast<int>(light*255);
        Vector3 toFire=fire-Vector3(v.position.x,v.position.y,v.position.z);
        toFire.Normalize();
        const float facing=.10f+.90f*std::abs(v.normal.x*toFire.X+v.normal.y*toFire.Y+v.normal.z*toFire.Z);
        vertices.emplace_back(Vector3(v.position.x,v.position.y,v.position.z),
            Color(value,static_cast<int>(facing*255),value,static_cast<int>(v.material)*36),Vector2(v.u,v.v));
    }
    solidCount_=static_cast<int>(vertices.size());
    solid_=std::make_unique<VertexBuffer>(device,VertexPositionColorTexture::getVertexDeclarationStatic(),solidCount_,BufferUsage::WriteOnly);
    solid_->SetData(vertices.data(),solidCount_);vertices.clear();
    const auto quad=[&](float x,float y,float z,float width,float height,int type,int phase,
                        int density=255,int spread=255) {
        const std::array uv{Vector2(0,0),Vector2(1,0),Vector2(1,1),Vector2(0,0),Vector2(1,1),Vector2(0,1)};
        for(const auto p:uv)vertices.emplace_back(Vector3(x+(p.X-.5f)*width,y+p.Y*height,z),Color(phase,density,spread,type),p);
    };
    // Farther planes first; horizontal billboard depth stays ordered for every viewing yaw.
    // Twelve broad, low-alpha quads extend the same smoke material into a 15m column: two
    // sheets of six strips, each strip twice the spacing tall so that neighbours overlap by
    // half (their tents sum to one) and wide enough for the plume at its top; the half-width
    // rides in the blue channel. One moving buffer/draw, 24 triangles.
    constexpr float PlumeBase=1.0f,PlumeSpacing=2.3f;
    for(int band=5;band>=0;--band)for(int sheet=0;sheet<2;++sheet){
        const float base=PlumeBase+static_cast<float>(band)*PlumeSpacing;
        const float halfWidth=1.3f*(.45f+.17f*(base+2.0f*PlumeSpacing));
        quad(sheet==0?-.18f:.21f,base,sheet==0?.18f:-.16f,2.0f*halfWidth,2.0f*PlumeSpacing,128,
            sheet*113,255,static_cast<int>(std::lround(halfWidth*40.0f)));
    }
    for(int i=0;i<3;++i)quad(-.12f*i,.40f+i*.37f,.45f-i*.03f,1.3f+i*.2f,1.6f,128,i*73);
    for(int i=0;i<6;++i)quad((i%3-1)*.20f,.15f,.30f-i*.12f,.68f,1.0f+.13f*(i%3),64,i*43);
    for(int i=0;i<12;++i)quad((i%4-1.5f)*.16f,.30f,-.45f,.028f,.065f,192,i*21);
    effectCount_=static_cast<int>(vertices.size());
    moving_=std::make_unique<VertexBuffer>(device,VertexPositionColorTexture::getVertexDeclarationStatic(),effectCount_,BufferUsage::WriteOnly);
    moving_->SetData(vertices.data(),effectCount_);
    bounds_=BoundingBox(Vector3(centre_.x-14,centre_.y-.4f,centre_.z-6.5f),
                        Vector3(centre_.x+6,centre_.y+22.0f,centre_.z+6.5f));
    std::cout<<"island-simulator: campfire "<<solidCount_/3<<" solid triangles, "<<effectCount_/3
        <<" flame/smoke/spark triangles in two culled draws; 11 stored logs six metres away\n";
    std::cout<<"island-simulator: campfire flame material uses broken translucent tongues, pale cores, a restrained blue base, and layered smoke\n";
    std::cout<<"island-simulator: campfire smoke uses 12 bounded upper quads in the existing moving draw, rising 15m and drifting west with the trade wind\n";
    std::cout<<"island-simulator: a passing gust front lays the campfire plume over further downwind and tears it sideways\n";
    std::cout<<"island-simulator: campfire bed uses a pale mottled ash centre, scorched rim, and mixed glowing charcoal\n";
    std::cout<<"island-simulator: campfire and stored firewood stand in the qualified east inland clearing\n";
}
CampfireRenderer::~CampfireRenderer()=default;
void CampfireRenderer::Draw(const Matrix& view,const Matrix& projection,const Vector3& camera,float seconds,bool effects) {
    if(!effects)draws_=0;
    if(!BoundingFrustum(view*projection).Intersects(bounds_))return;
    static bool reported=false;if(!reported){std::cout<<"island-simulator: campfire first visible animated draw\n";reported=true;}
    float matrix[16];(view*projection).ToColumnMajor(matrix);
    effect_->Apply();effect_->SetUniformMat4("uViewProjection",matrix);CampfireLighting::Bind(*effect_,seconds);
    Vector3 toward(camera.X-centre_.x,0,camera.Z-centre_.z);
    if(toward.LengthSquared()<.001f)toward=Vector3(0,0,1);else toward.Normalize();
    effect_->SetUniformVec3("uRight",toward.Z,0,-toward.X);
    effect_->SetUniformVec3("uForward",-toward.X,0,-toward.Z);
    effect_->SetUniformFloat("uMoving",effects?1.0f:0.0f);
    effect_->SetUniformVec3("uCamera",camera.X,camera.Y,camera.Z);
    effect_->SetUniformFloat("uGust",std::isfinite(seconds)?WindGust::Strength(centre_.x,centre_.z,seconds):0.0f);
    effect_->SetUniformFloat("uTend",tend_);
    effect_->SetUniformFloat("uDouse",weather_now::FireDousing());
    device_.setBlendStateProperty(effects?BlendState::AlphaBlend:BlendState::Opaque);
    device_.setDepthStencilStateProperty(effects?DepthStencilState::DepthRead:DepthStencilState::Default);
    device_.setRasterizerStateProperty(RasterizerState::CullNone);device_.SetVertexBuffer(effects?moving_.get():solid_.get());
    device_.DrawPrimitives(PrimitiveType::TriangleList,0,(effects?effectCount_:solidCount_)/3);++draws_;
}
}
