#include "IslandSimulator/FreighterLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
using P=FreighterPlacement;
constexpr float Pi=std::numbers::pi_v<float>;
constexpr float StepReach=0.45f;   // a floor this far above the feet still takes them (the step)
// One plumb wall in the ship's frame: from (along, across) to (along, across), standing from
// `low` to `high` above the keel, or above the deck where it stands when `onDeck`.
struct Wall {float a0,c0,a1,c1,low,high;bool onDeck;};
struct Post {float along,across,radius,low,high;};

float Sq(const float v) {return v*v;}

const std::vector<Wall>& Walls() {
    static const std::vector<Wall> walls=[] {
        std::vector<Wall> w;
        // the hull's shell (from below the keel to the deck) and the bulwark above it, round
        // both sides from the transom to the stem; the gangway's opening in the starboard bulwark
        constexpr float Step=0.5f;
        for(const float side:{-1.0f,1.0f}) {
            for(float a=0.0f;a<P::Length-1e-3f;a+=Step) {
                const float b=std::min(a+Step,P::Length);
                const float c0=side*P::HalfBreadth(a),c1=side*P::HalfBreadth(b);
                w.push_back({a,c0,b,c1,-6.0f,-0.02f,true});
                const float middle=(a+b)*0.5f;
                if(!(side>0.0f&&middle>P::GangwayAft&&middle<P::GangwayFore))
                    w.push_back({a,c0,b,c1,-0.02f,P::Bulwark,true});
            }
        }
        w.push_back({0.0f,-P::HalfBreadth(0.0f),0.0f,P::HalfBreadth(0.0f),-6.0f,P::Bulwark,true});
        // the hatch's coaming
        const float ha=P::HatchAft,hf=P::HatchFore,hh=P::HatchHalf;
        w.push_back({ha,-hh,hf,-hh,-0.02f,P::Coaming,true});w.push_back({ha,hh,hf,hh,-0.02f,P::Coaming,true});
        w.push_back({ha,-hh,ha,hh,-0.02f,P::Coaming,true});w.push_back({hf,-hh,hf,hh,-0.02f,P::Coaming,true});
        // the stairwell's railings: along its inner side from the hold's floor up (no walking
        // under the stair, nor off it into the hold) and across its after end on the deck
        w.push_back({P::StairFoot,P::StairInner,P::StairTop,P::StairInner,P::HoldFloor-0.3f,P::DeckUp(P::StairTop)+P::Bulwark,false});
        w.push_back({P::StairFoot,P::StairInner,P::StairFoot,P::HalfBeam,-0.02f,P::Bulwark,true});
        // the hold's end bulkheads, under the deck
        for(const float a:{P::HoldAft,P::HoldFore})
            w.push_back({a,-P::HalfBeam,a,P::HalfBeam,P::HoldFloor-0.3f,-P::DeckThickness-0.02f,true});
        // the deckhouse, its door in the forward face; the mess table inside
        const float ka=P::HouseAft,kf=P::HouseFore,kh=P::HouseHalf,kt=P::HouseHeight;
        w.push_back({ka,-kh,kf,-kh,-0.02f,kt,true});w.push_back({ka,kh,kf,kh,-0.02f,kt,true});
        w.push_back({ka,-kh,ka,kh,-0.02f,kt,true});
        w.push_back({kf,-kh,kf,P::DoorInner,-0.02f,kt,true});w.push_back({kf,P::DoorOuter,kf,kh,-0.02f,kt,true});
        w.push_back({3.4f,-1.7f,5.8f,-1.7f,-0.02f,0.78f,true});w.push_back({3.4f,-0.5f,5.8f,-0.5f,-0.02f,0.78f,true});
        w.push_back({3.4f,-1.7f,3.4f,-0.5f,-0.02f,0.78f,true});w.push_back({5.8f,-1.7f,5.8f,-0.5f,-0.02f,0.78f,true});
        // the windlass on the foredeck
        const float wa=P::Windlass-0.45f,wf=P::Windlass+0.45f,wh=0.85f;
        w.push_back({wa,-wh,wf,-wh,-0.02f,0.9f,true});w.push_back({wa,wh,wf,wh,-0.02f,0.9f,true});
        w.push_back({wa,-wh,wa,wh,-0.02f,0.9f,true});w.push_back({wf,-wh,wf,wh,-0.02f,0.9f,true});
        return w;
    }();
    return walls;
}

const std::vector<Post>& Posts() {
    // the masts and the ventilators' trunks
    static const std::vector<Post> posts{{P::MainMast,0.0f,0.2f,-0.02f,12.0f},{P::ForeMast,0.0f,0.2f,-0.02f,12.0f},
        {9.0f,-1.6f,0.2f,-0.02f,1.6f},{22.4f,-1.1f,0.2f,-0.02f,1.6f},{22.4f,1.1f,0.2f,-0.02f,1.6f}};
    return posts;
}

