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
    std::cout<<"Mine under the main mountain: portal, adit, chamber and exit routes valid\n";
}
