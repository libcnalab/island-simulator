#include "IslandSimulator/BirdWingNormal.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace Microsoft::Xna::Framework;

int main()
try {
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    std::size_t poses=0;
    for(float yaw:{-3.0f,0.0f,.7f,2.0f})for(float bank:{-.34f,0.0f,.34f})
    for(float fold:{0.0f,.6f,1.2f})for(float lift:{-.38f,-.1f,0.0f,.38f})
    for(float side:{-1.0f,1.0f}) {
        const Vector3 forward(std::cos(yaw),0,std::sin(yaw));
        const Vector3 levelRight(-forward.Z,0,forward.X);
        const auto right=levelRight*std::cos(bank)+Vector3::Up*std::sin(bank);
        const auto bodyUp=Vector3::Up*std::cos(bank)-levelRight*std::sin(bank);
        const auto up=bodyUp*std::cos(fold)+right*(side*std::sin(fold));
        const auto foldedRight=right*std::cos(fold)-bodyUp*(side*std::sin(fold));
        const auto span=foldedRight*(side*.5f)+up*lift-forward*.2f;
        const auto normal=island::BirdWingNormal(span,forward,up,side);
        require(std::abs(normal.LengthSquared()-1)<2e-6f,"Feather normal is not unit length");
        require(std::abs(Vector3::Dot(normal,span))<2e-6f,"Feathers do not follow the lifting span");
        require(std::abs(Vector3::Dot(normal,forward))<2e-6f,"Feather normal tilts along the chord");
        require(Vector3::Dot(normal,up)>.7f,"Mirrored wing points its upper face downward");
        if(lift==0)require(Vector3::Distance(normal,up)<2e-6f,"Neutral/folded frame changed unnecessarily");
        else require(std::abs(Vector3::Dot(up,span))>.09f,"Fixture failed to expose the old flat feather frame");
        ++poses;
    }
    require(island::BirdWingNormal(Vector3::Zero,Vector3::Forward,Vector3::Up,1)==Vector3::Up,
            "Degenerate span must preserve its fallback plane");
    require(island::BirdWingNormal({std::numeric_limits<float>::quiet_NaN(),0,0},
                                  Vector3::Forward,Vector3::Up,1)==Vector3::Up,
            "Invalid span must not generate nonfinite vertices");
    std::cout<<"Bird feather frames: "<<poses<<" banked, folded and flapping poses pass\n";
    return 0;
} catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
