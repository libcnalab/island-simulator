#pragma once
#include <algorithm>
#include <cmath>

namespace island::CanopyShafts {
// How strongly the sun shafts show from a viewpoint: they belong under the canopy (the
// light field's transmission at the camera well below open sky) and need the sun, so a
// cloud over the camera all but kills them. 0 in the open, 1 deep under an unclouded crown.
inline constexpr float OpenTransmission=0.80f,DeepTransmission=0.35f,CloudKill=0.85f;
[[nodiscard]] inline float Strength(const float transmission,const float cloudCover) noexcept {
    if(!std::isfinite(transmission)||!std::isfinite(cloudCover)) return 0.0f;
    const float under=std::clamp((OpenTransmission-transmission)/(OpenTransmission-DeepTransmission),0.0f,1.0f);
    return under*(1.0f-CloudKill*std::clamp(cloudCover,0.0f,1.0f));
}
}
