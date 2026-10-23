#include "IslandSimulator/SeabirdIslet.hpp"
#include "IslandSimulator/IslandEnvironmentField.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {
namespace {
constexpr float Pi=std::numbers::pi_v<float>;
float Hash(std::uint32_t v) {
    v^=v>>16U;v*=0x7feb352dU;v^=v>>15U;v*=0x846ca68bU;v^=v>>16U;
    return static_cast<float>(v>>8U)*(1.0f/16777216.0f);
}
// smooth value noise in the plane, 0..1
float Noise(const float x,const float z) {
    const float fx=std::floor(x),fz=std::floor(z);
    const float tx=x-fx,tz=z-fz;
    const auto corner=[](const float cx,const float cz) {
        return Hash(static_cast<std::uint32_t>(static_cast<std::int32_t>(cx)*73856093)^static_cast<std::uint32_t>(static_cast<std::int32_t>(cz)*19349663));
    };
    const float sx=tx*tx*(3.0f-2.0f*tx),sz=tz*tz*(3.0f-2.0f*tz);
    const float a=corner(fx,fz),b=corner(fx+1,fz),c=corner(fx,fz+1),d=corner(fx+1,fz+1);
    return (a+(b-a)*sx)+((c+(d-c)*sx)-(a+(b-a)*sx))*sz;
}
float RockHeight(const IsletRock& rock,const float x,const float z) {
    const float dx=x-rock.x,dz=z-rock.z;
    const float angle=std::atan2(dz,dx);
    // the outline lobed and buttressed round its edge
    const float reach=rock.radius*(0.78f+0.24f*Noise(angle*2.2f+static_cast<float>(rock.seed%97U),1.7f)
                                   +0.14f*Noise(angle*6.5f+3.0f,static_cast<float>(rock.seed%31U)));
    const float d=std::hypot(dx,dz)/reach;
    if(d>=1.32f) return SeabirdIsletLayout::SeaFloor-1.0f;
    float h=SeabirdIsletLayout::SeaFloor-1.0f;
    if(d<1.0f) {
        // near-vertical cliffs and a craggy top (a dome read as a loaf of bread)
        const float profile=std::pow(std::max(0.0f,1.0f-std::pow(d,4.2f)),0.32f);
        h=SeabirdIsletLayout::SeaFloor+(rock.height-SeabirdIsletLayout::SeaFloor)*profile
            +1.1f*(Noise(x*0.42f+7.0f,z*0.42f)-0.5f)*profile+0.35f*(Noise(x*1.3f,z*1.3f)-0.5f)*profile;
    }
    // the apron of fallen blocks round its foot, just awash, the waterline ragged along it
    const float t=std::clamp((1.32f-d)/0.42f,0.0f,1.0f);
    const float apron=SeabirdIsletLayout::SeaFloor+(0.35f-SeabirdIsletLayout::SeaFloor)*t*t*(3.0f-2.0f*t)
        +0.8f*(Noise(x*1.1f+11.0f,z*1.1f)-0.5f)*t;
    return std::max(h,apron);
}
}

float SeabirdIslet::SurfaceY(const float x,const float z) const noexcept {
    float best=SeabirdIsletLayout::SeaFloor;
    for(const IsletRock& rock:rocks) best=std::max(best,RockHeight(rock,x,z));
    return best;
}

float SeabirdIslet::Flatness(const float x,const float z) const noexcept {
    constexpr float Step=0.35f;
    const float gx=(SurfaceY(x+Step,z)-SurfaceY(x-Step,z))/(2.0f*Step);
    const float gz=(SurfaceY(x,z+Step)-SurfaceY(x,z-Step))/(2.0f*Step);
    return 1.0f/std::sqrt(1.0f+gx*gx+gz*gz);
}

std::vector<CircularObstacle> SeabirdIslet::Obstacles() const {
    std::vector<CircularObstacle> out;
    for(const IsletRock& rock:rocks) out.push_back({rock.x,rock.z,rock.radius*0.95f});
    return out;
}

