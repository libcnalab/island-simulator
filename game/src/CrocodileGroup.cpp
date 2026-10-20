#include "IslandSimulator/CrocodileGroup.hpp"

#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamRoute.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace island {
namespace {
constexpr float Tau=2.0f*std::numbers::pi_v<float>;
float Wrap(float value) noexcept {
    while(value>std::numbers::pi_v<float>)value-=Tau;
    while(value<-std::numbers::pi_v<float>)value+=Tau;
    return value;
}
float MoveAngle(float current,float target,float step) noexcept {
    return Wrap(current+std::clamp(Wrap(target-current),-step,step));
}
}

CrocodileGroup::CrocodileGroup(const IslandTerrain& terrain):terrain_(terrain) {
    InitializeAgents();
}

void CrocodileGroup::SetObstacles(const std::span<const CircularObstacle> obstacles) {
    obstacles_.assign(obstacles.begin(),obstacles.end());
    InitializeAgents();
}

void CrocodileGroup::InitializeAgents() {
    agents_.assign(CrocodileCount,Agent{});
    states_.resize(CrocodileCount);homes_.resize(CrocodileCount);
    elapsedSeconds_=0;frameIndex_=0;fullCount_=reducedCount_=dormantCount_=0;
    constexpr float lateral[CrocodileCount]={-8.0f,0.0f,8.0f};
    for(std::size_t i=0;i<CrocodileCount;++i) {
        Agent& agent=agents_[i];
        const auto land=FindAnchor(lateral[i],3.4f,true);
        const auto water=FindAnchor(lateral[i],-4.5f,false);
        agent.home={land.first,land.second,water.first,water.second,22.0f};
        agent.state.x=i==2?water.first:land.first;
        agent.state.z=i==2?water.second:land.second;
        agent.state.headingRadians=std::atan2(water.second-land.second,water.first-land.first);
        agent.state.scale=.92f+static_cast<float>(i)*.10f;
        agent.state.colorVariant=static_cast<std::uint8_t>(i);
        agent.state.behavior=i==2?CrocodileBehavior::Swim:CrocodileBehavior::Bask;
        // The basking pair gape a few seconds in, one after the other.
        agent.gapeDelay=5.0f+static_cast<float>(i)*4.0f;
        agent.gapeSeconds=GapeSeconds;
        agent.behaviorSeconds=8.0f+static_cast<float>(i)*4.0f;
        agent.phaseOffset=.8f+static_cast<float>(i)*1.71f;
        const auto sample=terrain_.Sample(agent.state.x,agent.state.z);
        agent.state.submerged=agent.state.behavior==CrocodileBehavior::Swim?1.0f:0.0f;
        agent.state.y=agent.state.submerged>0
            ?std::max(sample.height+.20f*agent.state.scale,-.20f)
            :sample.height+.10f*agent.state.scale;
        states_[i]=agent.state;homes_[i]=agent.home;
    }
}

std::pair<float,float> CrocodileGroup::FindAnchor(float lateral,float targetShore,
                                                  bool requireDry)const {
    const float mouthX=StreamRoute::SourceX-StreamRoute::NominalLength;
    const float mouthZ=StreamRoute::CentreZ(mouthX);
    float best=std::numeric_limits<float>::infinity(),bestX=0,bestZ=0;
    for(int ix=-16;ix<=16;++ix) for(int iz=-18;iz<=18;++iz) {
        const float x=mouthX+static_cast<float>(ix)*1.15f;
        const float z=mouthZ+lateral+static_cast<float>(iz)*.85f;
        const auto sample=terrain_.Sample(x,z);
        const auto experience=IslandEnvironmentField::SampleCoastalExperience(
            x,z,sample.signedShoreDistance);
        if(experience.mangroveMouth<.025f||sample.slopeRadians>.46f)continue;
        if(requireDry?(sample.height<.04f||sample.height>2.2f)
                     :(sample.height>-.18f||sample.height<-1.8f))continue;
        bool obstacleClear=true;
        for(const CircularObstacle& obstacle:obstacles_)
            if(std::hypot(x-obstacle.x,z-obstacle.z)<obstacle.radius+2.65f) {
                obstacleClear=false;break;
            }
        if(!obstacleClear)continue;
        const float score=std::abs(sample.signedShoreDistance-targetShore)*2.8f
            +std::abs((z-mouthZ)-lateral)*.20f+std::abs(x-mouthX)*.08f
            +(1.0f-experience.mangroveMouth)*3.0f+sample.slopeRadians*2.0f;
        if(score<best){best=score;bestX=x;bestZ=z;}
    }
    if(!std::isfinite(best))throw std::runtime_error("No suitable crocodile estuary anchor");
    return {bestX,bestZ};
}

