#pragma once

#include "IslandSimulator/OctopusGroundPose.hpp"
#include <sstream>
#include <string>

namespace island::octopus_inspection {
// Optional capture proof: sample the same rings and surface used by the visible renderer.
[[nodiscard]] inline std::string MotionProof(const OctopusState& o,const RenderedTerrainSurface& ground,
                                            unsigned frame,float seconds,std::size_t subject,int visible) {
    const auto body=octopus_pose::MakeFrame(o,&ground);
    float minimum=1000,maximum=0;
    for(int arm=0;arm<8;++arm) {
        const auto pose=octopus_pose::MakeArm(o,body,arm,&ground);
        for(const auto& station:pose) {
            float closest=1000;
            for(int side=0;side<octopus_pose::ArmSides;++side) {
                const auto p=octopus_pose::RingPoint(body,station,side);
                closest=std::min(closest,(p.Y-ground.Height(p.X,p.Z))/o.scale);
            }
            minimum=std::min(minimum,closest);maximum=std::max(maximum,closest);
        }
    }
    std::ostringstream proof;proof.precision(9);
    proof<<"island-simulator: octopus motion stage="<<frame/4U<<" updates="<<frame<<" time="<<seconds
        <<" subject="<<subject<<" behavior="<<static_cast<unsigned>(o.behavior)<<" contact="<<o.groundAmount
        <<" visible="<<visible<<" gap_min="<<minimum<<" gap_max="<<maximum
        <<" position="<<o.x<<','<<o.y<<','<<o.z;
    return proof.str();
}
}
