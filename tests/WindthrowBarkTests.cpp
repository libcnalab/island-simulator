#include "IslandSimulator/WindthrowBarkCoordinates.hpp"
#include "IslandSimulator/BarkMaterial.hpp"
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>

namespace
{
    void Near(float a, float b, const char* message)
    {
        if (std::abs(a-b)>0.0002f) throw std::runtime_error(message);
    }
}

int main() try
{
    constexpr float pi=std::numbers::pi_v<float>;
    // Rotation, translation, slope and the raised root end must not stretch axial bark into
    // world-vertical stripes. Sample the original analytic surface with independent angles.
    for (float yaw : {0.0f, 0.73f, 2.8f}) {
        island::WindthrowPlacement wood;
        wood.x=37; wood.z=-84; wood.dirX=std::cos(yaw); wood.dirZ=std::sin(yaw);
        const auto ground=[](float x,float z){return 4.0f+0.02f*x-0.03f*z;};
        wood.groundY=ground(wood.x,wood.z);
        for (float along : {-0.3f, 0.0f, 1.0f, 8.2f, 17.0f}) {
            const auto axis=wood.Axis(along,ground);
            const float radius=wood.RadiusAt(along);
            for (float angle : {-pi, -1.4f, 0.0f, 0.8f, pi}) {
                const std::array<float,3> point{
                    axis[0]+wood.AcrossX()*radius*std::cos(angle),
                    axis[1]+radius*std::sin(angle),
                    axis[2]+wood.AcrossZ()*radius*std::cos(angle)};
                const auto uv=island::WindthrowBarkCoordinates(wood,point,ground);
                Near(uv[0],std::cos(angle),"Bark circumference changed under rotation");
                Near(uv[1],std::sin(angle),"Bark does not follow the bent trunk axis");
                Near(uv[2],along+1.0f,"Bark stretched along the slope");
                if (uv[2]<0) throw std::runtime_error("Bark was classified as another material");
            }
        }
    }
    // A triangle crosses atan's -pi/pi cut. Circle interpolation stays near (-1,0), while
    // interpolated angles would falsely cross zero and spread their derivative over 2*pi.
    const float a=pi-.01f, b=-pi+.01f;
    const float x=(std::cos(a)+std::cos(b))*.5f, y=(std::sin(a)+std::sin(b))*.5f;
    if (x>-.99f || std::abs(y)>.00001f) throw std::runtime_error("Circular seam interpolation failed");
    Near(island::BarkMaterial::Sample(a,3,.002f,.001f,19),
         island::BarkMaterial::Sample(a+2*pi,3,.002f,.001f,19),"Bark noise is not seam-periodic");
    Near(island::BarkMaterial::Sample(-pi,3,.002f,.001f,19),
         island::BarkMaterial::Sample(pi,3,.002f,.001f,19),"Bark shade breaks at the angular seam");
    std::cout<<"Fallen bark follows the original rotated/bent trunk with seam-safe coordinates\n";
} catch (const std::exception& error) {
    std::cerr<<error.what()<<'\n'; return 1;
}
