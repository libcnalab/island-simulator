#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
// A ring spreading on the stream from where its water was cut or struck.
struct StreamRing {
    float x=0,z=0,age=0,strength=0;
    bool active=false;
};

// The rings on the stream: where the quad's wheels and a wading walker or animal cut its water,
// a frog dives in or a leaf lands. A fixed handful, the oldest giving way; a point that rings
// again close to a young ring adds none, so a wheel running through drops a ring every half
// metre and they overlap into its wake. Renderer-independent.
class StreamRingField final {
public:
    static constexpr std::size_t Capacity=16;
    static constexpr float LifeSeconds=1.6f;
    static constexpr float Spacing=0.5f;          // no new ring this close to one younger than
    static constexpr float SpacingSeconds=0.3f;   // this
    static constexpr float SpreadSpeed=0.45f;     // m/s: a small ripple's in shallow water
    void Note(const float x,const float z,const float strength) noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(strength)||strength<=0.0f) return;
        for(const StreamRing& ring:rings_)
            if(ring.active&&ring.age<SpacingSeconds&&std::hypot(ring.x-x,ring.z-z)<Spacing) return;
        // a free slot, else the oldest ring's (of rings as old, the first noted)
        std::size_t slot=0;
        bool free=false;
        for(std::size_t i=0;i<rings_.size()&&!free;++i) {
            if(!rings_[i].active) {slot=i;free=true;}
            else if(rings_[i].age>rings_[slot].age
                ||(rings_[i].age==rings_[slot].age&&serial_[i]<serial_[slot])) slot=i;
        }
        rings_[slot]={x,z,0.0f,std::min(strength,1.0f),true};
        serial_[slot]=count_++;
    }
    void Update(const float deltaSeconds) noexcept {
        const float dt=std::isfinite(deltaSeconds)?std::clamp(deltaSeconds,0.0f,0.25f):0.0f;
        for(StreamRing& ring:rings_) {
            if(!ring.active) continue;
            ring.age+=dt;
            if(ring.age>=LifeSeconds) ring={};
        }
    }
    [[nodiscard]] std::span<const StreamRing> Rings() const noexcept {return rings_;}
    [[nodiscard]] std::uint32_t Count() const noexcept {return count_;}
private:
    std::array<StreamRing,Capacity> rings_{};
    std::array<std::uint32_t,Capacity> serial_{};
    std::uint32_t count_=0;
};
}
