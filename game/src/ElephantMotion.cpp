#include "IslandSimulator/ElephantMotion.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
float Approach(const float value,const float target,const float step) {
    return value<target?std::min(target,value+step):std::max(target,value-step);
}
float Smooth(const float t) {const float k=std::clamp(t,0.0f,1.0f);return k*k*(3.0f-2.0f*k);}
// the lateral walk: left hind, left fore, right hind, right fore, a quarter cycle apart
// (legs in the order left fore, right fore, left hind, right hind)
constexpr std::array<float,4> LegOffset{{.25f,.75f,0.0f,.5f}};
constexpr float Duty=.65f;
}

ElephantMotion::ElephantMotion(const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles):terrain_(terrain) {
    // its round: the coastal flats behind the upper beach, inland of the tyrannosaur's round; a
    // point every 7.5 degrees, as near 26 m in from the water as the ground and the trees allow
    for(int k=0;k<48;++k) {
        const float bearing=2.0f*Pi*static_cast<float>(k)/48.0f;
        const float shore=terrain.ShoreRadius(bearing);
        const float c=std::cos(bearing),s=std::sin(bearing);
        for(const float in:{26.0f,30.0f,22.0f,34.0f,38.0f,19.0f,42.0f,46.0f}) {
            const float x=c*(shore-in),z=s*(shore-in);
            const auto sample=terrain.Sample(x,z);
            if(sample.height<1.0f||sample.slopeRadians>.28f) continue;
            bool clear=true;
            for(const auto& o:obstacles) clear=clear&&std::hypot(o.x-x,o.z-z)>o.radius+3.2f;
            if(!clear) continue;
            route_.push_back({x,sample.height,z});
            break;
        }
    }
    if(route_.size()<2) route_={{0,terrain.Height(0,0),0},{10,terrain.Height(10,0),0}};
    // the trees and rocks near its round, which it walks round rather than through
    for(const auto& o:obstacles) {
        bool near=false;
        for(std::size_t i=0;i<route_.size()&&!near;++i) {
            const ElephantPoint& a=route_[i];const ElephantPoint& b=route_[(i+1)%route_.size()];
            const float abx=b.x-a.x,abz=b.z-a.z,length=std::max(1e-3f,abx*abx+abz*abz);
            const float t=std::clamp(((o.x-a.x)*abx+(o.z-a.z)*abz)/length,0.0f,1.0f);
            near=std::hypot(a.x+abx*t-o.x,a.z+abz*t-o.z)<o.radius+14.0f;
        }
        if(near) obstacles_.push_back(o);
    }
    const ElephantPoint& start=route_.front();
    state_.x=start.x;state_.z=start.z;state_.y=Ground(start.x,start.z);
    state_.heading=std::atan2(route_[1].z-start.z,route_[1].x-start.x);
    state_.behavior=ElephantBehavior::Walk;
    walkSeconds_=50.0f;
    PoseLegs(0.0f);
}

float ElephantMotion::Ground(const float x,const float z) const noexcept {return terrain_.Height(x,z);}

void ElephantMotion::PlaceNear(const float x,const float z) {
    std::size_t best=0;
    for(std::size_t i=1;i<route_.size();++i)
        if(std::hypot(route_[i].x-x,route_[i].z-z)<std::hypot(route_[best].x-x,route_[best].z-z)) best=i;
    state_.x=route_[best].x;state_.z=route_[best].z;state_.y=Ground(state_.x,state_.z);
    target_=(best+1)%route_.size();
    state_.heading=std::atan2(route_[target_].z-state_.z,route_[target_].x-state_.x);
    state_.speed=0;
    previewHold_=false;goingToDrink_=false;fleeSeconds_=0.0f;
    Begin(ElephantBehavior::Stand,20.0f);
    PoseLegs(0.0f);
}

void ElephantMotion::PreviewTrumpet() {
    Begin(ElephantBehavior::Trumpet,1e6f);
    fleeX_=state_.x+std::cos(state_.heading)*20.0f;fleeZ_=state_.z+std::sin(state_.heading)*20.0f;
    previewHold_=true;
}

