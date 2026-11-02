#pragma once

#include <algorithm>

namespace island::rain_streak
{
    // The existing rain's narrow, approximately pixel-wide streak and near-eye blur.
    // Reuse them for heavier canopy/eave drops; their fall speed and length still differ.
    [[nodiscard]] inline float HalfWidth(float distance) noexcept
    {
        return 0.0009f+distance*0.00085f;
    }

    [[nodiscard]] inline float NearOpacity(float distance) noexcept
    {
        return std::clamp((distance-0.5f)/2.0f,0.0f,1.0f);
    }
}
