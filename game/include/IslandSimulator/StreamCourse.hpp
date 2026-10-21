#pragma once
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace island {
struct StreamStation {
    float x,z,distance,ground,water,bed,halfWidth;
};
// Authored route through the existing west valley. Construction inspects the
// uncarved terrain; no recursive terrain queries or whole-island hydrology.
class StreamCourse final {
public:
    static constexpr std::size_t Capacity=161;
    static constexpr float BankFreeboard=.36f;
    static constexpr float BedDepth=.28f;
    // The cascade: the steepest three-metre run of the natural water profile is gathered
    // into one lip and a single drop, followed by a wider, deeper plunge pool. The pool may
    // incise deeper than the ordinary channel limit.
    static constexpr float PoolExtraDepth=.34f;
    static constexpr float PoolWidthGain=.55f;
    static constexpr float PoolIncisionLimit=1.45f;
    static constexpr std::size_t PoolLength=7;
    explicit StreamCourse(const IslandTerrain& terrain) {
        float previousWater=100;
        float distance=0;
        for(std::size_t i=0;i<Capacity;++i) {
            const float x=StreamRoute::SourceX-static_cast<float>(i);
            const float z=StreamRoute::CentreZ(x);
            const float stepLength=i==0?0.0f:std::hypot(
                x-stations_[i-1].x,z-stations_[i-1].z);
            distance+=stepLength;
            const float ground=terrain.Height(x,z);
            const float water=std::max(0.0f,std::min(ground-BankFreeboard,
                i==0?previousWater:previousWater-.002f*stepLength));
            const float bed=water-BedDepth;
            if(ground-bed>1.0f) throw std::runtime_error("Stream course requires excessive incision");
            stations_[count_++]={x,z,distance,ground,water,bed,0};
            previousWater=water;
            if(terrain.SignedShoreDistance(x,z)<=0)break;
        }
        if(count_<2 || terrain.SignedShoreDistance(stations_[count_-1].x,stations_[count_-1].z)>0)
            throw std::runtime_error("Stream course does not reach the sea");
        for(std::size_t i=0;i<count_;++i) {
            stations_[i].halfWidth=StreamRoute::HalfWidth(stations_[i].x);
        }
        ShapeCascade();
    }
    [[nodiscard]] std::size_t Count() const noexcept {return count_;}
    // Whether a point lies on the stream's water (from the water mesh's first row to the
    // mouth): the course distance and the signed lateral fraction of the half width there.
    [[nodiscard]] bool Locate(const float x,const float z,float& distance,float& lateral) const noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||count_<14) return false;
        std::size_t best=12;float bestRange=std::numeric_limits<float>::max();
        for(std::size_t i=12;i<count_;++i) {
            const float range=std::hypot(x-stations_[i].x,z-stations_[i].z);
            if(range<bestRange) {bestRange=range;best=i;}
        }
        const StreamStation& station=stations_[best];
        const StreamStation& ahead=stations_[std::min(best+1,count_-1)];
        const StreamStation& behind=stations_[best>12?best-1:best];
        float dirX=ahead.x-behind.x,dirZ=ahead.z-behind.z;
        const float length=std::hypot(dirX,dirZ);
        if(length<0.001f) return false;
        dirX/=length;dirZ/=length;
        const float along=(x-station.x)*dirX+(z-station.z)*dirZ;
        const float across=-(x-station.x)*dirZ+(z-station.z)*dirX;
        if(std::abs(along)>1.0f||std::abs(across)>station.halfWidth) return false;
        distance=station.distance+along;
        lateral=across/std::max(station.halfWidth,0.01f);
        return true;
    }
    // The stream's water level at a point in its channel (its open water, from the water
    // mesh's first row to the mouth), by the nearest stretch of the course: the stations lie
    // a metre apart along x, so only those near the point's own are looked at.
    [[nodiscard]] bool WaterLevel(const float x,const float z,float& level) const noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||count_<4) return false;
        const int approximate=static_cast<int>(StreamRoute::Along(x));
        const int first=std::max(3,approximate-6),last=std::min(static_cast<int>(count_)-2,approximate+6);
        float nearest=std::numeric_limits<float>::max(),t=0,across=0;
        std::size_t best=count_;
        for(int candidate=first;candidate<=last;++candidate) {
            const StreamStation& a=stations_[static_cast<std::size_t>(candidate)];
            const StreamStation& b=stations_[static_cast<std::size_t>(candidate)+1];
            const float segmentX=b.x-a.x,segmentZ=b.z-a.z;
            const float u=std::clamp(((x-a.x)*segmentX+(z-a.z)*segmentZ)/(segmentX*segmentX+segmentZ*segmentZ),0.0f,1.0f);
            const float dx=x-std::lerp(a.x,b.x,u),dz=z-std::lerp(a.z,b.z,u);
            const float squared=dx*dx+dz*dz;
            if(squared<nearest) {nearest=squared;best=static_cast<std::size_t>(candidate);t=u;across=std::sqrt(squared);}
        }
        if(best>=count_) return false;
        const StreamStation& a=stations_[best];
        const StreamStation& b=stations_[best+1];
        // the bank's widest reach (each bank's own swing is within a quarter of the half width)
        if(across>1.25f*std::lerp(a.halfWidth,b.halfWidth,t)) return false;
        level=std::lerp(a.water,b.water,t);
        return true;
    }
    [[nodiscard]] const StreamStation& Station(std::size_t i) const {return stations_.at(i);}
    // Lip station: the water holds level here and drops to the next station.
    [[nodiscard]] std::size_t CascadeIndex() const noexcept {return cascade_;}
    [[nodiscard]] float CascadeDrop() const noexcept {
        return stations_[cascade_].water-stations_[cascade_+1].water;
    }
    // Course distance of the drop itself, between the lip and the pool head.
    [[nodiscard]] float CascadeDistance() const noexcept {
        return .5f*(stations_[cascade_].distance+stations_[cascade_+1].distance);
    }
    [[nodiscard]] float CascadeX() const noexcept {return stations_[cascade_].x;}
    [[nodiscard]] float CascadeZ() const noexcept {return stations_[cascade_].z;}
    [[nodiscard]] const StreamStation& PoolStation() const noexcept {
        return stations_[cascade_+1+PoolLength/2];
    }
