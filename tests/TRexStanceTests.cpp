#include "IslandSimulator/TRexMotion.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;const IslandTerrain terrain;const RenderedTerrainSurface ground(terrain);
    TRexMotion rex(terrain,{});rex.SetGround(&ground);
    int support=0,turnSupport=0,turnSwing=0,minSupport=2;float ballSlip=0,toeSlip=0,turned=0;
    const auto sample=[&](const TRexState& before,bool turn) {
        const auto& now=rex.State();int contacts=0;
        for(std::size_t i=0;i<2;++i) {
            const auto& leg=now.legs[i];contacts+=leg.planted;
            if(leg.planted&&before.legs[i].planted) {
                ++support;ballSlip=std::max(ballSlip,std::hypot(leg.ball.x-before.legs[i].ball.x,leg.ball.z-before.legs[i].ball.z));
                for(std::size_t j=0;j<3;++j)toeSlip=std::max(toeSlip,std::hypot(leg.toes[j].x-before.legs[i].toes[j].x,leg.toes[j].z-before.legs[i].toes[j].z));
                if(turn&&now.speed<.01f)++turnSupport;
            }
            if(turn&&now.speed<.01f&&!leg.planted)++turnSwing;
        }
        minSupport=std::min(minSupport,contacts);
        if(turn)turned+=std::abs(std::remainder(now.heading-before.heading,6.283185307f));
    };
    for(int i=0;i<180;++i){const auto before=rex.State();rex.Update(.05f);sample(before,false);}
    const auto start=rex.State();const float ox=start.x-std::sin(start.heading)*30,oz=start.z+std::cos(start.heading)*30;
    for(int i=0;i<160;++i){const auto before=rex.State();rex.Update(.05f,ox,oz,true,0);sample(before,true);}
    std::cout<<"support="<<support<<" ball-slip="<<ballSlip<<" toe-slip="<<toeSlip<<" min-support="<<minSupport
             <<" turn-support="<<turnSupport<<" stopped-swings="<<turnSwing<<" turned="<<turned<<'\n';
    TRexMotion night(terrain,{});night.SetGround(&ground);float nightJump=0;bool layDown=false,rose=false;
    for(int i=0;i<1200;++i) {
        const auto before=night.State();night.Update(.1f,0,0,false,i<800?1:0);
        const auto& now=night.State();layDown|=now.lie>.99f;rose|=i>=800&&now.lie==0;
        if(before.lie>0||now.lie>0)
            for(std::size_t j=0;j<2;++j)nightJump=std::max(nightJump,
                std::hypot(now.legs[j].ball.x-before.legs[j].ball.x,now.legs[j].ball.z-before.legs[j].ball.z));
    }
    std::cout<<"rest transition max ball displacement="<<nightJump<<'\n';
    TRexMotion route(terrain,{});route.SetGround(&ground);float distortion=0;
    for(int i=0;i<2400;++i) {
        route.Update(.05f);const auto& state=route.State();
        for(const auto& leg:state.legs) {
            const float length=std::hypot(std::hypot(leg.ankle.x-leg.ball.x,leg.ankle.z-leg.ball.z),leg.ankle.y-leg.ball.y);
            const float rise=TRexMotion::MetatarsusLength*(.866f*(1-state.lie)+.12f*state.lie);
            const float back=TRexMotion::MetatarsusLength*(.5f*(1-state.lie)+.99f*state.lie);
            distortion=std::max(distortion,std::abs(length-std::hypot(rise,back)));
        }
    }
    std::cout<<"max metatarsus length distortion="<<distortion<<'\n';
    if(distortion>.14f)throw std::runtime_error("Fixed feet overextend the existing leg rig");
    if(!layDown||!rose||nightJump>.45f)throw std::runtime_error("Rest/waking pops a previously anchored foot");
    if(support<200||ballSlip>.00003f||toeSlip>.00003f||minSupport<1||turnSupport<50||turnSwing<4||turned<1)
        throw std::runtime_error("Tyrannosaur drags its planted ball/toes or rotates without a step");
}
catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
