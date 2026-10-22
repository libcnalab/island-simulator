#include "IslandSimulator/FootprintTrail.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t FootprintTrail::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& print:prints_) count+=print.active?1U:0U;
    return count;
}

void FootprintTrail::Update(const float deltaSeconds,const float x,const float z,
                            const float velocityX,const float velocityZ,const bool onSand,const bool wetSand) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(Footprint& print:prints_) {
        if(!print.active) continue;
        print.age+=dt*(1.0f+RainErosion*rain_);
        const float life=print.life>0.0f?print.life:LifeSeconds;
        print.density=1.0f-print.age/life;
        if(print.age>=life) print.active=false;
    }
    if(!onSand||!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(velocityX)||!std::isfinite(velocityZ)) return;
    const float speed=std::hypot(velocityX,velocityZ);
    if(speed<MinimumSpeed) return;
    travelled_+=speed*dt;
    if(travelled_<stride_) return;
    travelled_-=stride_;
    // The oldest print gives way to the newest.
    Footprint* slot=&prints_[0];
    for(Footprint& print:prints_) {
        if(!print.active) { slot=&print; break; }
        if(print.age>slot->age) slot=&print;
    }
    const float heading=std::atan2(velocityZ,velocityX);
    const float side=nextLeft_?-1.0f:1.0f;
    slot->x=x-std::sin(heading)*FootAcross*side;
    slot->z=z+std::cos(heading)*FootAcross*side;
    slot->headingRadians=heading+side*0.10f;
    slot->age=0;slot->density=1;slot->left=nextLeft_;slot->active=true;slot->kind=kind_;
    slot->life=wetSand?WetLifeSeconds:LifeSeconds;
    nextLeft_=!nextLeft_;
    ++laid_;
}
}
