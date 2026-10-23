#include "IslandSimulator/ElephantSplashField.hpp"
#include "IslandSimulator/ElephantMotion.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

std::size_t ElephantSplashField::ActiveSpray() const noexcept {
    std::size_t count=0;
    for(const auto& puff:spray_) count+=puff.active?1U:0U;
    return count;
}

std::size_t ElephantSplashField::ActiveFoam() const noexcept {
    std::size_t count=0;
    for(const auto& puff:foam_) count+=puff.active?1U:0U;
    return count;
}

void ElephantSplashField::AddFoam(const float x,const float z,const float size) noexcept {
    for(DustPuff& puff:foam_) {
        if(puff.active) continue;
        puff.x=x;puff.z=z;puff.y=.04f;puff.size=size;puff.age=0;puff.density=0;puff.active=true;
        ++released_;
        return;
    }
}

void ElephantSplashField::Update(const float deltaSeconds,const ElephantMotion& elephant) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    const ElephantState& s=elephant.State();
    if(!primed_) {seenSprays_=elephant.SprayCount();primed_=true;}
    if(elephant.SprayCount()!=seenSprays_) {
        seenSprays_=elephant.SprayCount();
        // the shower: from the trunk's tip up and back over the body, fanned across the back
        const auto trunk=elephant.TrunkPoints();
        const ElephantPoint tip=trunk.back();
        const ElephantPoint back=elephant.ToWorld(0.0f,.9f,-.6f);
        const float bx=back.x-tip.x,bz=back.z-tip.z;
        for(int k=0;k<12;++k) {
            for(std::size_t i=0;i<spray_.size();++i) {
                MistPuff& puff=spray_[i];
                if(puff.active) continue;
                const float r1=Hash(released_*7U+static_cast<std::uint32_t>(k)),r2=Hash(released_*13U+static_cast<std::uint32_t>(k)*3U);
                puff.x=tip.x;puff.y=std::max(tip.y,0.1f);puff.z=tip.z;
                puff.size=.11f+.06f*r1;puff.age=0;puff.density=0;puff.active=true;
                // (thrown to come down on the back within the second)
                const float spread=(r2-.5f)*.9f,throw_=.85f+.45f*r1;
                velocity_[i]={bx*throw_+bz*spread*.3f,2.0f+1.6f*r2,bz*throw_-bx*spread*.3f};
                ++released_;
                break;
            }
        }
        // where it comes down, on the water about the body
        for(int k=0;k<3;++k) {
            const float side=k==0?-1.0f:k==1?1.0f:0.0f;
            const ElephantPoint p=elephant.ToWorld(side*1.1f,0.0f,-.8f+.8f*static_cast<float>(k));
            if(s.waterDepth>.3f) AddFoam(p.x,p.z,.35f);
        }
    }
    // foam round the body at the waterline: wading, the water pushed up about the chest; lying
    // down and rising, all round it; standing in the sea, the lapping now and then
    if(s.waterDepth>.3f) {
        foamClock_+=dt;
        const float every=s.speed>.2f?.30f:s.sink>.05f&&s.sink<.95f?.25f:.9f;
        if(foamClock_>=every) {
            foamClock_=0.0f;
            const float a=2.0f*std::numbers::pi_v<float>*Hash(released_*31U+5U);
            const ElephantPoint p=s.speed>.2f?elephant.ToWorld(std::sin(a)*.8f,0.0f,1.3f+.4f*std::cos(a))
                                             :elephant.ToWorld(std::sin(a)*1.0f,0.0f,std::cos(a)*2.1f);
            AddFoam(p.x,p.z,.28f+.12f*Hash(released_*3U));
        }
    }
    for(std::size_t i=0;i<spray_.size();++i) {
        MistPuff& puff=spray_[i];
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=SprayLifeSeconds||puff.y<-.1f) {puff.active=false;continue;}
        const float t=puff.age/SprayLifeSeconds;
        auto& v=velocity_[i];
        v[1]-=7.5f*dt;
        puff.x+=v[0]*dt;puff.y+=v[1]*dt;puff.z+=v[2]*dt;
        puff.size+=.45f*dt;
        puff.density=std::sin(std::numbers::pi_v<float>*std::min(1.0f,t*1.4f))*.9f;
    }
    for(DustPuff& puff:foam_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=FoamLifeSeconds) {puff.active=false;continue;}
        const float t=puff.age/FoamLifeSeconds;
        puff.size+=.22f*dt;
        puff.density=(t<.12f?t/.12f:1.0f-(t-.12f)/.88f)*.6f;
    }
}
}
