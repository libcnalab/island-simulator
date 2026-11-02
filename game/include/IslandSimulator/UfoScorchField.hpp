#pragma once

#include "IslandSimulator/UfoWreckLayout.hpp"
#include <algorithm>
#include <cmath>

namespace island::ufo_scorch
{
    inline float Smooth(float a, float b, float value) noexcept
    {
        const float t=std::clamp((value-a)/(b-a),0.0f,1.0f);
        return t*t*(3.0f-2.0f*t);
    }
    inline float Centre(float forward) noexcept
    {
        return 1.4f*std::sin(forward*.17f)+.6f*std::sin(forward*.41f+1.0f);
    }
    inline float Width(float forward) noexcept
    {
        return 1.4f+1.1f*(1.0f-(-forward-3.0f)/(UfoWreckPlacement::FurrowLength-3.0f));
    }
    // Continuous world-space fields: adjacent ground triangles share the same edge values.
    // Alpha, rather than whole-cell selection, lets the actual soil show through the boundary.
    inline float Apron(float forward,float right) noexcept
    {
        if(!std::isfinite(forward)||!std::isfinite(right)) return 0;
        const float a=std::atan2(right,forward);
        const float edge=7.0f+.48f*std::sin(3*a+.7f)+.24f*std::sin(7*a-1.1f);
        return .82f*Smooth(0,1.5f,edge-std::hypot(forward,right));
    }
    inline float Furrow(float forward,float right) noexcept
    {
        if(!std::isfinite(forward)||!std::isfinite(right)) return 0;
        const float tail=Smooth(0,3.0f,UfoWreckPlacement::FurrowLength+forward);
        const float head=1.0f-Smooth(-4.0f,-3.0f,forward);
        const float edge=1.15f-std::abs(right-Centre(forward))/std::max(.1f,Width(forward));
        return .76f*tail*head*Smooth(0,.4f,edge);
    }
}