bool ElephantMotion::ClearSight(const float x0,const float z0,const float x1,const float z1,const float margin) const noexcept {
    const float abx=x1-x0,abz=z1-z0,length=std::max(1e-3f,abx*abx+abz*abz);
    for(const auto& o:obstacles_) {
        const float t=std::clamp(((o.x-x0)*abx+(o.z-z0)*abz)/length,0.0f,1.0f);
        if(std::hypot(x0+abx*t-o.x,z0+abz*t-o.z)<o.radius+margin) return false;
    }
    return true;
}

ElephantPoint ElephantMotion::ToWorld(const float right,const float up,const float ahead) const noexcept {
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    const float rx=-fz,rz=fx;
    const float cp=std::cos(state_.pitch),sp=std::sin(state_.pitch);
    const float aheadX=fx*cp,aheadY=-sp,aheadZ=fz*cp;
    const float upX=fx*sp,upY=cp,upZ=fz*sp;
    const float cr=std::cos(state_.roll),sr=std::sin(state_.roll);
    const float rightX=rx*cr+upX*sr,rightY=upY*sr,rightZ=rz*cr+upZ*sr;
    const float upX2=upX*cr-rx*sr,upY2=upY*cr,upZ2=upZ*cr-rz*sr;
    return {state_.x+rightX*right+upX2*up+aheadX*ahead,
            state_.y+state_.bodyHeight+rightY*right+upY2*up+aheadY*ahead,
            state_.z+rightZ*right+upZ2*up+aheadZ*ahead};
}

ElephantMotion::HeadFrame ElephantMotion::Head() const noexcept {
    const ElephantPoint pivot=ToWorld(0,.35f,2.15f);
    const ElephantPoint o=ToWorld(0,0,0),r=ToWorld(1,0,0),u=ToWorld(0,1,0),a=ToWorld(0,0,1);
    const auto sub=[](const ElephantPoint& p,const ElephantPoint& q){return ElephantPoint{p.x-q.x,p.y-q.y,p.z-q.z};};
    const ElephantPoint right=sub(r,o),up=sub(u,o),ahead=sub(a,o);
    // turned by the yaw about the body's up, then raised by the pitch about the head's right
    const float cy=std::cos(state_.headYaw),sy=std::sin(state_.headYaw);
    const float cp=std::cos(state_.headPitch),sp=std::sin(state_.headPitch);
    const ElephantPoint yawAhead{ahead.x*cy+right.x*sy,ahead.y*cy+right.y*sy,ahead.z*cy+right.z*sy};
    const ElephantPoint headRight{right.x*cy-ahead.x*sy,right.y*cy-ahead.y*sy,right.z*cy-ahead.z*sy};
    const ElephantPoint headAhead{yawAhead.x*cp+up.x*sp,yawAhead.y*cp+up.y*sp,yawAhead.z*cp+up.z*sp};
    const ElephantPoint headUp{up.x*cp-yawAhead.x*sp,up.y*cp-yawAhead.y*sp,up.z*cp-yawAhead.z*sp};
    return {pivot,headRight,headUp,headAhead};
}

