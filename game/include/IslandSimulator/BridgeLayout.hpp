#pragma once
#include <array>
#include <cstddef>
#include <optional>

namespace island {
class IslandTerrain;
class StreamCourse;

// A rope bridge across the stream's lower reach: plank walkway between two bank anchors,
// hand ropes on end posts, a shallow sag. Placement only; the renderer draws it and the
// player's support reads it.
struct BridgePlacement {
    static constexpr float DeckHalfWidth=0.55f,PlankPitch=0.26f,PlankWidth=0.22f;
    static constexpr float Sag=0.12f,RailHeight=1.05f,PostAcross=0.62f;
    float ax=0,az=0,ay=0;          // deck height at anchor A (left bank), on the ground
    float bx=0,bz=0,by=0;          // anchor B (right bank)
    float dirX=1,dirZ=0,span=1;    // unit direction A->B and its length
    std::size_t station=0;         // stream station the bridge crosses
    [[nodiscard]] float AcrossX() const noexcept {return dirZ;}
    [[nodiscard]] float AcrossZ() const noexcept {return -dirX;}
    // Deck height for a distance along the walkway from anchor A, in metres.
    [[nodiscard]] float DeckHeight(float along) const noexcept;
    [[nodiscard]] bool OnDeck(float x,float z) const noexcept;
    [[nodiscard]] float SupportHeight(float x,float z,float terrainHeight) const noexcept;
};

class BridgeLayout final {
public:
    static constexpr float BankOffset=1.7f;              // anchors this far outside the channel
    static constexpr float MinimumBankFreeboard=0.24f;   // bank ground above the stream water
    static constexpr float MaximumBankDifference=0.60f;
    static constexpr float MaximumBankSlope=0.45f;
    // Searched downstream of the cascade pool so the crossing stays clear of the fall.
    [[nodiscard]] static std::optional<BridgePlacement> FindInWorld(const IslandTerrain& terrain,
                                                                    const StreamCourse& course);
};
}
