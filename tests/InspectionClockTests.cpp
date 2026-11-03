#include "IslandSimulator/InspectionClock.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

int main()try {
    double fast=0,slow=0,interactiveFast=0,interactiveSlow=0;
    for(int i=0;i<720;++i) {
        fast+=island::InspectionSimulationStep(true,.016f);
        slow+=island::InspectionSimulationStep(true,.083f);
        interactiveFast+=island::InspectionSimulationStep(false,.016f);
        interactiveSlow+=island::InspectionSimulationStep(false,.083f);
    }
    if(std::abs(fast-slow)>1e-6||std::abs(fast-72)>1e-4)
        throw std::runtime_error("Different render speeds benchmark different simulated states");
    if(interactiveSlow<interactiveFast*4||island::InspectionSimulationStep(false,.4f)!=.1f)
        throw std::runtime_error("Inspection clock changed ordinary real-time exploration or its stall bound");
    std::cout<<"720 inspection updates visit the same72s at16ms and83ms rendering speeds\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
