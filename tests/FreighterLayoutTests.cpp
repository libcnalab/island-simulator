#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/FreighterLayout.hpp"
#include "IslandSimulator/LifeboatLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include "IslandSimulator/SurfSprayField.hpp"

#include <cmath>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

namespace {
void Check(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
}

int main() {
    using Ship=island::FreighterPlacement;
    const island::IslandTerrain terrain;
    const auto found=island::FreighterLayout::FindInWorld(terrain,{},{});
    Check(found.has_value(),"no beach holds the grounded freighter");
    const Ship& ship=*found;
    constexpr float radius=island::PlayerMotion::CapsuleRadius,height=island::PlayerMotion::CapsuleHeight;
    const auto feet=[&](float along,float up,float across) {return ship.World(along,up+.002f,across);};
    const auto stern=ship.World(3.0f,0.0f,0.0f),bow=ship.World(27.0f,0.0f,0.0f);
    Check(terrain.Height(stern.x,stern.z)<-0.3f,"the stern does not lie in the surf");
    Check(terrain.SignedShoreDistance(bow.x,bow.z)>3.0f,"the bow is not up the beach");
    Check(ship.World(Ship::HoldAft,Ship::HoldFloor,-Ship::HalfBeam).y>0.2f,"the hold's floor lies below the sea");
    const float rise=Ship::DeckUp(Ship::GangwayFore)-ship.ladderFootUp;
    Check(rise>1.5f&&rise/ship.ladderRun<0.71f,"the accommodation ladder is missing or steeper than a stair");
    const auto foot=ship.LadderFoot();
    Check(terrain.Height(foot.x,foot.z)>0.1f,"the ladder's foot is in the water");

    // the walls: open where the walker goes, solid where the ship is
    Check(!ship.IntersectsCapsule(feet(14.0f,Ship::DeckUp(14.0f),2.85f),radius,height),"the starboard walkway is obstructed");
    Check(!ship.IntersectsCapsule(feet(15.0f,Ship::HoldFloor,0.0f),radius,height),"the hold is obstructed");
    Check(!ship.IntersectsCapsule(feet(5.0f,Ship::DeckUp(5.0f),1.0f),radius,height),"the deckhouse's mess is obstructed");
    Check(ship.IntersectsCapsule(feet(15.0f,Ship::DeckUp(15.0f),Ship::HatchHalf+0.15f),radius,height),"the hatch's coaming is not solid");
    Check(ship.IntersectsCapsule(feet(18.0f,Ship::DeckUp(18.0f),Ship::HalfBeam),radius,height),"the bulwark is not solid");
    Check(ship.IntersectsCapsule(feet(8.0f,-0.8f,Ship::HalfBeam+0.2f),radius,height),"a swimmer passes through the hull");
    float up=0.0f;
    Check(ship.SupportUp(15.0f,1.0f,Ship::DeckUp(15.0f),up)&&std::abs(up-Ship::HoldFloor)<1e-4f,
          "the open hatch does not drop to the hold's floor");
    Check(ship.SupportUp(14.0f,2.85f,Ship::DeckUp(14.0f),up)&&std::abs(up-Ship::DeckUp(14.0f))<1e-4f,
          "the deck does not carry a walker on it");
    Check(ship.SupportUp(14.0f,2.85f,Ship::HoldFloor,up)&&std::abs(up-Ship::HoldFloor)<1e-4f,
          "the deck captures a walker in the hold beneath it");

    // The real controller: up the ladder from the sand, through the gangway, round the hatch and
    // down the stair into the hold, then back up to the deck.
    island::PlayerMotion player;
    player.SetFreighter(&ship);
    player.SetPosition(terrain,foot.x,foot.z);
    const auto local=[&] {
        return ship.Local({player.Position().x,player.Position().y-island::PlayerMotion::EyeHeight,player.Position().z});
    };
    const auto walkTo=[&](float along,float across,const char* leg) {
        for(int frame=0;frame<1800;++frame) {
            const auto here=local();
            const float da=along-here.x,dc=across-here.z;
            if(std::hypot(da,dc)<0.12f) return;
            const float dx=ship.dirX*da+ship.AcrossX()*dc,dz=ship.dirZ*da+ship.AcrossZ()*dc;
            const float length=std::hypot(dx,dz);
            player.Update(terrain,dx/length,dz/length,1.0f/60.0f);
        }
        const auto here=local();
        throw std::runtime_error(std::string("the walker cannot ")+leg+": stuck at along "+std::to_string(here.x)
            +", across "+std::to_string(here.z)+", up "+std::to_string(here.y));
    };
    const float ladderMiddle=Ship::HalfBeam+0.1f+Ship::LadderWidth*0.5f;
    walkTo(ship.LadderFootAlong()+0.4f,ladderMiddle,"reach the ladder's foot");
    walkTo(13.0f,ladderMiddle,"climb the accommodation ladder");
    Check(std::abs(local().y-Ship::DeckUp(Ship::GangwayFore))<0.08f,"the ladder's head is not level with the deck");
    walkTo(13.0f,2.85f,"step through the gangway onto the deck");
    walkTo(10.6f,2.85f,"walk aft along the starboard side");
    walkTo(10.6f,-2.85f,"cross the deck behind the hatch");
    walkTo(21.0f,-2.85f,"walk forward along the port side");
    walkTo(21.0f,2.75f,"cross the foredeck");
    walkTo(15.2f,2.75f,"go down the stair into the hold");
    walkTo(12.5f,1.0f,"walk into the hold");
    Check(std::abs(local().y-Ship::HoldFloor)<0.08f,"the walker is not on the hold's floor");
    walkTo(15.2f,2.75f,"return to the stair's foot");
    walkTo(21.0f,2.75f,"climb the stair back to the deck");
    Check(std::abs(local().y-Ship::DeckUp(21.0f))<0.08f,"the stair does not bring the walker back to the deck");
    walkTo(21.0f,-2.85f,"cross the foredeck again");
    walkTo(10.6f,-2.85f,"walk aft along the port side");
    walkTo(10.6f,1.05f,"cross behind the hatch to the deckhouse's door");
    walkTo(8.6f,1.05f,"walk up to the deckhouse's door");
    walkTo(6.5f,1.05f,"enter the deckhouse");
    Check(std::abs(local().y-Ship::DeckUp(6.5f))<0.08f,"the deckhouse's floor does not carry the walker");
    // its lifeboat, keel up on dry level sand along the beach, clear of the hull
    const auto boat=island::LifeboatLayout::FindNear(terrain,ship,{},{});
    Check(boat.has_value(),"the coaster's lifeboat finds no beach beside it");
    for(const float along:{-3.6f,0.0f,3.6f}) for(const float across:{-1.15f,1.15f}) {
        const auto p=boat->At(along,across);
        Check(terrain.Sample(p[0],p[1]).surface==island::TerrainSurface::DrySand&&!ship.Blocks(p[0],p[1],2.5f),
              "the lifeboat does not lie on dry sand clear of the ship");
    }
    Check(boat->Obstacles().size()==3U&&std::hypot(boat->x-foot.x,boat->z-foot.z)<60.0f,"the lifeboat lies far from its ship");
    // the birds' perches on her: the mastheads high over the deck, the bow rail on the bulwark's
    // cap over the port bow, the lifeboat's keel a metre over the sand
    const auto perches=ship.BirdPerches();
    const auto deckAt=[&](const island::BirdPerchSite& site) {return Ship::DeckUp(ship.Local({site.x,site.y,site.z}).x);};
    Check(ship.Local({perches[0].x,perches[0].y,perches[0].z}).y>deckAt(perches[0])+7.5f
          &&ship.Local({perches[1].x,perches[1].y,perches[1].z}).y>deckAt(perches[1])+8.5f,"the mastheads' perches are not up at the trucks");
    const auto rail=ship.Local({perches[2].x,perches[2].y,perches[2].z});
    Check(std::abs(rail.y-(Ship::DeckUp(rail.x)+Ship::Bulwark+0.02f))<0.01f&&rail.z<0.0f
          &&std::abs(-rail.z-Ship::HalfBreadth(rail.x))<0.1f,"the bow rail's perch is not on the port bulwark's cap");
    const auto keel=boat->KeelPerch();
    const float keelOver=keel.y-terrain.Height(keel.x,keel.z);
    Check(keelOver>0.8f&&keelOver<1.1f&&std::hypot(keel.x-boat->x,keel.z-boat->z)<1.0f,"the keel's perch is not on the upturned lifeboat");
    // the flock takes them all and seats a bird on one as the island loads
    island::BirdFlock birds(terrain);
    const std::vector<island::BirdPerchSite> sites{perches[0],perches[1],perches[2],keel};
    birds.SetPerchSites(sites);
    birds.SettlePerched(1U,3U);
    Check(birds.Birds()[1].behavior==island::BirdBehavior::Perched&&birds.Birds()[1].perchSiteIndex==3U
          &&std::abs(birds.Birds()[1].y-keel.y-birds.Birds()[1].scale*0.12f)<0.001f,"a bird settled on the keel does not stand on it");
    // the breakers strike her stern in the surf: spray thrown up off it within a breaker's cycle
    {
        island::SurfSprayField spray(terrain);
        const auto stern=ship.World(0.0f,0.0f,0.0f);
        spray.AddFace(stern.x,stern.z,1.6f,2.6f,-ship.dirX,-ship.dirZ,2.2f);
        Check(spray.RockCount()==1U,"the stern does not stand as a spray face");
        bool nearStern=false;
        for(float t=0.0f;t<25.0f;t+=1.0f/30.0f) {
            spray.Update(1.0f/30.0f,t,stern.x,stern.z);
            for(const auto& puff:spray.Puffs())
                nearStern=nearStern||(puff.active&&std::hypot(puff.x-stern.x,puff.z-stern.z)<3.0f&&puff.size>0.4f);
        }
        Check(spray.BurstCount()>=1U&&nearStern,"no breaker throws spray up the coaster's stern");
    }
    std::cout<<"Freighter on the beach: ladder, deck, hatch, stair, hold and deckhouse routes valid\n";
}