private:
    void ShapeCascade() {
        // Keep the source reach and the mouth untouched; the water mesh starts at row 12.
        std::size_t best=12;
        float bestDrop=-1;
        for(std::size_t i=12;i+PoolLength+8<count_;++i) {
            const float drop=stations_[i].water-stations_[i+3].water;
            if(drop>bestDrop) {bestDrop=drop;best=i;}
        }
        // Gather the run: level lip at i+1, the whole drop between i+1 and i+2.
        cascade_=best+1;
        const float lipWater=std::min(stations_[best].water,
            stations_[cascade_].ground-BankFreeboard);
        stations_[cascade_].water=lipWater;
        stations_[cascade_].bed=lipWater-BedDepth;
        const float poolWater=stations_[best+3].water;
        stations_[best+2].water=poolWater;
        stations_[best+2].bed=poolWater-BedDepth;
        for(std::size_t k=0;k<PoolLength;++k) {
            auto& station=stations_[cascade_+1+k];
            const float u=(static_cast<float>(k)+.5f)/static_cast<float>(PoolLength);
            const float bell=std::sin(u*3.14159265f);
            station.bed=std::max(station.bed-PoolExtraDepth*bell,
                station.ground-PoolIncisionLimit);
            station.halfWidth*=1.0f+PoolWidthGain*bell;
        }
    }
    std::array<StreamStation,Capacity> stations_{};
    std::size_t count_=0;
    std::size_t cascade_=0;
};
}
