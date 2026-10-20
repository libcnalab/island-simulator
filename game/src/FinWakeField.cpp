#include "IslandSimulator/FinWakeField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t FinWakeField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

bool FinWakeField::Cutting(const SharkState& shark) noexcept {
    return shark.present&&shark.visible&&std::isfinite(shark.x)&&std::isfinite(shark.y)&&std::isfinite(shark.z)
        &&std::isfinite(shark.headingRadians)&&shark.y>CuttingDepth;
}

void FinWakeField::Update(const float deltaSeconds,const SharkState& shark) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(Cutting(shark)) {
        clock_+=dt;
        if(clock_>=ReleaseIntervalSeconds) {
            clock_=0;
            for(DustPuff& puff:puffs_) {
                if(puff.active) continue;
                // Laid at the dorsal fin, a little behind the head, on the surface.
                const float back=0.15f*shark.scale;
                puff.x=shark.x-std::cos(shark.headingRadians)*back;
                puff.z=shark.z-std::sin(shark.headingRadians)*back;
                puff.y=0.03f;
                puff.size=0.10f*shark.scale;
                puff.age=0;puff.density=0;puff.active=true;
                ++releases_;
                break;
            }
        }
    } else {
        clock_=ReleaseIntervalSeconds;
    }
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        // The foam spreads on the water and thins; it does not rise.
        puff.size+=0.22f*dt;
        puff.density=(t<0.15f?t/0.15f:1.0f-(t-0.15f)/0.85f)*0.8f;
    }
}
}
