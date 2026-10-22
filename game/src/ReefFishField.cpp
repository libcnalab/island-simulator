#include "IslandSimulator/ReefFishField.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
float Unit(std::uint32_t value) noexcept {
    value^=value>>16;value*=0x7feb352dU;value^=value>>15;value*=0x846ca68bU;value^=value>>16;
    return static_cast<float>(value&0xffffffU)/16777215.0f;
}
}

ReefFishField::ReefFishField(const std::span<const ReefHabitatPlacement> habitat) {
    // the living coral heads (not the bare rock form), deep enough for a cloud above them
    for(std::size_t i=0;i<habitat.size();++i) {
        const auto& coral=habitat[i];
        if(coral.variant>=3U||coral.scale<0.6f) continue;
        const float top=coral.y+0.55f*coral.scale;
        if(top>-0.75f) continue;
        Head head;
        head.x=coral.x;head.y=coral.y;head.z=coral.z;head.top=top;
        head.seed=static_cast<std::uint32_t>(i)*2654435761U+0x9e3779b9U;
        head.tangs=Unit(head.seed^0x5bd1e995U)<0.3f;
        heads_.push_back(head);
    }
}

std::size_t ReefFishField::HidingCount() const noexcept {
    std::size_t hiding=0;
    for(std::size_t s=0;s<chosenCount_;++s) hiding+=hide_[s]>0.5f?1U:0U;
    return hiding;
}

void ReefFishField::Update(const float deltaSeconds,const float observerX,const float observerY,const float observerZ) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.25f):0.0f;
    if(!std::isfinite(observerX)||!std::isfinite(observerZ)) {count_=0;chosenCount_=0;return;}
    clock_+=dt;
    // Choose again each second: the nearest heads within reach, the ones kept keeping their slots
    // (and their hiding) so a cloud does not jump to another head.
    chooseClock_-=dt;
    if(chooseClock_<0.0f) {
        chooseClock_=1.0f;
        std::array<std::size_t,HeadCount> best{};
        std::array<float,HeadCount> bestDistance{};
        std::size_t found=0;
        for(std::size_t i=0;i<heads_.size();++i) {
            const float dx=heads_[i].x-observerX,dz=heads_[i].z-observerZ;
            const float d=dx*dx+dz*dz;
            if(d>Reach*Reach) continue;
            if(found<HeadCount) {best[found]=i;bestDistance[found]=d;++found;}
            else {
                std::size_t worst=0;
                for(std::size_t k=1;k<HeadCount;++k) if(bestDistance[k]>bestDistance[worst]) worst=k;
                if(d<bestDistance[worst]) {best[worst]=i;bestDistance[worst]=d;}
            }
        }
        std::array<std::size_t,HeadCount> next{};
        std::array<float,HeadCount> nextHide{},nextHidden{};
        for(std::size_t k=0;k<found;++k) {
            next[k]=best[k];
            for(std::size_t s=0;s<chosenCount_;++s)
                if(chosen_[s]==best[k]) {nextHide[k]=hide_[s];nextHidden[k]=hiddenFor_[s];}
        }
        chosen_=next;hide_=nextHide;hiddenFor_=nextHidden;chosenCount_=found;
    }
    count_=0;
    for(std::size_t s=0;s<chosenCount_;++s) {
        const Head& head=heads_[chosen_[s]];
        // the swimmer near the head sends the cloud into the coral; HideSeconds after it has
        // gone the fish come out again
        const float near=std::hypot(head.x-observerX,head.top+0.5f-observerY,head.z-observerZ);
        if(near<HideDistance) hiddenFor_[s]=HideSeconds;
        else hiddenFor_[s]=std::max(0.0f,hiddenFor_[s]-dt);
        const float target=hiddenFor_[s]>0.0f?1.0f:0.0f;
        hide_[s]+=std::clamp(target-hide_[s],-dt*0.6f,dt*2.5f);
        const std::size_t members=head.tangs?3U:FishPerHead;
        for(std::size_t j=0;j<members&&count_<fish_.size();++j) {
            const std::uint32_t seed=head.seed+static_cast<std::uint32_t>(j)*0x27d4eb2dU;
            const float phase=Unit(seed)*2.0f*std::numbers::pi_v<float>;
            const float direction=Unit(seed^0x68e31da4U)<0.5f?-1.0f:1.0f;
            const float rate=(0.30f+0.25f*Unit(seed^0x1b873593U))*direction;
            const float hidden=hide_[s];
            const float radius=(0.22f+0.38f*Unit(seed^0x85ebca6bU))*(1.0f-0.65f*hidden);
            const float lift=(0.30f+0.45f*Unit(seed^0xc2b2ae35U))*(1.0f-0.85f*hidden)
                +0.08f*std::sin(clock_*0.9f+phase);
            const float angle=phase+clock_*rate;
            FishState& fish=fish_[count_++];
            fish.x=head.x+std::cos(angle)*radius;
            fish.z=head.z+std::sin(angle)*radius;
            fish.y=std::min(head.top+lift,-0.12f);
            // swimming round the circle: the heading along it
            fish.headingRadians=angle+direction*0.5f*std::numbers::pi_v<float>;
            fish.pitchRadians=0.12f*std::sin(clock_*1.3f+phase);
            fish.bankRadians=-direction*0.25f;
            fish.speed=std::abs(rate)*radius+0.5f*hidden;
            fish.bodyWave=clock_*(9.0f+4.0f*hidden)+phase;
            fish.finFan=0.7f*(1.0f-hidden);
            fish.finPhase=clock_*6.0f+phase;
            fish.flash=0.0f;
            fish.scale=head.tangs?0.17f+0.03f*Unit(seed^0x9e3779b9U):0.12f+0.03f*Unit(seed^0x9e3779b9U);
            fish.simulationTier=WildlifeSimulationTier::Full;
            fish.schoolIndex=0U;
            fish.colorVariant=head.tangs?TangVariant:ChromisVariant;
        }
    }
}
}
