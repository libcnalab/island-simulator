#pragma once
#include <cmath>

namespace island {

/** A coastal freighter that passes the island far offshore now and then: down a straight lane
 *  some 860 m out, square to the bearing it is closest at, taking about four minutes to cross
 *  from one end of the lane to the other, then gone over the horizon until the next pass. The
 *  first pass is half done as the island loads. Deterministic in the render clock. */
struct PassingShip final {
    static constexpr float Period=720.0f;      // seconds from one pass to the next
    static constexpr float Speed=6.5f;         // metres a second (some 12.5 knots)
    static constexpr float Offshore=860.0f;    // the lane's nearest distance from the island's middle
    static constexpr float HalfLane=820.0f;    // the lane runs this far either side of that point
    static constexpr float Bearing=0.35f;      // the bearing of the lane's nearest point
    static constexpr float Length=62.0f,Beam=10.0f;
    struct State {bool visible=false;float x=0,z=0,headingX=1,headingZ=0;};
    [[nodiscard]] static State At(const float seconds) noexcept {
        State state;
        if(!std::isfinite(seconds)) return state;
        const float laneTime=2.0f*HalfLane/Speed;
        float phase=std::fmod(seconds+HalfLane/Speed,Period);
        if(phase<0.0f) phase+=Period;
        if(phase>laneTime) return state;
        const float along=Speed*phase-HalfLane;
        const float cx=Offshore*std::cos(Bearing),cz=Offshore*std::sin(Bearing);
        state.headingX=-std::sin(Bearing);state.headingZ=std::cos(Bearing);
        state.x=cx+state.headingX*along;state.z=cz+state.headingZ*along;
        state.visible=true;
        return state;
    }
};

}
