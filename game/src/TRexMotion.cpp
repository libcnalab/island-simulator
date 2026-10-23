#include "IslandSimulator/TRexMotion.hpp"
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
}

TRexMotion::TRexMotion(const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles):terrain_(terrain) {
    // its round: the open upper beach all the way round, where a 12 m animal has room; a point
    // every five degrees, as near 16 m in from the water as the ground allows
    for(int k=0;k<72;++k) {
        const float bearing=2.0f*Pi*static_cast<float>(k)/72.0f;
        const float shore=terrain.ShoreRadius(bearing);
        const float c=std::cos(bearing),s=std::sin(bearing);
        bool found=false;
        for(const float in:{16.0f,13.0f,19.0f,10.0f,22.0f,26.0f,30.0f}) {
            const float x=c*(shore-in),z=s*(shore-in);
            const auto sample=terrain.Sample(x,z);
            if(sample.height<0.6f||sample.slopeRadians>.32f) continue;
            bool clear=true;
            for(const auto& o:obstacles) clear=clear&&std::hypot(o.x-x,o.z-z)>o.radius+2.6f;
            if(!clear) continue;
            route_.push_back({x,sample.height,z});
            found=true;
            break;
        }
        (void)found;
    }
    if(route_.size()<2) route_={{0,terrain.Height(0,0),0},{10,terrain.Height(10,0),0}};
    const TRexPoint& start=route_.front();
    state_.x=start.x;state_.z=start.z;state_.y=Ground(start.x,start.z);
    const TRexPoint& next=route_[1];
    state_.heading=std::atan2(next.z-start.z,next.x-start.x);
    state_.behavior=TRexBehavior::Walk;
    walkSeconds_=40.0f;
    PoseLegs(0.0f);
}

float TRexMotion::Ground(const float x,const float z) const noexcept {return terrain_.Height(x,z);}

void TRexMotion::PlaceNear(const float x,const float z) {
    std::size_t best=0;
    for(std::size_t i=1;i<route_.size();++i)
        if(std::hypot(route_[i].x-x,route_[i].z-z)<std::hypot(route_[best].x-x,route_[best].z-z)) best=i;
    state_.x=route_[best].x;state_.z=route_[best].z;state_.y=Ground(state_.x,state_.z);
    target_=(best+1)%route_.size();
    state_.heading=std::atan2(route_[target_].z-state_.z,route_[target_].x-state_.x);
    state_.speed=0;
    previewHold_=false;
    BeginStand(TRexBehavior::Stand,20.0f);
    PoseLegs(0.0f);
}

TRexPoint TRexMotion::ToWorld(const float right,const float up,const float ahead) const noexcept {
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    const float rx=-fz,rz=fx;
    const float cp=std::cos(state_.pitch),sp=std::sin(state_.pitch);
    // pitched about the right axis (nose down positive), rolled about the forward one
    const float aheadX=fx*cp,aheadY=-sp,aheadZ=fz*cp;
    const float upX=fx*sp,upY=cp,upZ=fz*sp;
    const float cr=std::cos(state_.roll),sr=std::sin(state_.roll);
    const float rightX=rx*cr+upX*sr,rightY=upY*sr,rightZ=rz*cr+upZ*sr;
    const float upX2=upX*cr-rx*sr,upY2=upY*cr,upZ2=upZ*cr-rz*sr;
    return {state_.x+rightX*right+upX2*up+aheadX*ahead,
            state_.y+state_.hipHeight+rightY*right+upY2*up+aheadY*ahead,
            state_.z+rightZ*right+upZ2*up+aheadZ*ahead};
}

std::array<CircularObstacle,3> TRexMotion::Obstacles() const noexcept {
    const TRexPoint hips=ToWorld(0,0,0),chest=ToWorld(0,0,2.0f),tail=ToWorld(0,0,-3.2f);
    return {{{hips.x,hips.z,BodyRadius},{chest.x,chest.z,1.1f},{tail.x,tail.z,.8f}}};
}

