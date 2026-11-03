#pragma once

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island::deer_gait
{
    inline constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    // A walking hoof spends most of the cycle supporting weight. Half-span .26 m
    // gives a .81 m adult stride; retreat lengthens it instead of only pedalling faster.
    inline constexpr float StanceFraction=.64f;
    struct Step { float along=0, lift=0; bool stance=true; };

    [[nodiscard]] inline float Phase(float value) noexcept
    {
        value=std::isfinite(value)?std::fmod(value,Tau):0.0f;
        if(value<0) value+=Tau;
        return value<Tau?value:0.0f;
    }
    [[nodiscard]] inline float Movement(float speed) noexcept
    {
        return std::isfinite(speed)?std::clamp(speed/.5f,0.0f,1.0f):0.0f;
    }
    [[nodiscard]] inline float HalfSpan(float speed) noexcept
    {
        return Movement(speed)*(.26f+.15f*std::clamp((speed-.8f)/2.3f,0.0f,1.0f));
    }
    [[nodiscard]] inline Step Sample(float phase,float speed,int leg) noexcept
    {
        if(Movement(speed)==0) return {};
        // Four-beat walk settles into diagonal pairs at retreat speed.
        const float t=std::clamp((speed-1.2f)/.9f,0.0f,1.0f);
        const float trot=t*t*(3.0f-2.0f*t);
        const float offset=leg==0?0.0f:leg==1?.5f:leg==2?.75f-.25f*trot:.25f*(1-trot);
        const float cycle=Phase(phase+offset*Tau)/Tau;
        const float span=HalfSpan(speed);
        if(cycle<StanceFraction)
            return {span*(1.0f-2.0f*cycle/StanceFraction),0,true};
        const float swing=(cycle-StanceFraction)/(1.0f-StanceFraction);
        return {-span*std::cos(std::numbers::pi_v<float>*swing),
                Movement(speed)*(.095f+.035f*trot)*std::sin(std::numbers::pi_v<float>*swing),false};
    }
    // Accepted distance along the support plane, including slope and juvenile size.
    // This schedules the existing stride. Live rendered-terrain contacts are held separately
    // by DeerHerd's GroundFootPlant, so a changing speed/heading cannot drag a supporting hoof.
    [[nodiscard]] inline float Advance(float phase,float distance,float speed,float scale) noexcept
    {
        phase=Phase(phase);
        if(!std::isfinite(distance)||distance<=0||!std::isfinite(speed)||speed<=0
            ||!std::isfinite(scale)||scale<=0) return phase;
        const float cycleDistance=2.0f*HalfSpan(speed)*scale/StanceFraction;
        const float advance=distance/cycleDistance*Tau;
        return std::isfinite(advance)?Phase(phase+std::fmod(advance,Tau)):phase;
    }
}
