#include "IslandSimulator/TidePoolMaterial.hpp"
#include <iostream>
#include <stdexcept>

namespace
{
    void Require(bool condition,const char* message)
    {
        if(!condition) throw std::runtime_error(message);
    }
}
int main() try
{
    namespace pool=island::tide_pool_material;
    using island::JungleMaterial::Smooth;
    Require(pool::UnitNoise(-.5f)==0 && pool::UnitNoise(0)==.5f && pool::UnitNoise(.5f)==1,
            "Pool fields must convert signed terrain noise to their authored unit range");
    // These old masks were identically zero over terrain valueNoise's complete range.
    Require(Smooth(.62f,.80f,.5f)==0 && Smooth(.70f,.86f,.5f)==0,
            "Regression fixture no longer demonstrates unreachable pits/barnacles");
    Require(Smooth(pool::PitStart,pool::PitEnd,pool::UnitNoise(.48f))>.99f
            && Smooth(.70f,.86f,pool::UnitNoise(.4f))>.99f,
            "Rock pits and barnacles must be reachable on the brighter field patches");
    Require(pool::BenchExtent(.5f,.5f)>1.4f,
            "The ordinary pool bench must extend outside a narrow artificial rim");
    Require(pool::BenchExtent(1,1)+.03f<pool::OuterReach,
            "The expanded rock bench must end before the bounded shader branch");
    for(float frequency:{3.7f,7.5f,11.0f,19.0f}) {
        float previous=1;
        for(int i=0;i<=100;++i) {
            const float weight=pool::DetailWeight(static_cast<float>(i)*.001f,frequency);
            Require(weight>=0 && weight<=previous,"Pool detail must fade monotonically with its pixel footprint");
            previous=weight;
        }
        Require(pool::DetailWeight(.51f/frequency,frequency)==0,
                "Pool grain and barnacles must not survive beyond Nyquist");
    }
    Require(pool::DetailWeight(.02f,7.5f)>0.99f && pool::DetailWeight(.02f,19.0f)<.5f,
            "Fine barnacles need earlier filtering than broad solution pits");
    std::cout<<"Pool unit-range masks are reachable and fine detail filters by its own scale\n";
} catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
