#pragma once

#include "IslandSimulator/BunkerLayout.hpp"
#include "IslandSimulator/QuadBike.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>

namespace island {

// Fixed inspection commands shared by CPU traversal and the rendered QA view.
// It only supplies rider inputs; placement, physics and collision remain authoritative.
// The narrow doorway requires a slow multipoint turn in the clear workshop aisle.
class BunkerTurnInspection final {
public:
    static constexpr unsigned CaptureFrames=800,StageInterval=50;
    BunkerTurnInspection() {
        for(int i=0;i<=351;++i)path_.push_back({-.5f,-6+i*.1f});
        for(int i=1;i<=80;++i)path_.push_back(EntryArc(i));
        for(int i=1;i<=18;++i)path_.push_back({2+i*.1f,31.6f});
    }
    [[nodiscard]] bool Completed()const noexcept{return phase_==3;}
    [[nodiscard]] unsigned Phase()const noexcept{return phase_;}
    [[nodiscard]] unsigned TurnSegment()const noexcept{return segment_;}
    void Observe(const QuadBike& quad,float floor,bool blocked)noexcept {
        blockedSamples_+=blocked;overturnedSamples_+=quad.Overturned();
        maximumFloorGap_=std::max(maximumFloorGap_,std::abs(quad.Position().y-.6f-floor));
    }
    [[nodiscard]] unsigned BlockedSamples()const noexcept{return blockedSamples_;}
    [[nodiscard]] unsigned OverturnedSamples()const noexcept{return overturnedSamples_;}
    [[nodiscard]] float MaximumFloorGap()const noexcept{return maximumFloorGap_;}
    void ObserveCamera(bool blocked,bool occluded)noexcept {
        blockedCameraSamples_+=blocked;occludedCameraSamples_+=occluded;
    }
    [[nodiscard]] unsigned BlockedCameraSamples()const noexcept{return blockedCameraSamples_;}
    [[nodiscard]] unsigned OccludedCameraSamples()const noexcept{return occludedCameraSamples_;}
    [[nodiscard]] BikeControls Controls(const BunkerLayout& bunker,const QuadBike& quad) {
        const auto p=quad.Position(),forward=quad.Forward();
        const auto local=bunker.Local({p.x,p.y,p.z});
        const auto tip=bunker.Local({p.x+forward.x,p.y,p.z+forward.z});
        const float angle=std::atan2(tip.z-local.z,tip.x-local.x);
        if(phase_==2&&local.z<-4)phase_=3;
        if(Completed())return {0,0,true};
        if(phase_==1) {
            const float target=turnStartAngle_-(segment_+1)*std::numbers::pi_v<float>/6;
            const float remaining=std::remainder(angle-target,2*std::numbers::pi_v<float>);
            if(remaining<.055f)braking_=true;
            BikeControls controls{0,segment_%2?1.0f:-1.0f,braking_};
            if(!braking_)controls.throttle=Throttle(quad,segment_%2?-.8f:.8f);
            if(braking_&&std::abs(quad.Speed())<.025f) {
                ++segment_;braking_=false;
                if(segment_==6) {
                    phase_=2;index_=0;path_.clear();
                    for(int i=0;i<=30;++i)path_.push_back({std::lerp(local.x,2.0f,i/30.0f),31.6f});
                    for(int i=79;i>=0;--i)path_.push_back(EntryArc(i));
                    for(int i=1;i<=341;++i)path_.push_back({-.5f,29.1f-i*.1f});
                }
            }
            return controls;
        }
        // Bounded forward projection and a 65 cm lookahead keep the body clear of
        // both doorway jambs, rather than steering through the joined room floors.
        float best=1e30f;std::size_t nearest=index_;
        for(std::size_t i=index_;i<std::min(path_.size(),index_+40);++i) {
            const float distance=std::hypot(path_[i].across-local.x,path_[i].depth-local.z);
            if(distance<best){best=distance;nearest=i;}
        }
        index_=nearest;
        const float endDistance=std::hypot(path_.back().across-local.x,path_.back().depth-local.z);
        if(index_>=path_.size()-3&&endDistance<.13f)braking_=true;
        if(braking_) {
            if(std::abs(quad.Speed())<.025f){phase_=1;turnStartAngle_=angle;braking_=false;}
            return {0,0,true};
        }
        std::size_t look=index_;float length=0;
        while(look+1<path_.size()&&length<.65f) {
            length+=std::hypot(path_[look+1].across-path_[look].across,
                               path_[look+1].depth-path_[look].depth);++look;
        }
        const auto target=bunker.World(path_[look].across,0,path_[look].depth);
        const auto right=quad.Right();const float dx=target.x-p.x,dz=target.z-p.z;
        const float limit=QuadBike::MaxSteerRadians*(1-.68f*std::clamp(std::abs(quad.Speed())/QuadBike::TopSpeed,0.0f,1.0f));
        const float steer=std::clamp(std::atan2(2*QuadBike::WheelBase*(dx*right.x+dz*right.z),dx*dx+dz*dz)/limit,-1.0f,1.0f);
        const float speed=std::min(1.5f,std::max(.25f,std::sqrt(1.2f*endDistance)));
        return {Throttle(quad,speed),steer,false};
    }
private:
    struct Point{float across,depth;};
    static Point EntryArc(int i)noexcept {
        const float angle=std::numbers::pi_v<float>*.5f*i/80;
        return {-.5f+2.5f*(1-std::cos(angle)),29.1f+2.5f*std::sin(angle)};
    }
    static float Throttle(const QuadBike& quad,float target)noexcept {
        return std::clamp((target-quad.Speed())*.9f,-1.0f,1.0f);
    }
    std::vector<Point> path_;
    std::size_t index_=0;
    unsigned phase_=0,segment_=0;
    bool braking_=false;
    float turnStartAngle_=0;
    unsigned blockedSamples_=0,overturnedSamples_=0;
    unsigned blockedCameraSamples_=0,occludedCameraSamples_=0;
    float maximumFloorGap_=0;
};
}
