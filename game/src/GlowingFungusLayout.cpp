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
                                                      const std::function<float(float,float)>& ground,
                                                      const std::span<const StandingSnagPlacement> snags) {
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
            // on the log's own surface: its axis bowed sideways and arching, its girth flared
            // at the butt and swollen at the knots (NaturalPropRenderer's log, which the layout's
            // straight radius had buried the clumps inside)
            const float u=std::clamp(0.5f+t/log.length,0.0f,1.0f);
            const float v=static_cast<float>(log.variant);
            const float bow=(0.20f+0.07f*v)*(log.variant==1?-1.0f:1.0f);
            const float sideways=(std::sin(u*Pi)*bow+0.05f*std::sin(u*3.0f*Pi))*log.radius;
            const float arch=std::sin(u*Pi)*(0.035f+0.012f*v)*log.radius;
            const auto knot=[u](const float at) {const float d=(u-at)/0.035f;return std::exp(-d*d);};
            const float girth=0.84f+0.22f*(1.0f-u)+0.20f*std::exp(-u*14.0f)+0.08f*knot(0.28f)+0.07f*knot(0.77f);
            onWood({{log.x+dirX*t-dirZ*sideways,log.y+arch,log.z+dirZ*t+dirX*sideways}},-dirZ,dirX,
                   log.radius*girth*1.02f,side+below,s);
        }
    }
    // low on the standing snags, where the rotting wood holds the damp: a clump or two between
    // a hand's breadth and a metre and a half up, facing out from the trunk (on its rest mesh:
    // the trunk leans a little toward local (0.20, h, -0.16), its radius 0.52-0.58 narrowing)
    for(std::size_t i=0;i<snags.size()&&clusters.size()<MaximumClusters;++i) {
        const StandingSnagPlacement& snag=snags[i];
        if(terrain.SignedShoreDistance(snag.x,snag.z)<25.0f) continue;
        const auto seed=0x5a6bU+static_cast<std::uint32_t>(i)*2246822519U;
        if(Hash(seed)>0.65f) continue;
        const float height=snag.variant==0?10.6f:8.8f,baseRadius=snag.variant==0?.52f:.58f,topRadius=snag.variant==0?.28f:.30f;
        const float c=std::cos(snag.yawRadians),s=std::sin(snag.yawRadians);
        const float groundY=ground(snag.x,snag.z);
        const int clumps=1+static_cast<int>(Hash(seed^0x4U)*1.99f);
        for(int k=0;k<clumps&&clusters.size()<MaximumClusters;++k) {
            const auto sk=seed^static_cast<std::uint32_t>((k+1)*0x7f4aU);
            // (above the butt's flare and buttresses)
            const float up=(0.9f+1.0f*Hash(sk))/snag.scale;           // up the rest mesh
            const float t=up/height;
            const float lx=.20f*t,lz=-.16f*t;
            const float radius=(baseRadius+(topRadius-baseRadius)*t)*1.10f;
            const float angle=2.0f*Pi*Hash(sk^0x9U);
            const float nx=std::cos(angle),nz=std::sin(angle);
            // the rest mesh's point and normal, scaled, turned by the yaw, set on the ground
            const float px=(lx+nx*radius)*snag.scale,pz=(lz+nz*radius)*snag.scale;
            FungusCluster cluster;
            cluster.at={{snag.x+px*c+pz*s,groundY+up*snag.scale,snag.z-px*s+pz*c}};
            cluster.normal={{nx*c+nz*s,0.0f,-nx*s+nz*c}};
            cluster.spread=0.07f+0.08f*Hash(sk^0x1U);
            cluster.count=static_cast<std::uint8_t>(6+static_cast<int>(Hash(sk^0x2U)*8.99f));
            cluster.seed=sk;
            clusters.push_back(cluster);
        }
    }
    return clusters;
}
}
