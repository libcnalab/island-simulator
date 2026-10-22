#include "IslandSimulator/VolcanoSite.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include "IslandSimulator/VegetationLayout.hpp"

#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>

namespace {
void Check(bool value,const std::string& message){if(!value)throw std::runtime_error(message);}
}

int main() {
    using V=island::VolcanoSite;
    const island::IslandTerrain terrain;
    constexpr float Pi=std::numbers::pi_v<float>;
    // inland, its foot on land, clear of the forest's paths
    Check(terrain.SignedShoreDistance(V::X,V::Z)>V::BaseRadius+5.0f,"the volcano's foot reaches the sea");
    Check(island::VegetationLayout::IsReservedRoute(V::X+10.0f,V::Z),"the forest grows on the cone");
    Check(!V::Reserves(V::X+V::BaseRadius+5.0f,V::Z),"the volcano reserves ground beyond its foot");
    // the rim is the summit, the crater's wall falls inside it to the lake
    for(int k=0;k<12;++k) {
        const float theta=2.0f*Pi*static_cast<float>(k)/12.0f;
        Check(std::abs(V::Profile(V::RimRadius-0.001f,theta)-V::Profile(V::RimRadius+0.001f,theta))<0.05f&&V::Profile(V::RimRadius,theta)>V::Height-0.4f,
              "the crater's wall and the outer slope part at the rim");
        Check(V::Profile(V::LavaRadius,theta)<V::Height-V::CraterDepth,"the lake is not down in the crater");
        // nowhere on the outer slope steeper than a walker climbs
        for(float r=V::RimRadius+0.2f;r<V::BaseRadius-0.5f;r+=0.5f) {
            const float rise=V::Profile(r,theta)-V::Profile(r+0.5f,theta);
            Check(std::atan2(rise,0.5f)<0.66f,"the cone is too steep to climb at r "+std::to_string(r));
        }
    }
    Check(V::BlocksWalker(V::X+5.0f,V::Z)&&!V::BlocksWalker(V::X+V::RimRadius+0.5f,V::Z),"the rim does not stop the walker");
    Check(std::abs(V::SupportHeight(V::X+20.0f,V::Z,0.0f)-V::SurfaceY(V::X+20.0f,V::Z))<1e-4f,"the cone does not carry the walker");

    // The real controller: from the lowland up the cone to the rim, where it stops, and back down.
    island::PlayerMotion player;
    player.SetVolcano(true);
    const float bearing=V::FlowAngle+1.4f;
    player.SetPosition(terrain,V::X+std::cos(bearing)*(V::BaseRadius+6.0f),V::Z+std::sin(bearing)*(V::BaseRadius+6.0f));
    for(int frame=0;frame<60*45;++frame) player.Update(terrain,-std::cos(bearing),-std::sin(bearing),1.0f/60.0f);
    const auto& p=player.Position();
    const float r=V::Radius(p.x,p.z);
    Check(r>V::RimRadius-0.3f&&r<V::RimRadius+0.8f,"the walker does not reach the rim: r "+std::to_string(r));
    Check(p.y-island::PlayerMotion::EyeHeight>V::BaseY+V::Height-1.2f,"the walker is not up on the rim");
    for(int frame=0;frame<60*20;++frame) player.Update(terrain,std::cos(bearing),std::sin(bearing),1.0f/60.0f);
    Check(V::Radius(player.Position().x,player.Position().z)>V::RimRadius+10.0f,"the walker cannot come back down the cone");
    std::cout<<"Volcano: a climbable cone to its rim, the crater kept from the walker\n";
}