std::optional<SeabirdIslet> SeabirdIsletLayout::FindInWorld(const IslandTerrain& terrain,const std::span<const std::array<float,2>> avoid,
                                                            const float keepAway) {
    std::optional<SeabirdIslet> best;
    float bestScore=-1.0f;
    for(int k=0;k<180;++k) {
        const float bearing=2.0f*Pi*static_cast<float>(k)/180.0f;
        const float rock=IslandEnvironmentField::CoastalRockPotential(bearing);
        if(rock<0.35f) continue;
        const float shore=terrain.ShoreRadius(bearing);
        for(float out=MinimumOffshore;out<=MaximumOffshore;out+=4.0f) {
            const float r=shore+out;
            if(r>320.0f) break;   // inside the swimmer's reach
            const float x=std::cos(bearing)*r,z=std::sin(bearing)*r;
            bool deep=true;
            for(int s=0;s<8&&deep;++s) {
                const float a=2.0f*Pi*static_cast<float>(s)/8.0f;
                deep=terrain.Height(x+std::cos(a)*10.0f,z+std::sin(a)*10.0f)<-1.6f;
            }
            if(!deep||terrain.Height(x,z)>-2.0f) continue;
            bool clear=true;
            for(const auto& p:avoid) clear=clear&&std::hypot(p[0]-x,p[1]-z)>keepAway;
            if(!clear) continue;
            // the most headland-like stretch, nearer rather than further out
            const float score=rock-0.004f*out;
            if(score>bestScore) {
                bestScore=score;
                SeabirdIslet islet;
                islet.x=x;islet.z=z;
                islet.towardShoreX=-std::cos(bearing);islet.towardShoreZ=-std::sin(bearing);
                // the main stack, a lower one beside it and two low reefs of rock
                const float ax=-std::sin(bearing),az=std::cos(bearing);
                islet.rocks={{x,z,6.2f,7.5f,11U},
                             {x+ax*7.0f-islet.towardShoreX*2.0f,z+az*7.0f-islet.towardShoreZ*2.0f,4.4f,4.6f,23U},
                             {x-ax*6.0f+islet.towardShoreX*3.0f,z-az*6.0f+islet.towardShoreZ*3.0f,3.2f,2.2f,37U},
                             {x-ax*2.5f-islet.towardShoreX*7.0f,z-az*2.5f-islet.towardShoreZ*7.0f,2.6f,1.2f,41U}};
                best=islet;
            }
            break;
        }
    }
    return best;
}

BoobyColony::BoobyColony(const SeabirdIslet& islet):islet_(&islet) {
    // nest sites: level places on the tops, at least a metre and a half apart, above the spray
    for(const IsletRock& rock:islet.rocks) {
        if(rock.height<1.5f) continue;
        for(int k=0;k<60&&nests_.size()<MaximumNests;++k) {
            const float angle=static_cast<float>(k)*2.39996f+static_cast<float>(rock.seed);
            const float r=rock.radius*0.7f*std::sqrt((static_cast<float>(k)+0.5f)/60.0f);
            const float x=rock.x+std::cos(angle)*r,z=rock.z+std::sin(angle)*r;
            const float y=islet.SurfaceY(x,z);
            if(y<1.2f||islet.Flatness(x,z)<0.86f) continue;
            bool apart=true;
            for(const auto& nest:nests_) apart=apart&&std::hypot(nest[0]-x,nest[2]-z)>1.5f;
            if(!apart) continue;
            nests_.push_back({{x,y,z,Hash(rock.seed*31U+static_cast<std::uint32_t>(k))*2.0f*Pi}});
        }
    }
}

