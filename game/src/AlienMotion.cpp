#include "IslandSimulator/AlienMotion.hpp"
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
float Wrap(float a) {
    while(a>Pi) a-=2.0f*Pi;
    while(a<-Pi) a+=2.0f*Pi;
    return a;
}
struct V3 {
    float x=0,y=0,z=0;
    V3 operator+(const V3& o) const {return {x+o.x,y+o.y,z+o.z};}
    V3 operator-(const V3& o) const {return {x-o.x,y-o.y,z-o.z};}
    V3 operator*(const float k) const {return {x*k,y*k,z*k};}
    [[nodiscard]] float Dot(const V3& o) const {return x*o.x+y*o.y+z*o.z;}
    [[nodiscard]] float Length() const {return std::sqrt(Dot(*this));}
};
AlienPoint To(const V3& v) {return {v.x,v.y,v.z};}
// The middle joint of a two-bone limb from `root` to `end`, bent toward `bend`; the end is pulled
// in when it is out of reach.
V3 Joint(const V3& root,V3& end,const float l1,const float l2,const V3& bend) {
    V3 d=end-root;
    float dist=d.Length();
    const float reach=l1+l2-0.004f;
    if(dist>reach) {end=root+d*(reach/dist);d=end-root;dist=reach;}
    if(dist<1e-4f) return root+bend*l1;
    const V3 dir=d*(1.0f/dist);
    const float a=(l1*l1-l2*l2+dist*dist)/(2.0f*dist);
    const float h=std::sqrt(std::max(0.0f,l1*l1-a*a));
    V3 perp=bend-dir*bend.Dot(dir);
    const float pl=perp.Length();
    perp=pl>1e-5f?perp*(1.0f/pl):V3{0,1,0};
    return root+dir*a+perp*h;
}
}

AlienMotion::AlienMotion(const IslandTerrain& terrain,const UfoWreckPlacement& wreck,const std::span<const CircularObstacle> obstacles)
    :terrain_(terrain),wreck_(wreck)
{
    for(const CircularObstacle& o:obstacles)
        if(std::hypot(o.x-wreck.x,o.z-wreck.z)<32.0f) obstacles_.push_back(o);
    float x=wreck.x+RingMinimum,z=wreck.z;
    for(int k=0;k<24;++k) if(PickRingGoal(static_cast<std::uint32_t>(k),RingMinimum,RingMaximum,x,z)) break;
    state_.x=x;state_.z=z;
    state_.heading=std::atan2(wreck.z-z,wreck.x-x);
    state_.y=Ground(x,z);
    Begin(AlienBehavior::Stand,3.0f);
    Pose(0.0f);
}

float AlienMotion::Ground(const float x,const float z) const noexcept {return terrain_.Height(x,z);}

bool AlienMotion::Free(const float x,const float z,const float margin) const noexcept {
    if(Ground(x,z)<0.8f) return false;
    for(const CircularObstacle& o:obstacles_)
        if(std::hypot(o.x-x,o.z-z)<o.radius+margin) return false;
    return true;
}

bool AlienMotion::PickRingGoal(const std::uint32_t salt,const float minRadius,const float maxRadius,float& outX,float& outZ) const noexcept {
    for(std::uint32_t attempt=0;attempt<16;++attempt) {
        const std::uint32_t seed=(decisions_+1U)*2654435761U+salt*40503U+attempt*977U;
        const float angle=2.0f*Pi*Hash(seed),radius=minRadius+(maxRadius-minRadius)*Hash(seed^0x5bd1U);
        const float x=wreck_.x+std::cos(angle)*radius,z=wreck_.z+std::sin(angle)*radius;
        if(Free(x,z,0.8f)) {outX=x;outZ=z;return true;}
    }
    return false;
}

void AlienMotion::Begin(const AlienBehavior what,const float seconds) {
    state_.behavior=what;behaviorSeconds_=seconds;goingTo_=false;
    seen_|=1U<<static_cast<unsigned>(what);
    if(what==AlienBehavior::TendHull||what==AlienBehavior::Scan) {focusX_=wreck_.x;focusZ_=wreck_.z;}
}

