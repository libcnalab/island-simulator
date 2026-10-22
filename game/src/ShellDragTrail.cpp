#include "IslandSimulator/ShellDragTrail.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::size_t ShellDragTrail::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& segment:segments_) count+=segment.active?1U:0U;
    return count;
}

void ShellDragTrail::Update(const float deltaSeconds,const std::span<const HermitCrabState> crabs) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(DragSegment& segment:segments_) {
        if(!segment.active) continue;
        segment.age+=dt;
        segment.density=1.0f-segment.age/LifeSeconds;
        if(segment.age>=LifeSeconds) segment.active=false;
    }
    for(std::size_t i=0;i<crabs.size()&&i<trackers_.size();++i) {
        const HermitCrabState& crab=crabs[i];
        Tracker& tracker=trackers_[i];
        if(!std::isfinite(crab.x)||!std::isfinite(crab.z)) { tracker.valid=false; continue; }
        const bool dragging=crab.behavior!=HermitCrabBehavior::Pause&&crab.speed>MinimumSpeed
            &&crab.simulationTier!=WildlifeSimulationTier::Dormant;
        if(!dragging||!tracker.valid) { tracker.x=crab.x;tracker.z=crab.z;tracker.valid=true; continue; }
        const float moved=std::hypot(crab.x-tracker.x,crab.z-tracker.z);
        if(moved<SegmentLength) continue;
        if(moved>1.0f) { tracker.x=crab.x;tracker.z=crab.z; continue; }   // a jump, not a drag
        DragSegment* slot=&segments_[0];
        for(DragSegment& segment:segments_) {
            if(!segment.active) { slot=&segment; break; }
            if(segment.age>slot->age) slot=&segment;
        }
        slot->x0=tracker.x;slot->z0=tracker.z;slot->x1=crab.x;slot->z1=crab.z;
        slot->age=0;slot->density=1;slot->active=true;
        tracker.x=crab.x;tracker.z=crab.z;
        ++laid_;
    }
}

void CrocodileDragTrail::Update(const float deltaSeconds,const std::span<const CrocodileState> crocodiles,
                                const std::span<const bool> dryGround) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(DragSegment& segment:segments_) {
        if(!segment.active) continue;
        segment.age+=dt;
        // fresh and dark for a minute, then the sand dries and the wind softens it
        segment.density=1.0f-std::clamp((segment.age-60.0f)/(LifeSeconds-60.0f),0.0f,1.0f);
        if(segment.age>=LifeSeconds) segment.active=false;
    }
    for(std::size_t i=0;i<crocodiles.size()&&i<trackers_.size();++i) {
        const CrocodileState& crocodile=crocodiles[i];
        Tracker& tracker=trackers_[i];
        if(!std::isfinite(crocodile.x)||!std::isfinite(crocodile.z)) { tracker.valid=false; continue; }
        const bool dragging=i<dryGround.size()&&dryGround[i]&&crocodile.submerged<0.1f&&crocodile.speed>MinimumSpeed;
        if(!dragging||!tracker.valid) { tracker.x=crocodile.x;tracker.z=crocodile.z;tracker.valid=true; continue; }
        const float moved=std::hypot(crocodile.x-tracker.x,crocodile.z-tracker.z);
        if(moved<SegmentLength) continue;
        if(moved>2.0f) { tracker.x=crocodile.x;tracker.z=crocodile.z; continue; }   // a jump, not a drag
        DragSegment* slot=&segments_[0];
        for(DragSegment& segment:segments_) {
            if(!segment.active) { slot=&segment; break; }
            if(segment.age>slot->age) slot=&segment;
        }
        *slot={tracker.x,tracker.z,crocodile.x,crocodile.z,0.0f,1.0f,true,HalfWidth*crocodile.scale};
        tracker.x=crocodile.x;tracker.z=crocodile.z;
        ++laid_;
    }
}
}
