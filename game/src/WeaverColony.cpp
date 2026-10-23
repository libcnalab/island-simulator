#include "IslandSimulator/WeaverColony.hpp"
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
float Smooth(const float k) {const float t=std::clamp(k,0.0f,1.0f);return t*t*(3.0f-2.0f*t);}
// the male's round, as fractions of its cycle: hanging under the nest displaying, down to the
// grass, picking a strip, back up, and weaving it in from the nest's side
constexpr float DisplayEnd=0.38f,OutEnd=0.46f,GroundEnd=0.60f,BackEnd=0.68f;
constexpr float FemaleVisit=14.0f,FemaleFlight=2.2f;
}

std::vector<std::uint32_t> WeaverColony::ChoosePalms(const std::span<const PalmPlacement> palms,
                                                     const IslandTerrain& terrain) {
    struct Candidate {std::uint32_t index;float rank;};
    std::vector<Candidate> candidates;
    for(std::uint32_t i=0;i<palms.size();++i) {
        const PalmPlacement& palm=palms[i];
        if(palm.condition!=PalmCondition::Living||palm.shoreDistance<12.0f||palm.shoreDistance>60.0f) continue;
        if(terrain.Height(palm.x,palm.z)<1.2f) continue;
        candidates.push_back({i,Hash(i*2654435761U+11U)});
    }
    std::sort(candidates.begin(),candidates.end(),[](const Candidate& a,const Candidate& b) {return a.rank<b.rank;});
    std::vector<std::uint32_t> chosen;
    for(const Candidate& candidate:candidates) {
        const PalmPlacement& palm=palms[candidate.index];
        const bool apart=std::ranges::all_of(chosen,[&](const std::uint32_t other) {
            return std::hypot(palms[other].x-palm.x,palms[other].z-palm.z)>120.0f;
        });
        if(!apart) continue;
        chosen.push_back(candidate.index);
        if(chosen.size()==ColonyPalms) break;
    }
    return chosen;
}

WeaverColony::WeaverColony(const std::span<const WeaverNestSite> sites) {
    sites_.assign(sites.begin(),sites.end());
    for(std::size_t i=0;i<sites_.size()&&nests_.size()<MaximumNests;++i) {
        const auto seed=static_cast<std::uint32_t>(i)*7919U+sites_[i].palm*131U+3U;
        // a frond in five or so left bare
        if(Hash(seed)<0.18f) continue;
        WeaverNest nest;
        nest.site=i;
        nest.seed=seed;
        nest.hang=0.12f+0.14f*Hash(seed+1U);
        nest.size=0.86f+0.28f*Hash(seed+2U);
        nest.yaw=sites_[i].bearing+(Hash(seed+3U)-0.5f)*2.4f;
        // last season's nests dried to straw, this season's still green
        nest.age=Hash(seed+4U)<0.35f?0.75f+0.25f*Hash(seed+5U):0.25f*Hash(seed+5U);
        nest.building=nest.age<0.2f&&Hash(seed+6U)<0.45f;
        nests_.push_back(nest);
    }
}

std::array<float,3> WeaverColony::EntranceLocal(const std::size_t nest) const noexcept {
    const WeaverNest& n=nests_[nest];
    if(n.building) return {0.0f,-(n.hang+0.19f*n.size),0.0f};   // the ring's lowest point, the perch it is woven from
    return {0.0f,-(n.hang+0.25f*n.size),0.085f*n.size};
}

WeaverColony::MalePlan WeaverColony::MaleAt(const std::size_t nest,const float seconds) const noexcept {
    const auto seed=nests_[nest].seed;
    const float period=26.0f+14.0f*Hash(seed+21U);
    const float k=std::fmod(seconds+Hash(seed+22U)*period,period)/period;
    if(k<DisplayEnd) return {Phase::Display,k/DisplayEnd};
    if(k<OutEnd) return {Phase::Out,(k-DisplayEnd)/(OutEnd-DisplayEnd)};
    if(k<GroundEnd) return {Phase::Ground,(k-OutEnd)/(GroundEnd-OutEnd)};
    if(k<BackEnd) return {Phase::Back,(k-GroundEnd)/(BackEnd-GroundEnd)};
    return {Phase::Roof,(k-BackEnd)/(1.0f-BackEnd)};
}

