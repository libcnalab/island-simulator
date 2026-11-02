#include "IslandSimulator/UfoScorchField.hpp"
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message) {if(!ok)throw std::runtime_error(message);};
    for(float f=-30;f<=12;f+=.125f) for(float r=-12;r<=12;r+=.125f) {
        const float a=ufo_scorch::Apron(f,r),b=ufo_scorch::Furrow(f,r);
        require(std::isfinite(a)&&a>=0&&a<=.82f&&std::isfinite(b)&&b>=0&&b<=.76f,
                "Scorch opacity is outside its finite range");
        // Neighbouring vertices sample one continuous field, including old cell boundaries.
        require(std::abs(a-ufo_scorch::Apron(f+.001f,r))<.002f,
                "Scorch has a discontinuous whole-cell edge");
        if(std::hypot(f,r)>8) require(a==0,"Scorch apron extends beyond its feathered footprint");
    }
    for(int k=0;k<128;++k) {
        const float radius=5.5f+static_cast<float>(k)*.02f;
        require(std::abs(ufo_scorch::Apron(-radius,.00001f)
                         -ufo_scorch::Apron(-radius,-.00001f))<.0001f,
                "Scorch angular seam is visible");
    }
    require(ufo_scorch::Apron(0,0)>.8f,"Impact core lost its scorch");
    require(ufo_scorch::Furrow(-UfoWreckPlacement::FurrowLength,0)==0
        &&ufo_scorch::Furrow(-3,0)==0,"Furrow does not fade at its ends");
    for(float bad:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()})
        require(ufo_scorch::Apron(bad,0)==0&&ufo_scorch::Apron(0,bad)==0
            &&ufo_scorch::Furrow(bad,0)==0&&ufo_scorch::Furrow(0,bad)==0,
            "Invalid scorch coordinates are accepted");
    std::cout<<"Continuous feathered impact/furrow fields passed\n";
}
catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
