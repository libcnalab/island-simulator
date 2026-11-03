#include "IslandSimulator/OctopusGroundPose.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

int main() try {
    using namespace island;
    using namespace octopus_pose;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;
    const RenderedTerrainSurface ground(terrain);
    OctopusGroup group(terrain),repeat(terrain);
    std::size_t rings=0,swimming=0,transitions=0;
    float minGap=1000,minFaceGap=1000,maxContactGap=0,minBodyGap=1000,oldMinGap=1000;
    // A full den/crawl/forage cycle is long; six simulated minutes include renewed swims.
    for(int frame=0;frame<7200;++frame) {
        const auto observer=group.Octopuses().front();
        group.Update(.05f,observer.x+10,observer.y,observer.z);
        repeat.Update(.05f,observer.x+10,observer.y,observer.z);
        for(std::size_t i=0;i<group.Octopuses().size();++i) {
            const auto& o=group.Octopuses()[i];
            require(o.groundAmount==repeat.Octopuses()[i].groundAmount&&o.groundAmount>=0&&o.groundAmount<=1,
                "Octopus contact blend is not deterministic or bounded");
            const auto b=MakeFrame(o,&ground),old=MakeFrame(o,nullptr);
            if(o.groundAmount==0) {
                require(b.c==old.c&&b.u==old.u&&b.f==old.f&&b.r==old.r,"Swimming body pose changed");
                ++swimming;
                const auto arm=MakeArm(o,b,0,&ground),legacy=MakeArm(o,old,0,nullptr);
                for(std::size_t j=0;j<arm.size();++j)
                    require(arm[j].centre==legacy[j].centre&&arm[j].width==legacy[j].width,
                        "Swimming arm pose changed");
            } else if(o.groundAmount<1) {++transitions;continue;}
            else {
                for(int arm=0;arm<8;++arm) {
                    const auto pose=MakeArm(o,b,arm,&ground),legacy=MakeArm(o,old,arm,nullptr);
                    for(std::size_t station=0;station<pose.size();++station) {
                        float closest=1000;
                        for(int side=0;side<ArmSides;++side) {
                            const auto p=RingPoint(b,pose[station],side),q=RingPoint(old,legacy[station],side);
                            const float gap=(p.Y-ground.Height(p.X,p.Z))/o.scale;
                            minGap=std::min(minGap,gap);closest=std::min(closest,gap);
                            if(frame==0&&i==0)oldMinGap=std::min(oldMinGap,(q.Y-ground.Height(q.X,q.Z))/o.scale);
                            require(std::isfinite(gap)&&gap>.0039f,"Resting octopus arm penetrates the visible seabed");
                            if(station>0) {
                                const auto a=RingPoint(b,pose[station-1],side);
                                const auto c=RingPoint(b,pose[station],(side+1)%ArmSides);
                                const auto d=RingPoint(b,pose[station-1],(side+1)%ArmSides);
                                for(const auto& centre:{(a+p+c)/3,(a+c+d)/3}) {
                                    const float faceGap=(centre.Y-ground.Height(centre.X,centre.Z))/o.scale;
                                    minFaceGap=std::min(minFaceGap,faceGap);
                                    require(faceGap>-.002f,"Octopus arm face cuts through a seabed triangle boundary");
                                }
                            }
                        }
                        maxContactGap=std::max(maxContactGap,closest);
                        require(closest<.0241f,"Resting octopus arm still floats above the seabed");
                        ++rings;
                    }
                }
                // These are the actual existing mantle/head ellipsoid vertices.
                const float hide=std::clamp(o.hideAmount,0.0f,1.0f),jet=std::clamp(o.jet,0.0f,1.0f);
                const float pulse=(1+o.mantlePulse*.045f)*(1-.22f*jet);
                for(int part=0;part<2;++part)for(int j=0;j<8;++j)for(int side=0;side<16;++side) {
                    const float lat=-std::numbers::pi_v<float>*.5f+std::numbers::pi_v<float>*j/7;
                    const float a=2*std::numbers::pi_v<float>*side/16;
                    const Vector3 centre=part==0?Vector3(0,.31f*(1-hide*.48f),-.34f):Vector3(0,.145f,.13f);
                    const Vector3 radii=part==0?Vector3(.19f*pulse,.33f*(1-hide*.45f),.55f*pulse):Vector3(.315f,.15f,.285f);
                    const auto p=b.point(centre.X+std::cos(a)*std::cos(lat)*radii.X,
                        centre.Y+std::sin(lat)*radii.Y,centre.Z+std::sin(a)*std::cos(lat)*radii.Z);
                    minBodyGap=std::min(minBodyGap,(p.Y-ground.Height(p.X,p.Z))/o.scale);
                }
            }
        }
    }
    std::cout<<"octopus rings="<<rings<<" swimming="<<swimming<<" transition poses="<<transitions
        <<" minimum arm gap="<<minGap<<" minimum face gap="<<minFaceGap<<" maximum supporting gap="<<maxContactGap
        <<" minimum body gap="<<minBodyGap<<" original first-forager gap="<<oldMinGap<<'\n';
    require(rings>100000&&swimming>100&&transitions>100,"Insufficient octopus contact/swim transition coverage");
    require(oldMinGap>.08f,"Original floating-arm deficiency was not reproduced");
    require(minBodyGap>=0,"Resting octopus body penetrates the visible seabed");
    return 0;
} catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
