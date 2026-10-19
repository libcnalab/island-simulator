#include "IslandSimulator/BridgeLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamCourse.hpp"
#include <algorithm>
#include <cmath>

namespace island {
float BridgePlacement::DeckHeight(const float along) const noexcept {
    if(!std::isfinite(along)||span<=0.001f) return ay;
    const float t=std::clamp(along/span,0.0f,1.0f);
    // A rope walkway hangs below its anchors: linear between the ends, minus a parabolic sag.
    return ay+(by-ay)*t-Sag*4.0f*t*(1.0f-t);
}

bool BridgePlacement::OnDeck(const float x,const float z) const noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)) return false;
    const float dx=x-ax,dz=z-az;
    const float along=dx*dirX+dz*dirZ,across=dx*AcrossX()+dz*AcrossZ();
    return along>=-0.25f&&along<=span+0.25f&&std::abs(across)<=DeckHalfWidth;
}

float BridgePlacement::SupportHeight(const float x,const float z,const float terrainHeight) const noexcept {
    if(!std::isfinite(terrainHeight)||!OnDeck(x,z)) return terrainHeight;
    const float along=(x-ax)*dirX+(z-az)*dirZ;
    return std::max(terrainHeight,DeckHeight(std::clamp(along,0.0f,span)));
}

std::optional<BridgePlacement> BridgeLayout::FindInWorld(const IslandTerrain& terrain,
                                                         const StreamCourse& course) {
    const std::size_t count=course.Count();
    if(count<20) return std::nullopt;
    // Start well below the plunge pool and stop short of the mouth reach.
    const std::size_t first=std::max(course.CascadeIndex()+StreamCourse::PoolLength+6,count*3/5);
    for(std::size_t i=first;i+2<count-8;i+=2) {
        const auto& a=course.Station(i);const auto& b=course.Station(i+1);
        const float tx=b.x-a.x,tz=b.z-a.z,length=std::hypot(tx,tz);
        if(length<0.01f) continue;
        const float nx=tz/length,nz=-tx/length;      // across the channel
        const float offset=a.halfWidth+BankOffset;
        BridgePlacement bridge;
        bridge.ax=a.x-nx*offset;bridge.az=a.z-nz*offset;
        bridge.bx=a.x+nx*offset;bridge.bz=a.z+nz*offset;
        const TerrainSample left=terrain.Sample(bridge.ax,bridge.az);
        const TerrainSample right=terrain.Sample(bridge.bx,bridge.bz);
        if(left.height<a.water+MinimumBankFreeboard||right.height<a.water+MinimumBankFreeboard) continue;
        if(std::abs(left.height-right.height)>MaximumBankDifference) continue;
        if(left.slopeRadians>MaximumBankSlope||right.slopeRadians>MaximumBankSlope) continue;
        if(left.IsSubmerged()||right.IsSubmerged()) continue;
        bridge.ay=left.height+0.10f;bridge.by=right.height+0.10f;
        bridge.dirX=nx;bridge.dirZ=nz;bridge.span=2.0f*offset;
        bridge.station=i;
        // The sagging middle must still clear the water by a plank's thickness and more.
        if(bridge.DeckHeight(bridge.span*0.5f)<a.water+0.18f) continue;
        return bridge;
    }
    return std::nullopt;
}
}
