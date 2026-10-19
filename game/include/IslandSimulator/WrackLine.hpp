#pragma once
#include <cmath>

namespace island::WrackLine
{
    // Where the last high tide left its strandings: an inland distance along the whole coast
    // that wanders with the coast angle. The terrain material draws the line here and the
    // beach crabs forage along it, so the shader form and the CPU form must agree.
    inline constexpr float BaseInland = 5.4f;
    inline constexpr const char* Shader = R"(
float wrackLineCentre(vec2 p) {
    float angle = atan(p.y, p.x);
    return 5.4 + 2.2 * sin(angle * 7.0 + 0.4) + 0.9 * sin(angle * 19.0 - 1.3);
}
)";
    [[nodiscard]] inline float Centre(const float x, const float z) noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z)) return BaseInland;
        const float angle = std::atan2(z, x);
        return BaseInland + 2.2f * std::sin(angle * 7.0f + 0.4f) + 0.9f * std::sin(angle * 19.0f - 1.3f);
    }
}
