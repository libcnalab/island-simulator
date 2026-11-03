#pragma once
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
struct GroundFootPoint {float x=0,z=0;};
struct GroundFootStep {
    GroundFootPoint initial,landing;
    bool swing=false;
    float seconds=1,lift=0,flex=0;
    bool startAllowed=true,adjust=false;
};
struct GroundFootPose {
    GroundFootPoint point;
    float lift=0,flex=0;
    bool planted=true;
};

// A single world-space contact and its finite return flight. The existing rig still
// solves the joints. A moving/turning body cannot drag a planted foot, change a
// flight's destination mid-air or strand it when the body stops.
class GroundFootPlant final {
public:
    void Reset() noexcept {valid_=active_=wasSwing_=false;}
    void Reset(GroundFootPoint point) noexcept {Reset();point_=point;valid_=true;}
    [[nodiscard]] float ReturnFraction() const noexcept {return active_?elapsed_/duration_:1.0f;}
    [[nodiscard]] GroundFootPose Advance(float seconds,const GroundFootStep& step) noexcept {
        if(!valid_)Reset(step.initial);
        const float dt=std::isfinite(seconds)?std::clamp(seconds,0.0f,.1f):0;
        if(dt>0) {
            if(active_) {
                elapsed_=std::min(duration_,elapsed_+dt);
                if(elapsed_>=duration_) {point_=to_;active_=false;}
            } else if(step.startAllowed&&(step.adjust||(step.swing&&!wasSwing_))) {
                from_=point_;to_=step.landing;elapsed_=0;
                duration_=std::max(step.seconds,.1f);lift_=step.lift;flex_=step.flex;active_=true;
            }
            if(active_||step.startAllowed)wasSwing_=step.swing;
        }
        if(!active_)return {point_,0,0,true};
        const float t=elapsed_/duration_,ease=t*t*(3-2*t);
        point_={std::lerp(from_.x,to_.x,ease),std::lerp(from_.z,to_.z,ease)};
        return {point_,lift_*std::sin(std::numbers::pi_v<float>*t),
            flex_*std::sin(std::numbers::pi_v<float>*std::min(1.0f,t*1.25f)),false};
    }
private:
    GroundFootPoint point_,from_,to_;
    float elapsed_=0,duration_=1,lift_=0,flex_=0;
    bool valid_=false,active_=false,wasSwing_=false;
};
}
