#pragma once

#include "IslandSimulator/MineSite.hpp"

namespace island {

class IslandTerrain;

struct MinePoint {float x=0,y=0,z=0;};

/** The walkable adit and chamber of the old mine and their collision: the floor, the timbered
 *  walls and the roof, and the terrain the portal's cutting removes. Local points are
 *  (across, up from the entrance, depth in). */
class MineLayout final {
public:
    explicit MineLayout(const IslandTerrain& terrain);
    [[nodiscard]] MinePoint Entrance() const noexcept {return entrance_;}
    [[nodiscard]] MinePoint World(float across,float up,float depth) const noexcept;
    [[nodiscard]] MinePoint Local(MinePoint world) const noexcept;
    /** The floor's height above the entrance at `depth`. */
    [[nodiscard]] static float FloorRelative(float depth) noexcept;
    /** The roof's height above the floor (the chamber's dome higher). */
    [[nodiscard]] static float Headroom(float across,float depth) noexcept;
    /** Whether a point on the floor plan is inside the adit or the chamber, clear of the cart. */
    [[nodiscard]] static bool WalkableLocal(float across,float depth) noexcept;
    /** The depth where the hillside first covers the roof: the portal's timber frame. */
    [[nodiscard]] float PortalDepth() const noexcept {return portalDepth_;}
    [[nodiscard]] bool SupportHeight(float worldX,float worldZ,float currentFeetY,float& height) const noexcept;
    [[nodiscard]] bool HidesTerrain(float worldX,float worldY,float worldZ) const noexcept;
    [[nodiscard]] bool IntersectsCapsule(MinePoint feet,float radius,float height) const noexcept;
    [[nodiscard]] float SafeTravelFraction(MinePoint from,MinePoint to,float radius,float height) const noexcept;
    // where the old cart lies tipped on its side in the chamber
    static constexpr float CartAcross=1.9f,CartDepth=51.0f,CartRadius=0.75f;
private:
    [[nodiscard]] static bool WalkableCircle(float across,float depth,float radius) noexcept;
    MinePoint entrance_;
    float cosine_=1,sine_=0,portalDepth_=2.5f;
};

}
