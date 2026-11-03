#pragma once
#include <algorithm>
#include <cmath>

namespace island {
struct LighthouseGlare {float radius=0,offset=0,intensity=0;};

// Keep the halo around the lens, including when the viewer is inside the lantern.
// A world-sized billboard must never move behind the eye or cover the whole view.
inline LighthouseGlare LighthouseGlareAt(float distance,float flare,float night) noexcept {
    distance=std::isfinite(distance)?std::max(0.0f,distance):0;
    flare=std::isfinite(flare)?std::clamp(flare,0.0f,1.0f):0;
    night=std::isfinite(night)?std::clamp(night,0.0f,1.0f):0;
    // tan(5.7 degrees) limits near-field coverage; daylight adaptation suppresses
    // the halo before the rotating lamp switches off at sunrise.
    const float offset=std::min(.40f,.5f*distance); // clear the existing 36cm opaque lens
    return {std::min(1.0f+1.8f*flare,.10f*(distance-offset)),offset,night*night};
}
}
