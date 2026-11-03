#include "IslandSimulator/CraterGlow.hpp"
#include "IslandSimulator/VolcanoFlowGlow.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    for(float height:{8.0f,14.0f})for(float radius:{15.0f,28.0f})
        for(float up:{0.0f,.25f,.5f,.9f,1.0f}) {
            const float extent=island::CraterGlowHalfHeight(height,radius*1.3f,up);
            if(!std::isfinite(extent)||extent<0||height-extent*up<.049f)
                throw std::runtime_error("Crater glow crosses the opaque lake and creates a depth-clipped band");
        }
    if(island::CraterGlowHalfHeight(50,10,1)!=10)
        throw std::runtime_error("Already clear glow is unnecessarily flattened");
    const float nan=std::numeric_limits<float>::quiet_NaN();
    if(island::CraterGlowHalfHeight(nan,10,1)!=0||island::CraterGlowHalfHeight(10,nan,1)!=0)
        throw std::runtime_error("Invalid glow geometry reaches a vertex buffer");
    const auto mesh=island::BuildVolcanoFlowGlow();float clearance=1e9f;
    if(mesh.empty()||mesh.size()%3||mesh.size()+1668>4096)
        throw std::runtime_error("Ground glow exceeds the shared light buffer or has incomplete triangles");
    for(std::size_t i=0;i<mesh.size();i+=3) {
        for(int a=0;a<=8;++a)for(int b=0;b<=8-a;++b) {
            const float u=a/8.0f,v=b/8.0f,w=1-u-v;
            const float x=u*mesh[i].x+v*mesh[i+1].x+w*mesh[i+2].x;
            const float y=u*mesh[i].y+v*mesh[i+1].y+w*mesh[i+2].y;
            const float z=u*mesh[i].z+v*mesh[i+1].z+w*mesh[i+2].z;
            clearance=std::min(clearance,y-island::VolcanoSite::SurfaceY(x,z));
        }
        for(std::size_t j=i;j<i+3;++j)if(!std::isfinite(mesh[j].x)||!std::isfinite(mesh[j].y)||!std::isfinite(mesh[j].z)
            ||!std::isfinite(mesh[j].weight)||mesh[j].weight<0||mesh[j].weight>1)
            throw std::runtime_error("Invalid flow light vertex or energy");
    }
    std::cout<<"Flow glow vertices="<<mesh.size()<<" minimum sampled surface clearance="<<clearance<<'\n';
    if(clearance<.01f)throw std::runtime_error("Flow light clips below the existing cone");
    std::cout<<"Crater halo clears the lake for horizontal through vertical camera up vectors\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
