#include "IslandSimulator/BeachLitterLayout.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

float BeachBottle::Length() const noexcept {
    switch(kind) {
    case Kind::Small: return 0.21f;
    case Kind::Soda: return 0.31f;
    case Kind::Crushed: return 0.30f;
    default: return 0.33f;
    }
}

float BeachBottle::Radius() const noexcept {
    switch(kind) {
    case Kind::Small: return 0.032f;
    case Kind::Soda: return 0.054f;
    case Kind::Crushed: return 0.045f;
    default: return 0.045f;
    }
}

std::vector<BeachBottle> BeachLitterLayout::Scatter(const IslandTerrain& terrain,
        const std::span<const CircularObstacle> obstacles,const std::span<const std::array<float,2>> avoid,
        const float keepAway) {
    constexpr int Steps=420;
    constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    std::vector<BeachBottle> bottles;
    const auto fits=[&](const float x,const float z,const float reach) {
        const TerrainSample sample=terrain.Sample(x,z);
        const float shore=terrain.SignedShoreDistance(x,z);
        if(sample.surface!=TerrainSurface::DrySand||sample.slopeRadians>0.30f||sample.height<0.2f
            ||shore<MinimumShore||shore>MaximumShore) return false;
        for(const auto& point:avoid) if(std::hypot(point[0]-x,point[1]-z)<keepAway) return false;
        for(const CircularObstacle& obstacle:obstacles)
            if(std::hypot(obstacle.x-x,obstacle.z-z)<obstacle.radius+reach+ObstacleClearance) return false;
        for(const BeachBottle& other:bottles)
            if(std::hypot(other.x-x,other.z-z)<reach+other.Length()*0.5f+0.08f) return false;
        return true;
    };
    for(int step=0;step<Steps;++step) {
        const auto seed=static_cast<std::uint32_t>(step)*0x9e3779b9U+0x5bd1e995U;
        const float angle=Tau*(static_cast<float>(step)+Hash(seed^0x1234567U))/static_cast<float>(Steps);
        const float inland=MinimumShore+0.3f+(MaximumShore-MinimumShore-0.6f)*Hash(seed^0x2545f491U);
        const float radius=terrain.ShoreRadius(angle)-inland;
        const float x=std::cos(angle)*radius,z=std::sin(angle)*radius;
        // more where the flotsam gathers, few on the open, scoured beaches
        const auto debris=IslandEnvironmentField::SampleCoastalDebris(x,z,terrain.SignedShoreDistance(x,z));
        const float chance=0.10f+0.26f*debris.accumulation;
        if(Hash(seed^0x68e31da4U)>=chance) continue;
        // now and then two or three washed up together
        const float group=Hash(seed^0x7f4a7c15U);
        const int count=group<0.62f?1:(group<0.88f?2:3);
        for(int i=0;i<count;++i) {
            const auto s=seed^(static_cast<std::uint32_t>(i+1)*0x27d4eb2fU);
            BeachBottle bottle;
            const float kind=Hash(s^0x165667b1U);
            bottle.kind=kind<0.42f?BeachBottle::Kind::Water:kind<0.66f?BeachBottle::Kind::Small
                :kind<0.84f?BeachBottle::Kind::Soda:BeachBottle::Kind::Crushed;
            const float offset=i==0?0.0f:0.35f+1.3f*Hash(s^0xd3a2646cU);
            const float around=Tau*Hash(s^0xfd7046c5U);
            bottle.x=x+std::cos(around)*offset;bottle.z=z+std::sin(around)*offset;
            if(!fits(bottle.x,bottle.z,bottle.Length()*0.5f)) continue;
            bottle.groundY=terrain.Height(bottle.x,bottle.z);
            bottle.yaw=Tau*Hash(s^0xb55a4f09U);
            bottle.roll=Tau*Hash(s^0x85ebca6bU);
            bottle.sink=0.08f+0.40f*Hash(s^0xc2b2ae35U);
            const float tint=Hash(s^0x27d4eb29U);
            if(bottle.kind==BeachBottle::Kind::Soda) bottle.tint=tint<0.55f?2:(tint<0.85f?0:3);
            else if(bottle.kind==BeachBottle::Kind::Water) bottle.tint=tint<0.45f?1:(tint<0.8f?0:3);
            else bottle.tint=tint<0.7f?0:3;
            bottle.labelColour=static_cast<std::uint8_t>(Hash(s^0x9e3779b1U)*4.0f);
            // the sun and the sand strip the labels: whole, torn half away, or gone
            const float label=Hash(s^0x61c88647U);
            bottle.label=label<0.35f?1.0f:(label<0.65f?0.3f+0.5f*Hash(s^0x7a3d1c5bU):0.0f);
            bottle.cap=Hash(s^0x3c6ef372U)<0.72f;
            bottle.capColour=static_cast<std::uint8_t>(Hash(s^0xa54ff53aU)*4.0f);
            bottles.push_back(bottle);
        }
    }
    return bottles;
}
}
