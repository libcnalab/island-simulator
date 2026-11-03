#pragma once
#include <algorithm>
#include <cmath>

namespace island {
// Half-height along a camera-facing billboard's up vector. Its lower edge must
// stay above the opaque lake, which otherwise depth-clips a hard band into the glow.
inline float CraterGlowHalfHeight(float heightAboveLake,float requested,float upY) noexcept {
    if(!std::isfinite(heightAboveLake)||!std::isfinite(requested)||!std::isfinite(upY))return 0;
    // Five centimetres of clearance avoid surface/depth precision at the lower rim.
    const float vertical=std::clamp(std::abs(upY),.001f,1.0f);
    return std::min(std::max(0.0f,requested),std::max(0.0f,heightAboveLake-.05f)/vertical);
}
}
