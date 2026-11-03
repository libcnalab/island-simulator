#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/WetlandMistField.hpp"
#include "IslandSimulator/ValleyMistField.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

template<class Field,class Advance>
void CheckRecycling(const char* name,Field field,Advance advance) {
    auto twin=field;
    int wraps=0;float worstStep=0,maxGap=0;bool interior=false;
    for(int step=0;step<6000;++step) {
        const auto puffs=field.Puffs();
        const std::vector<island::MistPuff> old(puffs.begin(),puffs.end());
        advance(field,step);advance(twin,step);
        for(std::size_t i=0;i<old.size();++i) {
            const auto& a=old[i];const auto& b=field.Puffs()[i];
            const auto& c=twin.Puffs()[i];
            if(b.x!=c.x||b.y!=c.y||b.z!=c.z||b.density!=c.density||b.age!=c.age||b.active!=c.active)
                throw std::runtime_error("Mist recycling is not deterministic");
            if(!std::isfinite(b.density)||b.density<0||b.density>1)
                throw std::runtime_error("Invalid mist opacity");
            if(step>0)worstStep=std::max(worstStep,std::abs(b.density-a.density));
            interior=interior||(b.age>4&&b.density>.2f);
            const float jump=std::hypot(b.x-a.x,b.z-a.z);
            if(jump>8) {
                ++wraps;maxGap=std::max(maxGap,jump);
                std::cout<<name<<" wrap "<<step<<" opacity "<<a.density<<" -> "<<b.density<<'\n';
                if(a.density>.001f||b.density>.001f||b.age>.101f)
                    throw std::runtime_error("A visible mist puff teleports at recycling");
            }
        }
    }
    if(wraps<3||!interior||worstStep>.06f)
        throw std::runtime_error("Mist fades abruptly, never recycles or loses its established interior");
    std::cout<<name<<" wraps="<<wraps<<" maximum position jump="<<maxGap
        <<" maximum opacity step="<<worstStep<<'\n';
}

int main()try {
    island::IslandTerrain terrain;
    CheckRecycling("wetland",island::WetlandMistField(terrain),[](auto& field,int step) {
        field.Update(.1f,static_cast<float>(step)*.1f);
    });
    CheckRecycling("valley",island::ValleyMistField(terrain),[](auto& field,int) {
        field.Update(.1f,6.5f,.3f,.8f);
    });
    island::ValleyMistField valley(terrain);
    for(int i=0;i<2000;++i) {
        const auto old=valley.Puffs();const std::vector<island::MistPuff> previous(old.begin(),old.end());
        valley.Update(.1f,6.5f,.3f,.8f);
        for(std::size_t p=0;p<previous.size();++p)if(std::hypot(previous[p].x-valley.Puffs()[p].x,previous[p].z-valley.Puffs()[p].z)>8) {
            const auto& puff=valley.Puffs()[p];
            const float lift=puff.y-terrain.Height(puff.x,puff.z);
            if(lift<.2f||lift>2.1f)throw std::runtime_error("Recycled valley mist retains the distant valley floor height");
        }
    }
    valley.Update(.1f,12,.3f,.8f);
    for(const auto& puff:valley.Puffs())if(puff.active||puff.density!=0)
        throw std::runtime_error("Recycling prevents the morning mist burning off");
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
