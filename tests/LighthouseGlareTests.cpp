#include "IslandSimulator/LighthouseGlare.hpp"
#include <iostream>
#include <stdexcept>

int main()
try {
    for(float distance:{.02f,.1f,.5f,1.0f,2.0f,10.0f,300.0f}) {
        const auto glare=island::LighthouseGlareAt(distance,1,1);
        if(glare.offset>=distance||glare.radius>(distance-glare.offset)*.15f)
            throw std::runtime_error("Beacon glare crosses the eye or overwhelms the lantern view");
    }
    std::cout<<"Beacon glare remains in front of the eye with bounded angular extent\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
