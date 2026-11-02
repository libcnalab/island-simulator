#include "IslandSimulator/TRexMotion.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
void Require(bool value,const char* message){if(!value)throw std::runtime_error(message);}
}
int main()
try {
    using namespace island;
    const IslandTerrain terrain;const RenderedTerrainSurface surface(terrain);
    TRexMotion rex(terrain,{});const auto initial=rex.State();
    Require(initial.y==terrain.Height(initial.x,initial.z),"Legacy tyrannosaur ground fallback changed");
    rex.SetGround(&surface);
    Require(rex.State().x==initial.x&&rex.State().z==initial.z&&rex.State().heading==initial.heading
        &&rex.State().behavior==initial.behavior,"Drawn support changes the tyrannosaur route or activity");
    int planted=0,swing=0;float worst=0;
    const auto check=[&] {
        const auto& r=rex.State();
        Require(std::abs(r.y-surface.Height(r.x,r.z))<1e-5f,"Tyrannosaur body ignores the visible terrain");
        for(const auto& leg:r.legs) {
            const float gap=leg.ball.y-.07f-surface.Height(leg.ball.x,leg.ball.z);
            Require(std::isfinite(gap)&&gap>=-1e-5f&&gap<.421f,"Tyrannosaur ball penetrates or floats beyond swing height");
            if(leg.planted){++planted;worst=std::max(worst,std::abs(gap));}
            else ++swing;
            for(const auto& toe:leg.toes)
                Require(toe.y-surface.Height(toe.x,toe.z)>.0199f,"Tyrannosaur toe is below the drawn surface");
        }
    };
    check();for(int i=0;i<2000;++i){rex.Update(.05f);check();}
    Require(worst<1e-5f&&planted>1000&&swing>100,"Tyrannosaur stance support drifts on its actual route");
    rex.PreviewRest();for(int i=0;i<50;++i){rex.Update(.1f);check();}
    Require(rex.State().lie>.99f,"Rest control did not exercise folded legs");
    rex.SetGround(nullptr);
    const auto& r=rex.State();Require(r.y==terrain.Height(r.x,r.z),"Tyrannosaur analytic fallback is lost");
    std::cout<<"Drawn tyrannosaur body/ball/toe support and rest: "<<planted<<" stance, "<<swing
             <<" swing samples; max planted error="<<worst<<'\n';
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
