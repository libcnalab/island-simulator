#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

struct FreighterPoint {float x=0,y=0,z=0;};

/** A rusting 30 m steel coaster a storm drove up a beach: its bow on the sand, its stern in the
 *  surf, trimmed by the stern and listing a little to port. In its own frame `along` runs from
 *  the stern (0) to the stem (Length), `up` from the keel and `across` to starboard; the frame
 *  is sheared, not rotated, by the trim and the list, so walls stay plumb and floors tilt. An
 *  accommodation ladder runs down its starboard side to the sand; the deck, the deckhouse, and,
 *  down a stair from the deck, the cargo hold are walkable. */
struct FreighterPlacement {
    static constexpr float Length=30.0f,HalfBeam=3.5f,Depth=4.2f,Bulwark=1.0f,DeckThickness=0.15f;
    // the cargo hatch, its coaming, the hold beneath it and the stair down into it
    static constexpr float HatchAft=11.5f,HatchFore=19.5f,HatchHalf=2.2f,Coaming=0.7f;
    static constexpr float HoldAft=10.2f,HoldFore=20.8f,HoldFloor=1.6f;
    static constexpr float StairTop=20.2f,StairFoot=16.0f,StairInner=2.25f,StairOuter=3.25f;
    // the deckhouse aft, its door in the forward face; the gangway's opening in the bulwark
    static constexpr float HouseAft=2.0f,HouseFore=8.0f,HouseHalf=2.7f,HouseHeight=2.4f;
    static constexpr float DoorInner=0.6f,DoorOuter=1.5f,DoorHeight=2.0f;
    static constexpr float GangwayAft=12.5f,GangwayFore=13.5f,LadderWidth=1.1f;
    static constexpr float MainMast=9.6f,ForeMast=22.4f,Windlass=27.0f;

    float x=0,z=0,baseY=0;        // the frame's origin: the stern's end on the centreline at the keel
    float dirX=1,dirZ=0;          // unit stern-to-bow direction
    float tanPitch=0,tanHeel=0;   // the rise of the frame per metre along and across
    float ladderRun=5.6f;         // the ladder's length along the hull from its head to its foot
    float ladderFootUp=0.5f;      // the ladder's foot on the sand, in the frame

    [[nodiscard]] float AcrossX() const noexcept {return -dirZ;}
    [[nodiscard]] float AcrossZ() const noexcept {return dirX;}
    [[nodiscard]] FreighterPoint World(float along,float up,float across) const noexcept;
    [[nodiscard]] FreighterPoint Local(FreighterPoint world) const noexcept;
    /** The hull's half breadth at the deck at `along` (zero off its ends). */
    [[nodiscard]] static float HalfBreadth(float along) noexcept;
    /** The deck's height above the keel at `along`: its sheer rising to the bow and the stern. */
    [[nodiscard]] static float DeckUp(float along) noexcept;
    [[nodiscard]] float LadderHeadAlong() const noexcept {return GangwayFore;}
    [[nodiscard]] float LadderFootAlong() const noexcept {return GangwayFore+ladderRun;}
    /** The walkable surface's height above the keel at (along, across) for feet at `feetUp`:
     *  the deck, the deckhouse's floor, the hold's floor, the stair or the ladder; false when the
     *  point is not on the ship (the sand round it keeps its own ground). */
    [[nodiscard]] bool SupportUp(float along,float across,float feetUp,float& up) const noexcept;
    /** The same in the world: the support's height under (x, z) for feet at `feetY`. */
    [[nodiscard]] bool SupportHeight(float worldX,float worldZ,float feetY,float& height) const noexcept;
    /** Whether a walker's upright capsule standing at `feet` meets the hull, a bulwark, a railing,
     *  the coaming, a deckhouse wall, a bulkhead, a mast or the windlass. */
    [[nodiscard]] bool IntersectsCapsule(FreighterPoint feet,float radius,float height) const noexcept;
    [[nodiscard]] float SafeTravelFraction(FreighterPoint from,FreighterPoint to,float radius,float height) const noexcept;
    /** Whether (x, z) lies within `margin` of the hull or the ladder (the quad's and the animals' keep-out). */
    [[nodiscard]] bool Blocks(float worldX,float worldZ,float margin) const noexcept;
    /** Circles round the hull and the ladder for the animals' paths (not the walker's). */
    [[nodiscard]] std::vector<CircularObstacle> Obstacles() const;
    /** Where a walker stands on the sand at the ladder's foot. */
    [[nodiscard]] FreighterPoint LadderFoot() const noexcept;
};

class FreighterLayout final {
public:
    static constexpr float BowInland=13.0f;   // the stem this far up the beach from the shoreline
    /** The first stretch of broad sandy beach round the island (from the south-west, sunwise)
     *  where the ship lies with its bow on the sand, its hold above the sea and the ladder's foot
     *  on dry sand; clear of the obstacles and at least `keepAway` metres from each `avoid` point. */
    [[nodiscard]] static std::optional<FreighterPlacement> FindInWorld(const IslandTerrain& terrain,
        std::span<const CircularObstacle> obstacles,std::span<const std::array<float,2>> avoid,float keepAway=45.0f);
};
}
