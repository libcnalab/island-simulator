#include "IslandSimulator/StreamDriftField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
StreamDriftField::StreamDriftField(const IslandTerrain& terrain):course_(terrain) {
    const float length=course_.Station(course_.Count()-1).distance;
    for(std::size_t i=0;i<Capacity;++i) {
        // Spread along the whole course so the stream is never empty of them.
        Launch(i,2.0f+(length-6.0f)*static_cast<float>(i)/static_cast<float>(Capacity));
    }
}

float StreamDriftField::SpeedAt(const float distance) const noexcept {
    const std::size_t lip=course_.CascadeIndex();
    const float lipDistance=course_.Station(lip).distance;
    const float dropEnd=course_.Station(lip+1).distance;
    const float poolEnd=course_.Station(std::min(lip+1+StreamCourse::PoolLength,course_.Count()-1)).distance;
    if(distance<lipDistance-LipRun) return ReachSpeed;
    if(distance<lipDistance) return std::lerp(ReachSpeed,LipSpeed,(distance-(lipDistance-LipRun))/LipRun);
    if(distance<dropEnd) return DropSpeed;
    if(distance<poolEnd) {
        // Fast out of the drop, then the pool's slack, then the reach speed again downstream.
        const float t=(distance-dropEnd)/std::max(poolEnd-dropEnd,0.1f);
        return t<0.3f?std::lerp(DropSpeed*0.5f,PoolSpeed,t/0.3f):std::lerp(PoolSpeed,ReachSpeed,(t-0.3f)/0.7f);
    }
    return ReachSpeed;
}

bool StreamDriftField::Surface(const float distance,const float lateral,float& x,float& y,float& z,float& headingRadians) const noexcept {
    if(!std::isfinite(distance)||distance<0.0f) return false;
    const std::size_t count=course_.Count();
    std::size_t i=0;
    while(i+2<count&&course_.Station(i+1).distance<distance) ++i;
    const StreamStation& a=course_.Station(i);
    const StreamStation& b=course_.Station(i+1);
    if(distance>b.distance) return false;
    const float span=std::max(b.distance-a.distance,0.001f);
    const float t=std::clamp((distance-a.distance)/span,0.0f,1.0f);
    const float dirX=(b.x-a.x)/span,dirZ=(b.z-a.z)/span;
    const float across=std::lerp(a.halfWidth,b.halfWidth,t)*0.72f*std::clamp(lateral,-1.0f,1.0f);
    x=std::lerp(a.x,b.x,t)-dirZ*across;
    z=std::lerp(a.z,b.z,t)+dirX*across;
    // Over the drop the surface falls with the water; elsewhere the stations' water level.
    y=std::lerp(a.water,b.water,t)+SurfaceLift;
    headingRadians=std::atan2(dirZ,dirX);
    return true;
}

std::size_t StreamDriftField::SeaCount() const noexcept {
    std::size_t count=0;
    for(const auto& leaf:leaves_) count+=leaf.active&&leaf.atSea?1U:0U;
    return count;
}

void StreamDriftField::Launch(const std::size_t index,const float distance) noexcept {
    DriftingLeaf& leaf=leaves_[index];
    ++launches_;
    leaf.atSea=false;leaf.seaSeconds=0;
    leaf.distance=distance;
    leaf.lateral=-0.8f+1.6f*static_cast<float>((launches_*7U+index*3U)%11U)/10.0f;
    leaf.spin=static_cast<float>((launches_*5U+index)%13U)*0.48f;
    leaf.variant=static_cast<std::uint8_t>((launches_+index)%3U);
    leaf.active=Surface(leaf.distance,leaf.lateral,leaf.x,leaf.y,leaf.z,leaf.yawRadians);
    leaf.yawRadians+=leaf.spin;
}

bool StreamDriftField::Adopt(const float distance,const float lateral,const std::uint8_t variant) noexcept {
    if(!std::isfinite(distance)||!std::isfinite(lateral)) return false;
    // A free slot if there is one; else the leaf furthest along (longest at sea, or
    // nearest the mouth), which was about to be launched again anyway.
    std::size_t slot=leaves_.size();
    for(std::size_t index=0;index<leaves_.size();++index) if(!leaves_[index].active) {slot=index;break;}
    if(slot==leaves_.size()) {
        slot=0;
        for(std::size_t index=1;index<leaves_.size();++index) {
            const DriftingLeaf& a=leaves_[index];const DriftingLeaf& b=leaves_[slot];
            const bool further=a.atSea!=b.atSea?a.atSea:(a.atSea?a.seaSeconds>b.seaSeconds:a.distance>b.distance);
            if(further) slot=index;
        }
    }
    DriftingLeaf& leaf=leaves_[slot];
    Launch(slot,std::max(0.0f,distance));
    leaf.lateral=std::clamp(lateral,-0.9f,0.9f);
    leaf.variant=variant;
    leaf.active=Surface(leaf.distance,leaf.lateral,leaf.x,leaf.y,leaf.z,leaf.yawRadians);
    if(!leaf.active) return false;
    ++adoptions_;
    return true;
}

void StreamDriftField::Update(const float deltaSeconds) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    for(std::size_t index=0;index<leaves_.size();++index) {
        DriftingLeaf& leaf=leaves_[index];
        if(!leaf.active) { Launch(index,1.0f); continue; }
        if(leaf.atSea) {
            // On the lagoon: downwind with a slow set and a bob, until it is lost.
            leaf.seaSeconds+=dt;
            leaf.x-=SeaDriftSpeed*dt;
            leaf.z+=0.05f*std::sin(leaf.seaSeconds*0.4f+leaf.spin)*dt;
            leaf.y=0.02f+0.02f*std::sin(leaf.seaSeconds*1.7f+leaf.spin);
            leaf.spin+=0.15f*dt;leaf.yawRadians+=0.15f*dt;
            if(leaf.seaSeconds>=SeaSeconds||!std::isfinite(leaf.x)) Launch(index,1.0f+static_cast<float>(launches_%5U)*0.6f);
            continue;
        }
        leaf.speed=SpeedAt(leaf.distance);
        leaf.distance+=leaf.speed*dt;
        // A slow wander across the channel and a slow spin, both faster in quick water.
        const float stir=0.3f+leaf.speed;
        leaf.lateral=std::clamp(leaf.lateral+std::sin(leaf.distance*0.9f+leaf.spin)*0.25f*stir*dt,-0.95f,0.95f);
        leaf.spin+=0.35f*stir*dt;
        float heading=0;
        if(!Surface(leaf.distance,leaf.lateral,leaf.x,leaf.y,leaf.z,heading)) {
            // Out of the mouth: the leaf is counted and carries on across the lagoon.
            ++passes_;
            leaf.atSea=true;leaf.seaSeconds=0;leaf.y=0.02f;
            continue;
        }
        leaf.yawRadians=heading+leaf.spin;
    }
}
}
