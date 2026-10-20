#include "IslandSimulator/CrabBubbleField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t CrabBubbleField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void CrabBubbleField::Update(const float deltaSeconds,const std::span<const CrabState> crabs) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t i=0;i<crabs.size()&&i<clocks_.size();++i) {
        const CrabState& crab=crabs[i];
        const bool feeding=crab.feeding>0.5f&&crab.simulationTier!=WildlifeSimulationTier::Dormant
            &&std::isfinite(crab.x)&&std::isfinite(crab.y)&&std::isfinite(crab.z);
        if(!feeding) { clocks_[i]=IntervalSeconds*0.5f; continue; }
        clocks_[i]+=dt;
        if(clocks_[i]<IntervalSeconds+0.3f*static_cast<float>(i%3U)) continue;
        clocks_[i]=0;
        for(MistPuff& puff:puffs_) {
            if(puff.active) continue;
            // At the mouth: ahead of the body's centre, a little up.
            puff.x=crab.x+std::cos(crab.bodyYawRadians)*0.14f*crab.scale;
            puff.z=crab.z+std::sin(crab.bodyYawRadians)*0.14f*crab.scale;
            puff.y=crab.y+0.09f*crab.scale;
            puff.size=0.012f*crab.scale;puff.age=0;puff.density=0;puff.active=true;
            ++bubbles_;
            break;
        }
    }
    for(MistPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        puff.y+=RiseSpeed*dt;
        puff.size+=0.006f*dt;
        // Bright at once, and popped at the end.
        puff.density=(t<0.1f?t/0.1f:1.0f-std::pow((t-0.1f)/0.9f,3.0f))*0.75f;
    }
}
}