std::array<ElephantPoint,16> ElephantMotion::TrunkPoints() const noexcept {
    const HeadFrame h=Head();
    const auto head=[&](const float x,const float y,const float z) {
        return ElephantPoint{h.pivot.x+h.right.x*x+h.up.x*y+h.ahead.x*z,h.pivot.y+h.right.y*x+h.up.y*y+h.ahead.y*z,
                             h.pivot.z+h.right.z*x+h.up.z*y+h.ahead.z*z};
    };
    std::array<ElephantPoint,16> points{};
    points[0]=head(0,-.42f,1.02f);
    // the trunk bends in the upright plane through the head's heading: its angle from straight
    // down toward ahead, along its length, a blend of hanging, raised, curled back to the mouth
    // and reaching forward to the ground
    const float hx=h.ahead.x,hz=h.ahead.z,hl=std::max(1e-4f,std::hypot(hx,hz));
    const float fx=hx/hl,fz=hz/hl;
    const float lift=std::clamp(state_.trunkLift,0.0f,1.0f),curl=std::clamp(state_.trunkCurl,0.0f,1.0f);
    const float reach=std::clamp(state_.trunkReach,0.0f,1.0f);
    const float hang=std::max(0.0f,1.0f-lift-curl-reach);
    const float pitchLean=-.6f*std::min(0.0f,state_.headPitch)+.5f*std::max(0.0f,state_.headPitch);
    constexpr float Segment=TrunkLength/15.0f;
    for(std::size_t i=1;i<points.size();++i) {
        const float s=(static_cast<float>(i)-.5f)/15.0f;
        const float hanging=.16f+.10f*s+.75f*s*s*s*s+pitchLean*(1.0f-s);
        const float raised=.85f+2.5f*s;
        const float curled=.25f-5.0f*std::pow(s,1.6f);
        const float reaching=.55f+.25f*s;
        const float angle=hang*hanging+lift*raised+curl*curled+reach*reaching;
        const float swing=state_.trunkSwing*s;
        const float sx=fx*std::cos(swing)+fz*std::sin(swing),sz=fz*std::cos(swing)-fx*std::sin(swing);
        const ElephantPoint& p=points[i-1];
        ElephantPoint q{p.x+sx*std::sin(angle)*Segment,p.y-std::cos(angle)*Segment,p.z+sz*std::sin(angle)*Segment};
        // the trunk lies along the ground where it meets it
        q.y=std::max(q.y,Ground(q.x,q.z)+.07f);
        points[i]=q;
    }
    return points;
}

std::array<CircularObstacle,3> ElephantMotion::Obstacles() const noexcept {
    const ElephantPoint rear=ToWorld(0,0,-1.1f),front=ToWorld(0,0,.9f),head=ToWorld(0,0,2.7f);
    return {{{rear.x,rear.z,1.05f},{front.x,front.z,1.1f},{head.x,head.z,.7f}}};
}

void ElephantMotion::Begin(const ElephantBehavior what,const float seconds) {
    state_.behavior=what;
    activitySeconds_=seconds;
    if(what==ElephantBehavior::Trumpet) {++calls_;lastCall_=1;}
}

