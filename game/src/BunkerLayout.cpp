#include "IslandSimulator/BunkerLayout.hpp"

#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace island {
namespace {
struct Wall {float left,right,front,back;};
// Match the rendered corridor dividers, their jambs, and the sealed end bulkhead.
// Floor support remains continuous beneath these walls; only body collision uses them.
constexpr float HalfWall=.02f; // small collision thickness around the drawn wall plane
constexpr auto& Doors=BunkerLayout::Doorways;
constexpr float Jamb=BunkerLayout::DoorJambDepth;
constexpr float Inner=BunkerLayout::DoorInnerAcross,Outer=BunkerLayout::DoorOuterAcross;
constexpr std::array<Wall,12> InteriorWalls{{
    {1.36f-HalfWall,1.36f+HalfWall,0,Doors[0].from},
    {1.36f-HalfWall,1.36f+HalfWall,Doors[0].to,Doors[2].from},
    {1.36f-HalfWall,1.36f+HalfWall,Doors[2].to,36.2f},
    {-1.36f-HalfWall,-1.36f+HalfWall,0,Doors[1].from},
    {-1.36f-HalfWall,-1.36f+HalfWall,Doors[1].to,36.2f},
    {-1.36f,1.36f,35.88f,36.2f},
    {Inner,Outer,Doors[0].from,Doors[0].from+Jamb},
    {Inner,Outer,Doors[0].to-Jamb,Doors[0].to},
    {-Outer,-Inner,Doors[1].from,Doors[1].from+Jamb},
    {-Outer,-Inner,Doors[1].to-Jamb,Doors[1].to},
    {Inner,Outer,Doors[2].from,Doors[2].from+Jamb},
    {Inner,Outer,Doors[2].to-Jamb,Doors[2].to}}};

bool CircleHitsWall(float x,float z,float radius,const Wall& wall) noexcept {
    const float dx=x-std::clamp(x,wall.left,wall.right);
    const float dz=z-std::clamp(z,wall.front,wall.back);
    return dx*dx+dz*dz<=radius*radius;
}

bool RectangleHitsWall(const std::array<BunkerPoint,4>& p,const Wall& wall) noexcept {
    // Separating axes of both rectangles also catch a thin wall entirely between
    // the vehicle's sampled corners/midpoints, which point-only queries can miss.
    const std::array<BunkerPoint,4> axes{{{1,0,0},{0,0,1},
        {p[1].z-p[0].z,0,p[0].x-p[1].x},{p[2].z-p[1].z,0,p[1].x-p[2].x}}};
    for(const auto axis:axes) {
        float low=1e30f,high=-1e30f;
        for(const auto v:p) {const float d=v.x*axis.x+v.z*axis.z;low=std::min(low,d);high=std::max(high,d);}
        const float a=wall.left*axis.x,b=wall.right*axis.x;
        const float c=wall.front*axis.z,d=wall.back*axis.z;
        if(high<std::min(a,b)+std::min(c,d)||low>std::max(a,b)+std::max(c,d)) return false;
    }
    return true;
}
}

BunkerLayout::BunkerLayout(const IslandTerrain& terrain) {
    cosine_=std::cos(BunkerSite::Angle);sine_=std::sin(BunkerSite::Angle);
    entrance_={BunkerSite::X,terrain.Height(BunkerSite::X,BunkerSite::Z),BunkerSite::Z};
    const auto sample=terrain.Sample(entrance_.x,entrance_.z);
    const auto deep=World(0,0,34);
    const float cover=terrain.Height(deep.x,deep.z)-(entrance_.y+FloorRelative(34)+3.05f);
    if(sample.signedShoreDistance<90.0f||sample.height<8.0f||cover<1.0f) {
        throw std::runtime_error("Bunker site is not concealed beneath an inland ridge");
    }
}

BunkerPoint BunkerLayout::World(float across,float height,float depth)const noexcept {
    return {entrance_.x-sine_*across-cosine_*depth,entrance_.y+height,
            entrance_.z+cosine_*across-sine_*depth};
}

BunkerPoint BunkerLayout::Local(BunkerPoint point)const noexcept {
    const float dx=point.x-entrance_.x,dz=point.z-entrance_.z;
    return {-sine_*dx+cosine_*dz,point.y-entrance_.y,-cosine_*dx-sine_*dz};
}

float BunkerLayout::FloorRelative(float depth)const noexcept {
    // Meet the packed north-facing hillside before roof-berm interpolation;
    // the rendered opening and the support plane share this approach slope.
    if(depth<=0)return BunkerSite::ApronSlope*depth;
    return depth<10.0f?-.32f*depth:-3.2f;
}

bool BunkerLayout::WalkableLocal(float across,float depth)const noexcept {
    // Include the packed-earth approach; the built walls begin at the portal.
    const bool corridor=std::abs(across)<=1.36f&&depth>=CorridorFrontDepth&&depth<=36.2f;
    const bool computer=across>=1.05f&&across<=9.25f&&depth>=11.0f&&depth<=23.2f;
    const bool reactor=across>=-10.1f&&across<=-1.05f&&depth>=22.8f&&depth<=36.2f;
    const bool service=across>=1.05f&&across<=7.1f&&depth>=25.8f&&depth<=35.5f;
    if(!(corridor||computer||reactor||service))return false;
    if(reactor&&std::hypot(across+5.55f,depth-29.5f)<1.72f)return false;
    return true;
}

