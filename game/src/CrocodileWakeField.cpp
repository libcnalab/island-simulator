#include "IslandSimulator/CrocodileWakeField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t CrocodileWakeField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

bool CrocodileWakeField::Cutting(const CrocodileState& crocodile) noexcept {
    return crocodile.simulationTier!=WildlifeSimulationTier::Dormant
        &&std::isfinite(crocodile.x)&&std::isfinite(crocodile.y)&&std::isfinite(crocodile.z)
        &&std::isfinite(crocodile.headingRadians)&&std::isfinite(crocodile.speed)
        &&crocodile.submerged>0.6f&&crocodile.speed>MinimumSpeed;
}

void CrocodileWakeField::Update(const float deltaSeconds,const std::span<const CrocodileState> crocodiles) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t i=0;i<crocodiles.size()&&i<clocks_.size();++i) {
        const CrocodileState& crocodile=crocodiles[i];
        if(!Cutting(crocodile)) { clocks_[i]=ReleaseIntervalSeconds; continue; }
        clocks_[i]+=dt;
        if(clocks_[i]<ReleaseIntervalSeconds) continue;
        clocks_[i]=0;
        for(DustPuff& puff:puffs_) {
            if(puff.active) continue;
            // Laid at the snout, on the surface, to trail behind as the crocodile moves on.
            const float ahead=0.9f*crocodile.scale;
            puff.x=crocodile.x+std::cos(crocodile.headingRadians)*ahead;
            puff.z=crocodile.z+std::sin(crocodile.headingRadians)*ahead;
            puff.y=0.04f;
            puff.size=0.09f*crocodile.scale;
            puff.age=0;puff.density=0;puff.active=true;
            ++releases_;
            break;
        }
    }
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        // The foam spreads on the water and thins, fainter than the shark's cut; it does not rise.
        puff.size+=0.18f*dt;
        puff.density=(t<0.15f?t/0.15f:1.0f-(t-0.15f)/0.85f)*0.55f;
    }
}
}