void AlienMotion::GoTo(const float x,const float z,const AlienBehavior then,const float thenSeconds,const float speed) {
    travelSeconds_=0.0f;
    goalX_=x;goalZ_=z;goalSpeed_=speed;then_=then;thenSeconds_=thenSeconds;goingTo_=true;
    if(state_.behavior!=AlienBehavior::Startle) {state_.behavior=AlienBehavior::Walk;seen_|=1U<<static_cast<unsigned>(AlienBehavior::Walk);}
}

void AlienMotion::Decide() {
    const std::uint32_t seed=decisions_*2654435761U+17U;
    const float r=Hash(seed),q=Hash(seed^0x9e37U);
    ++decisions_;
    float gx=0,gz=0;
    if(r<0.26f) {Begin(AlienBehavior::Stand,3.0f+3.0f*q);return;}
    if(r<0.48f) {
        const auto spot=wreck_.At(UfoWreckPlacement::DebrisSpot(static_cast<std::size_t>(q*6.0f))[0],UfoWreckPlacement::DebrisSpot(static_cast<std::size_t>(q*6.0f))[1]);
        focusX_=spot[0];focusZ_=spot[1];
        const float dx=state_.x-spot[0],dz=state_.z-spot[1],d=std::max(std::hypot(dx,dz),0.01f);
        gx=spot[0]+dx/d*0.8f;gz=spot[1]+dz/d*0.8f;
        if(Free(gx,gz,0.3f)) {GoTo(gx,gz,AlienBehavior::Examine,5.0f+3.0f*q,WalkSpeed);return;}
    } else if(r<0.66f) {
        for(std::uint32_t attempt=0;attempt<16;++attempt) {
            // on the side the saucer ploughed in on, where its sunk rim stands about chest high
            const float angle=std::atan2(wreck_.headingZ,wreck_.headingX)+(Hash(seed^(attempt*7919U+3U))-0.5f)*2.0f;
            gx=wreck_.x+std::cos(angle)*HullTouchRadius;gz=wreck_.z+std::sin(angle)*HullTouchRadius;
            if(Free(gx,gz,0.4f)) {GoTo(gx,gz,AlienBehavior::TendHull,6.0f+3.0f*q,WalkSpeed);return;}
        }
    } else if(r<0.80f) {
        if(PickRingGoal(11U,8.5f,10.5f,gx,gz)) {GoTo(gx,gz,AlienBehavior::Scan,4.0f+2.0f*q,WalkSpeed);return;}
    }
    if(PickRingGoal(23U,RingMinimum,RingMaximum,gx,gz)) GoTo(gx,gz,AlienBehavior::Stand,1.0f+2.0f*q,WalkSpeed);
    else Begin(AlienBehavior::Stand,2.0f);
}

void AlienMotion::Place(const float forward,const float right,const AlienBehavior what,const float seconds) {
    const auto p=wreck_.At(forward,right);
    state_.x=p[0];state_.z=p[1];state_.y=Ground(p[0],p[1]);state_.speed=0;
    focusX_=wreck_.x;focusZ_=wreck_.z;
    if(what==AlienBehavior::Examine) {
        float best=1e9f;
        for(std::size_t i=0;i<UfoWreckPlacement::DebrisCount;++i) {
            const auto s=wreck_.At(UfoWreckPlacement::DebrisSpot(i)[0],UfoWreckPlacement::DebrisSpot(i)[1]);
            const float d=std::hypot(s[0]-p[0],s[1]-p[1]);
            if(d<best) {best=d;focusX_=s[0];focusZ_=s[1];}
        }
    }
    state_.heading=std::atan2(focusZ_-state_.z,focusX_-state_.x);
    Begin(what,seconds);holdSeconds_=seconds+4.0f;
    // let the pose settle where it was put
    for(int k=0;k<40;++k) Update(0.1f,observerX_,observerZ_,false);
}

AlienPoint AlienMotion::ToWorld(const float right,const float up,const float ahead) const noexcept {
    const float cs=std::cos(state_.heading),sn=std::sin(state_.heading);
    return {state_.x+sn*right+cs*ahead,state_.y+up,state_.z-cs*right+sn*ahead};
}

