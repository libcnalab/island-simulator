#include "IslandSimulator/StreamFishField.hpp"
#include <algorithm>
#include <cmath>

namespace island {
StreamFishField::StreamFishField(const IslandTerrain& terrain):course_(terrain) {
    const std::size_t lip=course_.CascadeIndex();
    const std::size_t last=std::min(lip+StreamCourse::PoolLength,course_.Count()-1);
    poolStart_=course_.Station(lip+1).distance+0.4f;
    poolEnd_=course_.Station(last).distance-0.2f;
    deepDistance_=course_.PoolStation().distance;
    for(std::size_t i=0;i<Capacity;++i) {
        StreamFish& fish=fish_[i];
        const float t=(static_cast<float>(i)+0.5f)/static_cast<float>(Capacity);
        fish.holdDistance=std::lerp(poolStart_,poolEnd_,t);
        fish.holdLateral=-0.6f+1.2f*static_cast<float>((i*7U+3U)%10U)/9.0f;
        fish.distance=fish.holdDistance;fish.lateral=fish.holdLateral;
        fish.depth=std::lerp(MinimumDepth,MaximumDepth,static_cast<float>((i*3U)%5U)/4.0f);
        fish.nextDartSeconds=2.0f+static_cast<float>((i*5U)%7U);
        fish.tailPhase=static_cast<float>(i)*0.9f;
        fish.active=Place(fish.distance,fish.lateral,fish.depth,fish.x,fish.y,fish.z,fish.yawRadians);
    }
}

bool StreamFishField::Place(const float distance,const float lateral,const float depth,float& x,float& y,float& z,float& headingRadians) const noexcept {
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
    const float across=std::lerp(a.halfWidth,b.halfWidth,t)*0.6f*std::clamp(lateral,-1.0f,1.0f);
    x=std::lerp(a.x,b.x,t)-dirZ*across;
    z=std::lerp(a.z,b.z,t)+dirX*across;
    const float water=std::lerp(a.water,b.water,t),bed=std::lerp(a.bed,b.bed,t);
    y=std::max(water-depth,bed+0.03f);
    // Facing upstream: against the course direction.
    headingRadians=std::atan2(-dirZ,-dirX);
    return true;
}

void StreamFishField::NoteSplash(const float x,const float z) noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)) return;
    splashX_=x;splashZ_=z;splashSeconds_=SplashSeconds;
}

void StreamFishField::Update(const float deltaSeconds,const float observerX,const float observerZ) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f) return;
    // The walker near the pool: measured against the deep station, the pool's middle. A
    // splash in the pool (a frog's dive) counts as the walker there for SplashSeconds.
    float deepX=0,deepY=0,deepZ=0,deepHeading=0;
    const bool placed=Place(deepDistance_,0.0f,0.1f,deepX,deepY,deepZ,deepHeading);
    const float reach=ScatterDistance+(poolEnd_-poolStart_)*0.5f;
    const bool walkerNear=placed&&std::isfinite(observerX)&&std::isfinite(observerZ)
        &&std::hypot(observerX-deepX,observerZ-deepZ)<reach;
    const bool splashed=placed&&splashSeconds_>0.0f&&std::hypot(splashX_-deepX,splashZ_-deepZ)<reach;
    splashSeconds_=std::max(0.0f,splashSeconds_-dt);
    const bool near=walkerNear||splashed;
    if(near&&!wasNear_) {++scatters_;if(splashed&&!walkerNear) ++splashScatters_;}
    wasNear_=near;
    for(StreamFish& fish:fish_) {
        if(!fish.active) continue;
        fish.tailPhase+=dt*(fish.dartSeconds>0.0f||fish.hiding?14.0f:5.0f);
        if(near) {
            // Scatter: straight to the deep, crowded a little apart, and hold there.
            fish.hiding=true;fish.dartSeconds=0.0f;
            const float targetDistance=deepDistance_+fish.holdLateral*0.8f;
            fish.distance+=std::clamp(targetDistance-fish.distance,-2.4f*dt,2.4f*dt);
            fish.lateral+=std::clamp(fish.holdLateral*0.5f-fish.lateral,-1.5f*dt,1.5f*dt);
            fish.depth=std::min(MaximumDepth+0.1f,fish.depth+0.3f*dt);
        } else {
            fish.hiding=false;
            // Back up from the deep: a fish driven below its usual depth rises again slowly.
            fish.depth=std::max(fish.depth-0.1f*dt,std::min(fish.depth,MaximumDepth));
            if(fish.dartSeconds>0.0f) {
                // A dart upstream, then it drifts back to its station with the current.
                fish.dartSeconds=std::max(0.0f,fish.dartSeconds-dt);
                fish.distance=std::max(poolStart_,fish.distance-(DartLength/DartSeconds)*dt);
                if(fish.riseLateral>-2.0f) fish.lateral+=std::clamp(fish.riseLateral-fish.lateral,-1.5f*dt,1.5f*dt);
            } else {
                fish.riseLateral=-3.0f;
                fish.nextDartSeconds-=dt;
                // A leaf drifting over the holding fish, within reach and not yet past it,
                // brings the dart forward: the fish rises toward the leaf's line.
                if(fish.nextDartSeconds>1.0f) {
                    for(const DriftingLeaf& leaf:leaves_) {
                        if(!leaf.active||!std::isfinite(leaf.x)||!std::isfinite(leaf.z)) continue;
                        if(std::hypot(leaf.x-fish.x,leaf.z-fish.z)>RiseReach||leaf.distance>fish.distance+0.3f) continue;
                        fish.nextDartSeconds=0.0f;fish.riseLateral=std::clamp(leaf.lateral,-0.9f,0.9f);++rises_;
                        break;
                    }
                }
                if(fish.nextDartSeconds<=0.0f) {
                    fish.dartSeconds=DartSeconds;++darts_;
                    fish.nextDartSeconds=4.0f+static_cast<float>((darts_*5U)%6U);
                }
                fish.distance+=std::clamp(fish.holdDistance-fish.distance,-ReturnSpeed*dt,ReturnSpeed*dt);
                fish.lateral+=std::clamp(fish.holdLateral+0.15f*std::sin(fish.tailPhase*0.13f)-fish.lateral,-0.4f*dt,0.4f*dt);
            }
        }
        fish.distance=std::clamp(fish.distance,poolStart_,poolEnd_);
        float heading=0;
        fish.active=Place(fish.distance,fish.lateral,fish.depth,fish.x,fish.y,fish.z,heading);
        // A resting fish faces upstream with a slow sway; a darting or hiding one swings more.
        fish.yawRadians=heading+std::sin(fish.tailPhase*0.5f)*(fish.dartSeconds>0.0f?0.25f:0.08f);
    }
}
}
