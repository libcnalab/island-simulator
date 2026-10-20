#include "IslandSimulator/MudFleckField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t MudFleckField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void MudFleckField::Update(const float deltaSeconds,const BoarHerd::Shake& shake) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    if(shake.count!=seen_&&std::isfinite(shake.x)&&std::isfinite(shake.y)&&std::isfinite(shake.z)
       &&std::isfinite(shake.headingRadians)) {
        seen_=shake.count;
        std::size_t placed=0;
        for(std::size_t slot=0;slot<puffs_.size()&&placed<FlecksPerShake;++slot) {
            DustPuff& puff=puffs_[slot];
            if(puff.active) continue;
            // Each fleck leaves a flank sideways, from the shoulders to the haunches, a
            // little upward, the side alternating.
            const float side=(placed%2U==0U)?1.0f:-1.0f;
            const float along=(-0.45f+0.9f*static_cast<float>(placed/2U)/3.0f)*shake.scale;
            const float sx=-std::sin(shake.headingRadians)*side,sz=std::cos(shake.headingRadians)*side;
            puff.x=shake.x+std::cos(shake.headingRadians)*along+sx*0.28f*shake.scale;
            puff.z=shake.z+std::sin(shake.headingRadians)*along+sz*0.28f*shake.scale;
            puff.y=shake.y+(0.42f+0.08f*static_cast<float>(placed%3U))*shake.scale;
            puff.size=0.05f*shake.scale;
            puff.age=0;puff.density=0;puff.active=true;
            const float speed=FlingSpeed*(0.75f+0.25f*static_cast<float>(placed%3U)/2.0f);
            motion_[slot]={sx*speed,0.9f+0.3f*static_cast<float>(placed%2U),sz*speed};
            ++placed;
        }
    }
    for(std::size_t slot=0;slot<puffs_.size();++slot) {
        DustPuff& puff=puffs_[slot];
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        Motion& m=motion_[slot];
        m.vy-=Gravity*dt;
        puff.x+=m.vx*dt;puff.y+=m.vy*dt;puff.z+=m.vz*dt;
        const float t=puff.age/LifeSeconds;
        puff.density=(t<0.1f?t/0.1f:1.0f-(t-0.1f)/0.9f)*0.85f;
    }
}
}
