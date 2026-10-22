#include "IslandSimulator/FlyingFishShoal.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Smooth(const float a,const float b,const float x) noexcept {
    const float t=std::clamp((x-a)/(b-a),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
}

FlyingFishShoal::FlyingFishShoal(const IslandTerrain& terrain):terrain_(terrain) {}

float FlyingFishShoal::Random() noexcept {
    seed_=seed_*1664525U+1013904223U;
    return static_cast<float>(seed_>>8U)*(1.0f/16777216.0f);
}

void FlyingFishShoal::Start(const float x,const float z,const float headingRadians,const std::size_t least) noexcept {
    originX_=x;originZ_=z;heading_=headingRadians;
    elapsed_=0.0f;active_=true;++flights_;
    fishCount_=std::clamp(3+static_cast<std::size_t>(Random()*5.99f),least,MaximumFish);
    flightSeconds_=0.0f;
    for(std::size_t i=0;i<fishCount_;++i) {
        Flyer& f=flyers_[i];
        // they burst out together, a ragged second apart, and fan a little as they go
        f.delay=i==0?0.0f:1.1f*Random();
        f.lateral=(Random()-0.5f)*7.0f;
        f.along=(Random()-0.5f)*5.0f;
        f.speed=GlideSpeed*(0.85f+0.3f*Random());
        f.duration=2.0f+2.6f*Random();
        f.peak=0.5f+0.9f*Random();
        f.curve=(Random()-0.5f)*0.16f;
        // one in three touches down part way to scull on with its tail and lift off again
        f.taxiAt=Random()<0.35f?0.42f+0.16f*Random():0.0f;
        f.scale=0.85f+0.3f*Random();
        f.launched=f.touched=f.entered=false;
        flightSeconds_=std::max(flightSeconds_,f.delay+f.duration+0.2f);
    }
}

void FlyingFishShoal::Advance(const float deltaSeconds,const float observerX,const float observerZ,const float hour) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.25f):0.0f;
    lastStep_=dt;
    if(active_) {
        elapsed_+=dt;
        if(elapsed_>flightSeconds_) active_=false;
        return;
    }
    untilNext_-=dt;
    if(untilNext_>0.0f) return;
    untilNext_=MinimumInterval+(MaximumInterval-MinimumInterval)*Random();
    // by day, for a walker near enough the sea to see them
    if(hour<6.5f||hour>18.2f) return;
    if(!std::isfinite(observerX)||!std::isfinite(observerZ)) return;
    if(terrain_.SignedShoreDistance(observerX,observerZ)>ObserverShoreReach) return;
    // out from the shore nearest the walker, a little to either side
    const float bearing=std::atan2(observerZ,observerX)+(Random()-0.5f)*0.5f;
    const float radius=terrain_.ShoreRadius(bearing)+NearestOffshore+(FarthestOffshore-NearestOffshore)*Random();
    const float x=std::cos(bearing)*radius,z=std::sin(bearing)*radius;
    if(terrain_.SignedShoreDistance(x,z)>-NearestOffshore*0.8f||terrain_.Height(x,z)>-LaunchDepth) return;
    // along the coast one way or the other, a little in or out
    const float along=bearing+(Random()<0.5f?0.5f:-0.5f)*Pi+(Random()-0.5f)*1.2f;
    Start(x,z,along);
}

void FlyingFishShoal::PreviewFlight(const float x,const float z,const float headingRadians,const float progressSeconds) noexcept {
    Start(x,z,headingRadians,6);
    elapsed_=std::max(0.0f,progressSeconds);
    // they are all up already: no spray for a launch gone by
    for(std::size_t i=0;i<fishCount_;++i) {
        Flyer& f=flyers_[i];
        f.delay=std::min(f.delay,0.5f*elapsed_);
        f.launched=true;
        // and all of them stay up through the capture
        f.duration=std::max(f.duration,elapsed_-f.delay+1.6f);
        flightSeconds_=std::max(flightSeconds_,f.delay+f.duration+0.2f);
    }
    untilNext_=60.0f;
}

FlyingFishState FlyingFishShoal::Kinematics(const std::size_t index,float& endX,float& endZ) const noexcept {
    const Flyer& f=flyers_[index];
    FlyingFishState state;
    state.scale=f.scale;
    const float tau=std::clamp(elapsed_-f.delay,0.0f,f.duration);
    // slowing a quarter over the glide, curving gently as it banks
    const float s=f.speed*(tau-0.125f*tau*tau/f.duration);
    const float heading=heading_+f.curve*tau;
    const float mean=heading_+0.5f*f.curve*tau;
    const float fx=std::cos(heading_),fz=std::sin(heading_);
    const float startX=originX_+fx*f.along-fz*f.lateral,startZ=originZ_+fz*f.along+fx*f.lateral;
    state.x=startX+std::cos(mean)*s;state.z=startZ+std::sin(mean)*s;
    endX=state.x;endZ=state.z;
    state.headingRadians=heading;
    const float u=tau/f.duration;
    // up off the water in the first tenth, gliding a little lower all the way, down at the end;
    // a touch down mid-flight if it taxis
    const auto height=[&](const float v) {
        float h=f.peak*Smooth(0.0f,0.1f,v)*(1.0f-0.35f*v)*(1.0f-Smooth(0.86f,1.0f,v));
        if(f.taxiAt>0.0f) h*=1.0f-std::exp(-((v-f.taxiAt)/0.07f)*((v-f.taxiAt)/0.07f));
        return h;
    };
    state.y=height(u)+0.02f;
    const float du=0.02f;
    const float climb=(height(std::min(1.0f,u+du))-height(std::max(0.0f,u-du)))/(2.0f*du*f.duration*f.speed);
    state.pitchRadians=std::atan(climb)+0.06f;
    state.bankRadians=std::clamp(f.curve*f.speed*0.12f,-0.5f,0.5f);
    state.wings=Smooth(0.0f,0.06f,u)*(1.0f-Smooth(0.92f,1.0f,u));
    const float raw=elapsed_-f.delay;
    state.visible=active_&&raw>=0.0f&&raw<=f.duration;
    return state;
}

void FlyingFishShoal::Spray(const float x,const float y,const float z,const int puffs,const float size) noexcept {
    ++splashCount_;
    for(int k=0;k<puffs;++k) {
        MistPuff& puff=splashes_[nextSplash_];
        nextSplash_=(nextSplash_+1U)%splashes_.size();
        const float a=2.0f*Pi*Random(),r=0.12f*Random();
        puff.x=x+std::cos(a)*r;puff.z=z+std::sin(a)*r;puff.y=y+0.05f;
        puff.size=size*(0.7f+0.6f*Random());
        puff.age=0.0f;puff.density=0.0f;puff.active=true;
    }
}
}