void TRexMotion::BeginStand(const TRexBehavior what,const float seconds) {
    state_.behavior=what;
    activitySeconds_=seconds;
    if(what==TRexBehavior::Roar) {
        ++calls_;
        lastCall_=Hash(calls_*31U+5U)<.6f?1:2;
    }
}

void TRexMotion::PreviewRoar(const bool boom) {
    BeginStand(TRexBehavior::Roar,1e6f);
    lastCall_=boom?2:1;
    previewHold_=true;
}

void TRexMotion::PreviewRest() {
    BeginStand(TRexBehavior::Rest,1e6f);
    state_.lie=1.0f;
    previewHold_=true;
}

void TRexMotion::Update(const float seconds,const float observerX,const float observerZ,const bool observerValid,
                        const float nightness) {
    const float dt=std::isfinite(seconds)?std::clamp(seconds,0.0f,0.1f):0.0f;
    clock_+=dt;
    drinkCooldown_=std::max(0.0f,drinkCooldown_-dt);
    followCooldown_=std::max(0.0f,followCooldown_-dt);
    noticeCooldown_=std::max(0.0f,noticeCooldown_-dt);
    const float ox=observerX-state_.x,oz=observerZ-state_.z;
    const float observerDistance=observerValid&&std::isfinite(ox)&&std::isfinite(oz)?std::hypot(ox,oz):1e9f;
    const float night=std::isfinite(nightness)?std::clamp(nightness,0.0f,1.0f):0.0f;
    TRexBehavior& b=state_.behavior;
    if(holdSeconds_>0.0f) {
        holdSeconds_-=dt;
        if(b==TRexBehavior::Walk||b==TRexBehavior::Follow) b=TRexBehavior::Stand;
    } else if(!previewHold_) {
        activitySeconds_-=dt;
        const bool moving=b==TRexBehavior::Walk||b==TRexBehavior::Follow;
        if(b==TRexBehavior::Rest) {
            // up at dawn, or when the walker comes near
            if((night<0.3f&&activitySeconds_<=0.0f)||observerDistance<18.0f) {b=TRexBehavior::Stand;activitySeconds_=4.0f;}
        } else if(b==TRexBehavior::Walk&&observerDistance<40.0f&&noticeCooldown_<=0.0f) {
            // walking, it notices the walker within 40 m: it stops to look
            BeginStand(TRexBehavior::Watch,6.0f+6.0f*Hash(++decisions_));
            noticeCooldown_=45.0f;
        } else if(observerDistance<WatchDistance&&!moving&&b!=TRexBehavior::Roar&&b!=TRexBehavior::Drink) {
            // the walker near: it turns to watch, and now and then calls at it
            if(b!=TRexBehavior::Watch) BeginStand(TRexBehavior::Watch,6.0f+6.0f*Hash(++decisions_));
            if(activitySeconds_<=0.0f) {
                const float pick=Hash(++decisions_*7U);
                if(pick<.30f) BeginStand(TRexBehavior::Roar,3.4f);
                else if(pick<.55f&&followCooldown_<=0.0f&&observerDistance>FollowDistance+8.0f) {b=TRexBehavior::Follow;followSeconds_=25.0f;}
                else if(pick<.75f) BeginStand(TRexBehavior::Sniff,3.0f);
                else {b=TRexBehavior::Walk;walkSeconds_=30.0f;}
            }
        } else if(b==TRexBehavior::Follow) {
            followSeconds_-=dt;
            if(observerDistance<FollowDistance) BeginStand(TRexBehavior::Watch,5.0f);
            else if(followSeconds_<=0.0f||observerDistance>WatchDistance*1.5f) {followCooldown_=90.0f;b=TRexBehavior::Walk;walkSeconds_=30.0f;}
        } else if(b==TRexBehavior::Walk) {
            walkSeconds_-=dt;
            if(hasDrink_&&drinkCooldown_<=0.0f&&std::hypot(drinkX_-state_.x,drinkZ_-state_.z)<9.0f) {
                BeginStand(TRexBehavior::Drink,9.0f);drinkCooldown_=240.0f;
            } else if(walkSeconds_<=0.0f) {
                if(night>0.6f) BeginStand(TRexBehavior::Rest,60.0f);
                else {
                    const float pick=Hash(++decisions_*13U);
                    BeginStand(pick<.25f?TRexBehavior::Roar:pick<.6f?TRexBehavior::Sniff:TRexBehavior::Stand,
                               pick<.25f?3.4f:pick<.6f?3.0f:8.0f+10.0f*Hash(decisions_*17U));
                }
            }
        } else if(activitySeconds_<=0.0f) {
            // a stand, a sniff, a roar or a drink over: on again (or down for the night)
            if(night>0.6f) BeginStand(TRexBehavior::Rest,60.0f);
            else {b=TRexBehavior::Walk;walkSeconds_=40.0f+50.0f*Hash(++decisions_*19U);}
        }
    }
    const bool walking=b==TRexBehavior::Walk||b==TRexBehavior::Follow;
    // steering: its round, or toward the walker it follows; facing the walker it watches
    if(std::hypot(route_[target_].x-state_.x,route_[target_].z-state_.z)<6.0f) target_=(target_+1)%route_.size();
    float want=state_.heading;
    if(b==TRexBehavior::Follow||b==TRexBehavior::Watch) want=std::atan2(oz,ox);
    else if(walking) want=std::atan2(route_[target_].z-state_.z,route_[target_].x-state_.x);
    if(b==TRexBehavior::Drink&&hasDrink_) want=std::atan2(drinkZ_-state_.z,drinkX_-state_.x);
    const float turn=std::remainder(want-state_.heading,2.0f*Pi);
    state_.heading+=std::clamp(turn,-(walking?.30f:.22f)*dt,(walking?.30f:.22f)*dt);
    state_.speed=Approach(state_.speed,walking?WalkSpeed*(b==TRexBehavior::Follow?.8f:1.0f):0.0f,(walking?.35f:.6f)*dt);
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    state_.x+=fx*state_.speed*dt;state_.z+=fz*state_.speed*dt;
    state_.y=Ground(state_.x,state_.z);
    distance_+=state_.speed*dt;
    const float pace=std::clamp(state_.speed/WalkSpeed,0.0f,1.0f);
    const float phase=distance_/StrideLength*2.0f*Pi;
    // lying and crouching
    state_.lie=Approach(state_.lie,b==TRexBehavior::Rest?1.0f:0.0f,(b==TRexBehavior::Rest?.25f:.5f)*dt);
    state_.crouch=Approach(state_.crouch,b==TRexBehavior::Drink?1.0f:0.0f,.5f*dt);
    const float lie=state_.lie*state_.lie*(3.0f-2.0f*state_.lie);
    const float crouch=state_.crouch*state_.crouch*(3.0f-2.0f*state_.crouch);
    // the hips drop as each foot takes the weight and rise as it passes under; the body sways
    // onto the planted leg; the tail swings against the stride
    state_.hipHeight=HipHeight-.07f*pace*(1.0f+std::cos(2.0f*phase))*.5f-.02f;
    state_.hipHeight+=(LieHipHeight-HipHeight)*lie-.55f*crouch;
    state_.roll=.035f*pace*std::sin(phase);
    const float ahead=Ground(state_.x+fx*2.0f,state_.z+fz*2.0f),behind=Ground(state_.x-fx*2.0f,state_.z-fz*2.0f);
    state_.pitch=std::clamp(std::atan2(behind-ahead,4.0f)*.6f,-.2f,.2f)+.02f*std::sin(clock_*.4f)*(1.0f-lie)+.38f*crouch+.06f*lie;
    state_.tailSway=.09f*pace*std::sin(phase+.6f)+(1.0f-pace)*.05f*std::sin(clock_*.45f)*(1.0f-.7f*lie);
    // the head: looking about standing, ahead walking, at the walker watching; raised high to
    // sniff; flung up and then thrust out to roar; down to the water to drink; low resting
    float yawTarget=(1.0f-pace)*.55f*std::sin(clock_*.23f),pitchTarget=.05f*std::sin(clock_*.31f)+(1.0f-pace)*.06f;
    float jawTarget=.03f+.02f*std::sin(state_.breath);
    if(b==TRexBehavior::Watch||b==TRexBehavior::Follow) {
        yawTarget=std::clamp(std::remainder(std::atan2(oz,ox)-state_.heading,2.0f*Pi),-.6f,.6f);
        pitchTarget=.02f;
    }
    if(b==TRexBehavior::Sniff) {pitchTarget=.42f+.04f*std::sin(clock_*9.0f);yawTarget=.2f*std::sin(clock_*.9f);jawTarget=0.0f;}
    if(b==TRexBehavior::Roar) {
        const float k=std::clamp(1.0f-activitySeconds_/3.4f,0.0f,1.0f);
        const bool boom=lastCall_==2;
        // the boom with the mouth shut and the throat swelling, the roar with the jaws wide
        pitchTarget=boom?-.10f:.30f*std::sin(Pi*std::min(k*1.6f,1.0f))-.05f;
        jawTarget=boom?.05f:std::sin(Pi*std::clamp((k-.05f)/.8f,0.0f,1.0f));
        if(previewHold_) {pitchTarget=boom?-.10f:.22f;jawTarget=boom?.05f:1.0f;}
    }
    if(b==TRexBehavior::Drink) {pitchTarget=-1.05f*crouch;yawTarget=0.0f;jawTarget=.08f*crouch;}
    if(b==TRexBehavior::Rest) {pitchTarget=-.30f*lie;yawTarget=.25f*std::sin(clock_*.07f);}
    state_.neckYaw=Approach(state_.neckYaw,yawTarget,.5f*dt);
    state_.neckPitch=Approach(state_.neckPitch,pitchTarget,(b==TRexBehavior::Roar?1.6f:.6f)*dt);
    state_.jaw=Approach(state_.jaw,jawTarget,(b==TRexBehavior::Roar?2.5f:1.0f)*dt);
    state_.breath+=dt*(1.6f+.6f*pace);
    // a breath heard every few seconds while it stands
    breathClock_+=dt;
    if(!walking&&b!=TRexBehavior::Roar&&breathClock_>5.5f) {breathClock_=0.0f;++breaths_;}
    PoseLegs(phase);
    // its prints fade over a quarter of an hour
    for(Footprint& print:prints_) {
        if(!print.active) continue;
        print.age+=dt;
        print.density=std::clamp(1.0f-print.age/print.life,0.0f,1.0f);
        if(print.density<=0.0f) print.active=false;
    }
}