float SegmentDistance(const float px,const float pz,const Wall& w,float& t) {
    const float dx=w.a1-w.a0,dz=w.c1-w.c0;
    const float length=dx*dx+dz*dz;
    t=length>1e-9f?std::clamp(((px-w.a0)*dx+(pz-w.c0)*dz)/length,0.0f,1.0f):0.0f;
    return std::hypot(px-(w.a0+dx*t),pz-(w.c0+dz*t));
}

// The ladder's walkable strip beside the starboard side: its head's landing level with the
// deck, then the flight down to its foot.
bool OnLadder(const P& ship,const float along,const float across,float& up) {
    if(along<P::GangwayAft||along>ship.LadderFootAlong()) return false;
    const float inner=P::HalfBeam,outer=P::HalfBeam+0.1f+P::LadderWidth;
    if(across<inner||across>outer) return false;
    const float head=P::DeckUp(P::GangwayFore);
    if(along<=P::GangwayFore) {up=head;return true;}
    up=head+(along-P::GangwayFore)/ship.ladderRun*(ship.ladderFootUp-head);
    return true;
}
}

float FreighterPlacement::HalfBreadth(const float along) noexcept {
    if(along<0.0f||along>Length) return 0.0f;
    if(along<2.5f) return 3.0f+0.5f*std::sin(along/2.5f*Pi*0.5f);
    if(along<=21.0f) return HalfBeam;
    return HalfBeam*std::sqrt(std::max(0.0f,1.0f-Sq((along-21.0f)/9.0f)));
}

float FreighterPlacement::DeckUp(const float along) noexcept {
    return Depth+0.7f*Sq(std::max(0.0f,along-20.0f)/10.0f)+0.25f*Sq(std::max(0.0f,4.0f-along)/4.0f);
}

FreighterPoint FreighterPlacement::World(const float along,const float up,const float across) const noexcept {
    return {x+dirX*along+AcrossX()*across,baseY+up+along*tanPitch+across*tanHeel,z+dirZ*along+AcrossZ()*across};
}

FreighterPoint FreighterPlacement::Local(const FreighterPoint world) const noexcept {
    const float dx=world.x-x,dz=world.z-z;
    const float along=dx*dirX+dz*dirZ,across=dx*AcrossX()+dz*AcrossZ();
    return {along,world.y-baseY-along*tanPitch-across*tanHeel,across};
}

bool FreighterPlacement::SupportUp(const float along,const float across,const float feetUp,float& up) const noexcept {
    float ladder=0.0f;
    if(OnLadder(*this,along,across,ladder)) {up=ladder;return true;}
    if(along<=0.0f||along>=Length||std::abs(across)>=HalfBreadth(along)) return false;
    float floors[3];int count=0;
    const bool hatch=along>HatchAft&&along<HatchFore&&std::abs(across)<HatchHalf;
    const bool stairwell=along>StairFoot&&along<StairTop&&across>StairInner;
    if(!hatch&&!stairwell) floors[count++]=DeckUp(along);
    if(along>HoldAft&&along<HoldFore) floors[count++]=HoldFloor;
    if(along>=StairFoot&&along<=StairTop&&across>=StairInner)
        floors[count++]=HoldFloor+(along-StairFoot)/(StairTop-StairFoot)*(DeckUp(StairTop)-HoldFloor);
    if(count==0) return false;
    // the highest floor the feet can step onto; failing that, the one they would drop to
    float best=-1e9f,lowest=1e9f;
    for(int i=0;i<count;++i) {
        if(floors[i]<=feetUp+StepReach) best=std::max(best,floors[i]);
        lowest=std::min(lowest,floors[i]);
    }
    up=best>-1e8f?best:lowest;
    return true;
}

bool FreighterPlacement::SupportHeight(const float worldX,const float worldZ,const float feetY,float& height) const noexcept {
    const FreighterPoint local=Local({worldX,feetY,worldZ});
    float up=0.0f;
    if(!SupportUp(local.x,local.z,local.y,up)) return false;
    height=World(local.x,up,local.z).y;
    return true;
}

