#include "IslandSimulator/CastleRuinLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
std::vector<CircularObstacle> CastleRuinPlacement::WallObstacles() const {
    std::vector<CircularObstacle> circles;
    constexpr float Spacing=0.95f;
    const float mid=Half-Wall*0.5f;
    const auto add=[&](const float right,const float forward,const float radius=0.7f) {
        const auto p=At(right,forward);
        circles.push_back({p[0],p[1],radius});
    };
    for(float a=-mid;a<=mid+1e-3f;a+=Spacing) {
        add(a,-mid);                                             // the back
        if(std::abs(a)>GateHalf+0.4f) add(a,mid);                // the front, open at the gate
        add(-mid,a);                                             // the west side
        if(a<BreachFrom||a>BreachTo) add(mid,a);                 // the east side, open at the breach
    }
    for(const float r:{-Half,Half}) for(const float f:{-Half,Half}) add(r,f,TowerRadius*0.9f);
    const float keepMid=KeepHalf-KeepWall*0.5f;
    for(float a=-keepMid;a<=keepMid+1e-3f;a+=Spacing) {
        add(KeepRight+a,KeepForward-keepMid);
        add(KeepRight+a,KeepForward+keepMid);
        add(KeepRight-keepMid,KeepForward+a);
        add(KeepRight+keepMid,KeepForward+a);
    }
    return circles;
}

std::optional<CastleRuinPlacement> CastleRuinLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<CastleRuinPlacement> best;
    float bestScore=1e9f;
    constexpr float Step=6.0f,Extent=260.0f,Reach=CastleRuinPlacement::Half+CastleRuinPlacement::TowerRadius;
    for(float z=-Extent;z<=Extent;z+=Step) for(float x=-Extent;x<=Extent;x+=Step) {
        if(terrain.SignedShoreDistance(x,z)<MinimumShoreDistance) continue;
        const float centreHeight=terrain.Height(x,z);
        if(centreHeight<MinimumHeight||centreHeight>MaximumHeight) continue;
        bool far=true;
        for(const auto& point:avoid) far=far&&std::hypot(point[0]-x,point[1]-z)>keepAway;
        if(!far) continue;
        bool clear=true;
        for(const CircularObstacle& obstacle:obstacles)
            if(std::hypot(obstacle.x-x,obstacle.z-z)-obstacle.radius<Reach+ClearMargin) {clear=false;break;}
        if(!clear) continue;
        float low=centreHeight,high=centreHeight;
        for(int i=-3;i<=3;++i) for(int j=-3;j<=3;++j) {
            const float h=terrain.Height(x+Reach*static_cast<float>(i)/3.0f,z+Reach*static_cast<float>(j)/3.0f);
            low=std::min(low,h);high=std::max(high,h);
        }
        const float rise=high-low;
        if(rise>MaximumRise||low<MinimumHeight-2.0f) continue;
        const float score=rise-0.01f*centreHeight;
        if(score>=bestScore) continue;
        const float length=std::max(std::hypot(x,z),1.0f);
        bestScore=score;
        best=CastleRuinPlacement{x,z,x/length,z/length,low};
    }
    return best;
}
}