bool BunkerLayout::WalkableCircle(float across,float depth,float radius)const noexcept {
    if(!std::isfinite(across)||!std::isfinite(depth)||!std::isfinite(radius)||radius<0)return false;
    if(!WalkableLocal(across,depth))return false;
    for(const auto wall:InteriorWalls) if(CircleHitsWall(across,depth,radius,wall)) return false;
    constexpr int samples=16;
    for(int i=0;i<samples;++i) {
        const float angle=2.0f*std::numbers::pi_v<float>*i/samples;
        if(!WalkableLocal(across+std::cos(angle)*radius,depth+std::sin(angle)*radius))return false;
    }
    return true;
}

bool BunkerLayout::ContainsWalkable(float worldX,float worldZ,float margin)const noexcept {
    const auto local=Local({worldX,entrance_.y,worldZ});
    return WalkableCircle(local.x,local.z,std::max(0.0f,margin));
}

bool BunkerLayout::FootprintHitsInteriorWall(const std::array<BunkerPoint,4>& corners)const noexcept {
    std::array<BunkerPoint,4> local;
    for(std::size_t i=0;i<corners.size();++i) {
        if(!std::isfinite(corners[i].x)||!std::isfinite(corners[i].z)) return true;
        local[i]=Local(corners[i]);
    }
    for(const auto wall:InteriorWalls) if(RectangleHitsWall(local,wall)) return true;
    return false;
}

bool BunkerLayout::SupportHeight(float worldX,float worldZ,float currentFeetY,
                                 float& height)const noexcept {
    if(!std::isfinite(currentFeetY))return false;
    const auto local=Local({worldX,entrance_.y,worldZ});
    if(!WalkableLocal(local.x,local.z))return false;
    const float floor=entrance_.y+FloorRelative(local.z);
    // The hint prevents a player walking on the natural ridge above the rooms from
    // being pulled through the terrain onto the underground floor.
    if(std::abs(currentFeetY-floor)>1.15f)return false;
    height=floor;return true;
}

bool BunkerLayout::HidesTerrain(float worldX,float worldY,float worldZ)const noexcept {
    if(!std::isfinite(worldX)||!std::isfinite(worldY)||!std::isfinite(worldZ))return false;
    const auto local=Local({worldX,worldY,worldZ});
    const bool corridor=std::abs(local.x)<=1.40f&&local.z>=CorridorFrontDepth&&local.z<=36.35f;
    const bool computer=local.x>=1.02f&&local.x<=9.30f
        &&local.z>=10.95f&&local.z<=23.25f;
    const bool reactor=local.x>=-10.15f&&local.x<=-1.02f
        &&local.z>=22.75f&&local.z<=36.25f;
    const bool service=local.x>=1.02f&&local.x<=7.15f
        &&local.z>=25.75f&&local.z<=35.55f;
    if(!(corridor||computer||reactor||service))return false;
    const float ceiling=entrance_.y+FloorRelative(local.z)+(local.z<10.0f?2.75f:3.05f);
    // Preserve the real ridge wherever it is genuinely above the concrete roof, and the
    // hillside wherever it falls below the floor (in front of the portal, under the apron's
    // edge); remove only heightfield fragments that pass through the habitable volume.
    return worldY<ceiling+.04f&&worldY>entrance_.y+FloorRelative(local.z)-.03f;
}

bool BunkerLayout::IntersectsCapsule(BunkerPoint feet,float radius,float height)const noexcept {
    if(!std::isfinite(feet.x)||!std::isfinite(feet.y)||!std::isfinite(feet.z)
        ||!std::isfinite(radius)||!std::isfinite(height)||radius<=0||height<2*radius)return true;
    const auto local=Local(feet);
    // Outside the portal plane is ordinary open terrain. Previously collision began
    // before WalkableCircle could fit, producing an invisible wall at the entrance.
    if(local.z<-.35f||local.z>36.8f||std::abs(local.x)>10.8f)return false;
    const float floor=entrance_.y+FloorRelative(local.z);
    const float ceiling=floor+(local.z<10.0f?2.75f:3.05f);
    if(feet.y>ceiling+.15f||feet.y+height<floor-.15f)return false;
    if(feet.y<floor-.03f||feet.y+height>ceiling+.03f)return true;
    return !WalkableCircle(local.x,local.z,radius);
}

float BunkerLayout::SafeTravelFraction(BunkerPoint from,BunkerPoint to,
                                       float radius,float height)const noexcept {
    if(IntersectsCapsule(from,radius,height))return 0.0f;
    const float distance=std::hypot(to.x-from.x,to.z-from.z);
    if(!std::isfinite(distance)||distance>1000.0f)return 0.0f;
    const int steps=std::max(1,static_cast<int>(std::ceil(distance/.08f)));
    float previous=0.0f;
    for(int i=1;i<=steps;++i) {
        const float current=static_cast<float>(i)/steps;
        const BunkerPoint point{std::lerp(from.x,to.x,current),std::lerp(from.y,to.y,current),
                                std::lerp(from.z,to.z,current)};
        if(!IntersectsCapsule(point,radius,height)){previous=current;continue;}
        float low=previous,high=current;
        for(int pass=0;pass<12;++pass) {
            const float middle=(low+high)*.5f;
            const BunkerPoint probe{std::lerp(from.x,to.x,middle),std::lerp(from.y,to.y,middle),
                                    std::lerp(from.z,to.z,middle)};
            if(IntersectsCapsule(probe,radius,height))high=middle;else low=middle;
        }
        return low;
    }
    return 1.0f;
}

}