void ElephantMotion::Update(const float seconds,const float observerX,const float observerZ,const bool observerValid,
                            const float nightness,const float rivalX,const float rivalZ,const bool rivalValid) {
    const float dt=std::isfinite(seconds)?std::clamp(seconds,0.0f,0.1f):0.0f;
    clock_+=dt;
    drinkCooldown_=std::max(0.0f,drinkCooldown_-dt);
    noticeCooldown_=std::max(0.0f,noticeCooldown_-dt);
    trumpetCooldown_=std::max(0.0f,trumpetCooldown_-dt);
    const float ox=observerX-state_.x,oz=observerZ-state_.z;
    const float observerDistance=observerValid&&std::isfinite(ox)&&std::isfinite(oz)?std::hypot(ox,oz):1e9f;
    const float rx=rivalX-state_.x,rz=rivalZ-state_.z;
    const float rivalDistance=rivalValid&&std::isfinite(rx)&&std::isfinite(rz)?std::hypot(rx,rz):1e9f;
    const float night=std::isfinite(nightness)?std::clamp(nightness,0.0f,1.0f):0.0f;
    ElephantBehavior& b=state_.behavior;
    const auto onward=[&] {
        if(night>.6f) Begin(ElephantBehavior::Doze,90.0f);
        else {b=ElephantBehavior::Walk;walkSeconds_=40.0f+60.0f*Hash(++decisions_*19U);}
    };
    if(holdSeconds_>0.0f) {
        holdSeconds_-=dt;
        if(b==ElephantBehavior::Walk) b=ElephantBehavior::Stand;
    } else if(!previewHold_) {
        activitySeconds_-=dt;
        fleeSeconds_=std::max(0.0f,fleeSeconds_-dt);
        if(b!=ElephantBehavior::Trumpet&&trumpetCooldown_<=0.0f&&(observerDistance<TrumpetDistance||rivalDistance<35.0f)) {
            // too close, or the tyrannosaur near: ears out, trunk up, a trumpet; then it moves off
            const bool rival=rivalDistance<35.0f&&rivalDistance<observerDistance*2.0f;
            fleeX_=rival?rivalX:observerX;fleeZ_=rival?rivalZ:observerZ;
            Begin(ElephantBehavior::Trumpet,2.8f);
            trumpetCooldown_=45.0f;
        } else if(b==ElephantBehavior::Trumpet) {
            if(activitySeconds_<=0.0f) {b=ElephantBehavior::Walk;walkSeconds_=25.0f;fleeSeconds_=22.0f;goingToDrink_=false;}
        } else if(b==ElephantBehavior::Doze) {
            if((night<.3f&&activitySeconds_<=0.0f)||observerDistance<15.0f) Begin(ElephantBehavior::Watch,6.0f);
        } else if(observerDistance<WatchDistance&&fleeSeconds_<=0.0f&&noticeCooldown_<=0.0f
                  &&(b==ElephantBehavior::Walk||b==ElephantBehavior::Stand||b==ElephantBehavior::Graze)) {
            // the walker near: it stops, turns to face and raises its trunk to the scent
            Begin(ElephantBehavior::Watch,7.0f+6.0f*Hash(++decisions_));
            noticeCooldown_=50.0f;
            if(Hash(decisions_*29U)<.5f) {++calls_;lastCall_=0;}
        } else if(b==ElephantBehavior::Walk) {
            walkSeconds_-=dt;
            if(hasDrink_&&!goingToDrink_&&drinkCooldown_<=0.0f&&fleeSeconds_<=0.0f&&std::hypot(drinkX_-state_.x,drinkZ_-state_.z)<30.0f)
                goingToDrink_=true;
            if(goingToDrink_&&std::hypot(drinkX_-state_.x,drinkZ_-state_.z)<4.5f) {
                goingToDrink_=false;drinkCooldown_=300.0f;
                Begin(ElephantBehavior::Drink,21.0f);
            } else if(walkSeconds_<=0.0f&&!goingToDrink_&&fleeSeconds_<=0.0f) {
                if(night>.6f) Begin(ElephantBehavior::Doze,90.0f);
                else {
                    const float pick=Hash(++decisions_*13U);
                    if(pick<.40f) Begin(ElephantBehavior::Graze,18.0f+12.0f*Hash(decisions_*17U));
                    else if(pick<.62f) Begin(ElephantBehavior::Dust,10.0f);
                    else {
                        Begin(ElephantBehavior::Stand,8.0f+10.0f*Hash(decisions_*23U));
                        if(pick>.85f) {++calls_;lastCall_=0;}
                    }
                }
            }
        } else if(b==ElephantBehavior::Watch&&observerDistance>WatchDistance*1.3f&&activitySeconds_>1.0f) {
            activitySeconds_=1.0f;
        } else if(activitySeconds_<=0.0f) {
            onward();
        }
    }
    const bool walking=b==ElephantBehavior::Walk;
    // steering: its round, the drinking place, or away from what it trumpeted at; facing the
    // walker it watches; round the trees and rocks in its way
    if(std::hypot(route_[target_].x-state_.x,route_[target_].z-state_.z)<5.0f) target_=(target_+1)%route_.size();
    float wantX=route_[target_].x-state_.x,wantZ=route_[target_].z-state_.z;
    if(goingToDrink_||b==ElephantBehavior::Drink) {wantX=drinkX_-state_.x;wantZ=drinkZ_-state_.z;}
    if(fleeSeconds_>0.0f) {wantX=state_.x-fleeX_;wantZ=state_.z-fleeZ_;}
    {
        const float l=std::max(1e-3f,std::hypot(wantX,wantZ));
        wantX/=l;wantZ/=l;
    }
    if(walking) {
        for(const auto& o:obstacles_) {
            const float dx=state_.x-o.x,dz=state_.z-o.z,d=std::hypot(dx,dz),clear=o.radius+3.5f;
            if(d<clear&&d>1e-3f) {const float k=(clear-d)/clear*1.6f;wantX+=dx/d*k;wantZ+=dz/d*k;}
        }
    }
    float want=walking?std::atan2(wantZ,wantX):state_.heading;
    if(b==ElephantBehavior::Watch) want=std::atan2(oz,ox);
    if(b==ElephantBehavior::Trumpet) want=std::atan2(fleeZ_-state_.z,fleeX_-state_.x);
    if(b==ElephantBehavior::Drink&&hasDrink_) want=std::atan2(drinkZ_-state_.z,drinkX_-state_.x);
    const float turn=std::remainder(want-state_.heading,2.0f*Pi);
    const float rate=walking?.38f:b==ElephantBehavior::Trumpet?.5f:.25f;
    state_.heading+=std::clamp(turn,-rate*dt,rate*dt);
    // it turns on the spot before it sets off, rather than walking off sideways
    const float aligned=walking?std::clamp(1.0f-(std::abs(turn)-.6f)/.8f,.25f,1.0f):0.0f;
    state_.speed=Approach(state_.speed,WalkSpeed*aligned*(fleeSeconds_>0.0f?1.25f:1.0f),(walking?.3f:.6f)*dt);
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    state_.x+=fx*state_.speed*dt;state_.z+=fz*state_.speed*dt;
    // never inside a trunk or a rock
    for(const auto& o:obstacles_) {
        const float dx=state_.x-o.x,dz=state_.z-o.z,d=std::hypot(dx,dz),clear=o.radius+1.15f;
        if(d<clear&&d>1e-3f) {state_.x=o.x+dx/d*clear;state_.z=o.z+dz/d*clear;}
    }
    state_.y=Ground(state_.x,state_.z);
    distance_+=state_.speed*dt;
    const float pace=std::clamp(state_.speed/WalkSpeed,0.0f,1.3f);
    const float phase=distance_/StrideLength;
    state_.doze=Approach(state_.doze,b==ElephantBehavior::Doze?1.0f:0.0f,.2f*dt);
    const float doze=Smooth(state_.doze);
    // the body vaults over each stiff leg: highest as the legs pass under it, a gentle roll
    // onto the planted side, pitched with the ground
    const float stride=std::min(pace,1.0f);
    state_.bodyHeight=BodyHeight-.045f*stride*(.5f+.5f*std::cos(4.0f*Pi*phase))-.04f*doze;
    state_.roll=.025f*stride*std::sin(2.0f*Pi*phase);
    const float ahead=Ground(state_.x+fx*1.5f,state_.z+fz*1.5f),behind=Ground(state_.x-fx*1.5f,state_.z-fz*1.5f);
    state_.pitch=std::clamp(std::atan2(behind-ahead,3.0f)*.7f,-.18f,.18f);
    state_.tailSwing=.25f*std::sin(clock_*1.3f+.5f*std::sin(clock_*.37f))*(1.0f-.8f*doze)+.08f*stride*std::sin(2.0f*Pi*phase);
    // the head, the trunk and the ears for what it is doing
    float headYaw=.18f*std::sin(clock_*.21f)*(1.0f-stride),headPitch=-.10f+.03f*std::sin(2.0f*Pi*phase*2.0f)*stride;
    float lift=0.0f,curl=0.0f,reach=0.0f,swing=.10f*std::sin(2.0f*Pi*phase)*stride+.12f*std::sin(clock_*.7f)*(1.0f-stride);
    // the ears fanned against the heat, slowly walking, briskly standing in the day
    float ears=.18f+.12f*std::sin(clock_*1.6f);
    if(!walking&&night<.5f) ears=.35f+.32f*std::sin(clock_*3.4f);
    switch(b) {
    case ElephantBehavior::Graze: {
        // a cycle of six seconds: down to the grass, the trunk wrapping and pulling a tuft, then
        // curled up to the mouth
        const float t=std::fmod(clock_,6.0f);
        const float down=Smooth(t/1.2f)*(1.0f-Smooth((t-2.6f)/1.0f));
        const float up=Smooth((t-2.6f)/1.0f)*(1.0f-Smooth((t-4.9f)/.9f));
        headPitch=-.30f*down+.06f*up-.05f;
        reach=down;curl=.85f*up;
        swing=.25f*std::sin(clock_*4.0f)*down;
        break;
    }
    case ElephantBehavior::Drink: {
        // the trunk into the water, drawing it up, then curled to the mouth to empty it
        const float t=std::fmod(clock_,7.0f);
        const float down=Smooth(t/1.5f)*(1.0f-Smooth((t-3.4f)/1.0f));
        const float up=Smooth((t-3.4f)/1.0f)*(1.0f-Smooth((t-5.9f)/.9f));
        headPitch=-.26f*down+.18f*up-.05f;
        reach=.9f*down;curl=.95f*up;
        swing=0.0f;
        break;
    }
    case ElephantBehavior::Dust: {
        // a scoop of sand from the ground, then flung up over the head onto the back, one side
        // and then the other
        const float t=std::fmod(clock_,5.0f);
        const float down=Smooth(t/1.0f)*(1.0f-Smooth((t-1.6f)/.5f));
        const float toss=Smooth((t-1.6f)/.5f)*(1.0f-Smooth((t-3.0f)/1.2f));
        headPitch=-.25f*down+.2f*toss-.05f;
        reach=down;lift=.9f*toss;
        swing=(std::fmod(clock_,10.0f)<5.0f?.7f:-.7f)*toss;
        break;
    }
    case ElephantBehavior::Watch:
        headYaw=std::clamp(std::remainder(std::atan2(oz,ox)-state_.heading,2.0f*Pi),-.5f,.5f);
        headPitch=.10f;
        // the trunk raised in an S, its tip turned toward the scent
        lift=.38f+.06f*std::sin(clock_*2.3f);curl=.12f;
        swing=.12f*std::sin(clock_*1.1f);
        ears=.80f+.08f*std::sin(clock_*1.3f);
        break;
    case ElephantBehavior::Trumpet: {
        const float k=previewHold_?1.0f:Smooth((2.8f-activitySeconds_)/.5f);
        headPitch=-.10f+.40f*k;
        lift=k;swing=0.0f;
        ears=.25f+.75f*k;
        break;
    }
    case ElephantBehavior::Doze:
        headPitch=-.10f-.22f*doze;
        reach=.35f*doze;
        ears=.10f;
        swing=.04f*std::sin(clock_*.3f);
        break;
    default: break;
    }
    state_.headYaw=Approach(state_.headYaw,headYaw,.6f*dt);
    state_.headPitch=Approach(state_.headPitch,headPitch,(b==ElephantBehavior::Trumpet?1.4f:.5f)*dt);
    const float trunkRate=(b==ElephantBehavior::Trumpet?2.2f:1.4f)*dt;
    state_.trunkLift=Approach(state_.trunkLift,lift,trunkRate);
    state_.trunkCurl=Approach(state_.trunkCurl,curl,trunkRate);
    state_.trunkReach=Approach(state_.trunkReach,reach,trunkRate);
    state_.trunkSwing=Approach(state_.trunkSwing,swing,1.2f*dt);
    state_.earSpread=Approach(state_.earSpread,std::clamp(ears,0.0f,1.0f),2.4f*dt);
    state_.breath+=dt*1.1f;
    // now and then a rumble, standing about by day
    rumbleClock_+=dt;
    if(!walking&&b!=ElephantBehavior::Trumpet&&b!=ElephantBehavior::Doze&&rumbleClock_>40.0f) {
        rumbleClock_=0.0f;
        if(Hash(++decisions_*31U)<.5f) {++calls_;lastCall_=0;}
    }
    PoseLegs(phase);
    for(Footprint& print:prints_) {
        if(!print.active) continue;
        print.age+=dt;
        print.density=std::clamp(1.0f-print.age/print.life,0.0f,1.0f);
        if(print.density<=0.0f) print.active=false;
    }
}

