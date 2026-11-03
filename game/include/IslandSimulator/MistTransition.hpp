#pragma once
#include <algorithm>

namespace island {
// Recycling moves a broad, translucent bank many metres. Hide both ends of that
// move: fade before reaching the boundary, then form the new bank over three seconds.
inline constexpr float MistFormationSeconds=3.0f;
[[nodiscard]] inline float MistTransitionOpacity(float age,float remaining,float fadeDistance) noexcept {
    const auto smooth=[](float v) {
        const float t=std::clamp(v,0.0f,1.0f);
        return t*t*(3.0f-2.0f*t);
    };
    return smooth(age/MistFormationSeconds)*smooth(remaining/fadeDistance);
}
}
