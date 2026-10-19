#pragma once
#include "IslandSimulator/BirdPerchLayout.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstddef>
#include <optional>

namespace island {
class IslandTerrain;
struct HutPlacement;

struct JettyPost { float x=0,z=0,bottomY=0,topY=0; };

// The resident's landing: a plank jetty running from the beach below the hut out over the
// shallows, a mooring post at its end where birds perch, and a dugout canoe drawn up on the
// sand beside the root. Placement only; the renderer and the player's support read it.
struct JettyPlacement {
    static constexpr float Length=14.0f,DeckHalfWidth=0.80f,RampLength=4.0f;
    static constexpr float PostPitch=2.8f,PostAcross=0.72f,PostRadius=0.09f;
    static constexpr std::size_t PairCount=5,PostCount=PairCount*2+1;
    static constexpr float MooringAlong=13.5f,MooringHeight=0.95f;
    static constexpr float CanoeLength=4.2f,CanoeBeam=0.72f,CanoeDepth=0.34f;
    float rootX=0,rootZ=0;          // beach end of the deck, on the sand
    float dirX=1,dirZ=0;            // unit direction from the root out to sea
    float rootDeckY=0,deckY=0;      // deck height at the root and from the ramp end on
    float canoeX=0,canoeZ=0,canoeY=0,canoeYaw=0;
    std::array<JettyPost,PostCount> posts{};
    [[nodiscard]] float AcrossX() const noexcept {return dirZ;}
    [[nodiscard]] float AcrossZ() const noexcept {return -dirX;}
    // Deck height for a distance along the deck from the root, in metres.
    [[nodiscard]] float DeckHeight(float along) const noexcept;
    [[nodiscard]] bool OnDeck(float x,float z) const noexcept;
    // Terrain-or-deck support for the walker's feet, like the helicopter floor.
    [[nodiscard]] float SupportHeight(float x,float z,float terrainHeight) const noexcept;
    [[nodiscard]] std::array<CircularObstacle,2> CanoeObstacles() const noexcept;
    [[nodiscard]] BirdPerchSite MooringPerch() const noexcept;
};

class JettyLayout final {
public:
    static constexpr float DeckClearance=0.85f;         // level deck above mean water
    static constexpr float RootInland=2.2f;              // root this far inside the shoreline
    static constexpr float RootStep=0.12f;               // first plank above the sand
    static constexpr float MinimumRootHeight=0.04f,MaximumRootHeight=0.80f;
    static constexpr float MinimumEndHeight=-2.4f,MaximumEndHeight=-0.18f;
    static constexpr float MinimumCanoeHeight=0.08f,MaximumCanoeHeight=0.95f;
    // Searched outward along the hut's own radial, then fanned either side of it.
    [[nodiscard]] static std::optional<JettyPlacement> FindInWorld(const IslandTerrain& terrain,
                                                                   const HutPlacement& hut);
private:
    [[nodiscard]] static std::optional<JettyPlacement> Try(const IslandTerrain& terrain,float angle);
};
}