void AlienMotion::Update(float seconds,const float observerX,const float observerZ,const bool observerValid) {
    const float dt=std::clamp(seconds,0.0f,0.1f);
    observerX_=observerX;observerZ_=observerZ;observerValid_=observerValid;
    clock_+=dt;watchCooldown_-=dt;
    const bool held=holdSeconds_>0.0f;
    holdSeconds_=std::max(0.0f,holdSeconds_-dt);
    AlienState& s=state_;
    // the walker comes near
    if(!held&&observerValid&&s.behavior!=AlienBehavior::Startle) {
        const float d=std::hypot(observerX-s.x,observerZ-s.z);
        if(d<StartleDistance&&watchCooldown_<=0.0f) {
            // bolts for the far side of the saucer
            const float dx=wreck_.x-observerX,dz=wreck_.z-observerZ,n=std::max(std::hypot(dx,dz),0.01f);
            const float far=RingMinimum+1.0f;
            float gx=wreck_.x+dx/n*far,gz=wreck_.z+dz/n*far;
            if(!Free(gx,gz,0.8f)&&!PickRingGoal(31U,RingMinimum,RingMaximum,gx,gz)) {gx=s.x;gz=s.z;}
            s.behavior=AlienBehavior::Startle;seen_|=1U<<static_cast<unsigned>(AlienBehavior::Startle);
            GoTo(gx,gz,AlienBehavior::Stand,4.0f,RunSpeed);
            watchCooldown_=20.0f;++startles_;
        } else if(d<WatchDistance&&watchCooldown_<=0.0f&&!goingTo_) {
            Begin(AlienBehavior::Watch,5.0f);watchCooldown_=25.0f;
        }
    }
    float targetHeading=s.heading,turnRate=2.5f;
    float targetSpeed=0.0f;
    if(goingTo_&&!held) {
        const float dx=goalX_-s.x,dz=goalZ_-s.z,dist=std::hypot(dx,dz);
        targetHeading=std::atan2(dz,dx);
        // round whatever stands in the way, the way the saucer's curve leads
        const auto clearAhead=[&](const float angle) {return Free(s.x+std::cos(angle)*1.1f,s.z+std::sin(angle)*1.1f,0.35f);};
        if(!clearAhead(targetHeading)) {
            const float toward=(wreck_.x-s.x)*std::sin(targetHeading)-(wreck_.z-s.z)*std::cos(targetHeading);
            const float first=toward>0.0f?-1.0f:1.0f;
            bool found=false;
            for(int step=1;step<=7&&!found;++step) for(const float side:{first,-first}) {
                const float angle=targetHeading+side*0.4f*static_cast<float>(step);
                if(clearAhead(angle)) {targetHeading=angle;found=true;break;}
            }
        }
        travelSeconds_+=dt;
        if(travelSeconds_>60.0f) {goingTo_=false;travelSeconds_=0.0f;Begin(AlienBehavior::Stand,1.0f);}
        if(s.behavior==AlienBehavior::Startle) turnRate=6.0f;
        const float off=std::abs(Wrap(targetHeading-s.heading));
        targetSpeed=off<0.6f?goalSpeed_:0.15f;
        if(dist<0.3f) {
            goingTo_=false;targetSpeed=0.0f;
            Begin(then_,thenSeconds_);
        }
    } else if(s.behavior==AlienBehavior::Examine||s.behavior==AlienBehavior::TendHull||s.behavior==AlienBehavior::Scan) {
        targetHeading=std::atan2(focusZ_-s.z,focusX_-s.x);
    } else if(s.behavior==AlienBehavior::Watch&&observerValid) {
        targetHeading=std::atan2(observerZ-s.z,observerX-s.x);turnRate=1.6f;
    }
    const float diff=Wrap(targetHeading-s.heading);
    s.heading=Wrap(s.heading+std::clamp(diff,-turnRate*dt,turnRate*dt));
    s.speed+=(targetSpeed-s.speed)*(1.0f-std::exp(-5.0f*dt));
    if(s.speed>0.01f) {
        const float step=s.speed*dt,nx=s.x+std::cos(s.heading)*step,nz=s.z+std::sin(s.heading)*step;
        if(Free(nx,nz,0.3f)) {s.x=nx;s.z=nz;gait_+=step/0.88f;}
        else {s.speed=0.0f;if(goingTo_) {goingTo_=false;Begin(AlienBehavior::Stand,1.0f);}}
    }
    s.y=Ground(s.x,s.z);
    if(!goingTo_&&!held) {
        behaviorSeconds_-=dt;
        if(behaviorSeconds_<=0.0f) Decide();
    }
    Pose(dt);
}

