#include "IslandSimulator/TurtleBreathField.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t TurtleBreathField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void TurtleBreathField::Update(const float deltaSeconds,const float windSeconds,const SeaTurtleGroup::Breath& breath) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(breath.count!=seen_&&std::isfinite(breath.x)&&std::isfinite(breath.z)) {
        seen_=breath.count;
        for(MistPuff& puff:puffs_) {
            if(puff.active) continue;
            puff.x=breath.x+std::cos(breath.headingRadians)*NoseAhead*breath.scale;
            puff.z=breath.z+std::sin(breath.headingRadians)*NoseAhead*breath.scale;
            puff.y=0.12f*breath.scale;
            puff.size=0.1f*breath.scale;puff.age=0;puff.density=0;puff.active=true;
            break;
        }
    }
    for(MistPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        const float gust=std::isfinite(windSeconds)?WindGust::Strength(puff.x,puff.z,windSeconds):0.0f;
        puff.y+=RiseSpeed*(1.0f-0.9f*t)*dt;
        puff.x-=(0.25f+0.7f*gust)*dt;
        puff.size+=0.3f*dt;
        puff.density=std::sin(3.14159f*t)*0.7f;
    }
}
}
