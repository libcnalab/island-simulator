#pragma once

#include "IslandSimulator/CrocodileGroup.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island::crocodile_pose {
inline constexpr float Tau=2*std::numbers::pi_v<float>;
inline constexpr float StanceFraction=.72f;
struct Point {float x,y,z;};
struct Step {float along=0,lift=0;bool stance=true;};
struct Leg {Point shoulder,elbow,wrist,foot;};

[[nodiscard]] inline float Phase(float phase)noexcept {
    if(!std::isfinite(phase))return 0;
    const float value=std::fmod(phase,Tau);return value<0?value+Tau:value;
}
[[nodiscard]] inline float Movement(float speed)noexcept {
    return std::isfinite(speed)?std::clamp(speed/.15f,0.0f,1.0f):0;
}
[[nodiscard]] inline float HalfSpan(float speed)noexcept {
    if(Movement(speed)==0)return 0;
    return Movement(speed)*(.18f+.10f*std::clamp((speed-.34f)/.71f,0.0f,1.0f));
}
[[nodiscard]] inline Step Sample(float phase,float speed,int leg)noexcept {
    if(Movement(speed)==0)return {};
    // Overlapping support in a four-beat low crawl; same-side fore/hind feet
    // no longer paddle together. Feet return above ground during the swing.
    constexpr float offset[4]={0,.5f,.75f,.25f};
    const float cycle=Phase(phase+offset[leg%4]*Tau)/Tau,span=HalfSpan(speed);
    if(cycle<StanceFraction)return {span*(1-2*cycle/StanceFraction),0,true};
    const float swing=(cycle-StanceFraction)/(1-StanceFraction);
    return {-span*std::cos(std::numbers::pi_v<float>*swing),
        .08f*Movement(speed)*std::sin(std::numbers::pi_v<float>*swing),false};
}
[[nodiscard]] inline float Advance(float phase,float distance,float speed,float scale)noexcept {
    phase=Phase(phase);
    if(!std::isfinite(distance)||distance<=0||!std::isfinite(scale)||scale<=0||Movement(speed)==0)return phase;
    const float cycleDistance=2*HalfSpan(speed)*scale/StanceFraction;
    return Phase(phase+std::fmod(distance/cycleDistance*Tau,Tau));
}
[[nodiscard]] inline Leg SampleLeg(const CrocodileState& c,int index)noexcept {
    const float side=index%2?1.0f:-1.0f,root=index<2?.64f:-.66f;
    const float fold=std::clamp(c.legFold,0.0f,1.0f);
    const float land=(1-std::clamp(c.submerged,0.0f,1.0f))*(1-fold);
    const auto step=Sample(c.gaitPhase,c.gaitSpeed,index);
    const float along=step.along*land,lift=step.lift*land;
    const float wristY=std::lerp(.025f,.14f,fold)+land*(c.footSupport[index]+.002f)+lift;
    const Point shoulder{side*.52f,.34f,root};
    const Point elbow{side*std::lerp(.83f,.62f,fold),std::lerp(.18f,.22f,fold)+lift*.45f,
                      root+along*.5f-.28f*fold};
    const Point wrist{side*std::lerp(.78f,.56f,fold),wristY,root+.24f+along-.62f*fold};
    return {shoulder,elbow,wrist,{wrist.x,wrist.y+.018f,wrist.z+.075f}};
}
}
