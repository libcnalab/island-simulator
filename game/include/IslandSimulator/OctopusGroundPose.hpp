#pragma once

#include "IslandSimulator/OctopusGroup.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

namespace island::octopus_pose {
using Microsoft::Xna::Framework::Vector3;
inline constexpr int ArmSegments=7, ArmSides=6;
inline constexpr float ContactClearance=.004f;

struct Frame {
    Vector3 c,r,u,f;
    float s=1,contact=0;
    [[nodiscard]] Vector3 point(float x,float y,float z) const {
        return c+r*(x*s)+u*(y*s)+f*(z*s);
    }
};

[[nodiscard]] inline Frame MakeFrame(const OctopusState& o,const RenderedTerrainSurface* ground) {
    const Vector3 forward(std::cos(o.headingRadians),0,std::sin(o.headingRadians));
    Frame frame{{o.x,o.y,o.z},{-forward.Z,0,forward.X},Vector3::Up,forward,o.scale,0};
    if(!ground)return frame;
    frame.contact=std::clamp(o.groundAmount,0.0f,1.0f);
    if(frame.contact==0)return frame; // The existing swimming pose stays exact.
    const auto bed=ground->Sample(o.x,o.z);
    const Vector3 normal(bed.normal.x,bed.normal.y,bed.normal.z);
    frame.u=Vector3::Normalize(Vector3::Up+(normal-Vector3::Up)*frame.contact);
    frame.r=Vector3::Normalize(Vector3::Cross(forward,frame.u));
    frame.f=Vector3::Cross(frame.u,frame.r);
    // The mantle's lowest authored point is -.0203*scale, including its den posture.
    frame.c.Y=std::lerp(o.y,bed.height+.030f*o.scale,frame.contact);
    return frame;
}

struct ArmStation { Vector3 centre,across,along;float width=0; };
using Arm=std::array<ArmStation,ArmSegments+1>;

[[nodiscard]] inline Vector3 RingPoint(const Frame& frame,const ArmStation& q,int side) {
    const float theta=2*std::numbers::pi_v<float>*side/ArmSides+std::numbers::pi_v<float>/ArmSides;
    return q.centre+q.across*(std::cos(theta)*q.width)+frame.u*(std::sin(theta)*q.width*.65f);
}

[[nodiscard]] inline Arm MakeArm(const OctopusState& o,const Frame& b,int arm,
                                const RenderedTerrainSurface* ground) {
    const float angle=2*std::numbers::pi_v<float>*arm/8+.18f;
    const float hide=std::clamp(o.hideAmount,0.0f,1.0f),jet=std::clamp(o.jet,0.0f,1.0f);
    const float reach=arm==o.probeArm?std::clamp(o.probe,0.0f,1.0f):0;
    Arm result{};
    for(int segment=0;segment<=ArmSegments;++segment) {
        auto& q=result[static_cast<std::size_t>(segment)];
        const float t=static_cast<float>(segment)/ArmSegments;
        const float length=(.42f+.66f*(1-hide)+.12f*jet+.55f*reach)*t;
        const float curl=std::sin(o.armWave+arm*.93f+segment*.62f)*.17f*t*t*(1-.85f*jet)*(1-.9f*reach);
        const float a=angle+curl;
        q.centre=segment==0?b.point(std::cos(angle)*.17f,.075f,std::sin(angle)*.17f)
            :b.point(std::cos(a)*length,.047f+.035f*std::sin(o.armWave+segment*.8f)*(1-t),std::sin(a)*length);
        q.width=(segment==0?.094f:.088f*(1-t)*(1-.15f*t)+.006f)*b.s;
        q.across=b.f*std::cos(a)-b.r*std::sin(a);
        if(ground&&b.contact>0) {
            // Fit the actual six-sided underside, not an estimated centre height. Some
            // arms rise softly in the crawl; the others retain contact with the drawn bed.
            float supported=-1.0e6f;
            for(int side=0;side<ArmSides;++side) {
                const auto p=RingPoint(b,q,side);
                supported=std::max(supported,ground->Height(p.X,p.Z)-(p.Y-q.centre.Y));
            }
            const float lift=.020f*b.s*std::max(0.0f,std::sin(o.armWave+arm*.93f+segment*.8f));
            q.centre.Y=std::lerp(q.centre.Y,supported+ContactClearance*b.s+lift,b.contact);
        }
    }
    for(int segment=0;segment<=ArmSegments;++segment) {
        auto& q=result[static_cast<std::size_t>(segment)];
        q.along=Vector3::Normalize(result[static_cast<std::size_t>(std::min(segment+1,ArmSegments))].centre
            -result[static_cast<std::size_t>(std::max(segment-1,0))].centre);
    }
    return result;
}
}
