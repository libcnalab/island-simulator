#include "IslandSimulator/TermiteNestLayout.hpp"
#include "IslandSimulator/CanopyProfile.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
float Hash(std::uint32_t v) noexcept {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

std::array<float,6> TermiteNestLayout::Bark(const CanopyPlacement& tree,const float localHeight,const float angle) {
    // the instance transform the jungle's renderer and the monkeys' climbing use
    const auto contact=SampleCanopyTrunkSurface(tree.variant,localHeight,angle);
    const float c=std::cos(tree.yawRadians),s=std::sin(tree.yawRadians);
    const auto& p=contact.position;const auto& n=contact.normal;
    return {{tree.x+tree.scale*(p[0]*c+p[2]*s),tree.y+tree.scale*p[1],tree.z+tree.scale*(-p[0]*s+p[2]*c),
             n[0]*c+n[2]*s,n[1],-n[0]*s+n[2]*c}};
}

std::vector<TermiteNest> TermiteNestLayout::Place(const std::span<const CanopyPlacement> canopy) {
    std::vector<TermiteNest> nests;
    for(std::size_t i=0;i<canopy.size()&&nests.size()<MaximumCount;++i) {
        const CanopyPlacement& tree=canopy[i];
        // grown trees only: a sapling's trunk is too thin to carry one
        if(tree.variant>=NaturalCanopyFormCount||tree.scale<0.7f) continue;
        const auto seed=static_cast<std::uint32_t>(i)*2654435761U+0x7e417U;
        if(Hash(seed)>0.13f) continue;
        TermiteNest nest;
        nest.tree=i;nest.seed=seed;
        // 2.5 to 4.1 m up, clear over a walker's head, and low on the trunk below its crown
        nest.localHeight=std::min((2.5f+1.6f*Hash(seed^0x1U))/tree.scale,0.42f*MakeCanopyTrunkProfile(tree.variant).height);
        nest.angle=6.2831853f*Hash(seed^0x2U);
        const auto bark=Bark(tree,nest.localHeight,nest.angle);
        nest.bark={{bark[0],bark[1],bark[2]}};
        // the bark's normal turned level: the nest hangs on the trunk's side
        const float len=std::hypot(bark[3],bark[5]);
        nest.normal={{len>1e-4f?bark[3]/len:1.0f,0.0f,len>1e-4f?bark[5]/len:0.0f}};
        nest.height=0.75f+0.55f*Hash(seed^0x3U);
        nest.width=nest.height*(0.66f+0.16f*Hash(seed^0x4U));
        nest.trunkRadius=CanopyTrunkRadius(tree.variant,nest.localHeight)*tree.scale;
        nests.push_back(nest);
    }
    return nests;
}
}
