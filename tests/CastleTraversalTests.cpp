#include "IslandSimulator/CastleRuinLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/PlayerMotion.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

int main()
try {
    using namespace island;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    const IslandTerrain terrain;
    for(float heading:{0.0f,1.2f,std::atan2(130.0f,-170.0f)}) {
        const CastleRuinPlacement castle{-170,130,std::cos(heading),std::sin(heading),0};
        const auto walls=castle.WallObstacles();const WorldCollisionGrid grid(walls);
        const float front=CastleRuinPlacement::KeepForward+CastleRuinPlacement::KeepHalf
            -CastleRuinPlacement::KeepWall*.5f;
        const float door=CastleRuinPlacement::KeepRight+CastleRuinPlacement::KeepDoorCentre;
        for(int step=0;step<=80;++step) {
            const float offset=2.0f-static_cast<float>(step)*.05f;
            for(const auto p:{castle.At(door,front+offset),
                             castle.At(0,CastleRuinPlacement::Half+offset),
                             castle.At(CastleRuinPlacement::Half+offset,
                                 (CastleRuinPlacement::BreachFrom+CastleRuinPlacement::BreachTo)*.5f)})
                require(!grid.IntersectsCircle(p[0],p[1],PlayerMotion::CapsuleRadius),
                        "Castle proxy seals an existing doorway or breach");
        }
        // Exercise the actual movement solver, preserving the neighbouring solid wall.
        for(bool throughDoor:{true,false}) {
            const float across=throughDoor?door:CastleRuinPlacement::KeepRight+2.0f;
            const auto start=castle.At(across,front+1.5f);
            PlayerMotion player;player.SetPosition(terrain,start[0],start[1]);
            float depth=front+1.5f;
            for(int frame=0;frame<360&&depth>front-1.5f;++frame) {
                player.Update(terrain,-castle.faceX,-castle.faceZ,1.0f/60,grid);
                const auto p=player.Position();
                depth=(p.x-castle.x)*castle.faceX+(p.z-castle.z)*castle.faceZ;
                require(std::isfinite(p.y),"Castle traversal corrupts player contact");
            }
            require(throughDoor?depth<front-1.4f:depth>front+.4f,
                    "Player cannot enter the keep, or passes through its solid wall");
        }
    }
    std::cout<<"Castle gate/breach/keep doorway clear; player enters keep and solid walls block\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
