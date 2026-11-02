#include "IslandSimulator/ElephantMotion.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/LocalTerrainPatch.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WalkerShadow.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
void Require(bool value,const char* message) {if(!value)throw std::runtime_error(message);}
}
int main()
try {
    using namespace island;
    const IslandTerrain terrain;RenderedTerrainSurface ground(terrain);
    ElephantMotion elephant(terrain,{});elephant.PlaceNear(220.7f,59.14f);
    const auto before=elephant.State();
    Require(before.y==terrain.Height(before.x,before.z),"Legacy analytic ground fallback changed");
    float discrepancy=0;
    for(const auto& leg:before.legs)
        discrepancy=std::max(discrepancy,std::abs(leg.foot.y-ground.Height(leg.foot.x,leg.foot.z)));
    Require(discrepancy>.15f,"Contact fixture no longer exercises the coarse terrain disagreement");
    elephant.SetGround(&ground);
    Require(elephant.State().x==before.x&&elephant.State().z==before.z
        &&elephant.State().heading==before.heading&&elephant.State().behavior==before.behavior,
        "Installing drawn support changes navigation or activity");
    int planted=0,swing=0;float worst=0;
    const auto check=[&] {
        const auto& e=elephant.State();
        Require(std::abs(e.y-ground.Height(e.x,e.z))<1e-5f,"Body ground point ignores drawn terrain");
        const auto shadow=walker_shadow::ElephantLimbs(elephant);
        for(std::size_t i=0;i<e.legs.size();++i) {
            const auto& leg=e.legs[i];
            const float gap=leg.foot.y-ground.Height(leg.foot.x,leg.foot.z);
            Require(std::isfinite(gap)&&gap>=-1e-5f&&gap<=.2001f,"Sole sinks into or floats beyond swing over drawn ground");
            if(leg.planted){++planted;worst=std::max(worst,std::abs(gap));}
            else ++swing;
            const auto& s=shadow[10+i*2];
            Require(std::abs(s.bx-leg.foot.x)<1e-5f&&std::abs(s.by-leg.foot.y-.12f)<1e-5f
                &&std::abs(s.bz-leg.foot.z)<1e-5f,"Elephant shadow ignores the supported leg pose");
        }
    };
    check();
    for(int i=0;i<1800;++i){elephant.Update(.05f);check();}
    Require(worst<1e-5f&&planted>1000&&swing>100,"Actual route did not maintain planted contact through walking");
    // A local replacement changes both feet and body support without a second terrain system.
    elephant.PlaceNear(220.7f,59.14f);const auto e=elephant.State();
    const LocalTerrainPatch patch({LocalTerrainPatch::CoarseIndexBelow(e.x-7),LocalTerrainPatch::CoarseIndexAbove(e.x+7),
        LocalTerrainPatch::CoarseIndexBelow(e.z-7),LocalTerrainPatch::CoarseIndexAbove(e.z+7)},2,ground,
        [e](float x,float z,float h){return h-.3f*std::max(0.0f,1-((x-e.x)*(x-e.x)+(z-e.z)*(z-e.z))/64);});
    ground.SetLocalPatches({&patch,nullptr});elephant.SetGround(&ground);check();
    Require(elephant.State().y<e.y-.2f,"Local replacement fixture did not change support");
    elephant.SetGround(nullptr);
    Require(elephant.State().y==terrain.Height(e.x,e.z),"Removing drawn support loses analytic fallback");
    std::cout<<"Elephant drawn body/sole contact, local patch, shared shadow and fallback: "
             <<planted<<" support, "<<swing<<" swing samples; max planted error="<<worst<<'\n';
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
