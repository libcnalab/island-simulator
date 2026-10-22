#include "IslandSimulator/WindthrowLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
// how near a circle comes to the segment from a to b, past its radius
float Gap(const CircularObstacle& o,const std::array<float,2>& a,const std::array<float,2>& b) noexcept {
    const float dx=b[0]-a[0],dz=b[1]-a[1];
    const float length=dx*dx+dz*dz;
    const float t=length>1e-6f?std::clamp(((o.x-a[0])*dx+(o.z-a[1])*dz)/length,0.0f,1.0f):0.0f;
    return std::hypot(o.x-(a[0]+dx*t),o.z-(a[1]+dz*t))-o.radius;
}
}

float WindthrowPlacement::RadiusAt(const float along) noexcept {
    const float t=std::clamp(along/TrunkLength,0.0f,1.0f);
    // the buttressed foot swelling the first two metres
    return TipRadius+(BaseRadius-TipRadius)*(1.0f-t)+0.16f*std::max(0.0f,1.0f-along/2.0f);
}

std::vector<CircularObstacle> WindthrowPlacement::Obstacles() const {
    std::vector<CircularObstacle> circles;
    for(float along=0.6f;along<=TrunkLength-0.3f;along+=0.6f) {
        const auto p=At(along,0.0f);
        circles.push_back({p[0],p[1],RadiusAt(along)+0.05f});
    }
    for(float across=-PlateHalfWidth+0.4f;across<=PlateHalfWidth-0.39f;across+=0.9f) {
        const auto p=At(-0.15f,across);
        circles.push_back({p[0],p[1],0.5f});
    }
    return circles;
}

std::optional<WindthrowPlacement> WindthrowLayout::FindInWorld(
    const IslandTerrain& terrain,const std::span<const CanopyPlacement> canopy,const std::span<const CircularObstacle> obstacles,
    const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<WindthrowPlacement> best;
    float bestScore=-1e9f;
    constexpr int Headings=24;
    constexpr float Step=5.0f;
    using P=WindthrowPlacement;
    for(float gx=-250.0f;gx<=250.0f;gx+=Step) for(float gz=-250.0f;gz<=250.0f;gz+=Step) {
        // the cheap tests first: the stand round it, the points to keep away from
        int stand=0;
        for(const CanopyPlacement& tree:canopy)
            if(tree.variant<NaturalCanopyFormCount&&std::hypot(tree.x-gx,tree.z-gz)<StandRadius) ++stand;
        if(stand<MinimumStand||static_cast<float>(stand)<bestScore-1.0f) continue;
        bool clear=true;
        for(const auto& a:avoid) clear=clear&&std::hypot(a[0]-gx,a[1]-gz)>keepAway+P::TrunkLength*0.5f;
        if(!clear) continue;
        if(terrain.SignedShoreDistance(gx,gz)<MinimumShoreDistance) continue;
        const TerrainSample sample=terrain.Sample(gx,gz);
        if(sample.slopeRadians>MaximumSlope||sample.height<2.0f) continue;
        // only the obstacles that could reach the trunk, the plate or the pit
        std::vector<CircularObstacle> near;
        for(const CircularObstacle& o:obstacles)
            if(std::hypot(o.x-gx,o.z-gz)<P::TrunkLength+o.radius+4.0f) near.push_back(o);
        for(int h=0;h<Headings;++h) {
            const float angle=2.0f*std::numbers::pi_v<float>*static_cast<float>(h)/Headings;
            P placement;
            placement.x=gx;placement.z=gz;placement.dirX=std::cos(angle);placement.dirZ=std::sin(angle);
            const auto foot=placement.At(0.3f,0.0f),tip=placement.At(P::TrunkLength,0.0f);
            const auto left=placement.At(-0.2f,-P::PlateHalfWidth),right=placement.At(-0.2f,P::PlateHalfWidth);
            const auto pit=placement.At(-1.4f,0.0f);
            bool fits=true;
            for(const CircularObstacle& o:near) {
                fits=fits&&Gap(o,foot,tip)>P::BaseRadius+0.35f&&Gap(o,left,right)>0.4f
                    &&std::hypot(o.x-pit[0],o.z-pit[1])-o.radius>1.3f;
                if(!fits) break;
            }
            if(!fits) continue;
            float low=1e9f,high=-1e9f;
            bool away=true;
            for(float along=-P::PitReach;along<=P::TrunkLength&&away;along+=1.0f) {
                const auto p=placement.At(along,0.0f);
                const float ground=terrain.Height(p[0],p[1]);
                low=std::min(low,ground);high=std::max(high,ground);
                for(const auto& a:avoid) away=away&&std::hypot(a[0]-p[0],a[1]-p[1])>keepAway;
            }
            if(!away||high-low>MaximumRise) continue;
            // the deepest stand, then the gentlest lie
            const float score=static_cast<float>(stand)-0.5f*(high-low);
            if(score>bestScore) {
                bestScore=score;
                placement.groundY=terrain.Height(gx,gz);
                placement.seed=static_cast<std::uint32_t>(h*7919+static_cast<int>(gx*31.0f+gz*17.0f));
                best=placement;
            }
        }
    }
    return best;
}
}
