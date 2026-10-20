#include "IslandSimulator/SurfSprayField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
void SurfSprayField::SetRocks(const std::span<const RockPlacement> rocks) {
    rocks_.clear();
    for(const auto& rock:rocks) {
        if(!rock.offshore||!std::isfinite(rock.x)||!std::isfinite(rock.z)) continue;
        Rock r;
        r.x=rock.x;r.z=rock.z;
        r.radius=std::max(0.3f,std::max(rock.scaleX,rock.scaleZ)*0.5f);
        r.top=rock.y+rock.scaleY*0.6f;
        r.offshore=std::max(0.0f,-terrain_.SignedShoreDistance(rock.x,rock.z));
        r.coastAngle=std::atan2(rock.z,rock.x);
        const float radial=std::max(std::hypot(rock.x,rock.z),1.0f);
        r.outX=rock.x/radial;r.outZ=rock.z/radial;
        rocks_.push_back(r);
    }
}

void SurfSprayField::Burst(const Rock& rock) noexcept {
    std::size_t placed=0;
    for(MistPuff& puff:puffs_) {
        if(puff.active||placed>=PuffsPerBurst) continue;
        // Up the seaward face, spread along it.
        const float along=(static_cast<float>(placed)-1.0f)*0.45f*rock.radius;
        puff.x=rock.x+rock.outX*rock.radius*0.8f-rock.outZ*along;
        puff.z=rock.z+rock.outZ*rock.radius*0.8f+rock.outX*along;
        puff.y=std::max(rock.top-0.3f,0.1f);
        puff.size=0.25f+0.1f*static_cast<float>(placed);
        puff.age=0;puff.density=0;puff.active=true;
        ++placed;
    }
    ++bursts_;
}

void SurfSprayField::Update(const float deltaSeconds,const float seconds,const float observerX,const float observerZ) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f||!std::isfinite(seconds)) return;
    const bool observerValid=std::isfinite(observerX)&&std::isfinite(observerZ);
    for(Rock& rock:rocks_) {
        if(!observerValid||std::hypot(rock.x-observerX,rock.z-observerZ)>Reach) { rock.lastSine=0; continue; }
        const float sine=std::sin(BreakerPhase(rock.offshore,rock.coastAngle,seconds));
        if(sine>=0.92f&&rock.lastSine<0.92f) Burst(rock);
        rock.lastSine=sine;
    }
    for(MistPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        const float gust=WindGust::Strength(puff.x,puff.z,seconds);
        puff.y+=RiseSpeed*(1.0f-0.85f*t)*dt;
        puff.x-=(0.35f+0.9f*gust)*dt;
        puff.size+=0.55f*dt;
        puff.density=std::sin(3.14159f*t)*0.9f;
    }
}
}
