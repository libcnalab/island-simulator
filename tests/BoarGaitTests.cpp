#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/BoarGroundPose.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>

int main()
try
{
    const auto require=[](bool value,const char* message) {
        if (!value) throw std::runtime_error(message);
    };
    std::size_t flatContacts=0;
    // Independent world-space check: advance a body at constant speed and observe each
    // posed stance hoof. Neither the expected phase nor the stride formula is copied here.
    for (float scale : {.66f,1.0f,1.2f}) for (float speed : {.11f,.42f,.5f,.65f,.85f,2.8f}) {
        float phase=.17f,position=0;
        for (int frame=0;frame<720;++frame) {
            const float oldPhase=phase,oldPosition=position;
            constexpr float dt=1.0f/240.0f;
            position+=speed*dt;
            phase=island::AdvanceBoarGait(phase,position-oldPosition,speed,scale);
            require(std::isfinite(phase) && phase>=0 && phase<2*std::numbers::pi_v<float>,
                    "Distance-driven gait left its finite wrapped phase");
            for (bool front : {false,true}) for (bool left : {false,true}) {
                const float movement=island::BoarGaitMovement(speed);
                const auto a=island::EvaluateBoarLeg(oldPhase,movement,0,front,left);
                const auto b=island::EvaluateBoarLeg(phase,movement,0,front,left);
                if (!a.stance || !b.stance) continue;
                require(std::abs(position+scale*b.hoof.z-oldPosition-scale*a.hoof.z)<.000004f,
                        "A steady flat-ground stance hoof moves with the body");
                ++flatContacts;
            }
        }
    }
    const float nan=std::numeric_limits<float>::quiet_NaN();
    const float infinity=std::numeric_limits<float>::infinity();
    for (float input : {-1e-9f,-7.0f,0.0f,7.0f,nan,infinity})
        for (float distance : {0.0f,.1f}) {
            const float phase=island::AdvanceBoarGait(input,distance,.5f,1);
            require(std::isfinite(phase) && phase>=0 && phase<2*std::numbers::pi_v<float>,
                    "Normalized gait phase must exclude the period endpoint");
        }
    for (float invalid : {-1.0f,nan,infinity}) {
        require(island::AdvanceBoarGait(.3f,invalid,.5f,1)==.3f,
                "Invalid accepted travel advances the gait");
        require(island::AdvanceBoarGait(.3f,.1f,invalid,1)==.3f,
                "Invalid speed advances the gait");
        require(island::AdvanceBoarGait(.3f,.1f,.5f,invalid)==.3f,
                "Invalid body scale advances the gait");
    }
    require(island::AdvanceBoarGait(.3f,0,.5f,1)==.3f
        && island::AdvanceBoarGait(.3f,.1f,0,1)==.3f
        && island::AdvanceBoarGait(.3f,.1f,.5f,0)==.3f,
        "A stopped/blocked animal advances its gait");
    require(std::isfinite(island::AdvanceBoarGait(nan,.1f,.5f,1)),
            "Invalid input phase remains nonfinite");
    std::cout<<"flat steady stance contacts="<<flatContacts<<'\n';

    const island::IslandTerrain terrain;
    const island::RenderedTerrainSurface surface(terrain);
    island::BoarHerd inspection(terrain);
    float inspectedTravel=0;
    for (int frame=0;frame<64;++frame) {
        const auto before=inspection.Boars().front();
        inspection.Update(.1f,before.x,before.y+20.0f,before.z);
        const auto after=inspection.Boars().front();
        require(after.simulationTier==island::WildlifeSimulationTier::Full,
                "An overhead inspection observer throttles the animal");
        inspectedTravel+=std::hypot(after.x-before.x,after.z-before.z);
    }
    require(inspectedTravel>.1f,"Full-simulation motion inspection did not move naturally");
    island::BoarHerd herd(terrain);
    std::size_t contacts=0, slipping=0, adult=0, juvenile=0;
    float worstRatio=0;
    for (int frame=0;frame<3600;++frame) {
        std::array<island::BoarState,island::BoarHerd::BoarCount> previous;
        std::copy(herd.Boars().begin(),herd.Boars().end(),previous.begin());
        // All animals remain in full simulation, outside the observer's threat range.
        herd.Update(1.0f/120.0f,-75.0f,50.0f,-60.0f);
        for (std::size_t index=0;index<previous.size();++index) {
            const auto& a=previous[index];const auto& b=herd.Boars()[index];
            const float distance=std::hypot(b.x-a.x,b.z-a.z);
            if (distance<.0001f || a.speed!=b.speed || std::abs(a.headingRadians-b.headingRadians)>1e-5f
                || a.lieAmount>0 || b.lieAmount>0 || a.shake>0 || b.shake>0) continue;
            const auto before=island::FitBoarGroundPose(terrain,surface,a);
            const auto after=island::FitBoarGroundPose(terrain,surface,b);
            for (std::size_t foot=0;foot<4;++foot) {
                const auto& left=before.legs[foot];const auto& right=after.legs[foot];
                if (!left.stance || !right.stance || left.reachLimited || right.reachLimited) continue;
                const auto sole=[&](const auto& pose,const auto& leg) {
                    return pose.ToWorld({leg.hoof.x,leg.hoof.y-island::BoarLegPose::HoofHeight,
                                         leg.hoof.z+.025f});
                };
                const auto p=sole(before,left),q=sole(after,right);
                const float slip=std::hypot(q.x-p.x,q.z-p.z);
                worstRatio=std::max(worstRatio,slip/distance);
                slipping+=slip>distance*.05f+.00002f;
                ++contacts;(b.scale<.8f?juvenile:adult)++;
            }
        }
    }
    std::cout<<"live steady stance contacts="<<contacts<<" adult="<<adult<<" juvenile="<<juvenile
        <<" slipping="<<slipping<<" worst slip/body travel="<<worstRatio<<'\n';
    if (adult<100 || juvenile<100) throw std::runtime_error("Insufficient adult/juvenile moving stance coverage");
    if (slipping) throw std::runtime_error("Steady boar stance slides with the body over rendered ground");
}
catch (const std::exception& error)
{
    std::cerr<<error.what()<<'\n';return 1;
}
