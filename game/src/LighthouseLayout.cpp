#include "IslandSimulator/LighthouseLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Tau=2.0f*std::numbers::pi_v<float>;
float Wrap(const float angle) {return angle-Tau*std::floor(angle/Tau);}   // into [0, 2 pi)
}

int LighthousePlacement::StairHeights(const float angle,std::array<float,8>& heights) const noexcept {
    // the unwrapped angles of the stair over `angle`: from its end (the top) upward a turn at a time
    const float end=StairEnd();
    int count=0;
    for(float phi=end+Wrap(angle-end);phi<=StairStart+1e-4f&&count<8;phi+=Tau)
        heights[static_cast<std::size_t>(count++)]=TowerBaseY()+StairRisePerRadian()*(StairStart-phi);
    std::reverse(heights.begin(),heights.begin()+count);
    return count;
}

bool LighthousePlacement::LandingFloor(const float angle) const noexcept {
    // open over the stair's last turn wherever a walker climbing it would not clear the slab:
    // the stair there less than 2.35 m below the lantern's floor
    return StairRisePerRadian()*Wrap(angle-StairEnd())>=2.35f;
}

bool LighthousePlacement::OnSteps(const float worldX,const float worldZ,const float margin) const noexcept {
    const float dx=worldX-x,dz=worldZ-z;
    const float inland=-(dx*seaX+dz*seaZ),lateral=std::abs(dx*seaZ-dz*seaX);
    return lateral<=StepsHalfWidth+margin&&inland>=PlinthRadius-0.6f&&inland<=PlinthRadius+stepsRun+margin;
}

LighthousePoint LighthousePlacement::StepsFoot() const noexcept {
    const LighthousePoint p=At(Pi,PlinthRadius+stepsRun+0.6f,stepsFootY);
    return p;
}

bool LighthousePlacement::SupportHeight(const float worldX,const float worldZ,const float feetY,float& height) const noexcept {
    if(!std::isfinite(worldX)||!std::isfinite(worldZ)||!std::isfinite(feetY)) return false;
    const auto polar=Polar(worldX,worldZ);
    const float r=polar[0],angle=polar[1];
    if(r>PlinthRadius+stepsRun+1.0f) return false;
    const float base=TowerBaseY(),gallery=GalleryY();
    // up top: the gallery round the lantern and the lantern's floor (open over the stair)
    if(feetY>gallery-1.0f&&r<=GalleryRadius+0.05f&&(r>=LanternRadius||LandingFloor(angle))) {
        height=gallery;return true;
    }
    // inside the tower: the floor and the stair's turns, the highest within a step of the feet
    if(r<InnerRadius(base)) {
        float best=base;
        if(r>=NewelRadius) {
            std::array<float,8> heights{};
            const int count=StairHeights(angle,heights);
            for(int i=0;i<count;++i) {
                const float h=heights[static_cast<std::size_t>(i)];
                if(h<=feetY+0.45f&&h>best&&r<=InnerRadius(h)+0.02f) best=h;
            }
        }
        height=best;return true;
    }
    // the door's threshold through the wall, and the plinth's top round the tower
    if(r<=PlinthRadius+0.02f&&feetY>base-1.2f) {height=base;return true;}
    // the steps down inland from the door to the ground
    if(OnSteps(worldX,worldZ)) {
        const float dx=worldX-x,dz=worldZ-z;
        const float inland=-(dx*seaX+dz*seaZ);
        const float t=std::clamp((inland-PlinthRadius)/std::max(stepsRun,0.01f),0.0f,1.0f);
        height=base+(stepsFootY-base)*t;
        return true;
    }
    return false;
}

