#include "IslandSimulator/HatchlingField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
void HatchlingField::SetNests(const std::span<const HatchlingNest> nests) {
    nests_.clear();
    for(const auto& nest:nests) if(std::isfinite(nest.x)&&std::isfinite(nest.z)) nests_.push_back(nest);
}

std::size_t HatchlingField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& h:hatchlings_) count+=h.active?1U:0U;
    return count;
}

void HatchlingField::Emerge(const std::size_t nest) noexcept {
    const HatchlingNest& site=nests_[nest];
    const float radial=std::max(std::hypot(site.x,site.z),1.0f);
    const float seaward=std::atan2(site.z/radial,site.x/radial);
    std::size_t placed=0;
    for(std::size_t i=0;i<hatchlings_.size()&&placed<ClutchSize;++i) {
        Hatchling& h=hatchlings_[i];
        if(h.active) continue;
        // Out of the sand around the nest, each on its own line a little off seaward.
        const float spread=(static_cast<float>((placed*7U+clutches_*3U)%11U)/10.0f-0.5f)*0.7f;
        const float ring=0.15f+0.04f*static_cast<float>(placed%4U);
        const float around=static_cast<float>(placed)*0.52f+static_cast<float>(clutches_)*0.9f;
        h.x=site.x+std::cos(around)*ring;h.z=site.z+std::sin(around)*ring;
        h.y=terrain_.Height(h.x,h.z)+0.02f;
        h.headingRadians=seaward+spread;
        h.speed=ScrambleSpeed*(0.8f+0.4f*static_cast<float>((placed*5U)%7U)/6.0f);
        h.flipperPhase=static_cast<float>(placed)*1.3f;
        h.swimSeconds=0;h.swimming=false;h.active=true;
        ++placed;
    }
    ++clutches_;
}

void HatchlingField::Update(const float deltaSeconds) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f||nests_.empty()) return;
    clock_+=dt;
    if(clock_>=nextEmergence_) {
        Emerge(static_cast<std::size_t>(clutches_)%nests_.size());
        nextEmergence_=clock_+EmergenceIntervalSeconds;
    }
    for(Hatchling& h:hatchlings_) {
        if(!h.active) continue;
        h.flipperPhase+=dt*(h.swimming?6.0f:9.0f);
        if(h.swimming) {
            h.swimSeconds+=dt;
            h.x+=std::cos(h.headingRadians)*SwimSpeed*dt;h.z+=std::sin(h.headingRadians)*SwimSpeed*dt;
            h.y=0.0f;
            if(h.swimSeconds>=SwimSeconds) h.active=false;
            continue;
        }
        // The scramble: bursts and pauses, a wobble about the seaward line, and the sea's
        // pull straightening it as the sand wets.
        const float radial=std::max(std::hypot(h.x,h.z),1.0f);
        const float seaward=std::atan2(h.z/radial,h.x/radial);
        const float wobble=std::sin(h.flipperPhase*0.37f)*0.35f;
        const float toward=std::remainder(seaward+wobble-h.headingRadians,6.2831853f);
        h.headingRadians+=std::clamp(toward,-1.2f*dt,1.2f*dt);
        const float burst=0.55f+0.45f*std::sin(h.flipperPhase*0.9f);
        const float step=h.speed*burst*dt;
        h.x+=std::cos(h.headingRadians)*step;h.z+=std::sin(h.headingRadians)*step;
        const float ground=terrain_.Height(h.x,h.z);
        if(ground<=0.0f) { h.swimming=true;h.y=0.0f;++reachedSea_;h.headingRadians=seaward; }
        else h.y=ground+0.02f;
    }
}
}