void TRexMotion::PoseLegs(const float phase) {
    const float fx=std::cos(state_.heading),fz=std::sin(state_.heading);
    const float rx=-fz,rz=fx;
    const float pace=std::clamp(state_.speed/WalkSpeed,0.0f,1.0f);
    constexpr float Stance=.6f;
    for(int i=0;i<2;++i) {
        const float side=i==0?-1.0f:1.0f;
        TRexLeg& leg=state_.legs[static_cast<std::size_t>(i)];
        const float legPhase=phase+(i==0?0.0f:Pi);
        float u=std::fmod(legPhase/(2.0f*Pi),1.0f);
        if(u<0.0f) u+=1.0f;
        float along=0.0f,lift=0.0f;
        if(u<Stance) {along=StrideLength*(.3f-u);leg.planted=true;}
        else {
            const float k=(u-Stance)/(1.0f-Stance);
            along=StrideLength*(-.3f+.6f*k*k*(3.0f-2.0f*k));
            lift=.42f*std::sin(Pi*k);
            leg.planted=false;
        }
        // standing, the feet come to rest side by side; lying, they fold forward under it
        along*=pace;lift*=pace;
        along+=.62f*state_.lie;
        if(pace<.05f) leg.planted=true;
        const float bx=state_.x+fx*(along+.30f)+rx*side*.52f,bz=state_.z+fz*(along+.30f)+rz*side*.52f;
        leg.ball={bx,Ground(bx,bz)+.07f+lift,bz};
        // the toes splayed forward from the ball, the middle one longest
        for(int t=0;t<3;++t) {
            const float splay=(static_cast<float>(t)-1.0f)*side*.36f;
            const float length=ToeLength*(t==1?1.0f:.82f);
            const float tx=std::cos(state_.heading+splay),tz=std::sin(state_.heading+splay);
            const float x=bx+tx*length,z=bz+tz*length;
            leg.toes[static_cast<std::size_t>(t)]={x,std::max(Ground(x,z)+.02f,leg.ball.y-.07f-(lift>0.0f?.15f*lift:0.0f)),z};
        }
        // the metatarsus up and back from the ball at sixty degrees; lying, flat on the ground
        // under the body, the heel back under the hips (and the knee then forward by the flank)
        const float lie=state_.lie;
        const float rise=MetatarsusLength*(.866f*(1.0f-lie)+.12f*lie),back=MetatarsusLength*(.5f*(1.0f-lie)+.99f*lie);
        leg.ankle={bx-fx*back,leg.ball.y+rise,bz-fz*back};
        leg.hip=ToWorld(side*HipAcross,-.10f,-.05f);
        // the knee forward of the line from hip to ankle (two bones)
        float dx=leg.ankle.x-leg.hip.x,dy=leg.ankle.y-leg.hip.y,dz=leg.ankle.z-leg.hip.z;
        float d=std::sqrt(dx*dx+dy*dy+dz*dz);
        const float reach=FemurLength+TibiaLength-.02f;
        if(d>reach) {
            // too far: the ankle drawn up to the leg's reach
            const float k=reach/d;
            dx*=k;dy*=k;dz*=k;
            leg.ankle={leg.hip.x+dx,leg.hip.y+dy,leg.hip.z+dz};
            d=reach;
        }
        const float ux=dx/d,uy=dy/d,uz=dz/d;
        const float a=(FemurLength*FemurLength-TibiaLength*TibiaLength+d*d)/(2.0f*d);
        const float h=std::sqrt(std::max(0.0f,FemurLength*FemurLength-a*a));
        // the bend: the body's forward, square to the leg's line
        const float dot=fx*ux+fz*uz;
        float bx2=fx-ux*dot,by2=-uy*dot,bz2=fz-uz*dot;
        const float bl=std::max(1e-4f,std::sqrt(bx2*bx2+by2*by2+bz2*bz2));
        bx2/=bl;by2/=bl;bz2/=bl;
        leg.knee={leg.hip.x+ux*a+bx2*h,leg.hip.y+uy*a+by2*h,leg.hip.z+uz*a+bz2*h};
        // a footfall: the foot set down again
        if(leg.planted&&!wasPlanted_[static_cast<std::size_t>(i)]&&state_.speed>.3f) {
            ++footfalls_;
            lastFootfall_=leg.ball;
            Footprint& print=prints_[nextPrint_];
            nextPrint_=(nextPrint_+1)%prints_.size();
            print={};
            print.x=bx+fx*.12f;print.z=bz+fz*.12f;
            print.headingRadians=state_.heading;
            print.density=1.0f;print.left=i==0;print.active=true;print.kind=2U;
            print.life=900.0f;
        }
        wasPlanted_[static_cast<std::size_t>(i)]=leg.planted;
    }
}

}