bool LighthousePlacement::IntersectsCapsule(const LighthousePoint feet,const float radius,const float height) const noexcept {
    if(!std::isfinite(feet.x)||!std::isfinite(feet.y)||!std::isfinite(feet.z)) return true;
    const auto polar=Polar(feet.x,feet.z);
    const float r=polar[0],angle=polar[1];
    if(r>PlinthRadius+stepsRun+1.5f) return false;
    const float base=TowerBaseY(),gallery=GalleryY(),top=feet.y+height;
    const float dx=feet.x-x,dz=feet.z-z;
    // the plinth's sides, below its top, but for the steps
    if(feet.y<base-0.3f&&r<PlinthRadius+radius&&!OnSteps(feet.x,feet.z)) return true;
    // the tower's wall, up to the gallery, but for the door's passage under its lintel
    if(feet.y<TowerTopY()&&top>base) {
        const float inland=-(dx*seaX+dz*seaZ),lateral=std::abs(dx*seaZ-dz*seaX);
        const bool doorway=inland>0.0f&&lateral+radius<=DoorHalfWidth+0.01f&&top<=base+DoorHeight+0.01f&&feet.y>=base-0.45f;
        for(const float y:{std::max(feet.y,base),std::min(top,TowerTopY())}) {
            const float outer=OuterRadius(y),inner=outer-WallThickness;
            if(r+radius>inner&&r-radius<outer&&!doorway) return true;
        }
    }
    // the newel up to the lamp's pedestal, and the lens round the lamp
    if(r<NewelRadius+radius&&top>base&&feet.y<gallery+0.6f) return true;
    if(r<0.38f+radius&&top>gallery+0.55f&&feet.y<gallery+1.45f) return true;
    // the stair's turns over the head of a walker under them
    if(r>NewelRadius&&r<InnerRadius(std::max(feet.y,base))) {
        std::array<float,8> heights{};
        const int count=StairHeights(angle,heights);
        for(int i=0;i<count;++i) {
            const float h=heights[static_cast<std::size_t>(i)];
            if(h>feet.y+0.45f&&h-0.08f<top) return true;
        }
    }
    // the lantern's floor overhead, from below
    if(r<LanternRadius&&LandingFloor(angle)&&feet.y<gallery-0.3f&&top>gallery-0.22f) return true;
    // up top: the lantern's wall but for its door, and the gallery's rail
    if(feet.y>gallery-0.5f&&top>gallery) {
        const float doorX=seaX*std::cos(LanternDoorAngle)+seaZ*std::sin(LanternDoorAngle);
        const float doorZ=seaZ*std::cos(LanternDoorAngle)-seaX*std::sin(LanternDoorAngle);
        const float along=dx*doorX+dz*doorZ,lateral=std::abs(dx*doorZ-dz*doorX);
        const bool doorway=along>0.0f&&lateral+radius<=LanternRadius*std::sin(LanternDoorHalfAngle)+0.01f;
        if(r+radius>LanternRadius-0.05f&&r-radius<LanternRadius+0.05f&&!doorway) return true;
        if(r+radius>GalleryRadius-RailInset) return true;
    }
    return false;
}

float LighthousePlacement::SafeTravelFraction(const LighthousePoint from,const LighthousePoint to,
                                              const float radius,const float height) const noexcept {
    const float distance=std::hypot(to.x-from.x,to.z-from.z);
    if(!std::isfinite(distance)||distance>1000.0f) return 0.0f;
    // already caught (a teleport): let it move only out of the walls
    if(IntersectsCapsule(from,radius,height)) return IntersectsCapsule(to,radius,height)?0.0f:1.0f;
    const int steps=std::max(1,static_cast<int>(std::ceil(distance/0.06f)));
    float previous=0.0f;
    for(int i=1;i<=steps;++i) {
        const float current=static_cast<float>(i)/static_cast<float>(steps);
        const LighthousePoint point{std::lerp(from.x,to.x,current),std::lerp(from.y,to.y,current),std::lerp(from.z,to.z,current)};
        if(!IntersectsCapsule(point,radius,height)) {previous=current;continue;}
        float low=previous,high=current;
        for(int pass=0;pass<12;++pass) {
            const float middle=(low+high)*0.5f;
            const LighthousePoint probe{std::lerp(from.x,to.x,middle),std::lerp(from.y,to.y,middle),std::lerp(from.z,to.z,middle)};
            if(IntersectsCapsule(probe,radius,height)) high=middle; else low=middle;
        }
        return low;
    }
    return 1.0f;
}

std::vector<CircularObstacle> CottagePlacement::WallObstacles() const {
    std::vector<CircularObstacle> circles;
    constexpr float Radius=0.32f,Spacing=0.45f;
    const float ia=HalfLength-Wall*0.5f,ic=HalfWidth-Wall*0.5f;
    const auto add=[&](const float along,const float across) {
        const auto p=At(along,across);
        circles.push_back({p[0],p[1],Radius});
    };
    for(float a=-ia;a<=ia+1e-3f;a+=Spacing) {
        if(std::abs(a-DoorAlong)>DoorHalf+0.3f) add(a,-ic);   // the front, open at the door
        add(a,ic);                                              // the back
    }
    for(float c=-ic+Spacing;c<ic-1e-3f;c+=Spacing) {add(-ia,c);add(ia,c);}
    return circles;
}

