#include "IslandSimulator/TurtleGrazeField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t TurtleGrazeField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void TurtleGrazeField::Update(const float deltaSeconds,const SeaTurtleGroup::Bite& bite) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(bite.count!=seen_&&std::isfinite(bite.x)&&std::isfinite(bite.y)&&std::isfinite(bite.z)) {
        seen_=bite.count;
        for(DustPuff& puff:puffs_) {
            if(puff.active) continue;
            puff.x=bite.x;puff.y=bite.y+0.04f;puff.z=bite.z;
            puff.size=0.16f*bite.scale;puff.age=0;puff.density=0;puff.active=true;
            break;
        }
    }
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        puff.y+=RiseSpeed*(1.0f-0.7f*t)*dt;
        puff.size+=0.30f*dt;
        puff.density=std::sin(3.14159f*t)*0.8f;
    }
}
}
