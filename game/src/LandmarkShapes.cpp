#include "IslandSimulator/LandmarkShapes.hpp"
#include <algorithm>
#include <cmath>

namespace island {

std::uint64_t LandmarkShapes::Key(const float x,const float z) noexcept {
    const auto cell=[](const float v) {
        return static_cast<std::uint32_t>(static_cast<std::int32_t>(std::floor(std::clamp(v,-1.0e5f,1.0e5f)/Cell)));
    };
    return (static_cast<std::uint64_t>(cell(x))<<32U)|cell(z);
}

std::size_t LandmarkShapes::Add(const std::string_view landmark,const std::vector<std::array<float,3>>& triangles,
                                const Ground& ground,const std::initializer_list<Band> bands) {
    if(bands.size()==0U) return 0U;
    float lowest=1.0e9f,highest=-1.0e9f;
    std::vector<std::uint16_t> owners;
    for(const Band& band:bands) {
        lowest=std::min(lowest,band.low);highest=std::max(highest,band.high);
        owners.push_back(static_cast<std::uint16_t>(band.shapes.names_.size()));
        band.shapes.names_.emplace_back(landmark);
    }
    std::size_t added=0;
    for(std::size_t t=0;t+2U<triangles.size();t+=3U) {
        const auto& a=triangles[t];
        const auto& b=triangles[t+1U];
        const auto& c=triangles[t+2U];
        if(!std::isfinite(a[0]+a[1]+a[2]+b[0]+b[1]+b[2]+c[0]+c[1]+c[2])) continue;
        // Under a small triangle the ground is taken once, at its middle; under a large one it
        // is looked up at each sample. One wholly above the bands (with a margin for the ground
        // falling away under it) or wholly under them needs no closer look.
        const float plan=std::max({std::hypot(a[0]-b[0],a[2]-b[2]),std::hypot(b[0]-c[0],b[2]-c[2]),std::hypot(c[0]-a[0],c[2]-a[2])});
        const bool small=plan<1.0f;
        const float middle=small?ground((a[0]+b[0]+c[0])/3.0f,(a[2]+b[2]+c[2])/3.0f):0.0f;
        const float ga=small?middle:ground(a[0],a[2]),gb=small?middle:ground(b[0],b[2]),gc=small?middle:ground(c[0],c[2]);
        const float riseLow=std::min({a[1]-ga,b[1]-gb,c[1]-gc}),riseHigh=std::max({a[1]-ga,b[1]-gb,c[1]-gc});
        if(riseHigh<lowest-0.5f||riseLow>highest+0.5f) continue;
        const float span=std::max({plan,std::abs(a[1]-b[1]),std::abs(b[1]-c[1]),std::abs(c[1]-a[1])});
        const int steps=std::clamp(static_cast<int>(span/(Cell*0.5f))+1,1,200);
        std::uint64_t lastKey=~std::uint64_t{0};
        float lastRise=0.0f;
        for(int i=0;i<=steps;++i) for(int j=0;i+j<=steps;++j) {
            const float u=static_cast<float>(i)/static_cast<float>(steps),v=static_cast<float>(j)/static_cast<float>(steps),w=1.0f-u-v;
            const float x=a[0]*w+b[0]*u+c[0]*v,y=a[1]*w+b[1]*u+c[1]*v,z=a[2]*w+b[2]*u+c[2]*v;
            const float rise=y-(small?middle:ground(x,z));
            if(rise<lowest||rise>highest) continue;
            const std::uint64_t key=Key(x,z);
            // (the neighbouring sample mostly falls in the same cell at the same height)
            if(key==lastKey&&std::abs(rise-lastRise)<0.05f) continue;
            lastKey=key;lastRise=rise;
            std::size_t index=0;
            for(const Band& band:bands) {
                if(rise>=band.low&&rise<=band.high&&band.shapes.cells_.emplace(key,owners[index]).second&&index==0U) ++added;
                ++index;
            }
        }
    }
    return added;
}

std::string_view LandmarkShapes::At(const float x,const float z) const noexcept {
    const auto found=cells_.find(Key(x,z));
    return found==cells_.end()?std::string_view():std::string_view(names_[found->second]);
}

}