std::optional<CottagePlacement> LighthouseLayout::FindCottage(const IslandTerrain& terrain,const LighthousePlacement& lighthouse,
                                                              const std::span<const CircularObstacle> obstacles) {
    std::optional<CottagePlacement> best;
    float bestRange=0.9f;
    for(const float distance:{9.5f,11.0f,12.5f})
    for(const float offset:{0.75f,-0.75f,1.1f,-1.1f,1.5f,-1.5f}) {
        const LighthousePoint centre=lighthouse.At(LighthousePlacement::Pi+offset,distance,0.0f);
        CottagePlacement cottage;
        cottage.x=centre.x;cottage.z=centre.z;
        // the front faces the tower: across runs away from it, along square to that
        const float awayX=centre.x-lighthouse.x,awayZ=centre.z-lighthouse.z,norm=std::hypot(awayX,awayZ);
        cottage.dirX=awayZ/norm;cottage.dirZ=-awayX/norm;
        float low=1e9f,high=-1e9f;
        bool dry=true;
        for(const float a:{-1.0f,0.0f,1.0f}) for(const float c:{-1.0f,0.0f,1.0f}) {
            const auto p=cottage.At(a*(CottagePlacement::HalfLength+0.4f),c*(CottagePlacement::HalfWidth+0.4f));
            const TerrainSample sample=terrain.Sample(p[0],p[1]);
            low=std::min(low,sample.height);high=std::max(high,sample.height);
            dry=dry&&sample.height>0.6f&&sample.surface!=TerrainSurface::DrySand&&sample.surface!=TerrainSurface::WetSand;
        }
        if(!dry||high-low>=bestRange) continue;
        bool clear=true;
        for(const CircularObstacle& o:obstacles)
            clear=clear&&std::hypot(o.x-cottage.x,o.z-cottage.z)-o.radius>CottagePlacement::HalfLength+0.8f;
        // and clear of the lighthouse's plinth and steps
        for(float run=0.0f;run<=lighthouse.stepsRun+0.6f;run+=0.5f) {
            const LighthousePoint s=lighthouse.At(LighthousePlacement::Pi,LighthousePlacement::PlinthRadius+run,0.0f);
            clear=clear&&std::hypot(s.x-cottage.x,s.z-cottage.z)>CottagePlacement::HalfLength+1.6f;
        }
        if(!clear) continue;
        cottage.groundY=low;
        bestRange=high-low;
        best=cottage;
    }
    return best;
}

std::optional<LighthousePlacement> LighthouseLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    std::optional<LighthousePlacement> best;
    constexpr int Steps=180;
    for(int step=0;step<Steps;++step) {
        const float angle=Tau*static_cast<float>(step)/static_cast<float>(Steps);
        const float seaX=std::cos(angle),seaZ=std::sin(angle);
        const float shore=terrain.ShoreRadius(angle);
        for(float back=10.0f;back<=25.0f;back+=2.5f) {
            const float x=seaX*(shore-back),z=seaZ*(shore-back);
            const TerrainSample centre=terrain.Sample(x,z);
            if(centre.height<MinimumHeight||centre.height>MaximumHeight) continue;
            if(centre.surface==TerrainSurface::DrySand||centre.surface==TerrainSurface::WetSand) continue;
            if(best&&centre.height<=best->groundY) continue;
            bool far=true;
            for(const auto& point:avoid) far=far&&std::hypot(point[0]-x,point[1]-z)>keepAway;
            if(!far) continue;
            float low=centre.height,high=centre.height;
            for(const float dx:{-3.5f,0.0f,3.5f}) for(const float dz:{-3.5f,0.0f,3.5f}) {
                const float h=terrain.Height(x+dx,z+dz);
                low=std::min(low,h);high=std::max(high,h);
            }
            if(high-low>MaximumRise) continue;
            bool clear=true;
            for(const CircularObstacle& obstacle:obstacles)
                if(std::hypot(obstacle.x-x,obstacle.z-z)-obstacle.radius<LighthousePlacement::PlinthRadius+ClearRadius) {clear=false;break;}
            if(!clear) continue;
            best=LighthousePlacement{x,z,high,seaX,seaZ};
        }
    }
    if(best) {
        // the steps run inland from the door, falling no steeper than StepsSlope, until they meet
        // the ground
        LighthousePlacement& light=*best;
        const float top=light.TowerBaseY();
        light.stepsRun=6.0f;light.stepsFootY=top-6.0f*LighthousePlacement::StepsSlope;
        for(float run=0.4f;run<=6.0f;run+=0.05f) {
            const LighthousePoint p=light.At(LighthousePlacement::Pi,LighthousePlacement::PlinthRadius+run,0.0f);
            const float ground=terrain.Height(p.x,p.z);
            if(top-run*LighthousePlacement::StepsSlope<=ground+0.02f) {light.stepsRun=run;light.stepsFootY=ground;break;}
        }
    }
    return best;
}
}
