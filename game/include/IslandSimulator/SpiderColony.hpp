#pragma once
#include "IslandSimulator/SpiderHabitat.hpp"
#include "IslandSimulator/SpiderPose.hpp"
#include "IslandSimulator/WildlifeSimulation.hpp"
#include <cstdint>

namespace island {
struct SpiderGroundPose {
    SpiderPoint origin{},right{},up{},forward{};
    std::array<SpiderLegPose,8> legs{};
    std::array<SpiderPoint,8> feet{};
    std::array<bool,8> stance{};
    [[nodiscard]] SpiderPoint ToWorld(SpiderPoint local) const noexcept;
    [[nodiscard]] SpiderPoint ToLocal(SpiderPoint world) const noexcept;
    bool operator==(const SpiderGroundPose&) const = default;
};
enum class SpiderBehavior { Rest,Crawl,Retreat };
struct SpiderState {
    float x=0,y=0,z=0,heading=0,speed=0;
    SpiderBehavior behavior=SpiderBehavior::Rest;
    WildlifeSimulationTier tier=WildlifeSimulationTier::Dormant;
    // 0..1 the threat display: the walker standing at ground level between the retreat
    // range and DisplayDistance has the spider stop and rear, its front pair of legs raised
    // off the ground; stepping closer drops it for the retreat, leaving lets it down.
    float display=0;
    // 0..1 the groom: a spider at rest, unthreatened, draws one front leg (groomLeg: 0 the
    // left, 4 the right) in to its mouthparts and works it through them for a few seconds,
    // the legs taking turns; a threat or the next crawl drops it.
    float groom=0;
    std::uint8_t groomLeg=0;
    SpiderGroundPose pose{};
    bool operator==(const SpiderState&) const = default;
};
// Ground population only. Web occupants and anchors remain a separate habitat.
class SpiderColony final {
public:
    static constexpr float CrawlSpeed=.035f,RetreatSpeed=.065f;
    static constexpr float RetreatDistance=2.5f,DisplayDistance=4.0f,DisplayHeightReach=1.5f;
    static constexpr float DisplayRaiseRate=3.0f,DisplayLowerRate=2.0f;
    // Displays begun, and the spiders rearing this moment.
    [[nodiscard]] std::uint32_t DisplayCount() const noexcept {return displays_;}
    [[nodiscard]] std::size_t DisplayingCount() const noexcept;
    // Grooms begun (a rest GroomDelaySeconds in, every GroomPeriodSeconds, GroomSeconds long)
    // and the spiders grooming this moment.
    [[nodiscard]] std::uint32_t GroomCount() const noexcept {return grooms_;}
    [[nodiscard]] std::size_t GroomingCount() const noexcept;
    static constexpr float GroomDelaySeconds=1.0f,GroomSeconds=2.5f,GroomPeriodSeconds=12.0f;
    SpiderColony(const IslandTerrain& terrain,const JungleLayout& jungle,
        std::span<const CircularObstacle> obstacles);
    void Update(float dt,SpiderPoint observer);
    [[nodiscard]] std::span<const SpiderState> Spiders() const noexcept {return states_;}
    [[nodiscard]] std::span<const SpiderGroundHome> Homes() const noexcept {return homes_;}
private:
    struct Foot {SpiderPoint position{},start{},target{};float progress=0;bool swinging=false;};
    using Feet=std::array<Foot,8>;
    struct Intent {float remaining=0,heading=0;std::uint32_t decisions=0;bool displaying=false;float groomClock=0;bool grooming=false;};
    [[nodiscard]] SpiderGroundPose Frame(float x,float z,float heading) const;
    [[nodiscard]] SpiderGroundPose Pose(const SpiderState& state,const Feet& feet) const;
    [[nodiscard]] bool Valid(const SpiderGroundPose& pose) const;
    void AdvanceFeet(std::size_t index,float dt);
    void Step(std::size_t index,float dt,SpiderPoint observer,bool observerValid);
    RenderedTerrainSurface surface_;
    std::array<SpiderGroundHome,SpiderHabitat::GroundCount> homes_{};
    std::array<SpiderState,SpiderHabitat::GroundCount> states_{};
    std::array<Feet,SpiderHabitat::GroundCount> feet_{};
    std::array<Intent,SpiderHabitat::GroundCount> intents_{};
    std::array<float,SpiderHabitat::GroundCount> accumulatedSeconds_{};
    std::uint64_t frameIndex_=0;
    std::uint32_t displays_=0;
    std::uint32_t grooms_=0;
};
}
