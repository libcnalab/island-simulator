#pragma once
#include <numbers>

namespace island {
class RenderedTerrainSurface;
struct DeerState;
struct DeerHoofPoint { float x,y,z; };
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
