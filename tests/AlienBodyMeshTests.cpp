#include "IslandSimulator/AlienBodyMesh.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
namespace {
void Require(bool ok,const char* message) {if(!ok)throw std::runtime_error(message);}
bool CoversShoulder(const std::vector<VertexPositionColor>& mesh,const island::AlienState& state,float side) {
    const Vector3 right(std::sin(state.heading),0,-std::cos(state.heading));
    const float shoulderY=state.arms[side<0?0:1].shoulder.y;
    const auto project=[&](Vector3 p) {return std::array<float,2>{
        (p.X-state.chest.x)*right.X+(p.Z-state.chest.z)*right.Z,p.Y-shoulderY};};
    const auto cross=[](const auto& a,const auto& b,const auto& c) {
        return (b[0]-a[0])*(c[1]-a[1])-(b[1]-a[1])*(c[0]-a[0]);};
    for(int sample=0;sample<=60;++sample) {
        const std::array<float,2> p{side*.15f*static_cast<float>(sample)/60,0};bool covered=false;
        for(std::size_t i=0;i<mesh.size();i+=3) {
            const auto a=project(mesh[i].Position),b=project(mesh[i+1].Position),c=project(mesh[i+2].Position);
            const float area=cross(a,b,c);if(std::abs(area)<1e-10f)continue;
            const float u=cross(a,b,p)/area,v=cross(b,c,p)/area,w=cross(c,a,p)/area;
            if(u>=-1e-5f&&v>=-1e-5f&&w>=-1e-5f) {covered=true;break;}
        }
        if(!covered) {
            std::cerr<<"Shoulder silhouette gap at side="<<side<<" across="<<p[0]
                <<" height="<<shoulderY<<" chest="<<state.chest.y<<'\n';
            return false;
        }
    }
    return true;
}
}
int main()
try {
    using namespace island;
    const IslandTerrain terrain;const auto placement=UfoWreckLayout::FindInWorld(terrain,{}, {},0);
    Require(placement.has_value(),"Missing alien geometry fixture");
    AlienMotion alien(terrain,*placement,{});
    std::vector<VertexPositionColor> mesh,repeated;std::size_t poses=0,maxVertices=0;
    for(auto behavior:{AlienBehavior::Stand,AlienBehavior::Walk,AlienBehavior::Examine,
                       AlienBehavior::TendHull,AlienBehavior::Scan,AlienBehavior::Watch,AlienBehavior::Startle}) {
        alien.Place(-8.5f,-6,behavior,600);
        for(int frame=0;frame<20;++frame) {
            alien.Update(.1f);BuildAlienBodyMesh(alien.State(),mesh);BuildAlienBodyMesh(alien.State(),repeated);
            Require(!mesh.empty()&&mesh.size()%3==0&&mesh.size()<=16000,"Alien exceeds its existing dynamic mesh budget");
            Require(mesh.size()==repeated.size(),"Alien mesh is nondeterministic");
            for(std::size_t i=0;i<mesh.size();++i) {
                const auto a=mesh[i].Position,b=repeated[i].Position;
                Require(std::isfinite(a.X)&&std::isfinite(a.Y)&&std::isfinite(a.Z),"Nonfinite alien mesh position");
                Require(a.X==b.X&&a.Y==b.Y&&a.Z==b.Z&&mesh[i].Color==repeated[i].Color,
                        "Alien mesh changes without a pose update");
            }
            for(std::size_t i=0;i<mesh.size();i+=3)
                Require(Vector3::Cross(mesh[i+1].Position-mesh[i].Position,
                        mesh[i+2].Position-mesh[i].Position).LengthSquared()>1e-15f,"Degenerate alien triangle");
            if(behavior==AlienBehavior::Stand)
                Require(CoversShoulder(mesh,alien.State(),-1)&&CoversShoulder(mesh,alien.State(),1),
                        "Alien shoulders are disconnected from its chest silhouette");
            maxVertices=std::max(maxVertices,mesh.size());++poses;
        }
    }
    const float heading=2.694f;
    const UfoWreckPlacement actualSite{-130,8,std::cos(heading),std::sin(heading),terrain.Height(-130,8)};
    AlienMotion walking(terrain,actualSite,{});
    walking.Place(-8.5f,-6,AlienBehavior::Stand,600);
    Require(walking.PreviewWalk(),"No clear walking QA route at the actual saucer");
    const auto start=walking.State();float travel=0;
    for(int frame=0;frame<64;++frame) {
        const auto before=walking.State();walking.Update(.1f);BuildAlienBodyMesh(walking.State(),mesh);
        const auto& state=walking.State();
        travel+=std::hypot(state.x-before.x,state.z-before.z);
        Require(!mesh.empty()&&mesh.size()<=16000,"Moving alien exceeds the existing mesh budget");
        for(const auto& leg:state.legs) if(leg.planted)
            Require(std::abs(leg.foot.y-terrain.Height(leg.foot.x,leg.foot.z))<.015f,
                    "Walking alien's planted foot leaves the ground");
    }
    Require(travel>.3f&&std::hypot(walking.State().x-start.x,walking.State().z-start.z)>.3f,
            "Walking geometry test did not actually move");
    std::cout<<"Alien finite/deterministic poses="<<poses<<" max vertices="<<maxVertices
             <<" actual walk travel="<<travel
             <<"; both shoulder silhouettes connected\n";
}
catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