bool CrocodileGroup::IsValidHabitat(float x,float z,const CrocodileHome& home)const noexcept {
    if(!std::isfinite(x)||!std::isfinite(z))return false;
    const auto sample=terrain_.Sample(x,z);
    const auto experience=IslandEnvironmentField::SampleCoastalExperience(
        x,z,sample.signedShoreDistance);
    const float centreX=(home.landX+home.waterX)*.5f,centreZ=(home.landZ+home.waterZ)*.5f;
    return std::hypot(x-centreX,z-centreZ)<=home.radius
        &&sample.signedShoreDistance>=-18.0f&&sample.signedShoreDistance<=18.0f
        &&sample.height>=-2.2f&&sample.height<=3.0f&&sample.slopeRadians<=.52f
        &&experience.mangroveMouth>=.012f;
}

void CrocodileGroup::Update(float deltaSeconds,float observerX,float observerY,
                            float observerZ) noexcept {
    if(entry_.valid) {
        entry_.ageSeconds+=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
        if(entry_.ageSeconds>EntrySeconds) entry_.valid=false;
    }
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,.1f):0.0f;
    const bool observerValid=std::isfinite(observerX)&&std::isfinite(observerY)
        &&std::isfinite(observerZ);
    elapsedSeconds_+=dt;++frameIndex_;fullCount_=reducedCount_=dormantCount_=0;
    for(std::size_t i=0;i<agents_.size();++i) {
        Agent& agent=agents_[i];
        const float distance=observerValid?std::hypot(agent.state.y-observerY,
            std::hypot(agent.state.x-observerX,agent.state.z-observerZ))
            :std::numeric_limits<float>::infinity();
        agent.state.simulationTier=WildlifeActivationPolicy::Classify(
            distance,FullSimulationDistance,ReducedSimulationDistance);
        switch(agent.state.simulationTier) {
        case WildlifeSimulationTier::Full:++fullCount_;break;
        case WildlifeSimulationTier::Reduced:++reducedCount_;break;
        case WildlifeSimulationTier::Dormant:++dormantCount_;break;
        }
        agent.accumulatedSeconds+=dt;
        const auto divisor=WildlifeActivationPolicy::UpdateDivisor(agent.state.simulationTier);
        if((frameIndex_+i)%divisor==0) {
            UpdateAgent(agent,std::min(agent.accumulatedSeconds,.5f),observerX,observerZ,observerValid);
            agent.accumulatedSeconds=0;
        }
        states_[i]=agent.state;
    }
#ifndef NDEBUG
    assert(fullCount_+reducedCount_+dormantCount_==static_cast<int>(CrocodileCount));
    for(const auto& state:states_) {
        assert(std::isfinite(state.x)&&std::isfinite(state.y)&&std::isfinite(state.z));
        assert(std::isfinite(state.headingRadians)&&std::isfinite(state.speed));
        assert(state.submerged>=0&&state.submerged<=1);
    }
#endif
}

std::size_t CrocodileGroup::BlinkingCount()const noexcept {
    std::size_t count=0;
    for(const Agent& agent:agents_) count+=agent.blinking?1U:0U;
    return count;
}
std::size_t CrocodileGroup::GapingCount()const noexcept{
    std::size_t count=0;
    for(const Agent& agent:agents_) count+=agent.gaping?1U:0U;
    return count;
}

