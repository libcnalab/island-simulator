#include "IslandSimulator/OctopusGroundPose.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/RockSurface.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include <iostream>
#include <stdexcept>

int main() try {
    using namespace island;
    using namespace octopus_pose;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;
    const RenderedTerrainSurface ground(terrain);
    const VegetationLayout palms(terrain);
    const JungleLayout jungle(terrain);
    const NaturalPropLayout props(terrain,palms,jungle);
    std::vector<CircularObstacle> rocks;
    for(const auto& p:props.ReefHabitat())if(p.variant==3U)rocks.push_back({p.x,p.z,p.scale});
    require(!rocks.empty(),"Missing canonical reef boulders");
    // These circles enclose actual near AND far boulder surfaces, including rotation.
    for(const bool far:{false,true})for(const auto& v:BuildRockSurface(2,far).vertices)
        require(std::hypot(v.position[0],v.position[2])<=1,"Reef boulder escaped its collision circle");

    OctopusGroup group(terrain),repeat(terrain),legacy(terrain);
    for(int frame=0;frame<184;++frame) {
        const auto o=legacy.Octopuses()[3];legacy.Update(.1f,o.x+10,o.y,o.z);
    }
    const auto embedded=legacy.Octopuses()[3];
    bool overlaps=false;
    for(const auto& rock:rocks)
        overlaps|=std::hypot(embedded.x-rock.x,embedded.z-rock.z)<rock.radius+.5f*embedded.scale;
    require(overlaps,"Original subject3 reef-den overlap was not reproduced");

    group.SetReefRocks(rocks);repeat.SetReefRocks(rocks);
    auto previous=std::vector<OctopusState>(group.Octopuses().begin(),group.Octopuses().end());
    std::array<float,OctopusGroup::Count> travel{};
    std::size_t poses=0,vertices=0;float minimumGap=1000,minimumBedGap=1000;
    for(int frame=0;frame<3600;++frame) {
        const auto observer=group.Octopuses()[3];
        group.Update(.1f,observer.x+10,observer.y,observer.z);
        repeat.Update(.1f,observer.x+10,observer.y,observer.z);
        for(std::size_t i=0;i<group.Octopuses().size();++i) {
            const auto& o=group.Octopuses()[i];const auto& r=repeat.Octopuses()[i];
            require(o.x==r.x&&o.y==r.y&&o.z==r.z&&o.headingRadians==r.headingRadians,
                    "Reef avoidance lost determinism");
            const float step=std::hypot(o.x-previous[i].x,o.z-previous[i].z);
            require(step<=.221f,"Reef avoidance teleported a moving octopus");
            travel[i]+=step;previous[i]=o;
            const auto& den=group.Dens()[i];
            require(std::hypot(o.x-den.x,o.z-den.z)<=den.radius+.001f,"Reef avoidance escaped its den range");
            for(const auto& rock:rocks) {
                const float gap=std::hypot(o.x-rock.x,o.z-rock.z)-rock.radius-OctopusGroup::MaximumReach*o.scale;
                minimumGap=std::min(minimumGap,gap);
                require(gap>=.0249f,"Octopus extended reach overlaps a reef boulder");
            }
            if(frame%6!=0)continue;
            const auto b=MakeFrame(o,&ground);
            for(int arm=0;arm<8;++arm)for(const auto& q:MakeArm(o,b,arm,&ground))
                for(int side=0;side<ArmSides;++side) {
                    const auto p=RingPoint(b,q,side);
                    require(std::hypot(p.X-o.x,p.Z-o.z)<=OctopusGroup::MaximumReach*o.scale,
                            "Actual probing arm escaped the protected reef footprint");
                    if(o.groundAmount==1) {
                        const float bedGap=(p.Y-ground.Height(p.X,p.Z))/o.scale;
                        minimumBedGap=std::min(minimumBedGap,bedGap);
                        require(bedGap>=.0039f,"Relocated octopus arm penetrates the drawn bed");
                    }
                    ++vertices;
                }
            const float pulse=(1+o.mantlePulse*.045f)*(1-.22f*o.jet);
            for(int part=0;part<2;++part)for(int j=0;j<8;++j)for(int side=0;side<16;++side) {
                const float lat=-std::numbers::pi_v<float>*.5f+std::numbers::pi_v<float>*j/7;
                const float a=2*std::numbers::pi_v<float>*side/16;
                const Vector3 c=part==0?Vector3(0,.31f*(1-o.hideAmount*.48f),-.34f):Vector3(0,.145f,.13f);
                const Vector3 r=part==0?Vector3(.19f*pulse,.33f*(1-o.hideAmount*.45f),.55f*pulse):Vector3(.315f,.15f,.285f);
                const auto p=b.point(c.X+std::cos(a)*std::cos(lat)*r.X,c.Y+std::sin(lat)*r.Y,c.Z+std::sin(a)*std::cos(lat)*r.Z);
                require(std::hypot(p.X-o.x,p.Z-o.z)<=OctopusGroup::MaximumReach*o.scale,
                        "Actual mantle/head escaped the protected reef footprint");
                if(o.groundAmount==1)require(p.Y>=ground.Height(p.X,p.Z),"Relocated octopus body penetrates the drawn bed");
                ++vertices;
            }
            ++poses;
        }
    }
    for(const float distance:travel)require(distance>.5f,"Reef avoidance left an octopus permanently stuck");
    std::cout<<"reef poses="<<poses<<" body/arm vertices="<<vertices<<" minimum reach gap="<<minimumGap
             <<" minimum arm/bed gap="<<minimumBedGap<<" travel=";
    for(const float distance:travel)std::cout<<distance<<',';
    std::cout<<'\n';
    return 0;
} catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
