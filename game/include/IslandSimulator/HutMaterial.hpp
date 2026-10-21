#pragma once
namespace island {
inline constexpr const char* HutMaterialVertex=R"(
layout(location=0) in vec3 aPosition;
layout(location=1) in vec4 aColor;
layout(location=2) in vec2 aTextureCoordinate;
uniform mat4 uViewProjection;
uniform float uGust;      // gust front strength at the hut, 0..1
uniform float uSeconds;
uniform vec2 uHangTop;    // world y of the fish bar and of the net rail
out vec3 vWorld;
out vec4 vColor;
out vec2 vGrain;
void main() {
    vec3 world=aPosition;
    float tag=aColor.a*255.0;
    if(tag>36.0&&tag<52.0){
        // The hung things in the yard: the split fish (tag 40) hang 0.37 m from their bar,
        // the net (tag 48) 0.9 m from its rail. The further below the pivot, the more they
        // swing: a light sway in the breeze, laid over downwind (toward -x, with the
        // fronts) and shivering sideways while a gust front passes.
        bool fish=tag<44.0;
        float pivot=fish?uHangTop.x:uHangTop.y;
        float hang=clamp((pivot-aPosition.y)/(fish?0.37:0.9),0.0,1.0);
        float swing=0.5+0.5*sin(uSeconds*(fish?2.6:1.9)+aPosition.z*3.0+aColor.r*7.0);
        world.x-=hang*(0.012+uGust*0.09)*(0.4+0.6*swing);
        world.z+=hang*uGust*0.03*sin(uSeconds*3.1+aPosition.x*2.0+aColor.g*5.0);
    }
    vWorld=world; vColor=aColor; vGrain=aTextureCoordinate;
    gl_Position=uViewProjection*vec4(world,1.0);
    gl_Position.z=2.0*gl_Position.z-gl_Position.w;
}
)";
inline constexpr const char* HutMaterialFragment=R"(
in vec3 vWorld;
in vec4 vColor;
in vec2 vGrain;
uniform vec3 uCameraPosition;
uniform vec3 uHutCentre;
uniform sampler2D uThatchTexture;
uniform vec3 uThatchMean;
uniform float uWetness;   // 0..1 rain-wet (WeatherSystem)
uniform vec3 uToSun;      // the hour's shading direction (at 14:00 the fixed sun the faces were baked for)
out vec4 fragColor;
// Fade unresolved cycles to their mean; no texture, loops or dependent sampling.
float resolvedWave(float phase) {
    return sin(phase)*(1.0-smoothstep(0.7,2.5,fwidth(phase)));
}
void main() {
    float u=vGrain.x,v=vGrain.y;
    // Keep the dominant fibres coarse enough to survive a normal walking view.
    // The former 210/970-radian bands correctly antialiased to their mean and left
    // most boards looking like flat brown cards except at an extreme close-up.
    float warp=sin(v*2.1+sin(u*4.7))*0.92+sin(v*.73)*.48;
    float grain=resolvedWave(u*54.0+warp*1.45);
    float vessel=resolvedWave(u*118.0+warp*2.1+sin(v*5.2)*.55);
    float fine=resolvedWave(u*310.0+warp*3.0);
    float weather=sin(u*10.0+v*.55)*sin(v*2.2+u*1.7);
    float knot=resolvedWave(length(vec2(u*.82,v*.13)+vec2(.17,.09))*43.0
        +sin(v*1.7)*.7);
    float recessed=smoothstep(.72,1.0,abs(grain*.72+vessel*.38));
    float sunBleach=.5+.5*resolvedWave(v*.75+u*2.6);
    vec3 timber=vColor.rgb*(.94+.13*grain+.055*vessel+.022*fine
        -.12*recessed-.075*max(weather,0.0)+.045*sunBleach);
    timber+=vec3(.038,.024,.011)*max(knot,0.0)*(1.0-recessed);
    float grey=dot(timber,vec3(0.3,0.59,0.11));
    timber=mix(timber,vec3(grey)*vec3(1.04,1.02,0.96),0.18+0.08*weather);
    float rings=resolvedWave(length(vGrain)*46.0+sin(u*13.0)*.55);
    if(vColor.a>0.4&&vColor.a<0.65)
        timber=vColor.rgb*(.91+.16*rings-.06*smoothstep(.76,1.0,abs(rings)));
    float fibres=resolvedWave(u*460.0+sin(v*8.0)*0.28);
    float fineFibres=resolvedWave(u*1410.0+sin(v*13.0)*0.4);
    vec3 leafSample=texture(uThatchTexture,vGrain).rgb;
    vec3 leafVariation=clamp(leafSample/uThatchMean,vec3(0.5),vec3(1.55));
    vec3 thatch=vColor.rgb*mix(vec3(1.0),leafVariation,0.85)
        *(0.98+0.04*fibres+0.015*fineFibres);
    float glassClass=step(0.65,vColor.a)*(1.0-step(0.7,vColor.a));
    float reflectedSky=smoothstep(0.12,0.78,v)
        *(0.82+0.18*resolvedWave(u*7.0+v*2.0));
    float softGlint=0.5+0.5*resolvedWave((u-v)*18.0+vWorld.y*1.7);
    vec3 glass=mix(vColor.rgb*vec3(0.56,0.64,0.61),
        vec3(0.30,0.47,0.51),0.40*reflectedSky);
    glass+=vec3(0.055,0.075,0.072)*softGlint*reflectedSky;
    vec3 color=vColor.a>0.7?thatch:timber;
    color=mix(color,glass,glassClass);
    float detail=1.0-smoothstep(35.0,80.0,length(vWorld-uCameraPosition));
    color=mix(vColor.rgb,color,detail);
    float base=(1.0-smoothstep(-0.3,0.25,vWorld.y-uHutCentre.y))*(1.0-step(0.7,vColor.a));
    color*=1.0-base*0.12;
    // Rain soaks the hut: the thatch goes dark, the weathered boards less so (the walls stand
    // under the eaves), the glass not at all.
    color*=1.0-clamp(uWetness,0.0,1.0)*(1.0-glassClass)*(vColor.a>0.7?0.38:0.17);
    // Each face's light was baked for the fixed afternoon sun: the face (its normal from the
    // world position's derivatives, turned to the eye) is relit by the hour's sun, exactly as
    // baked at 14:00.
    vec3 faceNormal=normalize(cross(dFdx(vWorld),dFdy(vWorld)));
    if(dot(faceNormal,uCameraPosition-vWorld)<0.0) faceNormal=-faceNormal;
    color*=(0.48+0.52*max(0.0,dot(faceNormal,uToSun)))
        /(0.48+0.52*max(0.0,dot(faceNormal,vec3(-0.45,0.82,-0.34))));
    // The sun shadow map and the cumulus shade, with the same cool shadow tint as the palms.
    color*=mix(vec3(0.56,0.60,0.66),vec3(1.0),sunShadowVisibility(vWorld));
    color=mix(color,islandAtmosphereFog,smoothstep(180.0,300.0,length(vWorld-uCameraPosition)));
    fragColor=vec4(color,1.0);
}
)";
}
