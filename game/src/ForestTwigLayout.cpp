#include "IslandSimulator/ForestTwigLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
float Hash01(const std::uint32_t seed) noexcept {
    std::uint32_t h=seed*0x9e3779b9U;
    h^=h>>16U;h*=0x7feb352dU;h^=h>>15U;h*=0x846ca68bU;h^=h>>16U;
    return static_cast<float>(h&0xffffffU)/16777216.0f;
}
}

bool ForestTwigLayout::IsForestFloor(const IslandTerrain& terrain,const float x,const float z) noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)) return false;
    const TerrainSample sample=terrain.Sample(x,z);
    const bool ground=sample.surface==TerrainSurface::InteriorGround||sample.surface==TerrainSurface::CoastalGround;
    if(!ground||sample.height<=0.3f||sample.signedShoreDistance<MinimumShoreDistance||sample.slopeRadians>=0.5f) return false;
    if(std::abs(x-VegetationLayout::PrimaryPathCenterX(z))<PathClearance) return false;
    if(std::abs(z-StreamRoute::CentreZ(x))<StreamRoute::HalfWidth(x)+StreamClearance) return false;
    return true;
}

ForestTwigLayout::ForestTwigLayout(const IslandTerrain& terrain,const JungleLayout& jungle) {
    const auto& trees=jungle.Canopy();
    for(std::size_t t=0;t<trees.size()&&twigs_.size()<Capacity;++t) {
        const auto& tree=trees[t];
        for(std::uint32_t k=0;k<2U&&twigs_.size()<Capacity;++k) {
            const std::uint32_t seed=static_cast<std::uint32_t>(t)*4U+k;
            const float angle=Hash01(seed)*6.2831853f;
            const float distance=1.2f+Hash01(seed+1U)*1.8f;
            const float x=tree.x+std::cos(angle)*distance,z=tree.z+std::sin(angle)*distance;
            if(!IsForestFloor(terrain,x,z)) continue;
            ForestTwigPlacement twig;
            twig.x=x;twig.z=z;twig.y=terrain.Height(x,z);
            twig.yawRadians=Hash01(seed+2U)*6.2831853f;
            twig.length=0.45f+Hash01(seed+3U)*0.5f;
            twig.radius=0.018f+Hash01(seed+5U)*0.014f;
            twig.bend=0.08f+Hash01(seed+7U)*0.18f;
            twig.variant=static_cast<std::uint8_t>(seed%3U);
            twigs_.push_back(twig);
        }
    }
}
}
