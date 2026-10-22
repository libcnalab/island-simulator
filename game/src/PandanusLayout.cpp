#include "IslandSimulator/PandanusLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/MineSite.hpp"
#include "IslandSimulator/VolcanoSite.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
float Hash(std::uint32_t v) noexcept {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

std::vector<PandanusPlacement> PandanusLayout::Scatter(const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
                                                      const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::vector<PandanusPlacement> trees;
    constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    // a group every so often round the coast: up to four trees within a few metres of its middle
    constexpr int Groups=150;
    for(int g=0;g<Groups&&trees.size()<MaximumCount;++g) {
        const auto seed=static_cast<std::uint32_t>(g)*2654435761U+0x9a5dU;
        if(Hash(seed)>0.46f) continue;
        const float angle=Tau*(static_cast<float>(g)+0.5f*Hash(seed^0x1U))/Groups;
        const float shore=terrain.ShoreRadius(angle);
        const float inland=InlandFrom+(InlandTo-InlandFrom)*Hash(seed^0x2U);
        const float cx=std::cos(angle)*(shore-inland),cz=std::sin(angle)*(shore-inland);
        const int count=1+static_cast<int>(Hash(seed^0x3U)*3.99f);
        for(int k=0;k<count&&trees.size()<MaximumCount;++k) {
            const auto s=seed^(static_cast<std::uint32_t>(k+1)*0x85ebca6bU);
            const float a=Tau*Hash(s^0x4U),d=k==0?0.0f:2.5f+3.5f*Hash(s^0x5U);
            const float x=cx+std::cos(a)*d,z=cz+std::sin(a)*d;
            const TerrainSample sample=terrain.Sample(x,z);
            const float fromShore=terrain.SignedShoreDistance(x,z);
            if(fromShore<InlandFrom-1.0f||fromShore>InlandTo+3.0f) continue;
            if(sample.height<0.8f||sample.slopeRadians>0.32f) continue;
            if(sample.surface!=TerrainSurface::DrySand&&sample.surface!=TerrainSurface::CoastalGround) continue;
            if(MineSite::Reserves(x,z,2.0f)||VolcanoSite::Reserves(x,z,2.0f)) continue;
            bool clear=true;
            for(const CircularObstacle& o:obstacles) clear=clear&&std::hypot(o.x-x,o.z-z)>o.radius+1.3f;
            for(const auto& p:avoid) clear=clear&&std::hypot(p[0]-x,p[1]-z)>keepAway;
            for(const PandanusPlacement& t:trees) clear=clear&&std::hypot(t.x-x,t.z-z)>MinimumSpacing;
            if(!clear) continue;
            PandanusPlacement tree;
            tree.x=x;tree.z=z;tree.groundY=sample.height;
            const float radial=std::max(1.0f,std::hypot(x,z));
            tree.seaX=x/radial;tree.seaZ=z/radial;
            tree.height=4.2f+3.6f*Hash(s^0x6U);
            tree.lean=0.08f+0.3f*Hash(s^0x7U)*(1.0f-std::clamp((fromShore-InlandFrom)/(InlandTo-InlandFrom),0.0f,1.0f)*0.6f);
            tree.forks=static_cast<std::uint8_t>(1+static_cast<int>(Hash(s^0x8U)*2.99f));
            tree.seed=s;
            tree.fruiting=Hash(s^0x9U)<0.45f;
            trees.push_back(tree);
        }
    }
    return trees;
}
}