bool FreighterPlacement::IntersectsCapsule(const FreighterPoint feet,const float radius,const float height) const noexcept {
    if(!std::isfinite(feet.x)||!std::isfinite(feet.y)||!std::isfinite(feet.z)) return true;
    const FreighterPoint local=Local(feet);
    const float along=local.x,across=local.z,low=local.y,high=local.y+height;
    if(along<-radius-1.0f||along>Length+radius+1.0f||std::abs(across)>HalfBeam+LadderWidth+radius+1.0f) return false;
    for(const Wall& w:Walls()) {
        float t=0.0f;
        if(SegmentDistance(along,across,w,t)>=radius) continue;
        const float base=w.onDeck?DeckUp(w.a0+(w.a1-w.a0)*t):0.0f;
        if(low<base+w.high&&high>base+w.low) return true;
    }
    for(const Post& post:Posts()) {
        if(std::hypot(along-post.along,across-post.across)>=radius+post.radius) continue;
        const float base=DeckUp(post.along);
        if(low<base+post.high&&high>base+post.low) return true;
    }
    // the ladder's outer railing and the end of its head's landing
    const float outer=HalfBeam+0.1f+LadderWidth;
    const Wall rails[2]={{GangwayAft,outer,LadderFootAlong(),outer,-6.0f,DeckUp(GangwayFore)+Bulwark,false},
                         {GangwayAft,HalfBeam,GangwayAft,outer,-6.0f,DeckUp(GangwayFore)+Bulwark,false}};
    for(const Wall& w:rails) {
        float t=0.0f;
        if(SegmentDistance(along,across,w,t)<radius&&low<w.high&&high>w.low) return true;
    }
    return false;
}

float FreighterPlacement::SafeTravelFraction(const FreighterPoint from,const FreighterPoint to,
                                             const float radius,const float height) const noexcept {
    const float distance=std::hypot(to.x-from.x,to.z-from.z);
    if(!std::isfinite(distance)||distance>1000.0f) return 0.0f;
    // already caught (a teleport): let it move only out of the walls
    if(IntersectsCapsule(from,radius,height)) return IntersectsCapsule(to,radius,height)?0.0f:1.0f;
    const int steps=std::max(1,static_cast<int>(std::ceil(distance/0.08f)));
    float previous=0.0f;
    for(int i=1;i<=steps;++i) {
        const float current=static_cast<float>(i)/static_cast<float>(steps);
        const FreighterPoint point{std::lerp(from.x,to.x,current),std::lerp(from.y,to.y,current),std::lerp(from.z,to.z,current)};
        if(!IntersectsCapsule(point,radius,height)) {previous=current;continue;}
        float low=previous,high=current;
        for(int pass=0;pass<12;++pass) {
            const float middle=(low+high)*0.5f;
            const FreighterPoint probe{std::lerp(from.x,to.x,middle),std::lerp(from.y,to.y,middle),std::lerp(from.z,to.z,middle)};
            if(IntersectsCapsule(probe,radius,height)) high=middle; else low=middle;
        }
        return low;
    }
    return 1.0f;
}

bool FreighterPlacement::Blocks(const float worldX,const float worldZ,const float margin) const noexcept {
    const FreighterPoint local=Local({worldX,0.0f,worldZ});
    const float along=local.x,across=local.z;
    if(along>-margin&&along<Length+margin
        &&std::abs(across)<HalfBreadth(std::clamp(along,0.0f,Length))+margin+(along<0.0f||along>Length?0.0f:0.0f))
        return true;
    return along>GangwayAft-margin&&along<LadderFootAlong()+margin
        &&across>0.0f&&across<HalfBeam+0.1f+LadderWidth+margin;
}

std::vector<CircularObstacle> FreighterPlacement::Obstacles() const {
    std::vector<CircularObstacle> circles;
    for(float along=1.5f;along<=28.6f;along+=2.6f) {
        const FreighterPoint p=World(along,0.0f,0.0f);
        circles.push_back({p.x,p.z,HalfBreadth(along)+0.2f});
    }
    for(float along=GangwayAft+0.6f;along<LadderFootAlong();along+=1.2f) {
        const FreighterPoint p=World(along,0.0f,HalfBeam+0.1f+LadderWidth*0.5f);
        circles.push_back({p.x,p.z,0.8f});
    }
    return circles;
}

FreighterPoint FreighterPlacement::LadderFoot() const noexcept {
    const float along=LadderFootAlong()+0.9f,across=HalfBeam+0.1f+LadderWidth*0.5f;
    return World(along,ladderFootUp,across);
}

