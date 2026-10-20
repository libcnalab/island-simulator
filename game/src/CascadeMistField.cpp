#include "IslandSimulator/CascadeMistField.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
CascadeMistField::CascadeMistField(const IslandTerrain& terrain) {
    const StreamCourse course(terrain);
    const std::size_t lip=course.CascadeIndex();
    const StreamStation& top=course.Station(lip);
    const StreamStation& foot=course.Station(std::min(lip+1,course.Count()-1));
    footX_=foot.x;footZ_=foot.z;footY_=foot.water;
    dropHeight_=top.water-foot.water;halfWidth_=foot.halfWidth;
    const float length=std::max(std::hypot(foot.x-top.x,foot.z-top.z),0.01f);
    downX_=(foot.x-top.x)/length;downZ_=(foot.z-top.z)/length;
}

void CascadeMistField::Release(const std::size_t index) noexcept {
    MistPuff& puff=puffs_[index];
    ++releases_;
    const float across=(static_cast<float>((releases_*7U)%11U)/10.0f-0.5f)*1.2f*halfWidth_;
    puff.x=footX_-downZ_*across+downX_*0.3f;
    puff.z=footZ_+downX_*across+downZ_*0.3f;
    puff.y=footY_+0.05f;
    puff.size=0.22f+0.06f*static_cast<float>(releases_%3U);
    puff.age=0;puff.density=0;puff.active=true;
}

void CascadeMistField::Update(const float deltaSeconds,const float windSeconds) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f||!Present()) return;
    gust_=std::isfinite(windSeconds)?WindGust::Strength(footX_,footZ_,windSeconds):0.0f;
    releaseClock_+=dt;
    if(releaseClock_>=ReleaseIntervalSeconds) {
        releaseClock_=0;
        for(std::size_t i=0;i<puffs_.size();++i) if(!puffs_[i].active) { Release(i); break; }
    }
    const float spray=std::clamp(dropHeight_/1.5f,0.5f,1.4f);
    for(MistPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        // Up fast at first, slowing; downstream on the breeze; downwind (toward -x) in a gust.
        puff.y+=RiseSpeed*spray*(1.0f-0.7f*t)*dt;
        puff.x+=(downX_*DriftSpeed-GustLean*gust_)*dt;
        puff.z+=downZ_*DriftSpeed*dt;
        puff.size+=(0.32f+0.1f*spray)*dt;
        // Even a low step throws a visible mist; a tall drop a dense one.
        puff.density=std::sin(3.14159f*t)*std::clamp(spray,0.6f,1.0f);
    }
}
}
