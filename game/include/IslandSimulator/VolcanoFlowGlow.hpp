#pragma once
#include "IslandSimulator/VolcanoSite.hpp"
#include <vector>

namespace island {
struct VolcanoGlowPoint {float x,y,z,weight;};

// A small light decal on the existing cone, not another terrain surface. Built
// once; graded cells follow the lava crest instead of depth-clipping a floating fan.
inline std::vector<VolcanoGlowPoint> BuildVolcanoFlowGlow() {
    using V=VolcanoSite;
    constexpr int AcrossCells=12;
    const float start=V::RimRadius+.05f,end=V::RimRadius+V::FlowLength+3;
    const int alongCells=static_cast<int>(std::ceil(end-start));
    const auto smooth=[](float t){t=std::clamp(t,0.0f,1.0f);return t*t*(3-2*t);};
    const auto at=[&](int along,int across) {
        // Match the cone mesh: closer radial stations around its rounded rim.
        const float r=std::lerp(start,end,std::pow(static_cast<float>(along)/alongCells,1.35f));
        const float linear=-1+2*static_cast<float>(across)/AcrossCells;
        // Concentrate samples on the curved lava crest; the dim outer skirt is gentler.
        const float side=linear*std::abs(linear);
        const float width=3.2f+.08f*(r-V::RimRadius);
        const float theta=V::FlowCentre(r)+side*width/r;
        const float x=V::X+std::cos(theta)*r,z=V::Z+std::sin(theta)*r;
        const float falloff=(1-side*side)*(1-side*side);
        const float ends=smooth((r-start)/1.5f)*smooth((end-r)/3);
        return VolcanoGlowPoint{x,V::SurfaceY(x,z)+.08f,z,falloff*ends};
    };
    std::vector<VolcanoGlowPoint> result;
    result.reserve(static_cast<std::size_t>(alongCells*AcrossCells*6));
    for(int i=0;i<alongCells;++i)for(int j=0;j<AcrossCells;++j) {
        const auto a=at(i,j),b=at(i,j+1),c=at(i+1,j+1),d=at(i+1,j);
        for(const auto p:{a,b,c,a,c,d})result.push_back(p);
    }
    return result;
}
}
