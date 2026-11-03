#include "IslandSimulator/GroundFootPlant.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
    GroundFootPlant plant;GroundFootStep step{{4,7},{5,7},false,.35f,.12f,.8f};
    auto p=plant.Advance(0,step);require(p.planted&&p.point.x==4,"Initial contact is not planted");
    step.swing=true;p=plant.Advance(.05f,step);require(!p.planted&&p.point.x==4,"Liftoff jumps to a different world point");
    const auto paused=plant.Advance(0,step);require(paused.point.x==p.point.x&&paused.lift==p.lift,"Paused pose moves");
    step.landing={100,-100};step.swing=false; // stop the body and move its nominal target
    bool lifted=false;
    for(int i=0;i<20;++i){p=plant.Advance(.05f,step);lifted|=p.lift>.05f;
        require(p.point.x>=4&&p.point.x<=5&&p.point.z==7&&p.lift>=0&&p.lift<=.12001f,"Swing chases a changed destination");}
    require(lifted&&p.planted&&p.point.x==5&&p.point.z==7,"Body stopping strands the returning foot");
    step.initial={90,90};
    for(int i=0;i<40;++i){p=plant.Advance(.1f,step);require(p.planted&&p.point.x==5&&p.point.z==7,"Stance follows the moving body");}
    step.swing=true;step.startAllowed=false;
    p=plant.Advance(.1f,step);require(p.planted,"Support budget allowed an extra airborne foot");
    step.startAllowed=true;p=plant.Advance(.1f,step);require(!p.planted,"Deferred liftoff is lost");
    plant.Reset({-2,6});step.swing=false;
    p=plant.Advance(std::numeric_limits<float>::quiet_NaN(),step);
    require(p.planted&&p.point.x==-2&&p.point.z==6,"Explicit reposition retains an old anchor");
    std::cout<<"World contact, fixed landing, completed stop, paused clock and support budget pass\n";
}
catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
