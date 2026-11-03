#include "IslandSimulator/GroundInspectionCamera.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

template<class Height> float Clearance(float x,float z,float y,const std::array<float,3>& target,Height height) {
    float result=1000;
    for(int i=0;i<=2048;++i) {
        const float t=static_cast<float>(i)/2048;
        result=std::min(result,std::lerp(y,target[1],t)
            -height(std::lerp(x,target[0],t),std::lerp(z,target[2],t)));
    }
    return result;
}
int main()try {
    const auto ridge=[](float x,float){return 2.0f*std::max(0.0f,1.0f-std::abs(x-4.0f));};
    const std::array<float,3> target{6,.15f,0};
    if(Clearance(0,0,.65f,target,ridge)>=0)throw std::runtime_error("Fixture no longer reproduces the hidden downhill contact");
    const float eye=island::GroundInspectionEyeY(0,0,target,1.6f,ridge);
    if(Clearance(0,0,eye,target,ridge)<.119f)throw std::runtime_error("Inspection ray still crosses a ridge");
    const auto flat=[](float,float){return 0.0f;};
    if(island::GroundInspectionEyeY(0,0,target,1.6f,flat)>1.63f)
        throw std::runtime_error("An unobstructed inspection is raised unnecessarily");
    island::IslandTerrain terrain;
    const auto ground=std::make_unique<island::RenderedTerrainSurface>(terrain);
    const auto at=[&](float x,float z){return ground->Height(x,z);};
    // The accepted world camera's formerly occluded rock neighbourhood.
    const std::array<float,3> contact{-183.85f,at(-183.85f,-130.751f)+.15f,-130.751f};
    const float x=contact[0]-4,z=contact[2]-3;
    const float worldEye=island::GroundInspectionEyeY(x,z,contact,1.6f,at);
    if(Clearance(x,z,worldEye,contact,at)<.10f)throw std::runtime_error("World inspection is hidden by the drawn terrain");
    bool rejected=false;
    try{(void)island::GroundInspectionEyeY(0,0,target,-1,flat);}catch(const std::invalid_argument&){rejected=true;}
    if(!rejected)throw std::runtime_error("Invalid eye height was accepted");
    std::cout<<"Ground contact inspection clears synthetic and drawn-terrain ridges\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
