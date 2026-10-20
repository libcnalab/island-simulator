#pragma once
#include <cmath>

namespace island::CloudShadow {
// The fair-weather cumulus shadow field, one for the whole island: a slow world-anchored
// patch field drifting with the trade wind, soft 30 m edges, about a quarter coverage. The
// sea darkens by it and every sun-shadow receiver on land dims by it, so a shadow crossing
// the beach crosses the water beside it without a seam.
inline constexpr float DriftX=1.6f,DriftZ=0.35f;
inline constexpr float LandStrength=0.38f;   // how much a cloud dims the sun on land
inline constexpr const char* Shader=R"(
float cloudShadowHash(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}
float cloudShadowNoise(vec2 grid)
{
    vec2 cell = floor(grid);
    vec2 t = fract(grid);
    t = t * t * (3.0 - 2.0 * t);
    float lower = mix(cloudShadowHash(cell), cloudShadowHash(cell + vec2(1.0, 0.0)), t.x);
    float upper = mix(cloudShadowHash(cell + vec2(0.0, 1.0)),
                      cloudShadowHash(cell + vec2(1.0, 1.0)), t.x);
    return mix(lower, upper, t.y);
}
float cloudCover(vec2 position, float time)
{
    vec2 drift = position + vec2(1.6, 0.35) * time;
    float field = cloudShadowNoise(drift * 0.0062)
        + (cloudShadowNoise(drift * 0.0151 + 7.3) - 0.5) * 0.38;
    return smoothstep(0.56, 0.70, field);
}
)";
// The same field on the CPU, for tests and anything that wants to know where the shade is.
[[nodiscard]] inline float Fract(const float v) noexcept {return v-std::floor(v);}
[[nodiscard]] inline float Hash(const float x,const float y) noexcept {
    float px=Fract(x*0.1031f),py=Fract(y*0.1031f),pz=Fract(x*0.1031f);
    const float d=px*(py+33.33f)+py*(pz+33.33f)+pz*(px+33.33f);
    px+=d;py+=d;pz+=d;
    return Fract((px+py)*pz);
}
[[nodiscard]] inline float Noise(const float gx,const float gy) noexcept {
    const float cx=std::floor(gx),cy=std::floor(gy);
    float tx=gx-cx,ty=gy-cy;
    tx=tx*tx*(3.0f-2.0f*tx);ty=ty*ty*(3.0f-2.0f*ty);
    const float lower=std::lerp(Hash(cx,cy),Hash(cx+1.0f,cy),tx);
    const float upper=std::lerp(Hash(cx,cy+1.0f),Hash(cx+1.0f,cy+1.0f),tx);
    return std::lerp(lower,upper,ty);
}
[[nodiscard]] inline float Cover(const float x,const float z,const float time) noexcept {
    const float dx=x+DriftX*time,dz=z+DriftZ*time;
    const float field=Noise(dx*0.0062f,dz*0.0062f)+(Noise(dx*0.0151f+7.3f,dz*0.0151f+7.3f)-0.5f)*0.38f;
    const float t=std::fmin(1.0f,std::fmax(0.0f,(field-0.56f)/0.14f));
    return t*t*(3.0f-2.0f*t);
}
}
