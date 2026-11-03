#include "IslandSimulator/DeerHoofSupport.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/DeerGait.hpp"
#include <stdexcept>

namespace island {
namespace {
float Dot(DeerHoofPoint a,DeerHoofPoint b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
DeerHoofPoint Lerp(DeerHoofPoint a,DeerHoofPoint b,float t) {return a+(b-a)*t;}
}
DeerHoofPoint NeutralDeerHoof(const DeerSupportFrame& f,float s,int leg) noexcept {
    const bool hind=leg>=2;
    const float side=leg%2==0?-1.0f:1.0f;
    const auto body=f.origin+f.up*(1.03f*s);
    const auto hip=body+f.forward*((hind?-.47f:.48f)*s)+f.right*(side*.255f*s)-f.up*(.23f*s);
    const auto hoof=hip+f.forward*((hind?.12f:.05f)*s)-f.up*(.71f*s)+f.right*(side*.018f*s);
    return hoof+f.up*(DeerHoofSupport::AnkleHeight*s-Dot(hoof-f.origin,f.up));
}

DeerLegPose MakeDeerLegPose(const DeerState& deer,int leg,const RenderedTerrainSurface* ground) {
    if(leg<0||leg>=4)throw std::out_of_range("Invalid deer leg");
    const float s=deer.scale,bed=std::clamp(deer.bedAmount,0.0f,1.0f);
    const auto frame=MakeDeerSupportFrame(deer,ground);
    const auto origin=frame.origin,forward=frame.forward,right=frame.right,up=frame.up;
    const auto body=origin+up*((1.03f-.56f*bed)*s);
    const float fore=leg<2?.48f:-.47f,side=leg%2==0?-1.0f:1.0f;
    const bool hind=leg>=2;
    const auto step=deer_gait::Sample(deer.gaitPhase,deer.speed,leg);
    const auto hip=body+forward*(fore*s)+right*(side*.255f*s)-up*(.23f*s);
    const auto standingKnee=hip+forward*(((hind?.16f:-.07f)+step.along*.45f)*s)
        -up*(.37f*s)+right*(side*.018f*s);
    const auto standingHock=hip+forward*(((hind?-.06f:.01f)+step.along*.8f)*s)
        -up*(.71f*s)+right*(side*.018f*s);
    const auto standingHoof=hip+forward*(((hind?.12f:.05f)+step.along)*s)
        -up*(.71f*s)+right*(side*.018f*s);
    const auto foldedKnee=hip+forward*((hind?.22f:.10f)*s)
        -up*((hind?.12f:.14f)*s)+right*(side*.05f*s);
    const auto foldedHock=foldedKnee-forward*((hind?.28f:.30f)*s)-up*((hind?.05f:.03f)*s);
    const auto foldedHoof=foldedHock+forward*((hind?.30f:-.05f)*s);
    const bool plantedPose=deer.contactsValid&&bed==0;
    const auto& contact=deer.feet[static_cast<std::size_t>(leg)];
    const float stamp=(leg==0?std::clamp(deer.stamp,0.0f,1.0f):0)*.14f*s;
    const float lift=(plantedPose?contact.lift:step.lift*(1-bed)*s)+stamp;
    auto knee=Lerp(standingKnee,foldedKnee,bed)+up*(lift*.35f)+forward*(lift*.2f);
    auto hock=Lerp(standingHock,foldedHock,bed)+up*(lift*.8f)-forward*(lift*.3f);
    const auto hoof=Lerp(standingHoof,foldedHoof,bed)-forward*(lift*.2f);
    const auto nominal=hoof+up*(DeerHoofSupport::AnkleHeight*s-Dot(hoof-origin,up));
    const float x=plantedPose?contact.x:nominal.x,z=plantedPose?contact.z:nominal.z;
    const auto support=FitDeerHoofSupport(ground,deer.y,x,z,
        plantedPose?contact.heading:deer.headingRadians,s);
    // Share the target displacement through the same existing joints, including terrain relief.
    const DeerHoofPoint correction{x-nominal.x,support.centre.y-nominal.y,z-nominal.z};
    knee=knee+correction*DeerHoofSupport::KneeWeight;
    hock=hock+correction*DeerHoofSupport::HockWeight;
    // Seat the existing proximal muscle ring inside the chest/haunch. At the low,
    // wide hip anchor its bright end cap was exposed below and outside the barrel.
    // Knees, hocks and anchored hooves retain their solved positions.
    const auto collar=body+forward*((hind?-.40f:.40f)*s)+right*(side*.12f*s)+up*(.02f*s);
    return {hip,knee,hock,support.centre+up*lift,collar,support};
}
}
