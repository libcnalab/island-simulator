#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace island {
class IslandTerrain;

enum class DragonflyBehavior : std::uint8_t { Hover, Patrol, Dart, Perch };

struct DragonflyState {
    float x=0,y=0,z=0,headingRadians=0,speed=0,wingPhase=0,bankRadians=0,scale=1;
    // 0..1 the dip: on patrol the dragonfly drops to touch the water every few seconds.
    float dip=0;
    DragonflyBehavior behavior=DragonflyBehavior::Hover;
    WildlifeSimulationTier simulationTier=WildlifeSimulationTier::Dormant;
    std::uint8_t colorVariant=0;
};

struct DragonflyHome { float x=0,z=0,radius=0; };

/** Nine deterministic stream/wetland dragonflies with hover, patrol and short dart states. */
class DragonflySwarm final {
public:
    // Three dragonflies to a habitat: the wetland, two stream reaches, and the cascade's
    // plunge pool (found from the stream course at construction).
    static constexpr std::size_t Count=12,HabitatCount=4;
    static constexpr float PoolHabitatRadius=8.5f;   // the compact-range rule the core tests hold
    static constexpr float FullSimulationDistance=55,ReducedSimulationDistance=130;
    // One patrol in three ends on a perch: a stem's top at PerchHeight, the wings held still
    // for a few seconds, then off again. The walker's approach breaks it as any hover.
    static constexpr float PerchHeight=0.5f,PerchSeconds=4.0f;
    // The dip: every DipPeriodSeconds or so of patrol (a little per dragonfly) the dragonfly
    // drops to DipHeight over the ground for DipSeconds and touches the water; the touch is
    // noted, counted, for the fleck of spray it throws.
    static constexpr float DipPeriodSeconds=6.5f,DipSeconds=0.55f,DipHeight=0.30f;
    struct Dip { float x=0,y=0,z=0; std::uint32_t count=0; };
    [[nodiscard]] const Dip& LastDip()const noexcept{return dip_;}
    [[nodiscard]] std::uint32_t DipCount()const noexcept{return dip_.count;}
    [[nodiscard]] std::uint32_t PerchCount()const noexcept{return perches_;}
    [[nodiscard]] std::size_t PerchedCount()const noexcept;
    explicit DragonflySwarm(const IslandTerrain&);
    void Update(float deltaSeconds,float observerX,float observerY,float observerZ) noexcept;
    [[nodiscard]] std::span<const DragonflyState> Dragonflies()const noexcept{return states_;}
    [[nodiscard]] std::span<const DragonflyHome> Homes()const noexcept{return homes_;}
    [[nodiscard]] int FullSimulationCount()const noexcept{return fullCount_;}
    [[nodiscard]] int ReducedSimulationCount()const noexcept{return reducedCount_;}
    [[nodiscard]] int DormantSimulationCount()const noexcept{return dormantCount_;}
private:
    struct Agent { DragonflyState state;DragonflyHome home;float seconds=0,altitude=0;
        float turnA=0,turnB=0,vertical=0,accumulated=0;std::uint32_t transitions=0;
        float dipClock=0;bool touched=false; };
    void Initialize(std::size_t);
    void UpdateAgent(Agent&,float,float,float,bool) noexcept;
    [[nodiscard]] bool Valid(float,float,const DragonflyHome&)const noexcept;
    const IslandTerrain& terrain_;
    std::array<Agent,Count> agents_{};
    std::uint32_t perches_=0;
    Dip dip_;
    std::array<DragonflyState,Count> states_{};
    std::array<DragonflyHome,Count> homes_{};
    std::uint64_t frame_=0;int fullCount_=0,reducedCount_=0,dormantCount_=0;
    float poolX_=0,poolZ_=0;
};
}