bool WeaverColony::Displaying(const std::size_t nest,const float seconds) const noexcept {
    return nest<nests_.size()&&Hash(nests_[nest].seed+20U)>=0.2f&&MaleAt(nest,seconds).phase==Phase::Display;
}

bool WeaverColony::Beating(const std::size_t nest,const float seconds) const noexcept {
    return Displaying(nest,seconds)&&std::fmod(seconds+Hash(nests_[nest].seed+23U)*2.6f,2.6f)<1.6f;
}

std::array<float,2> WeaverColony::Swing(const std::size_t nest,const float seconds) const noexcept {
    if(nest>=nests_.size()) return {0.0f,0.0f};
    const WeaverNest& n=nests_[nest];
    const float t=std::isfinite(seconds)?seconds:0.0f;
    // a bird clinging to it (or just leaving it) sets it rocking on its strips
    float disturb=0.0f;
    if(Hash(n.seed+20U)>=0.2f) {
        const MalePlan plan=MaleAt(nest,t);
        if(plan.phase==Phase::Display) disturb=std::fmod(t+Hash(n.seed+23U)*2.6f,2.6f)<1.6f?1.0f:0.4f;
        else if(plan.phase==Phase::Out) disturb=1.0f-plan.k;
        else if(plan.phase==Phase::Roof) disturb=0.5f;
    }
    // a pendulum the length of the strips and half the chamber, and the wind's slow push
    const float omega=std::sqrt(9.8f/std::max(0.1f,n.hang+0.12f*n.size));
    const float p1=Hash(n.seed+24U)*6.283f,p2=Hash(n.seed+25U)*6.283f;
    const float x=0.035f*std::sin(t*1.3f+p1)+disturb*0.07f*std::sin(t*omega+p2);
    const float z=0.030f*std::sin(t*1.1f+p2)+disturb*0.05f*std::sin(t*omega*1.07f+p1);
    return {x,z};
}

std::array<float,3> WeaverColony::NestPoint(const std::size_t nest,const float seconds,const std::array<float,3> offset,
                                            const float across,const float up,const float ahead) const noexcept {
    const WeaverNest& n=nests_[nest];
    const WeaverNestSite& site=sites_[n.site];
    const float c=std::cos(n.yaw),s=std::sin(n.yaw);
    // forward (cos, 0, sin), right (-sin, 0, cos)
    float x=-s*across+c*ahead,y=up,z=c*across+s*ahead;
    const auto swing=Swing(nest,seconds);
    // tilted about the world's x axis, then its z axis
    const float cx=std::cos(swing[0]),sx=std::sin(swing[0]);
    const float y1=y*cx-z*sx,z1=y*sx+z*cx;
    const float cz=std::cos(swing[1]),sz=std::sin(swing[1]);
    const float x2=x*cz-y1*sz,y2=x*sz+y1*cz;
    x=x2;y=y2;z=z1;
    return {site.x+offset[0]+x,site.y+offset[1]+y,site.z+offset[2]+z};
}

std::array<float,2> WeaverColony::GrassSpot(const std::size_t nest,const float seconds,
                                            const std::function<float(float,float)>& ground) const {
    const WeaverNest& n=nests_[nest];
    const WeaverNestSite& site=sites_[n.site];
    const float period=26.0f+14.0f*Hash(n.seed+21U);
    const auto trip=static_cast<std::uint32_t>(std::max(0.0f,std::floor((seconds+Hash(n.seed+22U)*period)/period)));
    // a different tuft each trip, out from the palm, inland where the outward side is the beach
    for(int attempt=0;attempt<4;++attempt) {
        const auto seed=n.seed+trip*977U+static_cast<std::uint32_t>(attempt)*31U;
        const float angle=site.bearing+(Hash(seed+30U)-0.5f)*1.6f+static_cast<float>(attempt)*Pi*0.5f;
        const float distance=6.0f+7.0f*Hash(seed+31U);
        const float x=site.x+std::cos(angle)*distance,z=site.z+std::sin(angle)*distance;
        if(!ground||ground(x,z)>0.6f) return {x,z};
    }
    return {site.x+std::cos(site.bearing)*4.0f,site.z+std::sin(site.bearing)*4.0f};
}