std::optional<FreighterPlacement> FreighterLayout::FindInWorld(const IslandTerrain& terrain,
        const std::span<const CircularObstacle> obstacles,const std::span<const std::array<float,2>> avoid,
        const float keepAway) {
    constexpr int Steps=240;
    constexpr float Tau=2.0f*Pi;
    const float tanTrim=std::tan(5.0f*Pi/180.0f),tanList=std::tan(2.2f*Pi/180.0f),tanLadder=std::tan(34.0f*Pi/180.0f);
    const auto sandy=[&](const FreighterPoint& p) {
        const TerrainSample s=terrain.Sample(p.x,p.z);
        return (s.surface==TerrainSurface::DrySand||s.surface==TerrainSurface::WetSand)&&s.slopeRadians<0.25f;
    };
    for(int step=0;step<Steps;++step) {
        const float angle=2.4f+Tau*static_cast<float>(step)/static_cast<float>(Steps);
        const float shore=terrain.ShoreRadius(angle);
        const float sx=std::cos(angle)*shore,sz=std::sin(angle)*shore;
        // straight up the beach: across the shoreline where it lies
        float gx=terrain.SignedShoreDistance(sx+1.5f,sz)-terrain.SignedShoreDistance(sx-1.5f,sz);
        float gz=terrain.SignedShoreDistance(sx,sz+1.5f)-terrain.SignedShoreDistance(sx,sz-1.5f);
        const float g=std::hypot(gx,gz);
        if(g<1e-4f) continue;
        gx/=g;gz/=g;
        FreighterPlacement ship;
        ship.dirX=gx;ship.dirZ=gz;
        ship.x=sx+gx*(BowInland-P::Length);ship.z=sz+gz*(BowInland-P::Length);
        const auto centre=ship.World(P::Length*0.5f,0.0f,0.0f);
        bool far=true;
        for(const auto& point:avoid) far=far&&std::hypot(point[0]-centre.x,point[1]-centre.z)>keepAway;
        if(!far) continue;
        // the bow on a broad, gently sloping beach
        bool beach=true;
        for(const float along:{20.0f,23.0f,26.0f,29.0f,32.0f})
            for(const float across:{-3.0f,0.0f,3.0f}) beach=beach&&sandy(ship.World(along,0.0f,across));
        if(!beach) continue;
        // the stern in the surf: the keel there rests on the sea floor, the bow's dug into the sand
        const float stern=terrain.Height(ship.World(4.0f,0.0f,0.0f).x,ship.World(4.0f,0.0f,0.0f).z);
        if(stern>-0.3f||stern<-3.5f) continue;
        const float bow=terrain.Height(ship.World(27.0f,0.0f,0.0f).x,ship.World(27.0f,0.0f,0.0f).z)-0.55f;
        ship.tanPitch=std::clamp((bow-stern)/23.0f,0.0f,tanTrim);
        // the flat bottom bedded in the sand all along the beach, its sides too
        ship.baseY=bow-27.0f*ship.tanPitch;
        for(float along=14.0f;along<=27.0f;along+=1.0f)
            for(const float across:{-2.5f,0.0f,2.5f}) {
                const FreighterPoint p=ship.World(along,0.0f,across);
                const float ground=terrain.Height(p.x,p.z);
                if(ground<0.0f) continue;   // the bottom under the sea is the sea's to hide
                ship.baseY=std::min(ship.baseY,ground-0.45f-along*ship.tanPitch-across*tanList);
            }
        ship.tanHeel=tanList;
        // the hold's floor dry above the sea
        if(ship.World(P::HoldAft,P::HoldFloor,-P::HalfBeam).y<0.25f) continue;
        // the ladder's foot on dry sand, its flight no steeper than a stair
        bool ladder=false;
        for(int pass=0;pass<3;++pass) {
            const float across=P::HalfBeam+0.1f+P::LadderWidth*0.5f;
            const FreighterPoint foot=ship.World(ship.LadderFootAlong(),0.0f,across);
            const float ground=terrain.Height(foot.x,foot.z);
            ship.ladderFootUp=ship.Local({foot.x,ground,foot.z}).y;
            const float rise=P::DeckUp(P::GangwayFore)-ship.ladderFootUp;
            ship.ladderRun=std::clamp(rise/tanLadder,4.5f,7.4f);
            const FreighterPoint landing=ship.World(ship.LadderFootAlong()+0.9f,0.0f,across);
            ladder=ground>0.12f&&rise/ship.ladderRun<=0.70f&&sandy(foot)&&terrain.Height(landing.x,landing.z)>0.1f;
        }
        if(!ladder) continue;
        // nothing solid under or round it
        bool clear=true;
        for(const CircularObstacle& obstacle:obstacles) {
            const FreighterPoint local=ship.Local({obstacle.x,0.0f,obstacle.z});
            const float reach=obstacle.radius+1.5f;
            if(local.x>-reach&&local.x<P::Length+reach
                &&std::abs(local.z)<P::HalfBreadth(std::clamp(local.x,0.0f,P::Length))+reach+(local.z>0.0f?P::LadderWidth+0.1f:0.0f)) {
                clear=false;break;
            }
        }
        if(!clear) continue;
        return ship;
    }
    return std::nullopt;
}
}