void CrocodileGroup::UpdateAgent(Agent& agent,float dt,float observerX,float observerZ,
                                 bool observerValid) noexcept {
    if(dt<=0)return;
    const float observerDistance=observerValid
        ?std::hypot(agent.state.x-observerX,agent.state.z-observerZ)
        :std::numeric_limits<float>::infinity();
    bool alertFacing=false;
    if(observerDistance<14.0f) {
        agent.state.behavior=CrocodileBehavior::Retreat;
        agent.behaviorSeconds=7.0f;
    } else if(observerDistance<AlertDistance) {
        // Lie still and face the approaching player; the basking cycle resumes once they leave.
        // Coming alert on the bank, the crocodile hisses, its jaw gaping while it does.
        if(!agent.wasAlert&&agent.state.submerged<.2f) {
            agent.hissSeconds=HissSeconds;
            hiss_={agent.state.x+std::cos(agent.state.headingRadians)*1.2f*agent.state.scale,
                   agent.state.y+.25f*agent.state.scale,
                   agent.state.z+std::sin(agent.state.headingRadians)*1.2f*agent.state.scale,
                   hiss_.count+1U,true};
        }
        agent.state.behavior=CrocodileBehavior::Bask;
        agent.behaviorSeconds=std::max(agent.behaviorSeconds,.5f);
        alertFacing=true;
    } else {
        agent.behaviorSeconds-=dt;
        if(agent.behaviorSeconds<=0) {
            ++agent.transitions;
            switch(agent.state.behavior) {
            case CrocodileBehavior::Bask:
                agent.state.behavior=CrocodileBehavior::Crawl;agent.behaviorSeconds=6.0f;break;
            case CrocodileBehavior::Crawl:
                agent.state.behavior=CrocodileBehavior::Swim;agent.behaviorSeconds=15.0f;break;
            case CrocodileBehavior::Swim:
            case CrocodileBehavior::Retreat:
                agent.state.behavior=CrocodileBehavior::Bask;
                agent.behaviorSeconds=16.0f+static_cast<float>(agent.transitions%4U)*4.0f;
                agent.gapeDelay=4.0f+static_cast<float>(agent.transitions%3U)*2.0f;
                agent.gapeSeconds=GapeSeconds;
                break;
            }
        }
    }

    float targetX=agent.home.landX,targetZ=agent.home.landZ;
    if(agent.state.behavior==CrocodileBehavior::Swim
       ||agent.state.behavior==CrocodileBehavior::Retreat) {
        targetX=agent.home.waterX;targetZ=agent.home.waterZ;
        const float orbit=elapsedSeconds_*.09f+agent.phaseOffset;
        targetX+=std::cos(orbit)*3.4f;targetZ+=std::sin(orbit)*3.4f;
    }
    if(agent.state.behavior==CrocodileBehavior::Retreat&&observerValid) {
        // Slide toward the water about seventy degrees off the direct line away, so the
        // crocodile keeps the player in view along its flank instead of showing its tail.
        const float dx=agent.state.x-observerX,dz=agent.state.z-observerZ;
        const float length=std::max(.01f,std::hypot(dx,dz));
        const float side=(static_cast<std::size_t>(&agent-agents_.data())%2U)?-1.22f:1.22f;
        const float away=std::atan2(dz,dx)+side;
        targetX+=std::cos(away)*8.0f;targetZ+=std::sin(away)*8.0f;
        (void)length;
    }
    if(alertFacing&&observerValid) {targetX=observerX;targetZ=observerZ;}
    float desired=std::atan2(targetZ-agent.state.z,targetX-agent.state.x);
    desired+=std::sin(elapsedSeconds_*.17f+agent.phaseOffset)*.10f;
    // One bounded turn per update.  A blocked move below re-aims from this same starting
    // heading with the same budget instead of adding a second, faster turn in one frame.
    const float previousHeading=agent.state.headingRadians;
    const float turnStep=dt*(agent.state.behavior==CrocodileBehavior::Retreat?1.15f:.42f);
    agent.state.headingRadians=MoveAngle(previousHeading,desired,turnStep);
    const float targetSpeed=agent.state.behavior==CrocodileBehavior::Bask?0.0f:
        agent.state.behavior==CrocodileBehavior::Crawl?.34f:
        agent.state.behavior==CrocodileBehavior::Swim?.72f:1.05f;
    agent.state.speed+=std::clamp(targetSpeed-agent.state.speed,-dt*.7f,dt*.7f);
    const float nx=agent.state.x+std::cos(agent.state.headingRadians)*agent.state.speed*dt;
    const float nz=agent.state.z+std::sin(agent.state.headingRadians)*agent.state.speed*dt;
    if(IsValidHabitat(nx,nz,agent.home)) {agent.state.x=nx;agent.state.z=nz;}
    else agent.state.headingRadians=MoveAngle(previousHeading,
        std::atan2((agent.home.waterZ+agent.home.landZ)*.5f-agent.state.z,
                   (agent.home.waterX+agent.home.landX)*.5f-agent.state.x),turnStep);

    const auto sample=terrain_.Sample(agent.state.x,agent.state.z);
    const float targetSubmerged=(agent.state.behavior==CrocodileBehavior::Swim
        ||agent.state.behavior==CrocodileBehavior::Retreat)&&sample.height<.05f?1.0f:0.0f;
    const float previousSubmerged=agent.state.submerged;
    agent.state.submerged+=std::clamp(targetSubmerged-agent.state.submerged,-dt*.8f,dt*.8f);
    // The slide off the bank: the moment the body goes under is the splash.
    if(previousSubmerged<.2f&&agent.state.submerged>=.2f) entry_={agent.state.x,agent.state.z,0.0f,entry_.count+1U,true};
    const float groundY=sample.height+.10f*agent.state.scale;
    const float swimY=std::max(sample.height+.20f*agent.state.scale,-.18f);
    agent.state.y+=std::clamp(std::lerp(groundY,swimY,agent.state.submerged)-agent.state.y,
                              -dt*.55f,dt*.55f);
    agent.state.gaitPhase=std::fmod(agent.state.gaitPhase+dt*(1.1f+agent.state.speed*2.2f),Tau);
    agent.state.bodyWave=std::sin(agent.state.gaitPhase)*(.10f+.18f*agent.state.submerged);
    agent.wasAlert=alertFacing;
    agent.hissSeconds=std::max(0.0f,agent.hissSeconds-dt);
    // The gape: a few seconds into a lie-up on the bank the jaws open and stay open for
    // GapeSeconds; moving off, or the water, closes them.
    const bool lying=agent.state.behavior==CrocodileBehavior::Bask&&agent.state.submerged<.2f;
    if(lying&&!alertFacing) {
        if(agent.gapeDelay>0.0f) agent.gapeDelay-=dt;
        else if(agent.gapeSeconds>0.0f) {
            if(!agent.gaping) {agent.gaping=true;++gapes_;}
            agent.gapeSeconds-=dt;
        } else agent.gaping=false;
    } else {
        if(!lying) {agent.gapeSeconds=0.0f;agent.gapeDelay=0.0f;}
        agent.gaping=false;
    }
    const float jawTarget=agent.hissSeconds>0.0f?.62f:agent.gaping?GapeOpen
        :agent.state.behavior==CrocodileBehavior::Bask
        ?(.5f+.5f*std::sin(elapsedSeconds_*.12f+agent.phaseOffset))*.14f:0.0f;
    agent.state.jawOpen+=std::clamp(jawTarget-agent.state.jawOpen,-dt*.18f,dt*.18f);
    // The blink: every eight seconds or so (a little per crocodile) the eyes shut for a
    // moment and open again.
    const float blinkPeriod=BlinkPeriodSeconds+agent.phaseOffset*.5f;
    agent.blinkClock+=dt;
    if(agent.blinkClock>=blinkPeriod) agent.blinkClock=0.0f;
    const bool blinking=agent.blinkClock>=2.0f&&agent.blinkClock<2.0f+BlinkSeconds;
    if(blinking&&!agent.blinking) ++blinks_;
    agent.blinking=blinking;
    agent.state.blink+=std::clamp((blinking?1.0f:0.0f)-agent.state.blink,-dt*8.0f,dt*12.0f);
}

}
