#pragma once

#include "IslandSimulator/BunkerSite.hpp"
#include <array>

namespace island {

class IslandTerrain;

struct BunkerPoint {float x=0,y=0,z=0;};

/** Deterministic enterable floor plan and collision contract for the underground bunker. */
class BunkerLayout final {
public:
    static constexpr float CorridorFrontDepth=BunkerSite::ApronFrontDepth;
    struct Doorway {float across,from,to;};
    static constexpr std::array<Doorway,3> Doorways{{{1.36f,14,19},{-1.36f,27,30},{1.36f,30,33}}};
    static constexpr float DoorJambDepth=.16f;
    static constexpr float DoorInnerAcross=1.27f,DoorOuterAcross=1.48f;
    [[nodiscard]] static constexpr bool CorridorOpening(bool right,float from,float to)noexcept {
        for(const auto door:Doorways)
            if((door.across>0)==right&&from<door.to&&to>door.from)return true;
        return false;
    }
    explicit BunkerLayout(const IslandTerrain&);
    [[nodiscard]] BunkerPoint Entrance()const noexcept{return entrance_;}
    [[nodiscard]] BunkerPoint World(float across,float height,float depth)const noexcept;
    [[nodiscard]] BunkerPoint Local(BunkerPoint)const noexcept;
    [[nodiscard]] float EntranceGroundY()const noexcept{return entrance_.y;}
    [[nodiscard]] float FloorRelative(float depth)const noexcept;
    [[nodiscard]] bool ContainsWalkable(float worldX,float worldZ,float margin=0.0f)const noexcept;
    /** Tests the solid corridor dividers against the whole ordered vehicle rectangle. */
    [[nodiscard]] bool FootprintHitsInteriorWall(const std::array<BunkerPoint,4>& corners)const noexcept;
    [[nodiscard]] bool SupportHeight(float worldX,float worldZ,float currentFeetY,
                                     float& height)const noexcept;
    /** Terrain fragments below the built shell must not seal or fill its rooms. */
    [[nodiscard]] bool HidesTerrain(float worldX,float worldY,float worldZ)const noexcept;
    [[nodiscard]] bool IntersectsCapsule(BunkerPoint feet,float radius,float height)const noexcept;
    [[nodiscard]] float SafeTravelFraction(BunkerPoint from,BunkerPoint to,
                                           float radius,float height)const noexcept;
    [[nodiscard]] bool ReservesHabitat(float x,float z,float margin=0.0f)const noexcept {
        return BunkerSite::Reserves(x,z,margin);
    }
private:
    [[nodiscard]] bool WalkableLocal(float across,float depth)const noexcept;
    [[nodiscard]] bool WalkableCircle(float across,float depth,float radius)const noexcept;
    BunkerPoint entrance_;
    float cosine_=1,sine_=0;
};

}
