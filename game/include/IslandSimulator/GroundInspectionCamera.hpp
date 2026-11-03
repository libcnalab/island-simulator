#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace island {
// A fixed detail camera must see the contact point over intervening terrain. Raising
// only its local eye height can leave a downhill inspection hidden behind the ridge.
template<class Height>
[[nodiscard]] float GroundInspectionEyeY(float x,float z,const std::array<float,3>& target,
                                         float eyeHeight,Height height) {
    const float distance=std::hypot(target[0]-x,target[2]-z);
    if(!std::isfinite(distance)||distance<.1f||distance>60||!std::isfinite(target[1])
        ||!std::isfinite(eyeHeight)||eyeHeight<.1f)
        throw std::invalid_argument("Invalid ground inspection camera");
    const float ground=height(x,z);
    float y=ground+eyeHeight;
    const int steps=static_cast<int>(std::ceil(distance/.05f));
    for(int i=1;i<steps;++i) {
        const float t=static_cast<float>(i)/static_cast<float>(steps);
        const float h=height(std::lerp(x,target[0],t),std::lerp(z,target[2],t));
        if(!std::isfinite(h))throw std::invalid_argument("Invalid inspection terrain height");
        y=std::max(y,(h+.12f-target[1]*t)/(1-t));
    }
    if(!std::isfinite(y)||y>ground+12)
        throw std::runtime_error("Inspection target is not visible from a bounded camera");
    return y+.02f;
}
}
