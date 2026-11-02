#include "IslandSimulator/DeerHoofSupport.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace island {
namespace {
DeerHoofSupport MakeHoofFrame(const RenderedTerrainSurface* ground,
    float flatHeight,float x,float z,float heading,float scale) {
    if (!std::isfinite(flatHeight)||!std::isfinite(x)||!std::isfinite(z)
        ||!std::isfinite(heading)||!std::isfinite(scale)||scale<=0)
        throw std::invalid_argument("Invalid deer hoof support");
    DeerHoofSupport result;
    const auto normal=ground?ground->Sample(x,z).normal:TerrainNormal{0,1,0};
    result.up={normal.x,normal.y,normal.z};
    if (!(normal.y>0)) throw std::invalid_argument("Deer hoof needs upward terrain");
    const float c=std::cos(heading),s=std::sin(heading);
    const float rise=-(normal.x*c+normal.z*s)/normal.y;
    const float length=std::hypot(1.0f,rise);
    result.forward={c/length,rise/length,s/length};
    const auto& f=result.forward;const auto& u=result.up;
    result.right={f.y*u.z-f.z*u.y,f.z*u.x-f.x*u.z,f.x*u.y-f.y*u.x};
    result.centre={x,flatHeight+DeerHoofSupport::RearRadius*scale,z};
    return result;
}
}
DeerSupportFrame MakeDeerSupportFrame(const DeerState& deer,
    const RenderedTerrainSurface* ground) {
    const auto foot=MakeHoofFrame(ground,deer.y,deer.x,deer.z,deer.headingRadians,deer.scale);
    // Move the authored ankle plane down to its actual toe radius before adapting feet.
    return {{deer.x,deer.y-(DeerHoofSupport::AnkleHeight-DeerHoofSupport::RearRadius)
                *deer.scale/foot.up.y,deer.z},foot.forward,foot.right,foot.up};
}
DeerHoofSupport FitDeerHoofSupport(const RenderedTerrainSurface* ground,
    float flatHeight,float x,float z,float heading,float scale) {
    auto result=MakeHoofFrame(ground,flatHeight,x,z,heading,scale);
    const auto& f=result.forward;const auto& u=result.up;
    if (!ground) return result;
    float support=-std::numeric_limits<float>::infinity();
    // Query vertices and edge midpoints of BOTH existing LODs. The middle longitudinal
    // station also catches a terrain-triangle/patch boundary inside the short toe.
    for (const int sides:{6,8}) for (int edge=0;edge<sides*2;++edge) {
        const int side=edge/2;
        const float a=2*std::numbers::pi_v<float>*static_cast<float>(side)/sides+DeerHoofRingPhase(sides);
        const float b=2*std::numbers::pi_v<float>*static_cast<float>(side+1)/sides+DeerHoofRingPhase(sides);
        const float cr=edge%2?(.5f*(std::cos(a)+std::cos(b))):std::cos(a);
        const float sr=edge%2?(.5f*(std::sin(a)+std::sin(b))):std::sin(a);
        for (const float toe:{-DeerHoofSupport::ToeOffset,DeerHoofSupport::ToeOffset})
            for (const float t:{0.0f,.5f,1.0f}) {
                const float along=std::lerp(DeerHoofSupport::Rear,DeerHoofSupport::Front,t);
                const float radius=std::lerp(DeerHoofSupport::RearRadius,DeerHoofSupport::FrontRadius,t);
                const auto& r=result.right;
                const float rx=(f.x*along+r.x*(toe+cr*radius)-u.x*sr*radius)*scale;
                const float ry=(f.y*along+r.y*(toe+cr*radius)-u.y*sr*radius)*scale;
                const float rz=(f.z*along+r.z*(toe+cr*radius)-u.z*sr*radius)*scale;
                support=std::max(support,ground->Height(x+rx,z+rz)-ry);
            }
    }
    result.centre.y=support;
    return result;
}
}
