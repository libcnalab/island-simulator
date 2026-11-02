#include "IslandSimulator/RainShelter.hpp"
#include "IslandSimulator/RainStreakAppearance.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    using Point=std::array<float,3>;
    void Require(bool value,const char* message)
    {
        if (!value) throw std::runtime_error(message);
    }
    Point Along(const Point& a,const Point& b,float t)
    {
        return {std::lerp(a[0],b[0],t),std::lerp(a[1],b[1],t),std::lerp(a[2],b[2],t)};
    }
    // Independent roof-height membership, rather than the clipping half-spaces.
    bool Inside(const Point& p,float tolerance=0)
    {
        return std::abs(p[0])<=2+tolerance && std::abs(p[2])<=3+tolerance
            && p[1]<=4-std::abs(p[0])+tolerance;
    }
}

int main()
try
{
    island::GabledRainShelter shelter;
    Require(shelter.VisibleFraction({0,4.2f,0},{0,3.6f,0})==1,
            "Unconfigured shelter must preserve the entire streak");
    shelter.Set(0,0,0,2,3,2,4);
    Require(std::abs(shelter.VisibleFraction({0,4.2f,0},{0,3.6f,0})-1.0f/3)<1e-6f,
            "A streak beginning above the ridge must stop at the roof");
    for (float sign : {-1.0f,1.0f}) {
        Require(std::abs(shelter.VisibleFraction({sign*1.5f,3,0},{sign*1.5f,2.4f,0})-5.0f/6)<1e-6f,
                "Both roof slopes must clip the rain");
        Require(std::abs(shelter.VisibleFraction({sign*2.2f,1.8f,0},{sign*1.8f,1.2f,0})-.5f)<1e-6f,
                "Wind-slanted streaks must not cross the eave boundary");
        Require(std::abs(shelter.VisibleFraction({0,1.8f,sign*3.2f},{0,1.2f,sign*2.8f})-.5f)<1e-6f,
                "Both gable ends must bound the shelter");
    }
    Require(shelter.VisibleFraction({3,3,0},{3,2.4f,0})==1,
            "Rain beyond the overhang must remain unchanged");
    Require(shelter.VisibleFraction({0,4.8f,0},{0,4.2f,0})==1,
            "Rain wholly above the roof must remain unchanged");
    Require(shelter.VisibleFraction({0,3,0},{0,2.4f,0})==0,
            "Rain already beneath the roof must remain excluded");

    std::size_t cases=0,oldLeaks=0;
    for (float yaw : {-2.1f,-.3f,0.0f,1.2f}) {
        island::GabledRainShelter turned;
        turned.Set(107,-81,yaw,2,3,2,4);
        const auto world=[&](const Point& p) {
            return Point{107+std::cos(yaw)*p[0]+std::sin(yaw)*p[2],p[1],
                         -81-std::sin(yaw)*p[0]+std::cos(yaw)*p[2]};
        };
        for (int ix=-18;ix<=18;++ix) for (int iz=-14;iz<=14;++iz) for (int iy=16;iy<=48;++iy) {
            // Stay off exact plane boundaries in the rotation sweep: translating a
            // boundary float can move it a few ULPs across that discontinuity. Exact
            // ridge/eave/end contacts are covered separately above in local coordinates.
            const Point a{ix*.15f+.011f,iy*.1f+.007f,iz*.25f+.013f};
            const Point b{a[0]+.16f,a[1]-.58f,a[2]-.07f};
            const float fraction=shelter.VisibleFraction(a,b);
            Require(std::isfinite(fraction) && fraction>=0 && fraction<=1,
                    "Clipping fraction must remain finite and bounded");
            const float rotated=turned.VisibleFraction(world(a),world(b));
            if (std::abs(fraction-rotated)>=.0002f)
                std::cerr<<"Rotation case "<<yaw<<" at "<<a[0]<<','<<a[1]<<','<<a[2]
                    <<" fractions "<<fraction<<'/'<<rotated<<'\n';
            Require(std::abs(fraction-rotated)<.0002f,
                    "Rain shelter clipping must follow the hut's translated/rotated frame");
            for (int sample=0;sample<16;++sample) {
                const float t=fraction*sample/16.0f;
                if (fraction>0) Require(!Inside(Along(a,b,t),-1e-5f),
                        "A retained streak prefix entered the shelter");
            }
            if (fraction<1) Require(Inside(Along(a,b,fraction),2e-5f),
                    "A clipped streak stopped before reaching the shelter");
            // The old point-only test emitted this entire streak through the roof.
            oldLeaks+=!Inside(a) && Inside(b) && fraction<1;
            ++cases;
        }
    }
    Require(oldLeaks>100,"The audit must include the original leaking-streak defect");
    for (int step=0;step<=450;++step) {
        const float distance=.3f+step*.1f;
        Require(island::rain_streak::HalfWidth(distance)/distance<.004f,
                "Near drip billboards must not expand into broad opaque bars");
        Require(island::rain_streak::NearOpacity(distance)>=0
                    && island::rain_streak::NearOpacity(distance)<=1,
                "Drip near-eye opacity must remain bounded");
    }
    Require(island::rain_streak::NearOpacity(.4f)==0
                && island::rain_streak::NearOpacity(3)==1,
            "Near-eye drops must blur away while distant drops remain visible");
    for (float bad : {std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}) {
        Require(shelter.VisibleFraction({bad,4,0},{0,3.6f,0})==0,
                "Non-finite streak coordinates must never reach the vertex buffer");
    }
    std::cout<<cases<<" rain shelter segments checked; "<<oldLeaks<<" original point-test leaks\n";
}
catch (const std::exception& error)
{
    std::cerr<<error.what()<<'\n';
    return 1;
}
