#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/DeerGait.hpp"
#include "IslandSimulator/DeerHoofSupport.hpp"

namespace island {
void DeerHerd::PoseFeet(Agent& agent,float seconds,const DeerState& previous) const noexcept {
    if(!ground_) {
        agent.state.contactsValid=false;
        for(auto& foot:agent.footPlants)foot.Reset();
        return; // The legacy flat-plane fixtures have no authoritative per-hoof support.
    }
    if(seconds<=0||seconds>.10001f||agent.state.bedAmount>0) {
        AdvanceFeet(agent,seconds,previous.headingRadians);
        return;
    }
    // Keep AI/navigation and its accepted endpoint unchanged. Advance the existing foot
    // returns along that accepted body segment in bounded steps, including 100ms frames.
    // A single late update otherwise leaves a fast retreat's rear feet far behind the hips.
    auto& d=agent.state;
    const auto target=d;
    const int steps=std::max(1,static_cast<int>(std::ceil(seconds*60)));
    const float headingDelta=std::remainder(target.headingRadians-previous.headingRadians,deer_gait::Tau);
    const float phaseDelta=deer_gait::Phase(target.gaitPhase-previous.gaitPhase);
    float oldHeading=previous.headingRadians;
    for(int step=1;step<=steps;++step) {
        const float t=static_cast<float>(step)/steps;
        d.x=std::lerp(previous.x,target.x,t);d.z=std::lerp(previous.z,target.z,t);
        d.y=StandHeight(d.x,d.z);
        d.headingRadians=previous.headingRadians+headingDelta*t;
        d.speed=std::lerp(previous.speed,target.speed,t);
        d.gaitPhase=deer_gait::Phase(previous.gaitPhase+phaseDelta*t);
        AdvanceFeet(agent,seconds/steps,oldHeading);
        oldHeading=d.headingRadians;
    }
    d.x=target.x;d.y=target.y;d.z=target.z;d.headingRadians=target.headingRadians;
    d.speed=target.speed;d.gaitPhase=target.gaitPhase;
}

void DeerHerd::AdvanceFeet(Agent& agent,float seconds,float previousHeading) const noexcept {
    auto& d=agent.state;
    // Reduced-tier jumps and folded rests retain their authored pose. Reacquire contacts
    // on the next close update, rather than pulling an old foot across a distant jump.
    if(d.bedAmount>0||seconds>.10001f) {
        d.contactsValid=false;
        for(auto& foot:agent.footPlants)foot.Reset();
        return;
    }
    if(seconds<=0)return;
    const auto frame=MakeDeerSupportFrame(d,ground_);
    const auto angle=[](float a){return std::remainder(a,deer_gait::Tau);};
    const float yawRate=std::clamp(angle(d.headingRadians-previousHeading)/seconds,-1.5f,1.5f);
    int airborne=0;
    const bool stamping=d.stamp>.0001f;
    if(d.contactsValid)for(std::size_t i=0;i<d.feet.size();++i)
        airborne+=!d.feet[i].planted||(i==0&&stamping);
    else airborne=stamping?1:0;
    for(std::size_t i=0;i<d.feet.size();++i) {
        auto& contact=d.feet[i];
        const auto neutral=NeutralDeerHoof(frame,d.scale,static_cast<int>(i));
        const auto step=deer_gait::Sample(d.gaitPhase,d.speed,static_cast<int>(i));
        if(!d.contactsValid) {
            contact={neutral.x+frame.forward.x*step.along*d.scale,
                neutral.z+frame.forward.z*step.along*d.scale,0,d.headingRadians,true};
            agent.footPlants[i].Reset({contact.x,contact.z});
            agent.footFromHeading[i]=agent.footToHeading[i]=d.headingRadians;
        }
        const bool wasAirborne=!contact.planted;
        const float dx=contact.x-neutral.x,dz=contact.z-neutral.z;
        const float behind=(dx*frame.forward.x+dz*frame.forward.z)/d.scale;
        const float sideways=(dx*frame.right.x+dz*frame.right.z)/d.scale;
        const float span=deer_gait::HalfSpan(d.speed);
        const bool adjust=!wasAirborne&&(behind<-std::max(.11f,span+.035f)||behind>.45f
            ||(d.speed<.05f&&std::abs(behind)>.11f)
            ||std::abs(sideways)>.11f||std::abs(angle(contact.heading-d.headingRadians))>.32f);
        const float cycleDistance=2*std::max(span,.06f)*d.scale/deer_gait::StanceFraction;
        const float duration=std::clamp(adjust?.18f:
            (1-deer_gait::StanceFraction)*cycleDistance/std::max(d.speed,.2f),.1f,.34f);
        const float ahead=(adjust&&d.speed<.1f?0:span*d.scale)+d.speed*duration;
        const GroundFootPoint landing{neutral.x+frame.forward.x*ahead,neutral.z+frame.forward.z*ahead};
        const auto foot=agent.footPlants[i].Advance(seconds,{{contact.x,contact.z},landing,
            !step.stance&&d.speed>.001f,duration,(adjust?.055f:.105f)*d.scale,0,
            (airborne<2||wasAirborne)&&(i!=0||!stamping||wasAirborne),adjust});
        if(!foot.planted&&!wasAirborne) {
            agent.footFromHeading[i]=contact.heading;
            agent.footToHeading[i]=angle(d.headingRadians+yawRate*duration);
        }
        if(!foot.planted||wasAirborne) {
            const float t=agent.footPlants[i].ReturnFraction(),ease=t*t*(3-2*t);
            contact.heading=angle(agent.footFromHeading[i]+angle(agent.footToHeading[i]-agent.footFromHeading[i])*ease);
        }
        contact.x=foot.point.x;contact.z=foot.point.z;contact.lift=foot.lift;contact.planted=foot.planted;
        airborne+=static_cast<int>(!contact.planted||(i==0&&stamping))
            -static_cast<int>(wasAirborne||(i==0&&stamping));
    }
    d.contactsValid=true;
}
}
