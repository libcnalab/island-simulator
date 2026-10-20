#include "IslandSimulator/BlownSandField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PrevailingWind.hpp"
#include "IslandSimulator/WindGust.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Tau=2.0f*std::numbers::pi_v<float>;
}

BlownSandField::BlownSandField(const IslandTerrain& terrain):terrain_(terrain) {
    // Around the shore every three degrees, a few metres above the wet line: dry sand on
    // the exposed (windward and coastal) beaches is a source.
    constexpr int Bearings=120;
    constexpr std::array<float,3> Inland{{2.5f,5.0f,8.0f}};
    for(int bearing=0;bearing<Bearings&&sources_.size()<MaximumSources;++bearing) {
        const float angle=static_cast<float>(bearing)*Tau/static_cast<float>(Bearings);
        const float shore=terrain_.ShoreRadius(angle);
        for(const float inland:Inland) {
            const float x=std::cos(angle)*(shore-inland),z=std::sin(angle)*(shore-inland);
            const TerrainSample sample=terrain_.Sample(x,z);
            if(sample.surface!=TerrainSurface::DrySand||sample.height<0.25f) continue;
            if(wind::Exposure(x,z,sample.signedShoreDistance)<MinimumExposure) continue;
            sources_.push_back({x,sample.height,z,
                std::fmod(static_cast<float>(bearing)*0.618034f+inland*0.37f,1.0f)});
            if(sources_.size()>=MaximumSources) break;
        }
    }
    clocks_.assign(sources_.size(),0.0f);
}

std::size_t BlownSandField::ActiveCount() const noexcept {
    std::size_t count=0;
    for(const auto& puff:puffs_) count+=puff.active?1U:0U;
    return count;
}

void BlownSandField::Update(const float deltaSeconds,const float windSeconds,
                            const float cameraX,const float cameraZ) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    if(dt<=0.0f||!std::isfinite(windSeconds)) return;
    for(DustPuff& puff:puffs_) {
        if(!puff.active) continue;
        puff.age+=dt;
        if(puff.age>=LifeSeconds) { puff.active=false; continue; }
        const float t=puff.age/LifeSeconds;
        // Downwind with the trade wind, faster in the front itself; a little lift, swelling
        // as it thins.
        const float gust=WindGust::Strength(puff.x,puff.z,windSeconds);
        puff.x+=wind::DirectionX*DriftSpeed*(0.5f+0.5f*gust)*dt;
        puff.z+=wind::DirectionZ*DriftSpeed*(0.5f+0.5f*gust)*dt;
        puff.y+=RiseSpeed*(1.0f-0.5f*t)*dt;
        puff.size+=0.30f*dt;
        puff.density=t<0.25f?t/0.25f:1.0f-(t-0.25f)/0.75f;
    }
    const bool cameraValid=std::isfinite(cameraX)&&std::isfinite(cameraZ);
    for(std::size_t i=0;i<sources_.size();++i) {
        const BlownSandSource& source=sources_[i];
        if(cameraValid&&std::hypot(source.x-cameraX,source.z-cameraZ)>Reach) { clocks_[i]=0.0f; continue; }
        const float gust=WindGust::Strength(source.x,source.z,windSeconds);
        if(gust<GustThreshold) { clocks_[i]=0.0f; continue; }
        clocks_[i]+=dt;
        if(clocks_[i]<ReleaseIntervalSeconds*(0.8f+0.4f*source.phase)) continue;
        clocks_[i]=0.0f;
        for(DustPuff& puff:puffs_) {
            if(puff.active) continue;
            puff.x=source.x+wind::DirectionX*0.2f*source.phase;
            puff.y=source.y+0.06f;
            puff.z=source.z+(source.phase-0.5f)*0.6f;
            puff.size=0.16f+0.08f*source.phase;
            puff.age=0.0f;puff.density=0.0f;puff.active=true;
            ++releases_;
            break;
        }
    }
}
}
