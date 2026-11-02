#include "IslandSimulator/DeerGait.hpp"
#include "IslandSimulator/DeerHoofSupport.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/WalkerShadow.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/StreamBedPatch.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>

namespace {
using P=island::DeerHoofPoint;
P Add(P a,P b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
P Mul(P a,float k){return {a.x*k,a.y*k,a.z*k};}
float Dot(P a,P b){return a.x*b.x+a.y*b.y+a.z*b.z;}
P Cross(P a,P b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
P Unit(P a){return Mul(a,1.0f/std::sqrt(Dot(a,a)));}
void Require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
// Reconstruct the actual tapered Tube sides independently, with denser samples than
// the support solver. Include both LODs, polygon edges and their long face interiors.
float LowestGap(const island::DeerHoofSupport& support,float scale,int sides,
                const island::RenderedTerrainSurface* ground,float flatHeight) {
    using H=island::DeerHoofSupport;
    const auto forward=Unit(support.forward),side=Unit(support.right);
    const auto other=Cross(forward,side);
    float lowest=std::numeric_limits<float>::infinity();
    for (float toe:{-H::ToeOffset,H::ToeOffset}) for (int ring=0;ring<sides;++ring) {
        const float a=2*std::numbers::pi_v<float>*static_cast<float>(ring)/sides+island::DeerHoofRingPhase(sides);
        const float b=2*std::numbers::pi_v<float>*static_cast<float>(ring+1)/sides+island::DeerHoofRingPhase(sides);
        const auto radialA=Add(Mul(side,std::cos(a)),Mul(other,std::sin(a)));
        const auto radialB=Add(Mul(side,std::cos(b)),Mul(other,std::sin(b)));
        for(int edge=0;edge<=4;++edge) for(int station=0;station<=4;++station) {
            const float t=static_cast<float>(station)/4.0f,u=static_cast<float>(edge)/4.0f;
            const auto radial=Add(Mul(radialA,1-u),Mul(radialB,u));
            const auto offset=Add(Add(Mul(forward,std::lerp(H::Rear,H::Front,t)),Mul(side,toe)),
                                  Mul(radial,std::lerp(H::RearRadius,H::FrontRadius,t)));
            const auto p=Add(support.centre,Mul(offset,scale));
            lowest=std::min(lowest,p.y-(ground?ground->Height(p.x,p.z):flatHeight));
        }
    }
    return lowest;
}
}

int main()
try {
    using namespace island;
    const IslandTerrain terrain;
    const RenderedTerrainSurface plain(terrain);
    const StreamBedPatch bed(StreamCourse(terrain),plain);
    RenderedTerrainSurface drawn(terrain);drawn.SetLocalPatches({&bed,nullptr});
    DeerHerd herd(terrain,{});
    std::size_t poses=0,feet=0,bankPoses=0;
    float worstPenetration=0,largestGap=0,largestCorrection=0,largestMainChange=0,largestPasternChange=0;
    const auto length=[](const walker_shadow::Limb& limb) {
        return std::hypot(limb.bx-limb.ax,limb.by-limb.ay,limb.bz-limb.az);
    };
    const auto inspect=[&](DeerState deer,const RenderedTerrainSurface* surface,bool bank) {
        const auto limbs=walker_shadow::DeerLimbs(deer,surface);
        const auto frame=MakeDeerSupportFrame(deer,surface);
        auto flat=deer;flat.x=flat.y=flat.z=flat.headingRadians=0;
        const auto reference=walker_shadow::DeerLimbs(flat);
        const P centre=Add(frame.origin,Mul(frame.up,1.1f*deer.scale));
        for(const auto& limb:limbs) {
            for(const auto p:std::array<P,2>{{{limb.ax,limb.ay,limb.az},{limb.bx,limb.by,limb.bz}}}) {
                const float distance=std::hypot(p.x-centre.x,p.y-centre.y,p.z-centre.z)+limb.radius;
                Require(distance<=DeerSupportFrame::CullRadius*deer.scale,
                        "Leaning deer shadow limb leaves the model culling bound");
            }
        }
        for (int leg=0;leg<4;++leg) {
            const auto& lower=limbs[3+3*leg];
            const float lift=(deer_gait::Sample(deer.gaitPhase,deer.speed,leg).lift
                *(1.0f-std::clamp(deer.bedAmount,0.0f,1.0f))
                +(leg==0?deer.stamp:0)*.14f)*deer.scale;
            const float baseX=lower.bx-frame.up.x*lift,baseZ=lower.bz-frame.up.z*lift;
            auto support=FitDeerHoofSupport(surface,deer.y,baseX,baseZ,deer.headingRadians,deer.scale);
            Require(std::abs(lower.by-support.centre.y-frame.up.y*lift)<4e-6f,
                    "Shadow ankle disagrees with fitted visible hoof");
            support.centre={lower.bx,lower.by,lower.bz};
            Require(std::abs(Dot(support.up,support.forward))<2e-6f
                && std::abs(Dot(support.up,support.right))<2e-6f
                && std::abs(Dot(support.right,support.forward))<2e-6f
                && std::abs(Dot(support.forward,support.forward)-1)<2e-6f
                && std::abs(Dot(support.right,support.right)-1)<2e-6f,
                "Deer hoof surface frame is not orthonormal");
            std::array<float,2> gaps{};
            for(int lod=0;lod<2;++lod) {
                const float gap=LowestGap(support,deer.scale,lod==0?8:6,surface,deer.y);
                gaps[lod]=gap;
                if(lift==0) {
                    worstPenetration=std::min(worstPenetration,gap);
                    largestGap=std::max(largestGap,gap);
                    Require(gap>=-.002f && gap<=.002f,"Deer hoof is buried or floating above its support");
                } else Require(gap>=lift*.4f-.002f && gap<lift*1.6f+.002f,
                    "Swinging/stamping hoof no longer lifts clear of the terrain");
            }
            Require(std::abs(gaps[0]-gaps[1])<.002f,"Hoof LOD transition changes ground contact");
            const float nominalY=frame.origin.y+(.060f*deer.scale
                -frame.up.x*(baseX-frame.origin.x)-frame.up.z*(baseZ-frame.origin.z))/frame.up.y;
            largestCorrection=std::max(largestCorrection,std::abs(lower.by-frame.up.y*lift-nominalY));
            for(int segment=0;segment<2;++segment) {
                const int index=1+3*leg+segment;
                const float change=std::abs(length(limbs[index])/length(reference[index])-1);
                largestMainChange=std::max(largestMainChange,change);
                Require(change<.25f,"Support fitting excessively stretches a main leg segment");
            }
            const float pastern=std::abs(length(lower)-length(reference[3+3*leg]))/deer.scale;
            largestPasternChange=std::max(largestPasternChange,pastern);
            Require(pastern<.04f,"Support fitting excessively stretches a pastern");
            ++feet;
        }
        ++poses;bankPoses+=bank;
    };
    // Flat translated planes retain contact through folding, stamping and body scaling.
    for(float scale:{.1f,.7f,1.0f,1.25f}) for(float fold:{0.0f,.5f,1.0f})
        for(float stamp:{0.0f,1.0f}) for(float yaw:{0.0f,.8f,2.3f}) {
            DeerState deer;deer.x=deer.z=0;deer.y=3.5f;deer.scale=scale;
            deer.headingRadians=yaw;deer.bedAmount=fold;deer.stamp=stamp;
            inspect(deer,nullptr,false);
        }
    for(std::size_t index=0;index<herd.Deer().size();++index) {
        const auto original=herd.Deer()[index];
        for(float dx:{-8.0f,0.0f,8.0f}) for(float dz:{-8.0f,0.0f,8.0f}) {
            if(!herd.IsValidPosition(index,original.x+dx,original.z+dz))continue;
            for(float heading:{0.0f,.8f,1.7f,3.5f}) for(float fold:{0.0f,.5f,1.0f})
                for(float stamp:{0.0f,1.0f}) {
                    auto deer=original;deer.x+=dx;deer.z+=dz;deer.y=drawn.Height(deer.x,deer.z);
                    deer.headingRadians=heading;deer.bedAmount=fold;deer.stamp=stamp;
                    deer.speed=1.2f;deer.gaitPhase=1.3f;
                    inspect(deer,&drawn,false);
                }
        }
        // Longer retreat strides and all walking feet still fit the existing joints.
        for(float speed:{.78f,3.1f}) for(float phase:{0.0f,2.1f,4.2f}) {
            auto deer=original;deer.y=drawn.Height(deer.x,deer.z);
            deer.speed=speed;deer.gaitPhase=phase;
            inspect(deer,&drawn,false);
        }
        float x,z;
        if(herd.DrinkPoint(index,x,z)) for(float shift:{-.5f,0.0f,.5f})
            for(float heading:{0.0f,1.1f,2.5f}) for(float fold:{0.0f,1.0f}) {
                auto deer=original;deer.x=x+shift;deer.z=z;deer.y=drawn.Height(deer.x,deer.z);
                deer.headingRadians=heading;deer.bedAmount=fold;
                inspect(deer,&drawn,true);
                if(fold==0.0f) {
                    deer.speed=.78f;deer.gaitPhase=2.1f;
                    inspect(deer,&drawn,true);
                }
            }
    }
    const float nan=std::numeric_limits<float>::quiet_NaN();
    for(const auto values:std::array<std::array<float,5>,5>{{
        {nan,0,0,0,1},{0,nan,0,0,1},{0,0,nan,0,1},{0,0,0,nan,1},{0,0,0,0,0}}}) {
        bool rejected=false;
        try{(void)FitDeerHoofSupport(nullptr,values[0],values[1],values[2],values[3],values[4]);}
        catch(const std::invalid_argument&){rejected=true;}
        Require(rejected,"Invalid hoof support input accepted");
    }
    Require(poses>300 && bankPoses>0,"Insufficient habitat/drinking-bank support coverage");
    std::cout<<"deer support poses="<<poses<<" feet="<<feet<<" bank-poses="<<bankPoses
        <<" min-gap="<<worstPenetration<<" max-gap="<<largestGap
        <<" largest ankle correction="<<largestCorrection<<"m main change="<<largestMainChange
        <<" pastern change="<<largestPasternChange<<"m/model scale\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
