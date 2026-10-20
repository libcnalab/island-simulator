#include "IslandSimulator/DolphinBlowField.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t DolphinBlowField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void DolphinBlowField::Update(const float deltaSeconds,const float windSeconds,const std::span<const DolphinState> dolphins) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t i=0;i<dolphins.size()&&i<trackers_.size();++i) {
        const DolphinState& dolphin=dolphins[i];
        Tracker& tracker=trackers_[i];
        tracker.sinceBlow+=dt;
        if(!std::isfinite(dolphin.x)||!std::isfinite(dolphin.y)||!std::isfinite(dolphin.z)) continue;
        const bool below=dolphin.y<SurfaceY;
        const bool surfacing=tracker.wasBelow&&!below&&dolphin.simulationTier!=WildlifeSimulationTier::Dormant;
        tracker.wasBelow=below;
        if(!surfacing||tracker.sinceBlow<MinimumIntervalSeconds) continue;
        for(MistPuff& puff:puffs_) {
            if(puff.active) continue;
            puff.x=dolphin.x+std::cos(dolphin.headingRadians)*BlowholeAhead*dolphin.scale;
            puff.z=dolphin.z+std::sin(dolphin.headingRadians)*BlowholeAhead*dolphin.scale;
            puff.y=std::max(dolphin.y,0.0f)+BlowholeUp*dolphin.scale;
            puff.size=0.16f;puff.age=0;puff.density=0;puff.active=true;
            tracker.sinceBlow=0;++blows_;
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
        puff.x-=(0.3f+0.8f*gust)*dt;
        puff.size+=0.5f*dt;
        puff.density=std::sin(3.14159f*t)*0.85f;
    }
}
}
