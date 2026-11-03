#include "IslandSimulator/BirdHeadProfile.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

using Microsoft::Xna::Framework::Vector3;

int main()
try {
    using namespace island::bird_head;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    std::size_t samples=0,oldBuried=0;
    float minimum=1,maximum=-1;
    for(float h:{.80f,.84f,.86f,.95f}) {
        const Profile head(h);
        for(const auto [radius,outset]:std::array<std::array<float,2>,7>{{
            {.030f,.004f},{.020f,.004f},{.016f,.006f},{.014f,.006f},{.008f,.008f},
            {.009f,.008f},{.0035f,.0095f}}}) {
            const auto eye=head.EyeRing(radius,outset);
            for(int side=0;side<EyeSides;++side) {
                const auto a=eye[0],b=eye[static_cast<std::size_t>(side+1)],
                    c=eye[static_cast<std::size_t>((side+1)%EyeSides+1)];
                for(int i=0;i<=12;++i)for(int j=0;j<=12-i;++j) {
                    const float u=static_cast<float>(i)/12,v=static_cast<float>(j)/12;
                    const auto p=a+(b-a)*u+(c-a)*v;
                    const auto skin=head.LateralX(p.Y,p.Z);
                    require(skin.has_value(),"Eye fan left its supporting head triangles");
                    const float clearance=p.X-*skin;
                    minimum=std::min(minimum,clearance);maximum=std::max(maximum,clearance);
                    require(clearance>.0001f,"Eye fan is clipped by the head between its fitted vertices");
                    require(clearance<.012f,"Eye fan floats too far above the face");
                    ++samples;
                }
                // Original placement estimated a constant width and bowed the rim inward.
                const float angle=6.28318530718f*static_cast<float>(side)/EyeSides;
                const float surface=(Sections[3].halfWidth+(Sections[4].halfWidth-Sections[3].halfWidth)
                    *((EyeZ-.430f)/.045f))*h;
                const auto actual=head.LateralX((EyeY+std::sin(angle)*radius)*h,
                    EyeZ+std::cos(angle)*radius*h*1.15f);
                oldBuried+=actual&&surface*.88f+outset-radius*radius*6<*actual;
            }
        }
        require(!head.LateralX(10,.455f)&&!head.LateralX(.08f,-1),"Outside profile request accepted");
        require(!head.LateralX(std::numeric_limits<float>::quiet_NaN(),.455f),"Invalid sample accepted");
        for(std::size_t ring=0;ring<Sections.size();++ring)for(int side=0;side<=Sides;++side) {
            const float angle=6.28318530718f*static_cast<float>(side)/Sides;
            const auto& s=Sections[ring];
            const Vector3 original(std::cos(angle)*s.halfWidth*h,s.lift+std::sin(angle)*s.halfHeight*h,s.z);
            require(head.Point(ring,side)==original,"Original head mesh changed during extraction");
        }
    }
    require(oldBuried>100,"Fixture did not reproduce the old fragmented eye");
    std::cout<<samples<<" eye fan samples clear the actual head; clearance "<<minimum<<".."<<maximum
        <<"m; old buried rim samples "<<oldBuried<<'\n';
    return 0;
} catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
