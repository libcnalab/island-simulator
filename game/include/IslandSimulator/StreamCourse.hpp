#pragma once
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/StreamRoute.hpp"
#include <array>
#include <algorithm>
#include <cmath>
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
