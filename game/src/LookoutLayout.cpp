#include "IslandSimulator/LookoutLayout.hpp"
#include "IslandSimulator/HutLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>

namespace island {
bool LookoutPlacement::OnPlatform(const float px,const float pz) const noexcept {
    if(!std::isfinite(px)||!std::isfinite(pz)) return false;
    const float dx=px-x,dz=pz-z;
    const float u=dx*dirX+dz*dirZ,v=dx*AcrossX()+dz*AcrossZ();
    return std::abs(u)<=Half&&std::abs(v)<=Half;
}

bool LookoutPlacement::OnRamp(const float px,const float pz) const noexcept {
    if(!std::isfinite(px)||!std::isfinite(pz)) return false;
    const float dx=px-x,dz=pz-z;
    const float u=dx*dirX+dz*dirZ,v=dx*AcrossX()+dz*AcrossZ();
    return u>Half&&u<=Half+RampLength+0.3f&&std::abs(v)<=RampHalfWidth;
}

float LookoutPlacement::SupportHeight(const float px,const float pz,const float terrainHeight) const noexcept {
    if(!std::isfinite(terrainHeight)) return terrainHeight;
    if(OnPlatform(px,pz)) return std::max(terrainHeight,platformY);
    if(!OnRamp(px,pz)) return terrainHeight;
    const float u=(px-x)*dirX+(pz-z)*dirZ;
    const float t=std::clamp((u-Half)/RampLength,0.0f,1.0f);
    return std::max(terrainHeight,platformY+(rampFootY-platformY)*t);
}

std::array<CircularObstacle,4> LookoutPlacement::PostObstacles() const noexcept {
    std::array<CircularObstacle,4> posts{};
    for(std::size_t i=0;i<4;++i) {
        const float u=(i&1)?Half:-Half,v=(i&2)?Half:-Half;
        posts[i]={x+dirX*u+AcrossX()*v,z+dirZ*u+AcrossZ()*v,PostRadius+0.02f};
    }
    return posts;
}

std::optional<LookoutPlacement> LookoutLayout::FindInWorld(const IslandTerrain& terrain,
        const HutPlacement& hut,const std::span<const CircularObstacle> obstacles) {
    if(!std::isfinite(hut.x)||!std::isfinite(hut.z)) return std::nullopt;
    const RenderedTerrainSurface surface(terrain);
    for(int attempt=0;attempt<18;++attempt) {
        // Fan from the hut's left side round the back: the yard and veranda stay clear.
        const float bearing=hut.yawRadians+2.0f+static_cast<float>(attempt)*0.26f;
        LookoutPlacement site;
        site.x=hut.x+std::sin(bearing)*HutDistance;
        site.z=hut.z+std::cos(bearing)*HutDistance;
        const float away=std::hypot(site.x-hut.x,site.z-hut.z);
        site.dirX=(site.x-hut.x)/away;site.dirZ=(site.z-hut.z)/away;
        if(terrain.SignedShoreDistance(site.x,site.z)<MinimumShoreDistance) continue;
        const float footX=site.x+site.dirX*(LookoutPlacement::Half+LookoutPlacement::RampLength);
        const float footZ=site.z+site.dirZ*(LookoutPlacement::Half+LookoutPlacement::RampLength);
        const float minX=std::min(site.x,footX)-LookoutPlacement::Half-0.3f;
        const float maxX=std::max(site.x,footX)+LookoutPlacement::Half+0.3f;
        const float minZ=std::min(site.z,footZ)-LookoutPlacement::Half-0.3f;
        const float maxZ=std::max(site.z,footZ)+LookoutPlacement::Half+0.3f;
        const auto relief=surface.HeightRange(minX,minZ,maxX,maxZ);
        if(relief[0]<1.0f||relief[1]-relief[0]>MaximumRelief) continue;
        bool blocked=false;
        for(const auto& obstacle:obstacles) {
            if(std::hypot(site.x-obstacle.x,site.z-obstacle.z)<obstacle.radius+LookoutPlacement::Half+1.2f
                ||std::hypot(footX-obstacle.x,footZ-obstacle.z)<obstacle.radius+1.2f) { blocked=true; break; }
            for(float along=LookoutPlacement::Half;along<LookoutPlacement::Half+LookoutPlacement::RampLength;along+=1.0f) {
                const float px=site.x+site.dirX*along,pz=site.z+site.dirZ*along;
                if(std::hypot(px-obstacle.x,pz-obstacle.z)<obstacle.radius+1.0f) { blocked=true; break; }
            }
            if(blocked) break;
        }
        if(blocked) continue;
        site.groundY=-1000.0f;
        const auto posts=site.PostObstacles();
        for(std::size_t i=0;i<4;++i) {
            site.postBottomY[i]=terrain.Height(posts[i].x,posts[i].z)-0.45f;
            site.groundY=std::max(site.groundY,terrain.Height(posts[i].x,posts[i].z));
        }
        site.platformY=site.groundY+LookoutPlacement::PlatformRise;
        site.rampFootY=terrain.Height(footX,footZ)+0.08f;
        // The ramp must stay walkable: no steeper than about 25 degrees over its run.
        if(std::atan2(site.platformY-site.rampFootY,LookoutPlacement::RampLength)>0.47f) continue;
        return site;
    }
    return std::nullopt;
}
}
