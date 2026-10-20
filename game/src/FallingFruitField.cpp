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

void FallingFruitField::SlopeAt(const float x,const float z,float& slopeX,float& slopeZ) const noexcept {
    constexpr float Probe=0.3f;
    slopeX=(terrain_.Height(x+Probe,z)-terrain_.Height(x-Probe,z))/(2*Probe);
    slopeZ=(terrain_.Height(x,z+Probe)-terrain_.Height(x,z-Probe))/(2*Probe);
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
        ShakeCoconut(palm,hash);
    }
}

void FallingFruitField::ShakeCoconut(const PalmPlacement& palm,const std::uint32_t seed) noexcept {
    if(!std::isfinite(palm.x)||!std::isfinite(palm.z)||!std::isfinite(palm.y)) return;
    // The nut hangs under the crown: fruiting variants carry theirs at the frond base,
    // which the mesh puts at 9.4 m plus a variant step, scaled, and leaned downwind.
    FallingFruit* slot=FreeSlot();
    const float crown=(9.4f+static_cast<float>(palm.variant)*0.55f)*palm.scale;
    const float lean=(0.55f+static_cast<float>((palm.variant*5)%3)*0.25f)*palm.scale;
    const float angle=palm.yawRadians+static_cast<float>(seed%7U)*0.9f;
    slot->x=palm.x+std::cos(angle)*(lean+0.3f);
    slot->z=palm.z+std::sin(angle)*(lean+0.3f);
    // Over the shallows the nut strikes the water's surface, not the seabed.
    slot->groundY=std::max(terrain_.Height(slot->x,slot->z),0.0f)+0.10f;
    slot->y=std::max(palm.y+crown*0.9f,slot->groundY+1.0f);
    slot->velocityY=0;slot->velocityX=0;slot->velocityZ=0;slot->rolling=false;slot->afloat=false;
    slot->restSeconds=-1.0f;slot->variant=CoconutVariant;slot->active=true;
    ++coconutDrops_;
}

void FallingFruitField::Drop(const MonkeyState& monkey) noexcept {
    DropFrom(monkey.selectedTree);
}

void FallingFruitField::DropFrom(const CanopyPlacement& tree) noexcept {
    if(!std::isfinite(tree.x)||!std::isfinite(tree.z)||!std::isfinite(tree.y)||tree.scale<=0) return;
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
    slot->velocityY=0;slot->velocityX=0;slot->velocityZ=0;slot->rolling=false;slot->afloat=false;
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
    for(std::size_t index=0;index<fruit_.size();++index) {
        FallingFruit& f=fruit_[index];
        if(!f.active) continue;
        if(f.restSeconds<0.0f) {
            f.velocityY-=Gravity*dt;
            f.y+=f.velocityY*dt;
            if(f.velocityX!=0.0f||f.velocityZ!=0.0f) {
                f.x+=f.velocityX*dt;f.z+=f.velocityZ*dt;
                f.groundY=terrain_.Height(f.x,f.z)+GroundOffset(f);
            }
            if(f.y<=f.groundY) {
                // A small bounce, then rest; a coconut's first strike is the thud. The strike
                // kicks the fruit a little downhill and a little to one side, so it bounces
                // away from where it hit and rolls on from there.
                if(f.velocityY<-2.5f) {
                    if(f.variant==CoconutVariant&&terrain_.Height(f.x,f.z)<0.0f) {
                        // Into the shallows: no thud, the nut bobs up and drifts with the sea.
                        f.afloat=true;f.rolling=false;f.y=0.02f;f.velocityY=0;f.restSeconds=0.0f;
                        f.velocityX=-DriftSpeed;f.velocityZ=0;
                        continue;
                    }
                    if(f.variant==CoconutVariant) landing_={f.x,f.groundY,f.z,landing_.count+1U};
                    if(f.velocityX==0.0f&&f.velocityZ==0.0f) {
                        strikeX_[index]=f.x;strikeZ_[index]=f.z;
                        float slopeX=0,slopeZ=0;SlopeAt(f.x,f.z,slopeX,slopeZ);
                        const float kick=-f.velocityY*0.12f;
                        const float side=static_cast<float>((index*7U+3U)%5U)*0.1f-0.2f;
                        f.velocityX=(-slopeX*2.4f+side)*kick;
                        f.velocityZ=(-slopeZ*2.4f-side*0.6f)*kick;
                    }
                    f.y=f.groundY; f.velocityY=-f.velocityY*0.18f;
                }
                else {
                    f.y=f.groundY; f.velocityY=0; f.restSeconds=0.0f;
                    f.rolling=std::hypot(f.velocityX,f.velocityZ)>RollStopSpeed;
                    if(!f.rolling) { f.velocityX=0;f.velocityZ=0;lastRoll_=std::hypot(f.x-strikeX_[index],f.z-strikeZ_[index]); }
                }
            }
        } else if(f.afloat) {
            // Afloat: a drift downwind (toward -x, with the fronts) with a slow sideways set and
            // a bob; the nut beaches where the ground rises above the sea and lies there as any
            // other, or is lost to the open sea after AfloatLimitSeconds.
            f.restSeconds+=dt;
            f.velocityZ=0.08f*std::sin(f.restSeconds*0.3f+static_cast<float>(index));
            f.x+=f.velocityX*dt;f.z+=f.velocityZ*dt;
            f.y=0.02f+0.03f*std::sin(f.restSeconds*2.1f+static_cast<float>(index));
            const float ground=terrain_.Height(f.x,f.z);
            if(ground>=0.0f) {
                f.afloat=false;f.velocityX=0;f.velocityZ=0;f.groundY=ground+0.10f;f.y=f.groundY;
                f.restSeconds=0.0f;++beached_;
            } else if(f.restSeconds>=AfloatLimitSeconds||std::abs(f.x)>400.0f||std::abs(f.z)>400.0f) f.active=false;
        } else if(f.rolling) {
            // Rolling: pulled on by the slope, slowed by the litter it rolls through (a drag
            // and a rolling resistance that beats the pull on gentle ground), until it stops;
            // on a steep bank the roll is cut short after RollSeconds in a hollow.
            float slopeX=0,slopeZ=0;SlopeAt(f.x,f.z,slopeX,slopeZ);
            const float speed=std::hypot(f.velocityX,f.velocityZ);
            const float resistance=speed>0.0f?std::min(RollResistance*dt,speed)/speed:0.0f;
            f.velocityX+=(-slopeX*Gravity*RollPull-f.velocityX*RollDrag)*dt-f.velocityX*resistance;
            f.velocityZ+=(-slopeZ*Gravity*RollPull-f.velocityZ*RollDrag)*dt-f.velocityZ*resistance;
            f.x+=f.velocityX*dt;f.z+=f.velocityZ*dt;
            f.groundY=terrain_.Height(f.x,f.z)+GroundOffset(f);f.y=f.groundY;
            f.restSeconds+=dt;
            if(std::hypot(f.velocityX,f.velocityZ)<=RollStopSpeed||f.restSeconds>=RollSeconds) {
                f.rolling=false;f.velocityX=0;f.velocityZ=0;
                lastRoll_=std::hypot(f.x-strikeX_[index],f.z-strikeZ_[index]);
            }
        } else {
            f.restSeconds+=dt;
            if(f.restSeconds>=(f.variant==CoconutVariant?CoconutRestSeconds:RestSeconds)) f.active=false;
        }
    }
}
}
