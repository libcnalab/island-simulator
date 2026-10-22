#include "IslandSimulator/MineBatColony.hpp"
#include "IslandSimulator/MineLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include "IslandSimulator/VegetationLayout.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void Check(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
}

int main() {
    const island::IslandTerrain terrain;
    const island::MineLayout mine(terrain);
    constexpr float radius=island::PlayerMotion::CapsuleRadius,height=island::PlayerMotion::CapsuleHeight;
    const auto feet=[&](float across,float depth) {
        return mine.World(across,island::MineLayout::FloorRelative(depth)+.002f,depth);
    };
    // driven into the mountain: the slope covers the roof a few metres in, then deeply
    Check(mine.PortalDepth()>0.5f&&mine.PortalDepth()<5.0f,"the portal is not cut into the slope");
    for(float depth=mine.PortalDepth()+2.0f;depth<=island::MineSite::ChamberCentre+island::MineSite::ChamberHalfLength;depth+=2.0f) {
        const auto p=mine.World(0,0,depth);
        Check(terrain.Height(p.x,p.z)>mine.Entrance().y+island::MineLayout::FloorRelative(depth)
              +island::MineLayout::Headroom(0,depth)+1.0f,"the mountain does not cover the mine at depth "+std::to_string(depth));
    }
    // the forest grows clear of the cutting
    const auto cut=mine.World(0,0,1.5f);
    Check(island::VegetationLayout::IsReservedRoute(cut.x,cut.z),"the cutting is not kept clear of the forest");
    Check(!mine.IntersectsCapsule(feet(0,1),radius,height),"the portal is obstructed");
    Check(!mine.IntersectsCapsule(feet(0,25),radius,height),"the adit is obstructed");
    Check(!mine.IntersectsCapsule(feet(-1.5f,53),radius,height),"the chamber is obstructed");
    Check(mine.IntersectsCapsule(feet(1.0f,25),radius,height),"the adit's timber sets do not hold the walker in");
    Check(mine.IntersectsCapsule(feet(island::MineLayout::CartAcross,island::MineLayout::CartDepth),radius,height),
          "the tipped cart is not solid");
    float floor=0;
    const auto deep=mine.World(0,0,30);
    Check(mine.SupportHeight(deep.x,deep.z,mine.Entrance().y+island::MineLayout::FloorRelative(30),floor)
          &&std::abs(floor-(mine.Entrance().y+island::MineLayout::FloorRelative(30)))<1e-4f,"the adit's floor does not carry the walker");
    Check(!mine.SupportHeight(deep.x,deep.z,terrain.Height(deep.x,deep.z),floor),"the adit captures a walker on the mountain above");
    const auto portal=mine.World(0,1.0f,0.5f);
    Check(mine.HidesTerrain(portal.x,mine.Entrance().y+1.0f,portal.z),"the hillside still seals the portal");

    // The real controller: up the path into the cutting, along the adit past the lanterns to
    // the crystal chamber, round the cart, and back out.
    island::PlayerMotion player;
    player.SetMine(&mine);
    const auto outside=mine.World(0,0,-4.0f);
    player.SetPosition(terrain,outside.x,outside.z);
    const auto local=[&] {
        return mine.Local({player.Position().x,player.Position().y-island::PlayerMotion::EyeHeight,player.Position().z});
    };
    const auto walkTo=[&](float across,float depth,const char* leg) {
        for(int frame=0;frame<3600;++frame) {
            const auto here=local();
            const float da=across-here.x,dd=depth-here.z;
            if(std::hypot(da,dd)<0.15f) return;
            const auto a=mine.World(0,0,0),b=mine.World(da,0,dd);
            const float dx=b.x-a.x,dz=b.z-a.z,length=std::hypot(dx,dz);
            player.Update(terrain,dx/length,dz/length,1.0f/60.0f);
        }
        const auto here=local();
        throw std::runtime_error(std::string("the walker cannot ")+leg+": stuck at across "+std::to_string(here.x)
            +", depth "+std::to_string(here.z)+", up "+std::to_string(here.y));
    };
    walkTo(0.0f,2.0f,"enter the portal");
    walkTo(0.0f,30.0f,"walk down the adit");
    Check(std::abs(local().y-island::MineLayout::FloorRelative(30.0f))<0.08f,"the walker is not on the adit's floor");
    walkTo(0.0f,49.0f,"reach the chamber");
    walkTo(-1.6f,53.5f,"cross the chamber past the cart");
    Check(std::abs(local().y-island::MineLayout::FloorRelative(53.5f))<0.08f,"the walker is not on the chamber's floor");
    walkTo(0.0f,48.5f,"return to the adit");
    walkTo(0.0f,-3.0f,"walk back out of the mine");
    // the bats: hanging just under the dome by day, flying round under it when a walker comes,
    // out down the adit at dusk to hunt over the clearing, home again at dawn
    using Bats=island::MineBatColony;
    Bats bats(terrain,mine);
    for(std::size_t i=0;i<Bats::Count;++i) {
        const auto roost=mine.Local(bats.Roost(i));
        const float under=island::MineLayout::FloorRelative(roost.z)+Bats::DomeUp(roost.x,roost.z)-roost.y;
        Check(under>0.03f&&under<0.1f&&Bats::DomeUp(roost.x,roost.z)>2.8f,"a bat's roost is not just under the chamber's dome");
    }
    const auto run=[&](const float seconds,const island::MinePoint observer) {
        for(float t=0.0f;t<seconds;t+=1.0f/30.0f) bats.Update(1.0f/30.0f,observer.x,observer.y,observer.z);
    };
    const island::MinePoint far{1.0e4f,0.0f,1.0e4f};
    run(5.0f,far);
    Check(bats.HangingCount()==Bats::Count,"the bats do not hang by day");
    const auto underCluster=mine.World(0.0f,island::MineLayout::FloorRelative(island::MineSite::ChamberCentre)+1.7f,island::MineSite::ChamberCentre);
    bool insideChamber=true;
    for(float t=0.0f;t<4.0f;t+=1.0f/30.0f) {
        bats.Update(1.0f/30.0f,underCluster.x,underCluster.y,underCluster.z);
        for(const auto& bat:bats.Bats()) {
            const auto local=mine.Local({bat.x,bat.y,bat.z});
            const float up=local.y-island::MineLayout::FloorRelative(local.z);
            insideChamber=insideChamber&&up>0.4f&&up<Bats::DomeUp(local.x,local.z)+0.01f;
        }
    }
    Check(bats.FlutteringCount()>=4U&&bats.FlushCount()>=4U&&insideChamber,"a walker under the bats does not set them flying round under the dome");
    run(25.0f,far);
    Check(bats.HangingCount()==Bats::Count,"the flushed bats do not settle back when the chamber is quiet");
    bats.SetNightness(0.9f);
    bool inAdit=true;
    for(float t=0.0f;t<45.0f;t+=1.0f/30.0f) {
        bats.Update(1.0f/30.0f,far.x,far.y,far.z);
        for(const auto& bat:bats.Bats()) {
            const auto local=mine.Local({bat.x,bat.y,bat.z});
            if(local.z>1.0f&&local.z<island::MineSite::AditLength-1.0f) {
                const float up=local.y-island::MineLayout::FloorRelative(local.z);
                inAdit=inAdit&&std::abs(local.x)<island::MineSite::PostInner-0.3f&&up>0.8f&&up<island::MineSite::Height-0.3f;
            }
        }
    }
    Check(inAdit,"a bat leaving down the adit strays into its walls or roof");
    Check(bats.HuntingCount()==Bats::Count,"the bats are not all out hunting by night");
    for(const auto& bat:bats.Bats())
        Check(mine.Local({bat.x,bat.y,bat.z}).z<0.0f&&bat.y>terrain.Height(bat.x,bat.z)+2.0f,"a hunting bat is not out over the clearing");
    bats.SetNightness(0.1f);
    run(60.0f,far);
    Check(bats.HangingCount()==Bats::Count,"the bats are not home at dawn");
    // the island loading at night finds them out already
    Bats night(terrain,mine);
    night.SetNightness(1.0f);
    night.Update(1.0f/30.0f,far.x,far.y,far.z);
    Check(night.HuntingCount()==Bats::Count,"the island loading at night does not find the bats out");
    std::cout<<"Mine under the main mountain: portal, adit, chamber and exit routes valid; its bats roost, flush, leave and return\n";
}
