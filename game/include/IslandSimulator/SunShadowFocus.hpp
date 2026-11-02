#pragma once

#include "IslandSimulator/SunShadowFade.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include <algorithm>
#include <cmath>

namespace island
{
    // Reuse the existing local shadow map over the visible ground when flying. Below
    // the crowns its receiver column stays at the camera, preserving walking coverage.
    // The bounded look-ahead uses the map's existing target plane, not a new terrain raycast.
    [[nodiscard]] inline Microsoft::Xna::Framework::Vector3 SunShadowViewFocus(
        const Microsoft::Xna::Framework::Vector3& camera,
        const Microsoft::Xna::Framework::Vector3& forward, float groundClearance) noexcept
    {
        const float length = std::hypot(forward.X, forward.Y, forward.Z);
        if (!std::isfinite(groundClearance) || groundClearance <= 12.0f
            || !std::isfinite(length) || length < 1.0e-6f) return camera;
        const float down = -forward.Y / length;
        const float horizontal = std::hypot(forward.X, forward.Z);
        if (down <= 0.0f || horizontal < 1.0e-6f) return camera;
        const float heightWeight = sun_shadow_fade::SmoothStep(12.0f, 80.0f, groundClearance);
        const float lookWeight = sun_shadow_fade::SmoothStep(0.0f, 0.12f, down);
        const float toPlane = std::max(0.0f, camera.Y - sun_shadow_fade::TargetHeight)
            * (horizontal / length) / std::max(0.05f, down);
        // A shallow view must not chase the horizon or move the map arbitrarily far away.
        const float shift = std::min(280.0f, toPlane) * heightWeight * lookWeight;
        return {camera.X + forward.X / horizontal * shift, camera.Y,
                camera.Z + forward.Z / horizontal * shift};
    }
}
