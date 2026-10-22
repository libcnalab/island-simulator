#include "IslandSimulator/GlowingFungusLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <cmath>
#include <numbers>

namespace island {
namespace {
float Hash(std::uint32_t v) noexcept {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
constexpr float Pi=std::numbers::pi_v<float>;
}

std::vector<FungusCluster> GlowingFungusLayout::Place(const IslandTerrain& terrain,const WindthrowPlacement* windthrow,
                                                      const std::span<const FallenLogPlacement> logs,
                                                      const std::function<float(float,float)>& ground) {
    std::vector<FungusCluster> clusters;
    // a clump on a round piece of wood: its axis point, the across direction, the radius, and the
    // angle round from the across direction (0 one flank, pi the other, a little below level)
    const auto onWood=[&](const std::array<float,3>& axis,const float acrossX,const float acrossZ,const float radius,
                          const float angle,const std::uint32_t seed) {
        const float nx=acrossX*std::cos(angle),ny=std::sin(angle),nz=acrossZ*std::cos(angle);
        FungusCluster c;
        c.normal={{nx,ny,nz}};
        c.at={{axis[0]+nx*radius,axis[1]+ny*radius,axis[2]+nz*radius}};
        c.spread=0.07f+0.08f*Hash(seed^0x1U);
        c.count=static_cast<std::uint8_t>(6+static_cast<int>(Hash(seed^0x2U)*8.99f));
        c.seed=seed;
        clusters.push_back(c);
    };
    if(windthrow) {
        // along the giant's low flanks, in the damp just above the floor
        for(int j=0;j<8;++j) {
            const auto seed=0xf0a6U+static_cast<std::uint32_t>(j)*2654435761U;
            const float along=1.8f+1.75f*static_cast<float>(j)+0.6f*Hash(seed);
            const float side=j%2==0?0.0f:Pi;
            const float below=(side==0.0f?-1.0f:1.0f)*(0.1f+0.3f*Hash(seed^0x3U));
            onWood(windthrow->Axis(along,ground),windthrow->AcrossX(),windthrow->AcrossZ(),
                   WindthrowPlacement::RadiusAt(along)*1.04f,side+below,seed);
        }
    }
    for(std::size_t i=0;i<logs.size()&&clusters.size()<MaximumClusters;++i) {
        const FallenLogPlacement& log=logs[i];
        if(log.wetBank||terrain.SignedShoreDistance(log.x,log.z)<25.0f) continue;
        const auto seed=0x10c5U+static_cast<std::uint32_t>(i)*2246822519U;
        if(Hash(seed)>0.7f) continue;
        const float dirX=std::cos(log.yawRadians),dirZ=std::sin(log.yawRadians);
        const int clumps=1+static_cast<int>(Hash(seed^0x4U)*1.99f);
        for(int k=0;k<clumps&&clusters.size()<MaximumClusters;++k) {
            const auto s=seed^static_cast<std::uint32_t>((k+1)*0x9e37U);
            const float t=(Hash(s)-0.5f)*0.7f*log.length;
            const float side=Hash(s^0x5U)<0.5f?0.0f:Pi;
            const float below=(side==0.0f?-1.0f:1.0f)*(0.05f+0.35f*Hash(s^0x6U));
            onWood({{log.x+dirX*t,log.y,log.z+dirZ*t}},-dirZ,dirX,log.radius*1.03f,side+below,s);
        }
    }
    return clusters;
}
}
