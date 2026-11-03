#include "IslandSimulator/CraterGlow.hpp"
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
    std::cout<<"Crater halo clears the lake for horizontal through vertical camera up vectors\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
