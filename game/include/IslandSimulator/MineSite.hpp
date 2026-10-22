#pragma once

#include <algorithm>
#include <cmath>

namespace island {

/** Authored footprint of the old mine: an adit driven west-north-west under the main mountain
 *  from the foot of its eastern slope, above the forest path, ending in a crystal chamber. */
struct MineSite final {
    static constexpr float X=1.0f;
    static constexpr float Z=7.0f;
    static constexpr float Angle=2.79f;          // the adit's heading in: (cos, sin) of it
    static constexpr float AditLength=48.0f;     // from the entrance to the chamber
    static constexpr float ChamberCentre=52.2f,ChamberHalfLength=4.6f,ChamberHalfWidth=3.4f;
    static constexpr float HalfWidth=1.3f;       // between the rock walls; the posts stand inside
    static constexpr float PostInner=1.1f;       // the timber sets' posts' inner faces
    static constexpr float Height=2.5f,ChamberHeight=3.5f;
    static constexpr float Grade=0.01f;          // the floor rises inward, so the water runs out
    // The approach and the cutting, where the hillside is cut away to the portal: no trees,
    // bushes or grass there.
    static constexpr float CutFrom=-2.5f,CutTo=6.0f,CutHalfWidth=3.6f;

    [[nodiscard]] static float Across(const float x,const float z) noexcept {
        return -std::sin(Angle)*(x-X)+std::cos(Angle)*(z-Z);
    }
    [[nodiscard]] static float Depth(const float x,const float z) noexcept {
        return std::cos(Angle)*(x-X)+std::sin(Angle)*(z-Z);
    }
    [[nodiscard]] static bool Reserves(const float x,const float z,float margin=0.0f) noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(margin)) return false;
        margin=std::max(0.0f,margin);
        const float across=Across(x,z),depth=Depth(x,z);
        return std::abs(across)<=CutHalfWidth+margin&&depth>=CutFrom-margin&&depth<=CutTo+margin;
    }
};

}
