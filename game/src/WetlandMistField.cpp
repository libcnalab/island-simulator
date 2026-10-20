#include "IslandSimulator/WetlandMistField.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float GoldenAngle=2.39996323f;
constexpr float CentreX=IslandEnvironmentField::SeasonalWetlandX;
constexpr float CentreZ=IslandEnvironmentField::SeasonalWetlandZ;
constexpr float RadiusX=IslandEnvironmentField::SeasonalWetlandRadiusX;
constexpr float RadiusZ=IslandEnvironmentField::SeasonalWetlandRadiusZ;
}

float WetlandMistField::Extent(const float x,const float z) noexcept {
    const float dx=(x-CentreX)/RadiusX,dz=(z-CentreZ)/RadiusZ;
    return std::sqrt(dx*dx+dz*dz);
}

WetlandMistField::WetlandMistField(const IslandTerrain& terrain):terrain_(terrain) {
    // A golden-angle spiral over the wetland's inner four fifths; each puff its own size,
    // hover height, base density and breathing phase.
    for(std::size_t i=0;i<Capacity;++i) {
        const float t=(static_cast<float>(i)+0.5f)/static_cast<float>(Capacity);
        const float radial=0.82f*std::sqrt(t);
        const float angle=static_cast<float>(i)*GoldenAngle;
        baseDensity_[i]=0.58f+0.14f*static_cast<float>(i%3U);
        hover_[i]=HoverMinimum+0.09f*static_cast<float>(i%4U);
        phase_[i]=static_cast<float>(i)*0.7f;
        puffs_[i].size=3.0f+0.6f*static_cast<float>(i%4U);
        puffs_[i].active=true;
        Place(i,CentreX+std::cos(angle)*radial*RadiusX,CentreZ+std::sin(angle)*radial*RadiusZ);
    }
}

void WetlandMistField::Place(const std::size_t index,const float x,const float z) noexcept {
    MistPuff& puff=puffs_[index];
    puff.x=x;puff.z=z;
    puff.y=terrain_.Height(x,z)+hover_[index];
    puff.age=0;
    puff.density=baseDensity_[index];
}

void WetlandMistField::Update(const float deltaSeconds,const float windSeconds) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f||!std::isfinite(windSeconds)) return;
    clock_+=dt;
    for(std::size_t i=0;i<Capacity;++i) {
        MistPuff& puff=puffs_[i];
        // Downwind with the trade wind, faster in a front; a slow sideways wander; the
        // density breathes over about twenty seconds.
        const float gust=WindGust::Strength(puff.x,puff.z,windSeconds);
        const float speed=DriftSpeed+GustDrift*gust;
        puff.x+=wind::DirectionX*speed*dt;
        puff.z+=wind::DirectionZ*speed*dt+0.06f*std::sin(clock_*0.21f+phase_[i])*dt;
        puff.age+=dt;
        puff.density=baseDensity_[i]*(0.72f+0.28f*std::sin(clock_*0.31f+phase_[i]));
        const float hover=hover_[i]+0.05f*std::sin(clock_*0.17f+phase_[i]*1.3f);
        const float wantY=terrain_.Height(puff.x,puff.z)+std::clamp(hover,HoverMinimum,HoverMaximum);
        puff.y+=std::clamp(wantY-puff.y,-0.5f*dt,0.5f*dt);
        if(Extent(puff.x,puff.z)>1.0f) {
            // Out of the wetland: back in at the windward edge, on the same line.
            const float dz=std::clamp((puff.z-CentreZ)/RadiusZ,-0.75f,0.75f);
            const float dx=-wind::DirectionX*std::sqrt(std::max(0.0f,0.9f*0.9f-dz*dz));
            Place(i,CentreX+dx*RadiusX,CentreZ+dz*RadiusZ);
            ++wraps_;
        }
    }
}
}
