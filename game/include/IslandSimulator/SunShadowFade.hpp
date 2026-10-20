#pragma once

#include <algorithm>
#include <cmath>

// Where the sun shadow map's shadows fade out, kept free of graphics types so the core tests
// can check it; SunShadowMap and its receiver shader use the same numbers.
//
// The map is an orthographic square in the sun's view, re-centred on the camera's 8 m cell.
// Fading on that square's own edge moved the visible end of the shadows in 8 m jumps at every
// re-centre, so in flight it read as a band sweeping over the ground. The shadows instead fade
// on a rounded square in the sun's view around the viewer itself, which moves with the camera;
// the map's edge keeps only a thin safety band that the rounded square always clears. The
// light looks at one fixed height, so the viewer's point in the sun's view is continuous too.
namespace island::sun_shadow_fade
{
    inline constexpr float MapRadius = 72.0f;     // half-extent of the light-space square, metres
    inline constexpr float AnchorStep = 8.0f;     // the map re-centres when the camera leaves a cell
    inline constexpr float TargetHeight = 12.0f;  // the height the light looks at, metres
    inline constexpr float EdgeBand = 0.02f;      // safety fade at the map's edge, in map widths
    inline constexpr float FarFadeStart = 52.0f;  // light-space metres from the viewer
    inline constexpr float FarFadeEnd = 63.0f;
    // The far fade is complete before the edge band begins wherever the viewer sits in its
    // cell (at most half a cell diagonal, 5.66 m, from the map's centre in the sun's view; the
    // rounded square's radius is never below the larger of its two offsets).
    static_assert(FarFadeEnd + AnchorStep * 0.70711f <= MapRadius - EdgeBand * 2.0f * MapRadius,
                  "the far fade must finish inside the map's full-strength square");

    [[nodiscard]] inline float SmoothStep(const float edge0, const float edge1, const float x) noexcept
    {
        const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    /** @brief The map's centre on one axis for a camera coordinate (its 8 m cell). */
    [[nodiscard]] inline float AnchorOf(const float coordinate) noexcept
    {
        return std::round(coordinate / AnchorStep) * AnchorStep;
    }

    struct LightOffset
    {
        float across = 0;  // horizontal, square to the sun's azimuth
        float up = 0;      // the light view's vertical
    };

    /** @brief Light-space metres of the world point (x, height, z) from the point
     *  (columnX, TargetHeight, columnZ), for the direction (sunX, sunY, sunZ) towards the sun. */
    [[nodiscard]] inline LightOffset Offset(const float x, const float height, const float z,
                                            const float columnX, const float columnZ,
                                            const float sunX, const float sunY, const float sunZ) noexcept
    {
        const float length = std::sqrt(sunX * sunX + sunY * sunY + sunZ * sunZ);
        const float horizontal = std::hypot(sunX, sunZ) / length;   // cos of the sun's elevation
        const float sine = sunY / length;
        const float azimuthX = sunX / (horizontal * length), azimuthZ = sunZ / (horizontal * length);
        const float dx = x - columnX, dz = z - columnZ, dh = height - TargetHeight;
        return {dx * azimuthZ - dz * azimuthX, -(dx * azimuthX + dz * azimuthZ) * sine + dh * horizontal};
    }

    /** @brief The rounded-square radius (the 4-norm) of a light-space offset: at least the
     *  larger offset, at most 1.19 times it, so its fade has soft corners. */
    [[nodiscard]] inline float RoundedRadius(const float across, const float up) noexcept
    {
        const float a2 = across * across, u2 = up * up;
        return std::sqrt(std::sqrt(a2 * a2 + u2 * u2));
    }

    /** @brief 0..1, the share of the map's shadow a receiver takes: the thin safety band at the
     *  edge of the square around the camera's cell, times the far fade around the viewer. This
     *  mirrors the receiver shader's sunShadowVisibility. */
    [[nodiscard]] inline float Strength(const float x, const float height, const float z,
                                        const float viewerX, const float viewerZ,
                                        const float sunX, const float sunY, const float sunZ) noexcept
    {
        const LightOffset fromAnchor = Offset(x, height, z, AnchorOf(viewerX), AnchorOf(viewerZ), sunX, sunY, sunZ);
        const float edge = (MapRadius - std::max(std::abs(fromAnchor.across), std::abs(fromAnchor.up)))
                           / (2.0f * MapRadius);
        if (edge <= 0.0f) return 0.0f;
        const LightOffset fromViewer = Offset(x, height, z, viewerX, viewerZ, sunX, sunY, sunZ);
        const float far = SmoothStep(FarFadeStart, FarFadeEnd, RoundedRadius(fromViewer.across, fromViewer.up));
        return SmoothStep(0.0f, EdgeBand, edge) * (1.0f - far);
    }
}
