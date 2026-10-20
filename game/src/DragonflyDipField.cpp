#include "IslandSimulator/DragonflyDipField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t DragonflyDipField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void DragonflyDipField::Update(const float deltaSeconds,const DragonflySwarm::Dip& dip) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(dip.count!=seen_&&std::isfinite(dip.x)&&std::isfinite(dip.y)&&std::isfinite(dip.z)) {
        seen_=dip.count;
        for(MistPuff& puff:puffs_) {
            if(puff.active) continue;
            puff.x=dip.x;puff.y=dip.y+0.02f;puff.z=dip.z;
            puff.size=0.05f;puff.age=0;puff.density=0;puff.active=true;
            break;
        }
    }
    for(MistPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        puff.y+=RiseSpeed*(1.0f-0.6f*t)*dt;
        puff.size+=0.12f*dt;
        puff.density=std::sin(3.14159f*t)*0.9f;
    }
}
}
