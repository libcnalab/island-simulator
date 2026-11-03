#pragma once
#include <numbers>

namespace island {
class RenderedTerrainSurface;
struct DeerState;
struct DeerHoofPoint {
    float x,y,z;
    constexpr DeerHoofPoint operator+(DeerHoofPoint p) const noexcept {return {x+p.x,y+p.y,z+p.z};}
    constexpr DeerHoofPoint operator-(DeerHoofPoint p) const noexcept {return {x-p.x,y-p.y,z-p.z};}
    constexpr DeerHoofPoint operator*(float k) const noexcept {return {x*k,y*k,z*k};}
};
struct DeerHoofSupport {
    DeerHoofPoint centre,forward,right,up;
    static constexpr float AnkleHeight=.060f;
    static constexpr float Rear=-.030f,Front=.070f,ToeOffset=.018f;
    static constexpr float RearRadius=.024f,FrontRadius=.015f;
    // Share the small support correction through the existing procedural joints,
    // rather than putting the whole height change into the short pastern.
    static constexpr float KneeWeight=.35f,HockWeight=.8f;
};
struct DeerSupportFrame {
    DeerHoofPoint origin,forward,right,up;
    // Includes the curved antlers, head toss and foot relief around the posed body centre.
    static constexpr float CullRadius=2.1f;
};
struct DeerLegPose {
    DeerHoofPoint hip,knee,hock,hoof,collar;
    DeerHoofSupport support;
};
// Shared actual joint endpoints for the visible legs and their contact shadows.
[[nodiscard]] DeerLegPose MakeDeerLegPose(const DeerState& deer,int leg,
    const RenderedTerrainSurface* ground);
[[nodiscard]] DeerHoofPoint NeutralDeerHoof(const DeerSupportFrame& frame,float scale,int leg) noexcept;
// Follow the actual supporting plane before making small per-leg relief corrections.
[[nodiscard]] DeerSupportFrame MakeDeerSupportFrame(const DeerState& deer,
    const RenderedTerrainSurface* ground);
// Both existing LODs include the same downward radial vertex, avoiding a contact pop.
[[nodiscard]] constexpr float DeerHoofRingPhase(int sides) noexcept {
    return sides==6?std::numbers::pi_v<float>/6.0f:0.0f;
}
// Fit the existing two tapered toes to the actual drawn terrain, including local patches.
// A null surface supplies a flat plane for the existing CPU-only shadow fixtures.
[[nodiscard]] DeerHoofSupport FitDeerHoofSupport(const RenderedTerrainSurface* ground,
    float flatHeight,float x,float z,float heading,float scale);
}