void AlienMotion::Pose(const float dt) {
    AlienState& s=state_;
    const float walk=std::clamp(s.speed/WalkSpeed,0.0f,2.0f);
    // posture and gaze by behaviour
    float crouchTarget=0.0f,yawTarget=0.0f,pitchTarget=0.05f,armsTarget=0.0f;
    switch(s.behavior) {
    case AlienBehavior::Examine: crouchTarget=0.85f;pitchTarget=-0.5f;yawTarget=0.25f*std::sin(clock_*0.8f);break;
    case AlienBehavior::TendHull: crouchTarget=0.05f;pitchTarget=-0.2f;yawTarget=0.2f*std::sin(clock_*0.5f);break;
    case AlienBehavior::Scan: pitchTarget=0.5f;yawTarget=0.1f*std::sin(clock_*0.9f);break;
    case AlienBehavior::Watch: {
        const float bearing=std::atan2(observerZ_-s.z,observerX_-s.x);
        yawTarget=std::clamp(Wrap(bearing-s.heading),-1.2f,1.2f);pitchTarget=0.1f;break;
    }
    case AlienBehavior::Startle: crouchTarget=0.12f;pitchTarget=0.25f;armsTarget=1.0f;break;
    case AlienBehavior::Stand: yawTarget=0.7f*std::sin(clock_*0.55f);break;
    case AlienBehavior::Walk: yawTarget=0.1f*std::sin(clock_*1.3f);break;
    }
    const float k=dt>0.0f?1.0f-std::exp(-6.0f*dt):1.0f;
    s.crouch+=(crouchTarget-s.crouch)*k;
    s.headYaw+=(yawTarget-s.headYaw)*k;
    s.headPitch+=(pitchTarget-s.headPitch)*k;
    s.armsUp+=(armsTarget-s.armsUp)*k;
    const float cs=std::cos(s.heading),sn=std::sin(s.heading);
    const V3 right{sn,0,-cs},fwd{cs,0,sn};
    const V3 origin{s.x,s.y,s.z};
    const auto world=[&](const float r,const float u,const float a) {return origin+right*r+V3{0,u,0}+fwd*a;};
    const float pelvisY=PelvisHeight-0.30f*s.crouch+0.012f*std::sin(gait_*4.0f*Pi)*walk;
    const float lean=0.45f*s.crouch+0.08f*std::min(walk,1.0f)+0.12f*s.armsUp;
    const float back=-0.12f*s.crouch;
    const float chestY=pelvisY+0.46f*std::cos(lean),chestA=back+0.46f*std::sin(lean);
    const float neckY=chestY+0.12f*std::cos(lean*1.1f),neckA=chestA+0.12f*std::sin(lean*1.1f)+0.03f;
    s.pelvis=To(world(0,pelvisY,back));s.chest=To(world(0,chestY,chestA));s.neck=To(world(0,neckY,neckA));
    // the head: a frame turned by yaw and pitch, its middle 0.19 above the neck along it
    const float cy=std::cos(s.headYaw),sy=std::sin(s.headYaw),cp=std::cos(s.headPitch),sp=std::sin(s.headPitch);
    const V3 hf{sy*cp,sp,cy*cp},hr{cy,0,-sy};
    const V3 hu{hf.y*hr.z-hf.z*hr.y,hf.z*hr.x-hf.x*hr.z,hf.x*hr.y-hf.y*hr.x};
    const auto frame=[&](const V3& v) {return fwd*v.z+right*v.x+V3{0,v.y,0};};
    s.headAhead=To(frame(hf));s.headRight=To(frame(hr));s.headUp=To(frame(hu));
    const V3 headCentre=world(0,neckY,neckA)+frame(hu)*0.19f+frame(hf)*0.03f;
    s.head=To(headCentre);
    // the legs: each foot swings 0.22 m about its planted point, lifted on the way forward
    for(int i=0;i<2;++i) {
        const float side=i==0?-1.0f:1.0f;
        const float phase=std::fmod(gait_+static_cast<float>(i)*0.5f,1.0f);
        const float sweep=0.22f*std::sin(2.0f*Pi*phase)*std::min(walk,1.3f);
        const float lift=0.09f*std::max(0.0f,std::cos(2.0f*Pi*phase))*std::min(walk,1.3f);
        const float spread=0.09f+0.04f*s.crouch;
        const V3 hip=world(side*0.085f,pelvisY,back);
        V3 foot=world(side*spread,0,sweep+0.10f*s.crouch);
        foot.y=Ground(foot.x,foot.z)+lift;
        const V3 knee=Joint(hip,foot,Thigh,Shin,fwd);
        AlienLeg& leg=s.legs[static_cast<std::size_t>(i)];
        leg.hip=To(hip);leg.knee=To(knee);leg.foot=To(foot);leg.planted=lift<0.012f;
    }
    // the arms: hands to a pose of the behaviour, eased
    for(int i=0;i<2;++i) {
        const float side=i==0?-1.0f:1.0f;
        const V3 shoulder=world(side*0.15f,chestY-0.03f,chestA);
        const float phase=std::fmod(gait_+static_cast<float>(i)*0.5f,1.0f);
        std::array<float,3> target{side*0.07f,-0.70f,-0.18f*std::sin(2.0f*Pi*phase)*std::min(walk,1.3f)+0.10f*s.crouch};
        switch(s.behavior) {
        case AlienBehavior::Examine:
            if(i==1) {
                const float dx=focusX_-s.x,dz=focusZ_-s.z;
                target={dx*right.x+dz*right.z-side*0.0f,Ground(focusX_,focusZ_)+0.05f-(s.y+chestY-0.03f),dx*fwd.x+dz*fwd.z};
            } else target={side*0.14f,-0.42f,0.18f};
            break;
        case AlienBehavior::TendHull:
            target={side*0.14f,-0.25f,0.60f+0.04f*std::sin(clock_*2.0f+side)};
            break;
        case AlienBehavior::Scan:
            if(i==1) {
                const V3 up{0,wreck_.groundY+3.6f-(s.y+chestY-0.03f),0};
                const float dx=wreck_.x-s.x,dz=wreck_.z-s.z;
                V3 d{dx*right.x+dz*right.z,up.y,dx*fwd.x+dz*fwd.z};
                const float n=std::max(d.Length(),0.01f);
                target={d.x/n*0.68f,d.y/n*0.68f,d.z/n*0.68f};
            } else target={side*0.10f,-0.34f,0.14f};
            break;
        case AlienBehavior::Startle:
            target={side*0.30f,0.38f,0.18f};
            break;
        default: break;
        }
        const float rate=dt>0.0f?1.0f-std::exp(-8.0f*dt):1.0f;
        for(int c=0;c<3;++c) handLocal_[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)]+=(target[static_cast<std::size_t>(c)]-handLocal_[static_cast<std::size_t>(i)][static_cast<std::size_t>(c)])*rate;
        const auto& h=handLocal_[static_cast<std::size_t>(i)];
        V3 hand=shoulder+right*h[0]+V3{0,h[1],0}+fwd*h[2];
        const V3 elbow=Joint(shoulder,hand,UpperArm,Forearm,right*(side*0.7f)-fwd*0.3f-V3{0,0.25f,0});
        AlienArm& arm=s.arms[static_cast<std::size_t>(i)];
        arm.shoulder=To(shoulder);arm.elbow=To(elbow);arm.hand=To(hand);
    }
}
}
