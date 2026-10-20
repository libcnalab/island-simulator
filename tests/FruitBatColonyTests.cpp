#include "IslandSimulator/FruitBatColony.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/JungleLayout.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
{
    const auto require=[](const bool condition,const char* message) {
        if(!condition) throw std::runtime_error(message);
    };
    island::IslandTerrain terrain;
    island::JungleLayout jungle(terrain);
    island::FruitBatColony colony(terrain,jungle.Canopy());
    island::FruitBatColony repeated(terrain,jungle.Canopy());
    require(colony.Bats().size()==island::FruitBatColony::Count,"fruit-bat count changed");
    require(colony.Roosts().size()==island::FruitBatColony::Count,"fruit-bat roost count changed");
    std::size_t flying=0,roosting=0;
    for(std::size_t i=0;i<colony.Bats().size();++i) {
        const auto& bat=colony.Bats()[i]; const auto& same=repeated.Bats()[i];
        require(bat.x==same.x&&bat.y==same.y&&bat.z==same.z
            &&bat.behavior==same.behavior&&bat.scale==same.scale,
            "fruit-bat initialization is not deterministic");
        require(std::isfinite(bat.x)&&std::isfinite(bat.y)&&std::isfinite(bat.z),
            "fruit-bat position is not finite");
        require(terrain.SignedShoreDistance(colony.Roosts()[i].x,colony.Roosts()[i].z)>=45.0f,
            "fruit-bat roost escaped inland canopy");
        if(bat.behavior==island::FruitBatBehavior::DayFlight) ++flying; else ++roosting;
    }
    require(flying==island::FruitBatColony::MaximumDayFlights&&roosting==4U,
        "fixed daytime flight bound changed");
    for(int frame=0;frame<3600;++frame) {
        colony.Update(1.0f/60.0f,0.0f,20.0f,0.0f);
        repeated.Update(1.0f/60.0f,0.0f,20.0f,0.0f);
    }
    for(std::size_t i=0;i<colony.Bats().size();++i) {
        const auto& bat=colony.Bats()[i]; const auto& same=repeated.Bats()[i];
        require(bat.x==same.x&&bat.y==same.y&&bat.z==same.z&&bat.wingPose==same.wingPose,
            "fruit-bat simulation is not deterministic");
        require(std::hypot(bat.x-colony.Roosts()[i].x,bat.z-colony.Roosts()[i].z)<=12.2f,
            "fruit bat escaped its bounded crown home");
        require(bat.y>terrain.Height(bat.x,bat.z)+2.0f,"fruit bat clipped terrain");
    }
    require(colony.FullSimulationCount()+colony.ReducedSimulationCount()
        +colony.DormantSimulationCount()==static_cast<int>(island::FruitBatColony::Count),
        "fruit-bat activation tiers do not account for the colony");
    colony.Update(NAN,NAN,NAN,NAN);
    require(colony.DormantSimulationCount()==static_cast<int>(island::FruitBatColony::Count),
        "fruit bats do not fail closed for invalid updates");
    // Each roost remembers the crown it hangs in: the roost point sits beside that tree's
    // trunk, up in its crown.
    for(const auto& roost:colony.Roosts()) {
        require(roost.tree.scale>0.0f&&std::hypot(roost.x-roost.tree.x,roost.z-roost.tree.z)<1.0f,
            "a fruit-bat roost does not remember the crown it hangs in");
        require(roost.y>roost.tree.y+2.0f,"a fruit-bat roost's crown is not above its tree's foot");
    }
    {
        // Fanning: with the observer at a roost, within forty seconds a hanging bat fans its
        // wings for a few seconds, the fan rising to full and the wing pose beating, and
        // falls still again; a bat in flight never fans.
        island::FruitBatColony fanners(terrain,jungle.Canopy());
        const auto& sleeper=fanners.Bats()[2];
        float peak=0.0f,beatLow=1.0f,beatHigh=-1.0f; bool flyingFan=false,stilled=false;
        for(int step=0;step<2400;++step) {
            fanners.Update(1.0f/60.0f,sleeper.x+4.0f,sleeper.y,sleeper.z);
            for(const auto& bat:fanners.Bats()) {
                if(bat.behavior==island::FruitBatBehavior::DayFlight&&bat.fan>0.01f) flyingFan=true;
                if(bat.behavior!=island::FruitBatBehavior::Roost) continue;
                peak=std::max(peak,bat.fan);
                if(bat.fan>0.9f) { beatLow=std::min(beatLow,bat.wingPose); beatHigh=std::max(beatHigh,bat.wingPose); }
                if(peak>0.9f&&bat.fan<0.1f) stilled=true;
            }
        }
        require(fanners.FanCount()>=1&&peak>0.9f&&beatHigh-beatLow>1.0f&&stilled&&!flyingFan,
            "a hanging fruit bat does not fan its wings to cool");
        // The stretch: within a minute a hanging bat stretches a wing fully at least twice,
        // the wings taking turns, and folds it again; a bat in flight never stretches. (A
        // stretch easing off may overlap a fan beginning; only its start is gated.)
        island::FruitBatColony stretchers(terrain,jungle.Canopy());
        const auto& hanger=stretchers.Bats()[2];
        float stretchPeak=0.0f; bool leftWing=false,rightWing=false,folded=false,flyingStretch=false;
        for(int step=0;step<3600;++step) {
            stretchers.Update(1.0f/60.0f,hanger.x+4.0f,hanger.y,hanger.z);
            for(const auto& bat:stretchers.Bats()) {
                if(bat.behavior==island::FruitBatBehavior::DayFlight&&bat.stretch>0.01f) flyingStretch=true;
                if(bat.behavior!=island::FruitBatBehavior::Roost) continue;
                stretchPeak=std::max(stretchPeak,bat.stretch);
                if(bat.stretch>0.9f) { if(bat.stretchSide<0.0f) leftWing=true; else rightWing=true; }
                if(stretchPeak>0.9f&&bat.stretch<0.05f) folded=true;
            }
        }
        require(stretchers.StretchCount()>=2&&stretchPeak>0.9f&&leftWing&&rightWing&&folded&&!flyingStretch,
            "a hanging fruit bat does not stretch its wings in turn and fold them again");
    }
    std::cout<<"Fruit bats: six deterministic inland roosts, four hanging and two bounded daytime flights\n";
}
