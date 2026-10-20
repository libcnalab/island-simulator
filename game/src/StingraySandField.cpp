#include "IslandSimulator/StingraySandField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t StingraySandField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void StingraySandField::Update(const float deltaSeconds,const StingrayGroup::StingrayBurst& burst) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(burst.count!=seen_&&std::isfinite(burst.x)&&std::isfinite(burst.y)&&std::isfinite(burst.z)) {
        seen_=burst.count;
        std::size_t placed=0;
        for(DustPuff& puff:puffs_) {
            if(puff.active||placed>=PuffsPerBurst) continue;
            const float angle=static_cast<float>(placed)*1.2566f+static_cast<float>(seen_)*0.7f;
            const float ring=0.55f*burst.scale;
            puff.x=burst.x+std::cos(angle)*ring;puff.z=burst.z+std::sin(angle)*ring;
            puff.y=burst.y+0.05f;
            puff.size=0.22f*burst.scale;puff.age=0;puff.density=0;puff.active=true;
            ++placed;
        }
    }
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        puff.y+=RiseSpeed*(1.0f-0.6f*t)*dt;
        puff.size+=0.35f*dt;
        puff.density=std::sin(3.14159f*t)*0.9f;
    }
}
}
