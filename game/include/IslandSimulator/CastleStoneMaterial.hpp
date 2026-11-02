#pragma once

namespace island::castle_stone
{
    // Reuses the structure receiver's existing vertex colours, lighting, shadow map and fog.
    // World-space courses remain horizontal on the old walls; derivatives fade subpixel joints.
    inline constexpr const char* Shader = R"(
float castleHash(vec2 p) {
    vec3 q=fract(vec3(p.xyx)*0.1031);
    q+=dot(q,q.yzx+33.33);
    return fract((q.x+q.y)*q.z);
}
float castleCourse(vec2 p) {
    const vec2 size=vec2(0.85,0.34); // weathered limestone blocks, metres
    vec2 width=fwidth(p);
    float footprint=max(width.x,width.y);
    float detail=1.0-smoothstep(0.04,0.13,footprint);
    float row=floor(p.y/size.y);
    vec2 cell=vec2(p.x/size.x+0.5*mod(row,2.0),p.y/size.y);
    vec2 edge=min(fract(cell),1.0-fract(cell))*size;
    float aa=max(0.0005,0.6*footprint);
    float mortar=1.0-smoothstep(0.006-aa,0.018+aa,min(edge.x,edge.y));
    float block=0.95+0.09*castleHash(floor(cell));
    float grain=(castleHash(floor(p*42.0))-0.5)*0.045
        *(1.0-smoothstep(0.007,0.03,footprint));
    return mix(1.0,block*mix(1.0,0.70,mortar),detail)+grain;
}
vec3 castleStone(vec3 p,vec3 normal) {
    // Both projections/derivatives run unconditionally, including top and timber fragments.
    // Blending avoids a hard projection seam on the original round towers.
    float x=castleCourse(p.zy),z=castleCourse(p.xy);
    vec2 weight=normal.xz*normal.xz;weight*=weight;
    float wall=(x*weight.x+z*weight.y)/max(0.0001,weight.x+weight.y);
    float top=0.96+0.06*castleHash(floor(p.xz*4.0));
    float surface=mix(wall,top,smoothstep(0.5,0.85,abs(normal.y)));
    return vec3(surface);
}
)";
}
