#include "IslandSimulator/BunkerQuadSupport.hpp"
#include "IslandSimulator/BunkerChaseCamera.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;const BunkerLayout bunker(terrain);
    require(BunkerLayout::CorridorOpening(true,15.12f,16.48f)
        &&BunkerLayout::CorridorOpening(true,30.52f,31.88f)
        &&!BunkerLayout::CorridorOpening(false,30.52f,31.88f)
        &&!BunkerLayout::CorridorOpening(true,13.0f,14.0f),
            "Decorative corridor panels must be supported by a solid wall, not span an open doorway");
    const auto feet=[&](float across,float depth) {
        return bunker.World(across,bunker.FloorRelative(depth)+.002f,depth);
    };
    for(const auto door:BunkerLayout::Doorways) {
        const float middle=(door.from+door.to)*.5f;
        const float side=std::copysign(1.0f,door.across);
        require(!bunker.IntersectsCapsule(feet(door.across,middle),.34f,1.9f),
                "Drawn bunker doorway blocks the player's capsule");
        require(bunker.SafeTravelFraction(feet(0,middle),feet(side*2.4f,middle),.34f,1.9f)>.999f,
                "Player's full route through an open doorway is blocked");
        require(bunker.IntersectsCapsule(feet(door.across,door.from+.10f),.34f,1.9f)
            &&bunker.IntersectsCapsule(feet(door.across,door.to-.10f),.34f,1.9f),
                "Door jambs have no capsule collision");
    }
    for(const auto sample:{std::array<float,2>{1.36f,12}, {1.36f,28}, {-1.36f,24}, {0,36}}) {
        const auto p=feet(sample[0],sample[1]);
        require(bunker.IntersectsCapsule(p,.34f,1.9f),"Drawn divider or end bulkhead is permeable");
        auto above=p;above.y+=5;
        require(!bunker.IntersectsCapsule(above,.34f,1.9f),"Interior wall blocks walking above the buried roof");
        float height;
        require(bunker.SupportHeight(p.x,p.z,p.y,height),"Solid wall incorrectly removes continuous floor support");
    }
    for(bool open:{false,true}) {
        const float depth=open?31.5f:28.0f;
        const auto start=feet(0,depth),end=feet(1,depth);
        PlayerMotion player;player.SetBunker(&bunker);
        player.SetPosition(terrain,start.x,start.z,start.y);
        for(int frame=0;frame<90;++frame)player.Update(terrain,end.x-start.x,end.z-start.z,1.0f/60);
        const auto p=player.Position();const auto local=bunker.Local({p.x,p.y,p.z});
        require(open?local.x>2.0f:local.x<1.0f,"Real player movement disagrees with the rendered divider");
    }
    // A thin divider can lie wholly BETWEEN the old eight point samples. All
    // corners fit the joined floor plan, but the body's middle cuts the wall.
    const auto rect=[&](float depth) {
        return std::array<BunkerPoint,4>{feet(.7f,depth-.9f),feet(2.3f,depth-.9f),
                                       feet(2.3f,depth+.9f),feet(.7f,depth+.9f)};
    };
    for(const auto p:rect(28))require(bunker.ContainsWalkable(p.x,p.z),"SAT fixture corner is not on valid floor");
    require(bunker.FootprintHitsInteriorWall(rect(28)),"Vehicle footprint skips an enclosed thin divider");
    require(!bunker.FootprintHitsInteriorWall(rect(31.5f)),"Whole vehicle cannot fit the service doorway");
    QuadBike quad;
    const auto ground=[&](float,float){BikeGround g;g.height=bunker.EntranceGroundY()-3.2f;return g;};
    const auto start=feet(1.5f,28),heading=feet(2.5f,28);
    quad.Place({start.x,0,start.z},std::atan2(heading.x-start.x,heading.z-start.z),ground);
    require(BunkerQuadFootprintBlocked(bunker,quad,start.x,start.z),"Gameplay vehicle collision omits divider");
    const auto open=feet(1.5f,31.5f);
    require(!BunkerQuadFootprintBlocked(bunker,quad,open.x,open.z),"Gameplay vehicle collision seals service door");
    auto invalid=rect(31.5f);invalid[0].x=std::numeric_limits<float>::quiet_NaN();
    require(bunker.FootprintHitsInteriorWall(invalid),"Invalid vehicle footprint accepted");
    require(BunkerQuadFootprintBlocked(bunker,quad,invalid[0].x,open.z),"Invalid vehicle position accepted");
    // Both endpoints can be on valid floors but separated by a closed divider.
    // The old endpoint-only chase camera accepted this occluded sightline.
    const auto focus=bunker.World(4, -3.2f+1.3f,31.5f);
    const auto candidate=bunker.World(0, -3.2f+2.5f,34.0f);
    require(bunker.ContainsWalkable(focus.x,focus.z,.18f)
        &&bunker.ContainsWalkable(candidate.x,candidate.z,.18f),"Camera fixture endpoints are invalid");
    const auto fitted=FitBunkerChaseCamera(bunker,focus,candidate);
    require(std::hypot(fitted.x-candidate.x,fitted.z-candidate.z)>.3f,
            "Camera sightline crosses the workshop divider despite valid endpoints");
    for(int i=0;i<=100;++i) {
        const float t=i/100.0f;
        const BunkerPoint eye{std::lerp(focus.x,fitted.x,t),std::lerp(focus.y,fitted.y,t)-.18f,
                              std::lerp(focus.z,fitted.z,t)};
        require(!bunker.IntersectsCapsule(eye,.18f,.36f),"Fitted camera sphere clips wall or ceiling");
    }
    const auto underLamp=FitBunkerChaseCamera(bunker,bunker.World(0,-3.2f+1.3f,12),
                                            bunker.World(0,-3.2f+3.0f,13.5f));
    // The fixture bottom is floor+2.615m, below the actual ceiling slab. The old
    // slab-only bound put the eye inside its luminous box on the uphill exit.
    require(bunker.Local(underLamp).y+.18f< -3.2f+2.615f,
            "Camera near plane intersects the hanging corridor lamp");
    std::cout<<"Bunker dividers, real doorways/jambs, bulkhead, player and whole quad footprint agree\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
