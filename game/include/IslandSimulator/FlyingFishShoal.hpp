#pragma once
#include "IslandSimulator/CascadeMistField.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

struct FlyingFishState {
    float x=0,y=0,z=0;                    // world; y on or over the sea
    float headingRadians=0,pitchRadians=0,bankRadians=0;
    float wings=0;                        // 0 the pectoral fins folded along the body, 1 spread
    float scale=1;
    bool visible=false;                   // out of the water this frame
};

/** Flying fish off the shore: now and then by day a handful burst out of the sea together some
 *  out over the open water past the shallows off where the walker is near the coast (or swimming),
 *  glide low over the waves on their
 *  spread pectoral fins for a few seconds and forty-odd metres, some touching down mid-flight to
 *  scull on with their tails and lift off again, and drop back in. Each launch, touch and entry
 *  throws up a little spray (MistPuffs for the mist renderer). Renderer-independent and
 *  deterministic. */
class FlyingFishShoal final {
public:
    static constexpr std::size_t MaximumFish=8,SplashCapacity=48;
    static constexpr float MinimumInterval=24.0f,MaximumInterval=62.0f;   // seconds between flights
    static constexpr float GlideSpeed=15.0f;                              // metres a second
    static constexpr float NearestOffshore=55.0f,FarthestOffshore=140.0f; // the launch, out over open water past the shallows
    static constexpr float LaunchDepth=2.2f;                              // at least this deep under it
    static constexpr float ObserverShoreReach=140.0f;                     // flights only for a walker this near the sea
    static constexpr float SplashSeconds=0.9f;

    explicit FlyingFishShoal(const IslandTerrain& terrain);
    /** `hour` keeps the flights to the day; `seaHeight` is the surface's height under (x, z) now. */
    template<class SeaHeight>
    void Update(const float deltaSeconds,const float observerX,const float observerZ,const float hour,const SeaHeight& seaHeight) noexcept {
        Advance(deltaSeconds,observerX,observerZ,hour);
        Pose([&](const float x,const float z) {return static_cast<float>(seaHeight(x,z));});
    }
    /** Starts a flight of six or more launching at (x, z) toward `headingRadians`,
     *  `progressSeconds` into it, and holds the next one off for a minute (the QA views). */
    void PreviewFlight(float x,float z,float headingRadians,float progressSeconds) noexcept;

    [[nodiscard]] std::span<const FlyingFishState> Fish() const noexcept {return {states_.data(),fishCount_};}
    [[nodiscard]] std::span<const MistPuff> Splashes() const noexcept {return splashes_;}
    [[nodiscard]] std::uint32_t FlightCount() const noexcept {return flights_;}
    [[nodiscard]] std::uint32_t SplashCount() const noexcept {return splashCount_;}
    [[nodiscard]] bool Flying() const noexcept {return active_;}
    /** The flight's launch point and heading (valid while one is flying). */
    [[nodiscard]] std::array<float,3> Launch() const noexcept {return {originX_,originZ_,heading_};}

private:
    struct Flyer {
        float delay=0,lateral=0,along=0,speed=GlideSpeed,duration=3,peak=0.8f,curve=0,taxiAt=0,scale=1;
        bool launched=false,touched=false,entered=false;
    };
    void Advance(float deltaSeconds,float observerX,float observerZ,float hour) noexcept;
    template<class SeaHeight>
    void Pose(const SeaHeight& seaHeight) noexcept {
        for(std::size_t i=0;i<fishCount_;++i) {
            float x=0,z=0;
            states_[i]=Kinematics(i,x,z);
            const float sea=seaHeight(states_[i].x,states_[i].z);
            states_[i].y+=sea;
            // the spray where a fish breaks the surface: on launch, on a touch, on entry
            Flyer& f=flyers_[i];
            const float tau=elapsed_-f.delay;
            if(!f.launched&&tau>=0.0f) {f.launched=true;Spray(states_[i].x,sea,states_[i].z,3,0.22f);}
            if(f.taxiAt>0.0f&&!f.touched&&tau>=f.taxiAt*f.duration) {f.touched=true;Spray(states_[i].x,sea,states_[i].z,2,0.16f);}
            if(!f.entered&&tau>=f.duration) {f.entered=true;Spray(x,sea,z,4,0.26f);}
        }
        for(MistPuff& puff:splashes_) {
            if(!puff.active) continue;
            puff.age+=lastStep_;
            if(puff.age>SplashSeconds) {puff.active=false;puff.density=0.0f;continue;}
            puff.y+=lastStep_*(0.9f-1.6f*puff.age);
            puff.size+=lastStep_*0.35f;
            puff.density=std::min(1.0f,puff.age*10.0f)*(1.0f-puff.age/SplashSeconds)*0.9f;
        }
    }
    [[nodiscard]] FlyingFishState Kinematics(std::size_t index,float& endX,float& endZ) const noexcept;
    void Spray(float x,float y,float z,int puffs,float size) noexcept;
    void Start(float x,float z,float headingRadians,std::size_t least=3) noexcept;
    [[nodiscard]] float Random() noexcept;

    const IslandTerrain& terrain_;
    std::array<Flyer,MaximumFish> flyers_{};
    std::array<FlyingFishState,MaximumFish> states_{};
    std::array<MistPuff,SplashCapacity> splashes_{};
    std::size_t fishCount_=0,nextSplash_=0;
    float originX_=0,originZ_=0,heading_=0,elapsed_=0,flightSeconds_=0,untilNext_=12.0f,lastStep_=0;
    bool active_=false;
    std::uint32_t seed_=0x51f7e3u,flights_=0,splashCount_=0;
};
}
