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
        r.spread=r.radius;r.out=r.radius*0.8f;
        rocks_.push_back(r);
    }
}

void SurfSprayField::AddFace(const float x,const float z,const float top,const float spread,const float outX,const float outZ,
                             const float scale) {
    if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(top)||!std::isfinite(spread)||!std::isfinite(scale)) return;
    const float norm=std::hypot(outX,outZ);
    if(!(norm>1e-4f)) return;
    Rock r;
    r.x=x;r.z=z;r.top=top;r.radius=spread;r.spread=spread;r.scale=scale;r.out=0.25f;
    r.offshore=std::max(0.0f,-terrain_.SignedShoreDistance(x,z));
    r.coastAngle=std::atan2(z,x);
    r.outX=outX/norm;r.outZ=outZ/norm;
    rocks_.push_back(r);
}

void SurfSprayField::Burst(const Rock& rock) noexcept {
    std::size_t placed=0;
    for(MistPuff& puff:puffs_) {
        if(puff.active||placed>=PuffsPerBurst) continue;
        // Up the seaward face, spread along it.
        const float along=(static_cast<float>(placed)-1.0f)*0.45f*rock.spread;
        puff.x=rock.x+rock.outX*rock.out-rock.outZ*along;
        puff.z=rock.z+rock.outZ*rock.out+rock.outX*along;
        puff.y=std::max(rock.top-0.3f,0.1f);
        puff.size=(0.25f+0.1f*static_cast<float>(placed))*rock.scale;
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
