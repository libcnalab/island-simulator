#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <optional>
#include <span>

namespace island {
class IslandTerrain;
struct HutPlacement;

// The resident's lookout: a post tower beside the hut clearing with a railed plank platform
// three metres up and a cleated ramp the walker can climb. Placement only.
struct LookoutPlacement {
    static constexpr float Half=1.25f,PlatformRise=3.2f,RampLength=7.6f,RampHalfWidth=0.55f;
    static constexpr float RailHeight=1.05f,PostRadius=0.11f;
    float x=0,z=0;                 // tower centre
    float dirX=1,dirZ=0;           // unit direction the ramp runs away from the platform
    float groundY=0,platformY=0;   // highest ground under the posts, and the deck height
    float rampFootY=0;             // ground at the ramp's foot
    std::array<float,4> postBottomY{};
    [[nodiscard]] float AcrossX() const noexcept {return dirZ;}
    [[nodiscard]] float AcrossZ() const noexcept {return -dirX;}
    [[nodiscard]] bool OnPlatform(float x,float z) const noexcept;
    [[nodiscard]] bool OnRamp(float x,float z) const noexcept;
    [[nodiscard]] bool OnDeck(float x,float z) const noexcept {return OnPlatform(x,z)||OnRamp(x,z);}
    [[nodiscard]] float SupportHeight(float x,float z,float terrainHeight) const noexcept;
    [[nodiscard]] std::array<CircularObstacle,4> PostObstacles() const noexcept;
};

class LookoutLayout final {
public:
    static constexpr float HutDistance=8.4f;
    static constexpr float MaximumRelief=0.9f;
    static constexpr float MinimumShoreDistance=30.0f;
    // Candidate sites fan around the hut at HutDistance; the ramp points away from the hut.
    [[nodiscard]] static std::optional<LookoutPlacement> FindInWorld(const IslandTerrain& terrain,
        const HutPlacement& hut,std::span<const CircularObstacle> obstacles);
};
}