void ElephantMotion::PoseLegs(const float phase) {
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    const float rx=-fz,rz=fx;
    const float pace=std::clamp(state_.speed/WalkSpeed,0.0f,1.0f);
    for(std::size_t i=0;i<4;++i) {
        const bool front=i<2;
        const float side=i%2==0?-1.0f:1.0f;
        ElephantLeg& leg=state_.legs[i];
        float u=std::fmod(phase+LegOffset[i],1.0f);
        if(u<0.0f) u+=1.0f;
        float along=0.0f,lift=0.0f,flex=0.0f;
        if(u<Duty) {along=StrideLength*(Duty*.5f-u);leg.planted=true;}
        else {
            const float k=(u-Duty)/(1.0f-Duty);
            along=StrideLength*(-Duty*.5f+Duty*Smooth(k));
            lift=.20f*std::sin(Pi*k);
            flex=std::sin(Pi*std::min(1.0f,k*1.25f));
            leg.planted=false;
        }
        along*=pace;lift*=pace;flex*=pace;
        if(pace<.05f) leg.planted=true;
        const float base=front?ShoulderAhead+.05f:-HipBehind;
        const float across=front?.47f:.44f;
        const float px=state_.x+fx*(base+along)+rx*side*across,pz=state_.z+fz*(base+along)+rz*side*across;
        leg.foot={px,Ground(px,pz)+lift,pz};
        // the wrist (or hock) over the foot's pillar; as the foot swings it hangs back from the
        // bent joint
        const float high=front?WristHeight:HockHeight;
        const float trail=front?.16f*flex:.10f*flex;
        leg.low={px+fx*(trail-(front?0.0f:.05f)),leg.foot.y+high-.06f*flex,pz+fz*(trail-(front?0.0f:.05f))};
        leg.top=front?ToWorld(side*across,.08f,ShoulderAhead):ToWorld(side*across,-.02f,-HipBehind);
        const float upper=front?UpperArmLength:FemurLength,lower=front?ForearmLength:TibiaLength;
        float dx=leg.low.x-leg.top.x,dy=leg.low.y-leg.top.y,dz=leg.low.z-leg.top.z;
        float d=std::sqrt(dx*dx+dy*dy+dz*dz);
        const float reach=upper+lower-.01f;
        if(d>reach) {
            const float k=reach/d;
            dx*=k;dy*=k;dz*=k;
            leg.low={leg.top.x+dx,leg.top.y+dy,leg.top.z+dz};
            d=reach;
        }
        const float ux=dx/d,uy=dy/d,uz=dz/d;
        const float a=(upper*upper-lower*lower+d*d)/(2.0f*d);
        const float h=std::sqrt(std::max(0.0f,upper*upper-a*a));
        // the elbow bends back, the knee forward
        const float bend=front?-1.0f:1.0f;
        const float dot=fx*ux+fz*uz;
        float bx=(fx-ux*dot)*bend,by=(-uy*dot)*bend,bz=(fz-uz*dot)*bend;
        const float bl=std::max(1e-4f,std::sqrt(bx*bx+by*by+bz*bz));
        bx/=bl;by/=bl;bz/=bl;
        leg.middle={leg.top.x+ux*a+bx*h,leg.top.y+uy*a+by*h,leg.top.z+uz*a+bz*h};
        if(leg.planted&&!wasPlanted_[i]&&state_.speed>.25f) {
            ++footfalls_;
            Footprint& print=prints_[nextPrint_];
            nextPrint_=(nextPrint_+1)%prints_.size();
            print={};
            print.x=px;print.z=pz;
            print.headingRadians=state_.heading;
            print.density=1.0f;print.left=side<0.0f;print.active=true;
            print.kind=front?3U:4U;
            print.life=900.0f;
        }
        wasPlanted_[i]=leg.planted;
    }
}

}
