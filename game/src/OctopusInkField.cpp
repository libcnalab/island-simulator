#include "IslandSimulator/OctopusInkField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t OctopusInkField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void OctopusInkField::Update(const float deltaSeconds,const OctopusGroup::InkEvent& ink) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(ink.count!=seen_&&std::isfinite(ink.x)&&std::isfinite(ink.y)&&std::isfinite(ink.z)) {
        seen_=ink.count;
        std::size_t placed=0;
        for(DustPuff& puff:puffs_) {
            if(puff.active||placed>=PuffsPerCloud) continue;
            const float angle=static_cast<float>(placed)*1.5708f+static_cast<float>(seen_)*0.9f;
            const float ring=0.18f*ink.scale;
            puff.x=ink.x+std::cos(angle)*ring;puff.z=ink.z+std::sin(angle)*ring;
            puff.y=ink.y+0.1f*ink.scale;
            puff.size=0.2f*ink.scale;puff.age=0;puff.density=0;puff.active=true;
            ++placed;
        }
    }
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        puff.y+=0.08f*(1.0f-t)*dt;
        puff.size+=0.32f*dt;
        puff.density=std::sin(3.14159f*std::min(1.0f,t*1.6f))*(1.0f-t*0.5f);
    }
}
}
