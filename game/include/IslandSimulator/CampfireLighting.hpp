#pragma once
#include "IslandSimulator/CampfireLayout.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/WeatherSystem.hpp"
#include <algorithm>
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include <cmath>
namespace island {
struct CampfireLighting final {
    static constexpr const char* Shader=R"(
uniform vec4 uCampfire;
// xyz: one over the scene's night grade, so the fire's light survives the grade that darkens
// everything drawn before it; w: 0 by day, 1 in the full night.
uniform vec4 uCampfireNight;
// how much of its light the fire gives: less as the rain beats it down
uniform float uCampfireStrength;
float campfirePulse(float t){return .83+.10*sin(t*8.7)+.07*sin(t*17.1+1.3);}
// facing: how squarely the surface faces the flames (1 for ground and plants, whose normal the
// caller does not know).
vec3 applyCampfireLightFacing(vec3 color,vec3 world,float facing){
    vec3 offset=world-uCampfire.xyz;float squared=dot(offset,offset);
    if(squared>=36.0)return color;
    // Nearly inverse-square from a fire about a metre across: bright beside the flames and
    // fading through the first few metres; the old flat profile lit one uniform saturated
    // pool out to three metres. The window takes it to nothing at six.
    float window=max(0.0,1.0-squared/36.0);
    float f=1.15/(1.0+squared/5.0)*window*window*campfirePulse(uCampfire.w)*uCampfireStrength*facing;
    // By day a faint warm addition; at night the fire lights the surfaces' own colours warm,
    // keeping their texture instead of washing them into one flat glow.
    float night=uCampfireNight.w;
    vec3 light=vec3(.16,.072,.018)*f*(1.0-.75*night)+color*vec3(1.0,.55,.22)*f*1.1*night;
    return min(uCampfireNight.xyz,color+light*uCampfireNight.xyz);
}
vec3 applyCampfireLight(vec3 color,vec3 world){return applyCampfireLightFacing(color,world,1.0);}
)";
    static void Bind(Microsoft::Xna::Framework::Graphics::ShaderEffect& effect,float seconds) {
        effect.SetUniformVec4("uCampfire",CampfireSite::X,CampfireLayout::GroundY()+.45f,CampfireSite::Z,
            std::isfinite(seconds)?std::remainder(seconds,628.31853f):0);
        const auto& light=sun_light::Current().lighting;
        const auto inverse=[](float grade){return 1.0f/std::clamp(grade,0.08f,1.0f);};
        effect.SetUniformVec4("uCampfireNight",inverse(light.grade.x),inverse(light.grade.y),inverse(light.grade.z),
            1.0f-light.sunVisibility);
        effect.SetUniformFloat("uCampfireStrength",Strength());
    }
    /** @brief The share of its light the fire gives (1 burning well, 0.4 beaten down by a downpour). */
    [[nodiscard]] static float Strength() noexcept {return 1.0f-0.6f*weather_now::FireDousing();}
};
}
