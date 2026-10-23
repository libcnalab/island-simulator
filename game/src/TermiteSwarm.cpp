#include "IslandSimulator/TermiteSwarm.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Smooth(const float a,const float b,const float v) {
    const float t=std::clamp((v-a)/(b-a),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
}

float TermiteSwarm::Strength(const float sunElevationDegrees,const float wetness) noexcept {
    if(!std::isfinite(sunElevationDegrees)||!std::isfinite(wetness)) return 0.0f;
    const float dusk=Smooth(5.0f,1.0f,sunElevationDegrees)*(1.0f-Smooth(-5.0f,-9.0f,sunElevationDegrees));
    return dusk*Smooth(0.30f,0.70f,wetness);
}

std::array<float,3> TermiteSwarm::Mouth(const TermiteNest& nest) noexcept {
    return {{nest.bark[0]+nest.normal[0]*nest.width*0.42f,nest.bark[1]+nest.height*0.48f,
             nest.bark[2]+nest.normal[2]*nest.width*0.42f}};
}

void TermiteSwarm::Gather(const float seconds,const float strength,const std::span<const TermiteNest> nests,const float eyeX,
                          const float eyeY,const float eyeZ,std::vector<Alate>& alates,std::vector<FruitBatState>& bats,
                          std::vector<GeckoState>& geckos) {
    if(!(strength>0.01f)||!std::isfinite(seconds)) return;
    for(const TermiteNest& nest:nests) {
        const auto mouth=Mouth(nest);
        if(std::hypot(mouth[0]-eyeX,mouth[1]-eyeY,mouth[2]-eyeZ)>Reach) continue;
        // the nest's own air: the column leans away from the trunk and a little with the trade
        const float nx=nest.normal[0],nz=nest.normal[2];
        for(int k=0;k<AlatesPerNest;++k) {
            const auto seed=nest.seed*977U+static_cast<std::uint32_t>(k)*31U;
            if(Hash(seed+1U)>strength) continue;
            const float period=6.0f+3.0f*Hash(seed+2U);
            const float age=std::fmod(seconds+Hash(seed+3U)*period,period);
            // out of the mouth, climbing on fluttering wings, the spiral opening as it rises,
            // then away over the canopy
            const float rise=age*(0.45f+0.35f*Hash(seed+5U))+0.35f*age*age/period;
            const float spin=age*(2.2f+1.6f*Hash(seed+7U))+Hash(seed+11U)*2.0f*Pi;
            const float spread=0.12f+1.3f*age/period;
            const float out=0.25f*age+0.12f*age*age;
            Alate alate;
            alate.x=mouth[0]+nx*out*0.6f+0.35f*age+std::cos(spin)*spread;
            alate.y=mouth[1]+rise+0.15f*std::sin(spin*1.7f);
            alate.z=mouth[2]+nz*out*0.6f+std::sin(spin)*spread;
            alate.heading=spin+Pi*0.5f;
            alate.flap=seconds*55.0f+Hash(seed+13U)*6.2831853f;
            alate.scale=std::clamp(age/0.4f,0.0f,1.0f)*std::clamp((period-age)/1.2f,0.0f,1.0f);
            alates.push_back(alate);
        }
        // the bats round the column, a few metres up, swooping through it now and then
        if(strength>0.15f) {
            for(int b=0;b<BatsPerNest;++b) {
                const auto seed=nest.seed*409U+static_cast<std::uint32_t>(b)*53U;
                const float radius=2.4f+1.6f*Hash(seed+1U);
                const float speed=(1.3f+0.5f*Hash(seed+2U))*(b%2==0?1.0f:-1.0f);
                const float angle=seconds*speed+Hash(seed+3U)*2.0f*Pi;
                // the swoop: every few seconds the circle tightens through the column and drops
                const float swoopPhase=std::fmod(seconds*0.35f+Hash(seed+5U),1.0f);
                const float swoop=swoopPhase<0.25f?std::sin(swoopPhase/0.25f*Pi):0.0f;
                const float r=radius*(1.0f-0.85f*swoop);
                FruitBatState bat;
                bat.x=mouth[0]+0.8f+std::cos(angle)*r;
                bat.y=mouth[1]+3.6f+1.2f*Hash(seed+7U)+0.6f*std::sin(seconds*0.9f+Hash(seed+11U)*6.0f)-1.4f*swoop;
                bat.z=mouth[2]+std::sin(angle)*r;
                bat.headingRadians=angle+(speed>0.0f?0.5f:-0.5f)*Pi;
                bat.speed=std::abs(speed)*r;
                bat.wingPose=std::sin(seconds*15.0f+Hash(seed+13U)*6.0f);
                bat.hangAmount=0.0f;
                bat.scale=0.42f;   // a small insect-eating bat, not a flying fox
                bat.behavior=FruitBatBehavior::DayFlight;
                bat.colorVariant=3U;
                bats.push_back(bat);
            }
        }
        // the gecko on the bark just below the mouth, creeping up and snapping at an alate
        if(strength>0.3f) {
            const float period=7.0f+2.0f*Hash(nest.seed+71U);
            const float t=std::fmod(seconds+Hash(nest.seed+73U)*period,period);
            GeckoState gecko;
            const float below=t<4.0f?0.42f-0.06f*(t/4.0f):(t<4.2f?0.30f:0.36f);
            gecko.x=nest.bark[0]+nx*0.03f;
            gecko.y=nest.bark[1]-below;
            gecko.z=nest.bark[2]+nz*0.03f;
            gecko.wallNormalX=nx;gecko.wallNormalZ=nz;
            gecko.headingRadians=0.0f;   // straight up the trunk
            gecko.scale=0.14f;
            gecko.visibility=1.0f;
            gecko.colorVariant=static_cast<std::uint8_t>(nest.seed%4U);
            if(t<4.0f) {gecko.behavior=GeckoBehavior::Stalk;gecko.tailWag=0.8f;gecko.tailWagPhase=seconds*9.0f;gecko.speed=0.015f;}
            else if(t<4.2f) gecko.behavior=GeckoBehavior::Lunge;
            else if(t<5.2f) {gecko.behavior=GeckoBehavior::Rest;gecko.lipLick=std::sin((t-4.2f)*Pi);}
            else gecko.behavior=GeckoBehavior::Rest;
            gecko.gaitPhase=seconds*6.0f;
            geckos.push_back(gecko);
        }
    }
}

}
