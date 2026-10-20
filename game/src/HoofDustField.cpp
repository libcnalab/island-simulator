#include "IslandSimulator/HoofDustField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t HoofDustField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void HoofDustField::Update(const float deltaSeconds,const float windSeconds,
                           const std::span<const HoofDustRunner> runners) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t i=0;i<runners.size()&&i<clocks_.size();++i) {
        const HoofDustRunner& runner=runners[i];
        const bool running=runner.dryGround&&std::isfinite(runner.x)&&std::isfinite(runner.z)
            &&std::isfinite(runner.headingRadians)&&std::isfinite(runner.speed)&&runner.speed>=MinimumSpeed;
        if(!running) { clocks_[i]=ReleaseIntervalSeconds*0.5f; continue; }
        clocks_[i]+=dt;
        if(clocks_[i]<ReleaseIntervalSeconds) continue;
        clocks_[i]=0;
        for(DustPuff& puff:puffs_) {
            if(puff.active) continue;
            // Kicked up behind the hooves, alternating a little either side of the line.
            const float side=(releases_%2U==0U?1.0f:-1.0f)*0.14f*runner.scale;
            const float back=HoofBehind*runner.scale;
            puff.x=runner.x-std::cos(runner.headingRadians)*back-std::sin(runner.headingRadians)*side;
            puff.z=runner.z-std::sin(runner.headingRadians)*back+std::cos(runner.headingRadians)*side;
            puff.y=terrain_.Height(puff.x,puff.z)+0.10f*runner.scale;
            puff.size=0.12f+0.04f*static_cast<float>(releases_%3U)+0.02f*std::min(runner.speed,4.0f);
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
        const float gust=std::isfinite(windSeconds)?WindGust::Strength(puff.x,puff.z,windSeconds):0.0f;
        puff.y+=0.30f*(1.0f-0.8f*t)*dt;
        puff.x-=(0.08f+0.6f*gust)*dt;
        puff.size+=0.35f*dt;
        puff.density=std::sin(3.14159f*t)*0.85f;
    }
}
}
