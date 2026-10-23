#include "IslandSimulator/BalloonFlight.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
constexpr int Samples=2048;
constexpr float BurnPeriod=23.0f,BurnLength=3.6f;
constexpr float TurnPerLap=0.9f;
// the figure of eight (a lemniscate of Gerono) at its parameter s
std::array<float,2> Figure(const float s) noexcept {
    return {BalloonFlight::Reach*std::sin(s),BalloonFlight::Width*std::sin(2.0f*s)};
}
struct Table {
    std::array<float,Samples+1> along{};
    float length=0;
    Table() {
        auto last=Figure(0.0f);
        for(int i=1;i<=Samples;++i) {
            const auto p=Figure(2.0f*Pi*static_cast<float>(i)/Samples);
            length+=std::hypot(p[0]-last[0],p[1]-last[1]);
            along[static_cast<std::size_t>(i)]=length;
            last=p;
        }
    }
};
const Table& ArcTable() {static const Table table;return table;}
// where on the figure (turned with its laps) it is at the clock's `seconds`
std::array<float,2> Position(const float seconds) noexcept {
    const Table& table=ArcTable();
    const float travelled=seconds*BalloonFlight::Speed;
    float d=std::fmod(travelled,table.length);
    if(d<0.0f) d+=table.length;
    const auto it=std::lower_bound(table.along.begin(),table.along.end(),d);
    const auto i=static_cast<int>(std::clamp<std::ptrdiff_t>(it-table.along.begin(),1,Samples));
    const float a=table.along[static_cast<std::size_t>(i-1)],b=table.along[static_cast<std::size_t>(i)];
    const float k=b>a?(d-a)/(b-a):0.0f;
    const float s=2.0f*Pi*(static_cast<float>(i-1)+k)/Samples;
    const auto p=Figure(s);
    const float turn=travelled/table.length*TurnPerLap;
    const float c=std::cos(turn),sn=std::sin(turn);
    return {p[0]*c-p[1]*sn,p[0]*sn+p[1]*c};
}
}

BalloonFlight::BalloonFlight(const IslandTerrain& terrain) {
    // the highest ground in each 20 m cell (the sea counts as 0), then the highest of each
    // cell's neighbours
    std::vector<float> cell(static_cast<std::size_t>(Cells*Cells),0.0f);
    const float origin=-CellSize*Cells*0.5f;
    for(int j=0;j<Cells;++j)
        for(int i=0;i<Cells;++i) {
            float high=0.0f;
            for(int b=0;b<5;++b)
                for(int a=0;a<5;++a)
                    high=std::max(high,terrain.Height(origin+(static_cast<float>(i)+static_cast<float>(a)*0.25f)*CellSize,
                                                      origin+(static_cast<float>(j)+static_cast<float>(b)*0.25f)*CellSize));
            cell[static_cast<std::size_t>(j*Cells+i)]=high;
        }
    ceiling_.assign(cell.size(),0.0f);
    for(int j=0;j<Cells;++j)
        for(int i=0;i<Cells;++i) {
            float high=0.0f;
            for(int b=-2;b<=2;++b)
                for(int a=-2;a<=2;++a) {
                    const int x=std::clamp(i+a,0,Cells-1),z=std::clamp(j+b,0,Cells-1);
                    high=std::max(high,cell[static_cast<std::size_t>(z*Cells+x)]);
                }
            ceiling_[static_cast<std::size_t>(j*Cells+i)]=high;
        }
}

float BalloonFlight::Ceiling(const float x,const float z) const noexcept {
    // bilinear between the cells' middles
    const float origin=-CellSize*Cells*0.5f;
    const float fx=std::clamp((x-origin)/CellSize-0.5f,0.0f,static_cast<float>(Cells-1)-1e-3f);
    const float fz=std::clamp((z-origin)/CellSize-0.5f,0.0f,static_cast<float>(Cells-1)-1e-3f);
    const int i=static_cast<int>(fx),j=static_cast<int>(fz);
    const float u=fx-static_cast<float>(i),v=fz-static_cast<float>(j);
    const auto at=[this](const int a,const int b) {
        return ceiling_[static_cast<std::size_t>(std::min(b,Cells-1)*Cells+std::min(a,Cells-1))];
    };
    return (at(i,j)*(1.0f-u)+at(i+1,j)*u)*(1.0f-v)+(at(i,j+1)*(1.0f-u)+at(i+1,j+1)*u)*v;
}

bool BalloonFlight::Burning(const float seconds) noexcept {
    if(!std::isfinite(seconds)) return false;
    float phase=std::fmod(seconds,BurnPeriod);
    if(phase<0.0f) phase+=BurnPeriod;
    return phase<BurnLength;
}

int BalloonFlight::BurnCount(const float seconds) noexcept {
    if(!std::isfinite(seconds)||seconds<0.0f) return 0;
    return static_cast<int>(std::floor(seconds/BurnPeriod))+1;
}

BalloonFlight::State BalloonFlight::At(const float seconds) const noexcept {
    const float t=std::isfinite(seconds)?seconds:0.0f;
    State state;
    const auto p=Position(t);
    const auto ahead=Position(t+1.0f);
    state.x=p[0];state.z=p[1];
    state.heading=std::atan2(ahead[1]-p[1],ahead[0]-p[0]);
    // the ceiling looked ahead of it too, so it rises before the ground does
    const float ground=std::max(Ceiling(p[0],p[1]),Ceiling(p[0]+(ahead[0]-p[0])*10.0f,p[1]+(ahead[1]-p[1])*10.0f));
    state.y=ground+Clearance+50.0f*(0.5f+0.5f*std::sin(t*0.011f));
    state.spin=t*0.021f;
    state.tiltX=0.025f*std::sin(t*0.37f);
    state.tiltZ=0.020f*std::sin(t*0.29f+1.0f);
    if(Burning(t)) {
        float phase=std::fmod(t,BurnPeriod);
        if(phase<0.0f) phase+=BurnPeriod;
        // the flame catching, roaring and dying, with a flicker
        const float envelope=std::min(1.0f,phase/0.25f)*std::min(1.0f,(BurnLength-phase)/0.4f);
        state.burner=envelope*(0.85f+0.15f*std::sin(t*37.0f));
    }
    return state;
}

}
