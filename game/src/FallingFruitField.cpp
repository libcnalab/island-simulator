#include "IslandSimulator/FallingFruitField.hpp"
#include "IslandSimulator/CanopyProfile.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t FallingFruitField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& f:fruit_) count+=f.active?1U:0U;
    return count;
}

void FallingFruitField::Drop(const MonkeyState& monkey) noexcept {
    const CanopyPlacement& tree=monkey.selectedTree;
    if(!std::isfinite(tree.x)||!std::isfinite(tree.z)||tree.scale<=0) return;
    FallingFruit* slot=nullptr;
    for(auto& f:fruit_) if(!f.active) { slot=&f; break; }
    if(!slot) {
        // Replace the longest-lying fruit.
        slot=&fruit_[0];
        for(auto& f:fruit_) if(f.restSeconds>slot->restSeconds) slot=&f;
    }
    // One of the four fruiting lobes (the odd clusters), hung on its outer underside like
    // the crown geometry, then the instance scale, yaw and translation.
    const int cluster=1+2*static_cast<int>(drops_%4U);
    const auto profile=MakeCanopyCrownProfile(tree.variant,cluster);
    const float angle=profile.angle+0.72f*static_cast<float>(drops_%5U)+profile.phase*2.3f;
    const float localX=profile.x+std::cos(angle)*profile.width*0.66f;
    const float localZ=profile.z+std::sin(angle)*profile.width*0.66f;
    const float localY=profile.y-profile.height*0.35f;
    const float c=std::cos(tree.yawRadians),s=std::sin(tree.yawRadians);
    slot->x=tree.x+(localX*c+localZ*s)*tree.scale;
    slot->z=tree.z+(-localX*s+localZ*c)*tree.scale;
    slot->y=tree.y+localY*tree.scale;
    slot->groundY=terrain_.Height(slot->x,slot->z)+0.06f;
    if(slot->y<=slot->groundY) slot->y=slot->groundY+0.5f;
    slot->velocityY=0;
    slot->restSeconds=-1.0f;
    slot->variant=tree.variant;
    slot->active=true;
    ++drops_;
}

void FallingFruitField::Update(const float deltaSeconds,const std::span<const MonkeyState> monkeys) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    for(std::size_t i=0;i<monkeys.size()&&i<pickTimers_.size();++i) {
        const MonkeyState& monkey=monkeys[i];
        if(monkey.behavior==MonkeyBehavior::InspectTree&&monkey.pickingFruit
            &&monkey.simulationTier!=WildlifeSimulationTier::Dormant) {
            pickTimers_[i]+=dt;
            if(pickTimers_[i]>=DropIntervalSeconds) { pickTimers_[i]=0; Drop(monkey); }
        } else {
            pickTimers_[i]=DropIntervalSeconds*0.6f;   // the first fruit comes soon after picking starts
        }
    }
    for(auto& f:fruit_) {
        if(!f.active) continue;
        if(f.restSeconds<0.0f) {
            f.velocityY-=Gravity*dt;
            f.y+=f.velocityY*dt;
            if(f.y<=f.groundY) {
                // A small bounce, then rest.
                if(f.velocityY<-2.5f) { f.y=f.groundY; f.velocityY=-f.velocityY*0.18f; }
                else { f.y=f.groundY; f.velocityY=0; f.restSeconds=0.0f; }
            }
        } else {
            f.restSeconds+=dt;
            if(f.restSeconds>=RestSeconds) f.active=false;
        }
    }
}
}
