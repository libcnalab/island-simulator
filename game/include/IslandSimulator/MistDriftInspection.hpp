#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace island {
// Find a real recycling event without editing particle state. The capture starts two
// seconds before it, so the same camera observes both ends of the transition.
struct MistDriftInspection {
    int warmSteps=0;
    std::size_t subject=0;
    float x=0,y=0,z=0;
    template<class Field,class Advance>
    static MistDriftInspection Find(Field field,Advance advance) {
        for(int step=0;step<12000;++step) {
            const Field previous=field;
            advance(field,step);
            for(std::size_t i=0;i<field.Puffs().size();++i) {
                const auto& a=previous.Puffs()[i];const auto& b=field.Puffs()[i];
                if(std::hypot(a.x-b.x,a.z-b.z)>8.0f)
                    return {std::max(0,step-20),i,a.x,a.y,a.z};
            }
        }
        throw std::runtime_error("No mist recycling event found for capture");
    }
};
}
