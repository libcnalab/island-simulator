#include "IslandSimulator/ElephantMotion.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;
    const IslandTerrain terrain;const RenderedTerrainSurface ground(terrain);
    ElephantMotion elephant(terrain,{});elephant.SetGround(&ground);
    int support=0,turnSupport=0,turnSwing=0,minSupport=4;float worst=0,turnWorst=0,turned=0;
    const auto sample=[&](const ElephantState& before,bool turning) {
        const auto& now=elephant.State();
        int contacts=0;
        for(std::size_t i=0;i<4;++i) {
            contacts+=now.legs[i].planted;
            const auto& leg=now.legs[i];
            if(leg.planted&&before.legs[i].planted) {
                const float slip=std::hypot(leg.foot.x-before.legs[i].foot.x,leg.foot.z-before.legs[i].foot.z);
                ++support;worst=std::max(worst,slip);
                if(turning&&now.speed<.01f) {++turnSupport;turnWorst=std::max(turnWorst,slip);}
            }
            if(turning&&now.speed<.01f&&!leg.planted)++turnSwing;
            const float gap=leg.foot.y-ground.Height(leg.foot.x,leg.foot.z);
            if(!std::isfinite(gap)||gap<-.0001f||gap>.201f)
                throw std::runtime_error("Supported elephant foot leaves the rendered terrain/swing bounds");
        }
        minSupport=std::min(minSupport,contacts);
        if(turning)turned+=std::abs(std::remainder(now.heading-before.heading,6.283185307f));
    };
    for(int frame=0;frame<360;++frame) {const auto before=elephant.State();elephant.Update(.05f);sample(before,false);}
    const auto start=elephant.State();
    const float observerX=start.x-std::sin(start.heading)*25,observerZ=start.z+std::cos(start.heading)*25;
    for(int frame=0;frame<140;++frame) {const auto before=elephant.State();
        elephant.Update(.05f,observerX,observerZ,true,0,0,0,false);sample(before,true);}
    std::cout<<"support="<<support<<" max-slip="<<worst<<" turn-support="<<turnSupport
             <<" turn-slip="<<turnWorst<<" min-support="<<minSupport<<" turn-swing="<<turnSwing<<" turned="<<turned<<'\n';
    // The previous rig needed up to22cm of wrist correction on this coarse coastal
    // route. Fixed contacts must not worsen that reach while turning and restarting.
    ElephantMotion route(terrain,{});route.SetGround(&ground);float wristResidual=0;
    for(int frame=0;frame<2400;++frame) {
        const auto before=route.State();
        if(frame>360&&frame<500)route.Update(.05f,before.x-std::sin(before.heading)*25,
            before.z+std::cos(before.heading)*25,true,0,0,0,false);
        else route.Update(.05f);
        for(std::size_t i=0;i<4;++i) {
            const auto& leg=route.State().legs[i];
            if(leg.planted)wristResidual=std::max(wristResidual,std::abs(leg.low.y-leg.foot.y
                -(i<2?ElephantMotion::WristHeight:ElephantMotion::HockHeight)));
        }
    }
    std::cout<<"coastal wrist correction="<<wristResidual<<'\n';
    // Keep at least a2cm margin below the measured22cm legacy correction.
    if(wristResidual>.20f)throw std::runtime_error("Fixed feet overextend the existing leg rig");
    if(minSupport<2||support<500||worst>.00003f||turnSupport<100||turnWorst>.00003f||turnSwing<10||turned<1)
        throw std::runtime_error("Elephant stance slides with acceleration/turning or turns without lifting its feet");
}
catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
