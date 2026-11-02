#include "IslandSimulator/BunkerQuadSupport.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;const RenderedTerrainSurface surface(terrain);const BunkerLayout bunker(terrain);
    QuadBike quad;bool inside=false;
    const auto ground=[&](float x,float z) {
        if(auto floor=SampleBunkerQuadGround(bunker,x,z,quad.Position().y-.6f,inside))return *floor;
        const auto s=surface.Sample(x,z);BikeGround result;
        result.height=s.height;result.normal={s.normal.x,s.normal.y,s.normal.z};return result;
    };
    const auto start=bunker.World(0,0,-6),portal=bunker.World(0,0,0);
    quad.Place({start.x,0,start.z},std::atan2(portal.x-start.x,portal.z-start.z),ground);
    const auto roofClear=[&]() {
        const auto head=quad.ToWorld({0,1.10f,-.15f}); // the real QuadBike hull head point
        const auto local=bunker.Local({head.x,head.y,head.z});
        if(local.z>=0&&local.z<36) {
            const float ceiling=bunker.EntranceGroundY()+bunker.FloorRelative(local.z)
                +(local.z<10?2.75f:3.05f);
            require(head.y<ceiling+.03f,"Portal transition drives the rider through the roof");
        }
    };
    int anticipated=0;float worstAirGap=0;
    // Real four-wheel springs over the packed rendered terrain, at the same .1s
    // capture clock. Old centre-only selection launched the body over 6m above it.
    for(int frame=0;frame<32;++frame) {
        auto p=quad.Position();float floor;
        inside=bunker.SupportHeight(p.x,p.z,p.y-.6f,floor);
        if(!inside)for(int w=0;w<4;++w) {
            const auto mount=quad.ToWorld(QuadBike::WheelMount(w));
            anticipated+=SampleBunkerQuadGround(bunker,mount.x,mount.z,p.y-.6f,false).has_value();
        }
        quad.Step(.1f,{1,0,false},ground);roofClear();
        p=quad.Position();
        worstAirGap=std::max(worstAirGap,p.y-.6f-ground(p.x,p.z).height);
        require(std::isfinite(p.y)&&!quad.Overturned(),"Portal approach overturns or corrupts the quad");
    }
    const auto p=quad.Position();const auto local=bunker.Local({p.x,p.y,p.z});
    float floor;
    require(bunker.SupportHeight(p.x,p.z,p.y-.6f,floor)&&local.z>1.5f,
            "Quad did not enter the bunker under the original capture controls");
    require(std::abs(p.y-.6f-floor)<.2f&&worstAirGap<1.2f,
            "Quad is launched by hidden terrain at the portal");
    require(anticipated>0,"No front footprint queried the floor before the centre entered");
    std::size_t roofChecks=0;
    for(float depth:{0.0f,2.0f,12.0f,25.0f,34.0f})for(float across:{-.8f,0.0f,.8f}) {
        const auto point=bunker.World(across,0,depth);
        const float y=bunker.EntranceGroundY()+bunker.FloorRelative(depth);
        require(SampleBunkerQuadGround(bunker,point.x,point.z,y,false).has_value(),
                "Valid low wheel query misses the bunker floor");
        require(!SampleBunkerQuadGround(bunker,point.x,point.z,y+3.05f,false),
                "A quad above the concrete roof is pulled onto the underground floor");
        ++roofChecks;
    }
    for(float depth:{-7.0f,-4.0f,-3.25f}) {
        const auto point=bunker.World(0,0,depth);
        require(!SampleBunkerQuadGround(bunker,point.x,point.z,bunker.EntranceGroundY(),false),
                "Exposed approach terrain replaced by an invisible apron");
    }
    // After turning in a wide room, the same portal must work in the other direction.
    // Rear-drive reverse traction on the18-degree ramp is a separate existing limitation.
    inside=true;
    const auto exitStart=bunker.World(0,0,8),exitTarget=bunker.World(0,0,7);
    quad.Place({exitStart.x,0,exitStart.z},std::atan2(exitTarget.x-exitStart.x,
                                                   exitTarget.z-exitStart.z),ground);
    float exitDepth=8,exitGap=0;
    for(int frame=0;frame<64&&exitDepth>-4;++frame) {
        auto now=quad.Position();float h;
        inside=bunker.SupportHeight(now.x,now.z,now.y-.6f,h);
        quad.Step(.1f,{1,0,false},ground);roofClear();now=quad.Position();
        exitDepth=bunker.Local({now.x,now.y,now.z}).z;
        exitGap=std::max(exitGap,now.y-.6f-ground(now.x,now.z).height);
        require(!quad.Overturned(),"Forward portal exit overturns the quad");
    }
    require(exitDepth<-4&&exitGap<1.2f,"Portal exit hits a heightfield lip or launches the quad");
    std::cout<<"exit depth="<<exitDepth<<" max tyre-plane gap="<<exitGap<<'\n';
    const auto apron=bunker.World(0,0,BunkerLayout::CorridorFrontDepth);
    const float apronFloor=bunker.EntranceGroundY()+bunker.FloorRelative(BunkerLayout::CorridorFrontDepth);
    require(std::abs(surface.Height(apron.x,apron.z)-apronFloor)<.06f,
            "Apron begins with a lip above the packed approach terrain");
    for(float depth:{-2.8f,-1.8f,-.9f,0.0f}) {
        const auto p=bunker.World(0,0,depth);const float raw=surface.Height(p.x,p.z);
        const float floor=bunker.EntranceGroundY()+bunker.FloorRelative(depth);
        require(raw<floor-.03f||bunker.HidesTerrain(p.x,raw,p.z),
                "Packed heightfield blocks the drawn apron/portal floor");
    }
    const float nan=std::numeric_limits<float>::quiet_NaN();
    require(!SampleBunkerQuadGround(bunker,nan,0,0,true)
        &&!SampleBunkerQuadGround(bunker,0,nan,0,true)
        &&!SampleBunkerQuadGround(bunker,0,0,nan,true),"Invalid wheel query accepted");
    std::cout<<"entry depth="<<local.z<<" body-floor gap="<<p.y-floor
             <<" max tyre-plane gap="<<worstAirGap<<" anticipated queries="<<anticipated
             <<" roof guards="<<roofChecks<<'\n';
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
