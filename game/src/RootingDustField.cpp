#include "IslandSimulator/RootingDustField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t RootingDustField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void RootingDustField::Update(const float deltaSeconds,const float windSeconds,const std::span<const BoarState> boars) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t i=0;i<boars.size()&&i<clocks_.size();++i) {
        const BoarState& boar=boars[i];
        const bool rooting=boar.behavior==BoarBehavior::Forage&&boar.rootingPose>0.8f
            &&boar.simulationTier!=WildlifeSimulationTier::Dormant;
        if(!rooting||!std::isfinite(boar.x)||!std::isfinite(boar.z)) { clocks_[i]=ReleaseIntervalSeconds*0.5f; continue; }
        clocks_[i]+=dt;
        if(clocks_[i]<ReleaseIntervalSeconds) continue;
        clocks_[i]=0;
        for(DustPuff& puff:puffs_) {
            if(puff.active) continue;
            // Thrown up at the snout, a little to one side as the head sweeps.
            const float sweep=std::sin(static_cast<float>(releases_)*2.1f)*0.18f;
            puff.x=boar.x+std::cos(boar.headingRadians)*SnoutAhead*boar.scale-std::sin(boar.headingRadians)*sweep;
            puff.z=boar.z+std::sin(boar.headingRadians)*SnoutAhead*boar.scale+std::cos(boar.headingRadians)*sweep;
            puff.y=terrain_.Height(puff.x,puff.z)+SnoutHeight*boar.scale;
            puff.size=0.10f+0.03f*static_cast<float>(releases_%3U);
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
        puff.y+=0.22f*(1.0f-0.8f*t)*dt;
        puff.x-=(0.06f+0.5f*gust)*dt;   // the breeze, and the fronts, carry it toward -x
        puff.size+=0.24f*dt;
        puff.density=std::sin(3.14159f*t);
    }
}
}
