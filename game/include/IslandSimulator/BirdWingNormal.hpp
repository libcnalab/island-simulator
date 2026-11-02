#pragma once

#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <cmath>

namespace island
{
    // Flight feathers share their supporting arm/hand plane. Keeping the body's
    // up vector while the wing rises made their vanes turn edge-on into needles.
    [[nodiscard]] inline Microsoft::Xna::Framework::Vector3 BirdWingNormal(
        const Microsoft::Xna::Framework::Vector3& span,
        const Microsoft::Xna::Framework::Vector3& forward,
        const Microsoft::Xna::Framework::Vector3& fallbackUp, float side) noexcept
    {
        using Microsoft::Xna::Framework::Vector3;
        const auto normal = Vector3::Cross(span * side, forward);
        const float lengthSquared = normal.LengthSquared();
        return std::isfinite(lengthSquared) && lengthSquared > 1.0e-10f
            ? normal / std::sqrt(lengthSquared) : fallbackUp;
    }
}
