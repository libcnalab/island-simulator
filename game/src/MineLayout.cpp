#include "IslandSimulator/MineLayout.hpp"

#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace island {

MineLayout::MineLayout(const IslandTerrain& terrain) {
    cosine_=std::cos(MineSite::Angle);sine_=std::sin(MineSite::Angle);
    entrance_={MineSite::X,terrain.Height(MineSite::X,MineSite::Z),MineSite::Z};
    // the portal stands where the slope first rises over the roof
    portalDepth_=MineSite::CutTo;
    for(float depth=0.0f;depth<=MineSite::CutTo;depth+=0.05f) {
        const auto p=World(0.0f,0.0f,depth);
        if(terrain.Height(p.x,p.z)>=entrance_.y+FloorRelative(depth)+MineSite::Height) {portalDepth_=depth;break;}
    }
    const auto deep=World(0.0f,0.0f,MineSite::AditLength*0.5f);
    const float cover=terrain.Height(deep.x,deep.z)-(entrance_.y+FloorRelative(MineSite::AditLength*0.5f)+MineSite::Height);
    if(terrain.SignedShoreDistance(entrance_.x,entrance_.z)<60.0f||portalDepth_>=MineSite::CutTo||cover<4.0f)
        throw std::runtime_error("Mine site is not driven into the mountain's slope");
}

MinePoint MineLayout::World(const float across,const float up,const float depth) const noexcept {
    return {entrance_.x+cosine_*depth-sine_*across,entrance_.y+up,entrance_.z+sine_*depth+cosine_*across};
}

MinePoint MineLayout::Local(const MinePoint point) const noexcept {
    const float dx=point.x-entrance_.x,dz=point.z-entrance_.z;
    return {-sine_*dx+cosine_*dz,point.y-entrance_.y,cosine_*dx+sine_*dz};
}

float MineLayout::FloorRelative(const float depth) noexcept {
    return MineSite::Grade*std::max(depth,0.0f);
}

float MineLayout::Headroom(const float across,const float depth) noexcept {
    const float a=across/MineSite::ChamberHalfWidth,d=(depth-MineSite::ChamberCentre)/MineSite::ChamberHalfLength;
    const float inside=1.0f-a*a-d*d;
    if(inside<=0.0f) return MineSite::Height;
    return std::clamp(MineSite::Height+(MineSite::ChamberHeight-MineSite::Height)*std::sqrt(inside)*1.6f,
                      MineSite::Height,MineSite::ChamberHeight);
}

bool MineLayout::WalkableLocal(const float across,const float depth) noexcept {
    const bool adit=std::abs(across)<=MineSite::PostInner&&depth>=-0.75f&&depth<=MineSite::AditLength+0.4f;
    const float a=across/(MineSite::ChamberHalfWidth-0.25f),d=(depth-MineSite::ChamberCentre)/(MineSite::ChamberHalfLength-0.25f);
    const bool chamber=a*a+d*d<=1.0f;
    if(!(adit||chamber)) return false;
    if(depth>CrateFrom&&depth<CrateTo&&across<CrateInner) return false;
    if(depth>RubbleFrom&&depth<RubbleTo&&across>RubbleInner) return false;
    return std::hypot(across-CartAcross,depth-CartDepth)>CartRadius;
}

bool MineLayout::WalkableCircle(const float across,const float depth,const float radius) noexcept {
    if(!std::isfinite(across)||!std::isfinite(depth)||!std::isfinite(radius)||radius<0.0f) return false;
    if(!WalkableLocal(across,depth)) return false;
    constexpr int Samples=16;
    for(int i=0;i<Samples;++i) {
        const float angle=2.0f*std::numbers::pi_v<float>*static_cast<float>(i)/Samples;
        if(!WalkableLocal(across+std::cos(angle)*radius,depth+std::sin(angle)*radius)) return false;
    }
    return true;
}

bool MineLayout::SupportHeight(const float worldX,const float worldZ,const float currentFeetY,float& height) const noexcept {
    if(!std::isfinite(currentFeetY)) return false;
    const auto local=Local({worldX,entrance_.y,worldZ});
    if(local.z<-0.3f||!WalkableLocal(local.x,local.z)) return false;
    const float floor=entrance_.y+FloorRelative(local.z);
    // a walker on the mountain above the adit keeps the mountain's ground
    if(std::abs(currentFeetY-floor)>1.15f) return false;
    height=floor;return true;
}

bool MineLayout::HidesTerrain(const float worldX,const float worldY,const float worldZ) const noexcept {
    if(!std::isfinite(worldX)||!std::isfinite(worldY)||!std::isfinite(worldZ)) return false;
    const auto local=Local({worldX,worldY,worldZ});
    if(std::abs(local.x)>MineSite::HalfWidth+0.25f||local.z<-0.8f||local.z>MineSite::AditLength+0.4f) return false;
    return worldY<entrance_.y+FloorRelative(local.z)+MineSite::Height+0.04f;
}

bool MineLayout::IntersectsCapsule(const MinePoint feet,const float radius,const float height) const noexcept {
    if(!std::isfinite(feet.x)||!std::isfinite(feet.y)||!std::isfinite(feet.z)
        ||!std::isfinite(radius)||!std::isfinite(height)||radius<=0.0f||height<2.0f*radius) return true;
    const auto local=Local(feet);
    // in front of the portal is the open hillside
    if(local.z<-0.35f||local.z>MineSite::ChamberCentre+MineSite::ChamberHalfLength+1.0f
        ||std::abs(local.x)>MineSite::ChamberHalfWidth+1.0f) return false;
    const float floor=entrance_.y+FloorRelative(local.z);
    const float roof=floor+Headroom(local.x,local.z);
    if(feet.y>roof+0.15f||feet.y+height<floor-0.15f) return false;
    if(feet.y<floor-0.03f||feet.y+height>roof+0.03f) return true;
    return !WalkableCircle(local.x,local.z,radius);
}

float MineLayout::SafeTravelFraction(const MinePoint from,const MinePoint to,const float radius,const float height) const noexcept {
    if(IntersectsCapsule(from,radius,height)) return IntersectsCapsule(to,radius,height)?0.0f:1.0f;
    const float distance=std::hypot(to.x-from.x,to.z-from.z);
    if(!std::isfinite(distance)||distance>1000.0f) return 0.0f;
    const int steps=std::max(1,static_cast<int>(std::ceil(distance/0.08f)));
    float previous=0.0f;
    for(int i=1;i<=steps;++i) {
        const float current=static_cast<float>(i)/static_cast<float>(steps);
        const MinePoint point{std::lerp(from.x,to.x,current),std::lerp(from.y,to.y,current),std::lerp(from.z,to.z,current)};
        if(!IntersectsCapsule(point,radius,height)) {previous=current;continue;}
        float low=previous,high=current;
        for(int pass=0;pass<12;++pass) {
            const float middle=(low+high)*0.5f;
            const MinePoint probe{std::lerp(from.x,to.x,middle),std::lerp(from.y,to.y,middle),std::lerp(from.z,to.z,middle)};
            if(IntersectsCapsule(probe,radius,height)) high=middle; else low=middle;
        }
        return low;
    }
    return 1.0f;
}

}