void WeaverColony::At(const float seconds,const std::span<const std::array<float,3>> offsets,
                      const std::function<float(float,float)>& ground,std::vector<BirdState>& out) const {
    out.clear();
    const float t=std::isfinite(seconds)?seconds:0.0f;
    const auto offsetOf=[&](const std::size_t nest) {
        const std::size_t site=nests_[nest].site;
        return site<offsets.size()?offsets[site]:std::array<float,3>{0.0f,0.0f,0.0f};
    };
    const auto mouth=[&](const std::size_t nest) {
        const auto e=EntranceLocal(nest);
        return NestPoint(nest,t,offsetOf(nest),e[0],e[1],e[2]);
    };
    for(std::size_t i=0;i<nests_.size();++i) {
        const WeaverNest& nest=nests_[i];
        // one old nest in five abandoned
        if(Hash(nest.seed+20U)<0.2f) continue;
        BirdState bird;
        bird.species=BirdSpecies::Weaver;
        bird.colorVariant=0U;   // the male
        bird.scale=BirdScale*(0.95f+0.1f*Hash(nest.seed+26U));
        const MalePlan plan=MaleAt(i,t);
        const auto entrance=mouth(i);
        const auto flight=[&](const std::array<float,3>& from,const std::array<float,3>& to,const float k) {
            const float s=Smooth(k);
            bird.behavior=BirdBehavior::Fly;
            bird.x=from[0]+(to[0]-from[0])*s;
            bird.z=from[2]+(to[2]-from[2])*s;
            // a dipping, bounding flight
            bird.y=from[1]+(to[1]-from[1])*s+std::sin(k*Pi)*1.2f+0.12f*std::sin(k*Pi*6.0f);
            bird.headingRadians=std::atan2(to[2]-from[2],to[0]-from[0]);
            bird.speed=5.0f;
            bird.wingPose=std::sin(t*34.0f+Hash(nest.seed+27U)*6.0f);
            bird.perchAmount=0.0f;
        };
        switch(plan.phase) {
        case Phase::Display: {
            // hung upside down from the spout, the wings beating in bursts
            bird.behavior=BirdBehavior::Perched;
            bird.bankRadians=Pi;
            const bool burst=Beating(i,t);
            bird.perchAmount=burst?0.35f:1.0f;
            bird.wingPose=burst?std::sin(t*30.0f):0.0f;
            bird.x=entrance[0];bird.y=entrance[1]-0.10f*bird.scale-0.01f;bird.z=entrance[2];
            bird.headingRadians=nest.yaw+0.3f*std::sin(t*0.7f+Hash(nest.seed+28U)*6.0f);
            break;
        }
        case Phase::Out:
        case Phase::Back: {
            const auto spot=GrassSpot(i,t,ground);
            const float gy=ground?ground(spot[0],spot[1]):entrance[1]-8.0f;
            const std::array<float,3> grass{spot[0],gy+0.12f*bird.scale,spot[1]};
            if(plan.phase==Phase::Out) flight(entrance,grass,plan.k);
            else flight(grass,entrance,plan.k);
            break;
        }
        case Phase::Ground: {
            const auto spot=GrassSpot(i,t,ground);
            bird.behavior=BirdBehavior::GroundForage;
            bird.perchAmount=1.0f;
            bird.x=spot[0];bird.z=spot[1];
            bird.y=(ground?ground(spot[0],spot[1]):0.0f)+0.12f*bird.scale;
            bird.headingRadians=Hash(nest.seed+29U)*2.0f*Pi+0.8f*std::sin(t*0.9f);
            // tugging at a strip of grass
            bird.headBob=0.5f+0.5f*std::sin(t*9.0f);
            break;
        }
        case Phase::Roof: {
            // clinging upright to the chamber's back, weaving the strip in (the ring's side
            // while it is only a ring)
            bird.behavior=BirdBehavior::Perched;
            bird.perchAmount=1.0f;
            const auto at=NestPoint(i,t,offsetOf(i),0.0f,-(nest.hang+(nest.building?0.10f:0.06f)*nest.size),
                                    -(nest.building?0.0f:0.10f)*nest.size);
            bird.x=at[0];bird.y=at[1];bird.z=at[2];
            bird.headingRadians=nest.yaw+(nest.building?Pi*0.5f:0.0f);
            bird.preenAmount=0.0f;
            bird.headBob=0.5f+0.5f*std::sin(t*7.0f);
            break;
        }
        }
        out.push_back(bird);
    }
    // a female in each palm, going from nest to nest to look the work over
    std::vector<std::uint32_t> palms;
    for(const WeaverNest& nest:nests_)
        if(std::ranges::find(palms,sites_[nest.site].palm)==palms.end()) palms.push_back(sites_[nest.site].palm);
    for(const std::uint32_t palm:palms) {
        std::vector<std::size_t> own;
        for(std::size_t i=0;i<nests_.size();++i) if(sites_[nests_[i].site].palm==palm&&!nests_[i].building) own.push_back(i);
        if(own.empty()) continue;
        const float shifted=t+Hash(palm*31U+7U)*FemaleVisit*static_cast<float>(own.size());
        const auto visit=static_cast<std::size_t>(std::max(0.0f,std::floor(shifted/FemaleVisit)));
        const float within=std::fmod(shifted,FemaleVisit);
        const std::size_t to=own[visit%own.size()],from=own[(visit+own.size()-1U)%own.size()];
        BirdState bird;
        bird.species=BirdSpecies::Weaver;
        bird.colorVariant=1U;
        bird.scale=BirdScale*0.94f;
        const auto side=[&](const std::size_t nest) {
            // beside the spout, upright, clinging to the chamber's front
            return NestPoint(nest,t,offsetOf(nest),0.10f*nests_[nest].size,-(nests_[nest].hang+0.16f*nests_[nest].size),
                             0.06f*nests_[nest].size);
        };
        if(within<FemaleFlight) {
            const auto a=side(from),b=side(to);
            const float k=within/FemaleFlight,s=Smooth(k);
            bird.behavior=BirdBehavior::Fly;
            bird.x=a[0]+(b[0]-a[0])*s;bird.z=a[2]+(b[2]-a[2])*s;
            bird.y=a[1]+(b[1]-a[1])*s+std::sin(k*Pi)*0.6f;
            bird.headingRadians=std::atan2(b[2]-a[2],b[0]-a[0]);
            bird.wingPose=std::sin(t*32.0f);
        } else {
            const auto at=side(to);
            bird.behavior=BirdBehavior::Perched;
            bird.perchAmount=1.0f;
            bird.x=at[0];bird.y=at[1];bird.z=at[2];
            bird.headingRadians=nests_[to].yaw+Pi*0.5f+0.4f*std::sin(t*0.8f);
            bird.headBob=0.5f+0.5f*std::sin(t*3.0f);
        }
        out.push_back(bird);
    }
}

WeaverColony::Census WeaverColony::Count(const float seconds) const {
    std::vector<BirdState> birds;
    At(seconds,{},{},birds);
    Census census;
    for(const BirdState& bird:birds) {
        if(bird.behavior==BirdBehavior::Fly) ++census.flying;
        else if(bird.behavior==BirdBehavior::GroundForage) ++census.grounded;
        else if(bird.bankRadians>3.0f) ++census.displaying;
        else ++census.perched;
    }
    return census;
}

}
