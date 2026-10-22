#include "IslandSimulator/ValleyMistField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
float Smooth(const float a,const float b,const float v) noexcept {
    const float t=std::clamp((v-a)/(b-a),0.0f,1.0f);
    return t*t*(3.0f-2.0f*t);
}
}

float ValleyMistField::Strength(const float hour,const float dew,const float wetness) noexcept {
    if(!std::isfinite(hour)||!std::isfinite(dew)||!std::isfinite(wetness)) return 0.0f;
    // gathering through the last hour of the night, thickest at first light, burnt off by 09:30
    const float morning=Smooth(4.5f,5.8f,hour)*(1.0f-Smooth(7.6f,9.5f,hour));
    return morning*std::clamp(2.0f*dew+0.6f*wetness,0.0f,1.0f);
}

ValleyMistField::ValleyMistField(const IslandTerrain& terrain):terrain_(terrain) {
    // Spread down the valley from the springs to above the mouth, alternately either side of
    // the channel, each bank its own size, height, density and breathing phase.
    for(std::size_t i=0;i<Capacity;++i) {
        side_[i]=(i%2U==0U?-1.0f:1.0f)*(1.0f+0.8f*static_cast<float>(i%3U));
        hover_[i]=HoverMinimum+0.35f*static_cast<float>(i%5U);
        baseDensity_[i]=0.42f+0.10f*static_cast<float>(i%3U);
        phase_[i]=static_cast<float>(i)*1.3f;
        puffs_[i].size=5.5f+1.2f*static_cast<float>(i%4U);
        puffs_[i].active=false;
        Place(i,8.0f+(StreamRoute::NominalLength-20.0f)*(static_cast<float>(i)+0.5f)/static_cast<float>(Capacity));
    }
}

void ValleyMistField::Place(const std::size_t index,const float along) noexcept {
    along_[index]=along;
    const float x=StreamRoute::SourceX-along;
    MistPuff& puff=puffs_[index];
    puff.x=x;
    puff.z=StreamRoute::CentreZ(x)+side_[index];
    puff.y=terrain_.Height(puff.x,puff.z)+hover_[index];
    puff.age=0;
}

void ValleyMistField::Update(const float deltaSeconds,const float hour,const float dew,const float wetness) noexcept {
    const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.1f):0.0f;
    clock_+=dt;
    const float strength=Strength(hour,dew,wetness);
    for(std::size_t i=0;i<Capacity;++i) {
        MistPuff& puff=puffs_[i];
        // down the valley with the cool air draining off the hill, a slow sway across it
        along_[i]+=DriftSpeed*dt;
        if(along_[i]>StreamRoute::NominalLength-6.0f) along_[i]=6.0f;
        const float x=StreamRoute::SourceX-along_[i];
        puff.x=x;
        puff.z=StreamRoute::CentreZ(x)+side_[i]+0.6f*std::sin(clock_*0.05f+phase_[i]);
        const float wantY=terrain_.Height(puff.x,puff.z)+hover_[i]+0.15f*std::sin(clock_*0.11f+phase_[i]);
        puff.y+=std::clamp(wantY-puff.y,-0.4f*std::max(dt,1e-4f),0.4f*std::max(dt,1e-4f));
        if(dt<=0.0f) puff.y=wantY;
        puff.age+=dt;
        puff.density=strength*baseDensity_[i]*(0.78f+0.22f*std::sin(clock_*0.23f+phase_[i]));
        puff.active=puff.density>0.01f;
    }
}
}
