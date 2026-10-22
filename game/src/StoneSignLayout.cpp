#include "IslandSimulator/StoneSignLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace island {
namespace {
// The letters' strokes in a unit cell (x across 0..1, y up 0..1.5).
struct Stroke {float x0,y0,x1,y1;};
const std::vector<Stroke>& Letter(const char letter) {
    static const std::vector<Stroke> H{{0,0,0,1.5f},{1,0,1,1.5f},{0,0.75f,1,0.75f}};
    static const std::vector<Stroke> E{{0,0,0,1.5f},{0,1.5f,1,1.5f},{0,0.75f,0.8f,0.75f},{0,0,1,0}};
    static const std::vector<Stroke> L{{0,0,0,1.5f},{0,0,1,0}};
    static const std::vector<Stroke> P{{0,0,0,1.5f},{0,1.5f,0.75f,1.5f},{0.75f,1.5f,1,1.28f},{1,1.28f,1,0.97f},
                                       {1,0.97f,0.75f,0.75f},{0.75f,0.75f,0,0.75f}};
    static const std::vector<Stroke> M{{0,0,0,1.5f},{0,1.5f,0.5f,0.8f},{0.5f,0.8f,1,1.5f},{1,1.5f,1,0}};
    static const std::vector<Stroke> none{};
    switch(letter) {case 'H':return H;case 'E':return E;case 'L':return L;case 'P':return P;case 'M':return M;default:return none;}
}
constexpr const char* Text="HELP ME";
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
}

float StoneSignPlacement::Length() noexcept {
    float length=0;
    for(const char* c=Text;*c;++c) length+=*c==' '?WordGap:LetterWidth+LetterGap;
    return length-LetterGap;
}

std::vector<SignStone> StoneSignPlacement::Stones() const {
    std::vector<SignStone> stones;
    const float scale=LetterHeight/1.5f;
    float origin=0;
    std::uint32_t index=0;
    for(const char* c=Text;*c;++c) {
        if(*c==' ') {origin+=WordGap;continue;}
        for(const Stroke& stroke:Letter(*c)) {
            const float x0=origin+stroke.x0*LetterWidth,y0=stroke.y0*scale,x1=origin+stroke.x1*LetterWidth,y1=stroke.y1*scale;
            const float length=std::hypot(x1-x0,y1-y0);
            const float acrossX=-(y1-y0)/length,acrossY=(x1-x0)/length;
            const int count=std::max(2,static_cast<int>(std::round(length/0.42f))+1);
            for(int n=0;n<count;++n) for(const float row:{-0.24f,0.24f}) {
                const float t=static_cast<float>(n)/static_cast<float>(count-1);
                ++index;
                const float shift=row+(n%2==0?0.0f:0.06f);
                const float u=x0+(x1-x0)*t+acrossX*shift+(Hash(index*3U)-0.5f)*0.10f;
                const float v=y0+(y1-y0)*t+acrossY*shift+(Hash(index*3U+1U)-0.5f)*0.10f;
                const auto p=At(u,v);
                SignStone stone;
                stone.x=p[0];stone.z=p[1];
                stone.radius=0.18f+0.12f*Hash(index*3U+2U);
                stone.yaw=6.2831853f*Hash(index*7U+5U);
                stone.tone=static_cast<std::uint8_t>(Hash(index*11U+3U)<0.12f?4U:static_cast<unsigned>(Hash(index*13U+1U)*4.0f)%4U);
                stones.push_back(stone);
            }
        }
        origin+=LetterWidth+LetterGap;
    }
    return stones;
}

std::optional<StoneSignPlacement> StoneSignLayout::FindInWorld(
        const IslandTerrain& terrain,const std::span<const CircularObstacle> obstacles,
        const std::span<const std::array<float,2>> avoid,const float keepAway) {
    constexpr int Steps=144;
    constexpr float Tau=2.0f*std::numbers::pi_v<float>;
    const float length=StoneSignPlacement::Length();
    for(int step=0;step<Steps;++step) {
        const float angle=-1.5708f+Tau*static_cast<float>(step)/static_cast<float>(Steps);
        const float radius=terrain.ShoreRadius(angle)-InlandFromShore;
        const float seaX=std::cos(angle),seaZ=std::sin(angle);
        StoneSignPlacement sign;
        // read from the sea: the viewer faces inland (-sea), the text runs to that viewer's right
        sign.upX=-seaX;sign.upZ=-seaZ;
        sign.alongX=std::sin(angle);sign.alongZ=-std::cos(angle);
        sign.x=seaX*radius-sign.alongX*length*0.5f;sign.z=seaZ*radius-sign.alongZ*length*0.5f;
        bool fits=true;
        float low=1e9f,high=-1e9f;
        for(float along=-0.8f;along<=length+0.8f&&fits;along+=1.2f) for(const float up:{-0.4f,2.0f,4.5f,7.4f}) {
            const auto p=sign.At(along,up);
            const TerrainSample sample=terrain.Sample(p[0],p[1]);
            if(sample.surface!=TerrainSurface::DrySand||sample.slopeRadians>0.18f||sample.height<0.35f) {fits=false;break;}
            low=std::min(low,sample.height);high=std::max(high,sample.height);
        }
        if(!fits||high-low>2.2f) continue;
        const auto mid=sign.At(length*0.5f,3.5f);
        // the distance from a point to the sign's rectangle (its stones and a margin round them)
        const auto outside=[&](const float px,const float pz) {
            const float dx=px-mid[0],dz=pz-mid[1];
            const float along=std::abs(dx*sign.alongX+dz*sign.alongZ)-length*0.5f,up=std::abs(dx*sign.upX+dz*sign.upZ)-4.2f;
            return std::hypot(std::max(along,0.0f),std::max(up,0.0f));
        };
        for(const auto& point:avoid) fits=fits&&outside(point[0],point[1])>keepAway;
        for(const CircularObstacle& obstacle:obstacles) {
            if(!fits) break;
            if(outside(obstacle.x,obstacle.z)-obstacle.radius<2.0f) fits=false;
        }
        if(fits) return sign;
    }
    return std::nullopt;
}
}
