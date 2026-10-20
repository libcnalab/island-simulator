#include "IslandSimulator/CanopyLeafFallField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
constexpr float Tau=6.28318531f;
}

std::size_t CanopyLeafFallField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& leaf:leaves_) count+=leaf.active?1U:0U;
    return count;
}

std::size_t CanopyLeafFallField::FallingCount() const noexcept {
    std::size_t count=0;
    for(std::size_t i=0;i<Capacity;++i) count+=leaves_[i].active&&rest_[i]<0.0f?1U:0U;
    return count;
}

void CanopyLeafFallField::Release(const CanopyPlacement& crown,const std::uint32_t seed) noexcept {
    if(!std::isfinite(crown.x)||!std::isfinite(crown.z)||!std::isfinite(crown.y)||crown.scale<=0) return;
    // From somewhere in the crown: a point around the trunk at the crown's height, the
    // crown geometry putting its lobes six to nine metres up a unit tree.
    const float angle=static_cast<float>(seed%360U)*(Tau/360.0f);
    const float radius=(1.2f+static_cast<float>((seed>>9U)%100U)*0.014f)*crown.scale;
    const float x=crown.x+std::cos(angle)*radius,z=crown.z+std::sin(angle)*radius;
    const float y=std::max(crown.y+(6.0f+static_cast<float>((seed>>16U)%30U)*0.1f)*crown.scale,
                           terrain_.Height(x,z)+3.0f);
    Let(x,y,z,angle*1.7f,seed);
    ++releases_;
}

void CanopyLeafFallField::Shake(const float x,const float y,const float z,const std::uint32_t seed) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)) return;
    if(y<terrain_.Height(x,z)+1.5f) return;   // nothing to shake loose this low
    std::uint32_t hash=seed*0x9e3779b9U^0x7f4a7c15U;
    hash^=hash>>16U;hash*=0x7feb352dU;hash^=hash>>15U;
    const float angle=static_cast<float>(hash%360U)*(Tau/360.0f);
    Let(x+std::cos(angle)*0.4f,y-0.3f,z+std::sin(angle)*0.4f,angle,hash);
    ++shakes_;
}

void CanopyLeafFallField::Feather(const float x,const float y,const float z,const std::uint32_t seed) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)) return;
    if(y<terrain_.Height(x,z)+0.3f) return;   // a bird on the floor lets nothing go
    std::uint32_t hash=seed*0x9e3779b9U^0x5bd1e995U;
    hash^=hash>>16U;hash*=0x7feb352dU;hash^=hash>>15U;
    const float angle=static_cast<float>(hash%360U)*(Tau/360.0f);
    const std::size_t slot=Let(x+std::cos(angle)*0.25f,y-0.15f,z+std::sin(angle)*0.25f,angle,hash);
    leaves_[slot].variant=FeatherVariant;
    leaves_[slot].speed=FeatherFallSpeed;
    ++feathers_;
}

std::size_t CanopyLeafFallField::Let(const float x,const float y,const float z,const float yaw,const std::uint32_t seed) noexcept {
    // A free slot, or the leaf that has lain longest.
    std::size_t slot=Capacity;
    for(std::size_t i=0;i<Capacity;++i) if(!leaves_[i].active) {slot=i;break;}
    if(slot==Capacity) {
        slot=0;
        for(std::size_t i=1;i<Capacity;++i) if(rest_[i]>rest_[slot]) slot=i;
    }
    DriftingLeaf& leaf=leaves_[slot];
    leaf.x=x;leaf.y=y;leaf.z=z;
    leaf.yawRadians=yaw;
    leaf.spin=0;leaf.distance=0;leaf.lateral=0;leaf.speed=FallSpeed;
    // Old leaves go first: mostly brown and yellow, now and then a green one.
    const std::uint32_t tint=(seed>>5U)%5U;
    leaf.variant=static_cast<std::uint8_t>(tint==0U?0U:1U+(tint&1U));
    leaf.active=true;leaf.atSea=false;leaf.seaSeconds=0;
    phase_[slot]=static_cast<float>((seed>>3U)%628U)*0.01f;
    rest_[slot]=-1.0f;
    return slot;
}

void CanopyLeafFallField::Update(const float seconds,const float deltaSeconds,
                                 const float observerX,const float observerZ) noexcept {
    if(!std::isfinite(deltaSeconds)||deltaSeconds<=0) return;
    const float dt=std::min(deltaSeconds,0.1f);
    if(std::isfinite(seconds)&&std::isfinite(observerX)&&std::isfinite(observerZ)) {
        if(crownGust_.size()!=crowns_.size()) crownGust_.assign(crowns_.size(),0.0f);
        for(std::size_t i=0;i<crowns_.size();++i) {
            const CanopyPlacement& crown=crowns_[i];
            if(std::hypot(crown.x-observerX,crown.z-observerZ)>Range) {crownGust_[i]=0.0f;continue;}
            const float gust=WindGust::Strength(crown.x,crown.z,seconds);
            const bool frontArrives=gust>0.85f&&crownGust_[i]<=0.85f;
            crownGust_[i]=gust;
            if(!frontArrives) continue;
            // A deterministic per-crown, per-front draw, as the palms' coconuts use.
            const std::uint32_t front=static_cast<std::uint32_t>((seconds*WindGust::FrontSpeed-crown.x)/WindGust::FrontSpacing);
            std::uint32_t hash=static_cast<std::uint32_t>(i)*0x9e3779b9U^front*0x85ebca6bU^0x3c6ef372U;
            hash^=hash>>16U;hash*=0x7feb352dU;hash^=hash>>15U;
            if(static_cast<float>(hash&1023U)/1024.0f>=LeafChance) continue;
            Release(crown,hash);
        }
    }
    for(std::size_t i=0;i<Capacity;++i) {
        DriftingLeaf& leaf=leaves_[i];
        if(!leaf.active) continue;
        if(rest_[i]<0.0f) {
            // Falling: a steady descent at the leaf's own speed (a feather's is slower and
            // its rock wider), carried downwind, rocking side to side and turning.
            const float rock=leaf.variant==FeatherVariant?1.8f:1.0f;
            phase_[i]=std::fmod(phase_[i]+dt*2.2f,Tau);
            leaf.x+=(-DriftSpeed*(leaf.variant==FeatherVariant?0.6f:1.0f)+std::sin(phase_[i])*SwaySpeed*rock)*dt;
            leaf.z+=std::cos(phase_[i]*0.9f)*SwaySpeed*0.7f*rock*dt;
            leaf.y-=leaf.speed*dt;
            leaf.yawRadians=std::fmod(leaf.yawRadians+SpinRate*dt,Tau);
            const float ground=std::max(terrain_.Height(leaf.x,leaf.z),0.0f)+0.03f;
            if(leaf.y<=ground) {leaf.y=ground;rest_[i]=0.0f;leaf.speed=0.0f;}
        } else {
            rest_[i]+=dt;
            if(rest_[i]>=RestSeconds) leaf.active=false;
        }
    }
}
}
