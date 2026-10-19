#include "IslandSimulator/FallingFruitField.hpp"
#include "IslandSimulator/CanopyProfile.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t FallingFruitField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& f:fruit_) count+=f.active?1U:0U;
    return count;
}

FallingFruit* FallingFruitField::FreeSlot() noexcept {
    for(auto& f:fruit_) if(!f.active) return &f;
    // Replace the longest-lying fruit.
    FallingFruit* slot=&fruit_[0];
    for(auto& f:fruit_) if(f.restSeconds>slot->restSeconds) slot=&f;
    return slot;
}

void FallingFruitField::UpdateCoconuts(const float seconds,const float observerX,const float observerZ) noexcept {
    if(!std::isfinite(seconds)||!std::isfinite(observerX)||!std::isfinite(observerZ)) return;
    if(palmGust_.size()!=palms_.size()) palmGust_.assign(palms_.size(),0.0f);
    for(std::size_t i=0;i<palms_.size();++i) {
        const PalmPlacement& palm=palms_[i];
        if(palm.condition!=PalmCondition::Living||(palm.variant!=1U&&palm.variant!=3U)) continue;
        if(std::hypot(palm.x-observerX,palm.z-observerZ)>CoconutRange) { palmGust_[i]=0.0f; continue; }
        const float gust=WindGust::Strength(palm.x,palm.z,seconds);
        const bool frontArrives=gust>0.85f&&palmGust_[i]<=0.85f;
        palmGust_[i]=gust;
        if(!frontArrives) continue;
        // A deterministic per-palm, per-front draw.
        const std::uint32_t front=static_cast<std::uint32_t>((seconds*WindGust::FrontSpeed-palm.x)/WindGust::FrontSpacing);
        std::uint32_t hash=static_cast<std::uint32_t>(i)*0x9e3779b9U^front*0x85ebca6bU;
        hash^=hash>>16U;hash*=0x7feb352dU;hash^=hash>>15U;
        if(static_cast<float>(hash&1023U)/1024.0f>=CoconutChance) continue;
        // The nut hangs under the crown: fruiting variants carry theirs at the frond base,
        // which the mesh puts at 9.4 m plus a variant step, scaled, and leaned downwind.
        FallingFruit* slot=FreeSlot();
        const float crown=(9.4f+static_cast<float>(palm.variant)*0.55f)*palm.scale;
        const float lean=(0.55f+static_cast<float>((palm.variant*5)%3)*0.25f)*palm.scale;
        const float angle=palm.yawRadians+static_cast<float>(hash%7U)*0.9f;
        slot->x=palm.x+std::cos(angle)*(lean+0.3f);
        slot->z=palm.z+std::sin(angle)*(lean+0.3f);
        slot->groundY=terrain_.Height(slot->x,slot->z)+0.10f;
        slot->y=std::max(palm.y+crown*0.9f,slot->groundY+1.0f);
        slot->velocityY=0;slot->restSeconds=-1.0f;slot->variant=CoconutVariant;slot->active=true;
        ++coconutDrops_;
    }
}

void FallingFruitField::Drop(const MonkeyState& monkey) noexcept {
    const CanopyPlacement& tree=monkey.selectedTree;
    if(!std::isfinite(tree.x)||!std::isfinite(tree.z)||tree.scale<=0) return;
    FallingFruit* slot=FreeSlot();
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
            if(f.restSeconds>=(f.variant==CoconutVariant?CoconutRestSeconds:RestSeconds)) f.active=false;
        }
    }
}
}