void BoobyColony::At(const float seconds,std::vector<BirdState>& out) const {
    out.clear();
    const float t=std::isfinite(seconds)?seconds:0.0f;
    for(std::size_t i=0;i<nests_.size();++i) {
        const auto& nest=nests_[i];
        // a pair at one nest in three, the mate standing beside the sitting bird
        const int birds=i%3U==0U?2:1;
        for(int b=0;b<birds;++b) {
            const auto seed=static_cast<std::uint32_t>(i)*7919U+static_cast<std::uint32_t>(b)*131U;
            BirdState bird;
            bird.species=BirdSpecies::Booby;
            bird.behavior=BirdBehavior::Perched;
            bird.perchAmount=1.0f;
            bird.scale=1.25f+0.12f*Hash(seed+1U);
            bird.colorVariant=static_cast<std::uint8_t>(b);
            const float side=b==0?0.0f:0.55f;
            bird.x=nest[0]+std::cos(nest[3]+1.6f)*side;
            bird.z=nest[2]+std::sin(nest[3]+1.6f)*side;
            // the sitting bird low on the nest, the standing one up on its feet
            const bool sitting=b==0&&i%2U==0U;
            bird.y=islet_->SurfaceY(bird.x,bird.z)+(sitting?0.04f:0.12f)*bird.scale;
            bird.headingRadians=nest[3]+(b==0?0.0f:Pi*0.7f)+0.25f*std::sin(t*0.05f+Hash(seed+3U)*6.0f);
            // the bouts: preening, stretching a wing, the bill pointed at the sky, scratching
            const float cycle=std::fmod(t+Hash(seed+5U)*40.0f,40.0f);
            const auto bout=[&](const float start,const float length) {
                const float k=(cycle-start)/length;
                return k>0.0f&&k<1.0f?std::sin(k*Pi):0.0f;
            };
            bird.preenAmount=bout(4.0f,7.0f);
            bird.preenSide=Hash(seed+7U)<0.5f?-1.0f:1.0f;
            bird.stretchAmount=bout(14.0f,2.5f);
            bird.stretchSide=-bird.preenSide;
            bird.ruffleAmount=bout(11.0f,0.8f);
            bird.scratchAmount=sitting?0.0f:bout(24.0f,1.6f);
            bird.scratchSide=bird.preenSide;
            out.push_back(bird);
        }
    }
    // the fliers: wide circles round the islet, gliding, flapping now and then, and every so
    // often a plunge: the wings folded back, straight down into the sea
    for(std::size_t f=0;f<FlyingCount;++f) {
        const auto seed=static_cast<std::uint32_t>(f)*104729U+17U;
        BirdState bird;
        bird.species=BirdSpecies::Booby;
        bird.behavior=BirdBehavior::Fly;
        bird.scale=1.3f;
        const float radius=14.0f+10.0f*Hash(seed+1U);
        const float speed=(0.32f+0.1f*Hash(seed+2U))*(f%2U==0U?1.0f:-1.0f);
        const float angle=t*speed+Hash(seed+3U)*2.0f*Pi;
        const float period=26.0f+8.0f*Hash(seed+5U);
        const float phase=std::fmod(t+Hash(seed+7U)*period,period);
        float height=9.0f+3.0f*std::sin(t*0.21f+Hash(seed+11U)*6.0f);
        float fold=0.0f;
        if(phase<1.4f) {   // the plunge
            const float k=phase/1.4f;
            height=height*(1.0f-k)-0.5f*k;
            fold=1.0f;
        } else if(phase<3.0f) {   // up out of the water
            const float k=(phase-1.4f)/1.6f;
            height=-0.3f+height*k;
            fold=0.0f;
        }
        bird.x=islet_->x+std::cos(angle)*radius;
        bird.z=islet_->z+std::sin(angle)*radius;
        bird.y=height;
        bird.headingRadians=angle+(speed>0.0f?0.5f:-0.5f)*Pi;
        bird.bankRadians=(speed>0.0f?-0.25f:0.25f)*(1.0f-fold);
        bird.speed=std::abs(speed)*radius;
        const bool flapping=std::fmod(t*0.4f+Hash(seed+13U)*3.0f,3.0f)<0.8f;
        bird.wingPose=flapping?std::sin(t*9.0f+Hash(seed+17U)*6.0f):0.1f;
        bird.glideAmount=flapping?0.0f:1.0f;
        // folded for the plunge: the wings swept back along the body (the perched fold)
        bird.perchAmount=fold*0.33f;
        out.push_back(bird);
    }
}

}
