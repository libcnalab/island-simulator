#pragma once
#include <algorithm>
#include <cmath>
namespace island {
// Authored in the east inland clearing, away from the hut, cave and coast.
struct CampfireSite final {
    static constexpr float X=43.0f,Z=-18.0f;
    static constexpr float WoodX=49.0f,WoodZ=-18.0f;
    static constexpr float PitRadius=1.25f,WoodRadius=1.40f;
    static constexpr float VegetationClearance=5.5f,WoodClearance=2.0f;
    // The resident pausing this close to the pit is tending the fire (the route's campfire
    // call walks up to the pit's edge, or stands about eight metres off where it cannot).
    static constexpr float TendDistance=9.0f;
    [[nodiscard]] static bool Tending(const float x,const float z,const bool pausing) noexcept {
        return pausing&&std::isfinite(x)&&std::isfinite(z)&&std::hypot(x-X,z-Z)<TendDistance;
    }
    [[nodiscard]] static bool Reserves(float x,float z,float margin=0) noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(margin))return false;
        margin=std::max(0.0f,margin);
        return std::hypot(x-X,z-Z)<VegetationClearance+margin
            ||std::hypot(x-WoodX,z-WoodZ)<WoodClearance+margin;
    }
    [[nodiscard]] static float SoundPresence(float x,float y,float z,float fireY) noexcept {
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(fireY))return 0;
        const float t=std::clamp((std::hypot(x-X,y-fireY,z-Z)-3.0f)/21.0f,0.0f,1.0f);
        return 1-t*t*(3-2*t);
    }
};
}
