#include "IslandSimulator/SunSteamField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
float Smooth(const float a,const float b,const float v) noexcept {
    const float t=std::clamp((v-a)/(b-a),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
}

float SunSteamField::Strength(const float wetness,const float rain,const float sunOut,const float sunElevationDegrees) noexcept {
    if(!std::isfinite(wetness)||!std::isfinite(rain)||!std::isfinite(sunOut)||!std::isfinite(sunElevationDegrees)) return 0.0f;
    return Smooth(0.25f,0.70f,wetness)*(1.0f-Smooth(0.02f,0.15f,rain))*std::clamp(sunOut,0.0f,1.0f)
        *Smooth(12.0f,35.0f,sunElevationDegrees);
}

std::size_t SunSteamField::ActiveCount() const noexcept {
    return static_cast<std::size_t>(std::count_if(puffs_.begin(),puffs_.end(),[](const MistPuff& p){return p.active;}));
}

void SunSteamField::Update(const float deltaSeconds,const float x,const float z,const float strength,
                           const float windX,const float windZ,const GroundProbe& probe) {
    if(!warmed_) {
        warmed_=true;
        for(int step=0;step<static_cast<int>(LifeSeconds*10.0f);++step) Update(0.1f,x,z,strength,windX,windZ,probe);
    }
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    const float s=std::isfinite(strength)?std::clamp(strength,0.0f,1.0f):0.0f;
    for(std::size_t i=0;i<Capacity;++i) {
        MistPuff& puff=puffs_[i];
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) {puff.active=false;puff.density=0;continue;}
        const float t=puff.age/LifeSeconds;
        // climbing, slowing as it spreads, and leaning away on the breeze
        puff.y+=RiseSpeed*(1.0f-0.45f*t)*dt;
        puff.x+=windX*DriftSpeed*t*dt;
        puff.z+=windZ*DriftSpeed*t*dt;
        puff.size=0.45f+1.25f*t;
        puff.density=peak_[i]*Smooth(0.0f,0.22f,t)*(1.0f-Smooth(0.45f,1.0f,t));
    }
    if(s<=0.0f||!std::isfinite(x)||!std::isfinite(z)||!probe) {pending_=0;return;}
    pending_+=s*SpawnPerSecond*dt;
    // a few tries for each wisp due: spots under the crowns or on the water are passed over
    for(int tries=0;pending_>=1.0f&&tries<12;++tries) {
        const auto next=[this]{seed_=seed_*1664525U+1013904223U;return static_cast<float>(seed_>>8U)*(1.0f/16777216.0f);};
        const float angle=6.2831853f*next();
        const float distance=NearClear+(Reach-NearClear)*std::sqrt(next());
        const float px=x+std::cos(angle)*distance,pz=z+std::sin(angle)*distance;
        float ground=0;
        if(!probe(px,pz,ground)||!std::isfinite(ground)) continue;
        pending_-=1.0f;
        const std::size_t slot=next_;
        next_=(next_+1U)%Capacity;
        MistPuff& puff=puffs_[slot];
        puff={px,ground+0.08f,pz,0.45f,0.0f,0.0f,true};
        peak_[slot]=s*(0.22f+0.10f*next());
        ++started_;
    }
    pending_=std::min(pending_,2.0f);
}
}
