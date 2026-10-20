#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"
#include "IslandSimulator/WorldCollision.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>

namespace island {

class IslandTerrain;

enum class CrocodileBehavior : std::uint8_t {Bask,Crawl,Swim,Retreat};

struct CrocodileState {
    float x=0,y=0,z=0;
    float headingRadians=0;
    float speed=0;
    float gaitPhase=0;
    float bodyWave=0;
    float jawOpen=0;
    /** @brief 0..1 the blink: the eyes shut for a moment every several seconds, basking or
     *  afloat; held open while dormant. */
    float blink=0;
    float submerged=0;
    float scale=1;
    CrocodileBehavior behavior=CrocodileBehavior::Bask;
    WildlifeSimulationTier simulationTier=WildlifeSimulationTier::Dormant;
    std::uint8_t colorVariant=0;
};

struct CrocodileHome {
    float landX=0,landZ=0;
    float waterX=0,waterZ=0;
    float radius=0;
};

/** Three peaceful saltwater crocodiles local to the stream estuary. */
class CrocodileGroup final {
public:
    static constexpr std::size_t CrocodileCount=3;
    // Within this range a crocodile lies facing the player; closer than 14 m it slides off obliquely.
    static constexpr float AlertDistance=24.0f;
    static constexpr float FullSimulationDistance=150;
    static constexpr float ReducedSimulationDistance=360;

    explicit CrocodileGroup(const IslandTerrain& terrain);
    void SetObstacles(std::span<const CircularObstacle> obstacles);
    void Update(float deltaSeconds,float observerX,float observerY,float observerZ) noexcept;

    [[nodiscard]] std::span<const CrocodileState> Crocodiles()const noexcept{return states_;}
    /** @brief The last crocodile to slide off the bank into the water: the entry point and
     *  the ring's age, counted so an entry is noted once. */
    struct Entry { float x=0,z=0,ageSeconds=0; std::uint32_t count=0; bool valid=false; };
    [[nodiscard]] const Entry& LastEntry()const noexcept{return entry_;}
    /** @brief The last crocodile to come alert on the bank at the walker: where its head is,
     *  counted so the hiss is given once per alert; its jaw gapes while it hisses. */
    struct Hiss { float x=0,y=0,z=0; std::uint32_t count=0; bool valid=false; };
    [[nodiscard]] const Hiss& LastHiss()const noexcept{return hiss_;}
    static constexpr float HissSeconds=1.6f;
    /** @brief Gapes: a few seconds into each lie-up on the bank a basking crocodile opens
     *  its jaws and holds them open for GapeSeconds (the thermoregulatory gape), closing
     *  them again or when it moves off; counted as they begin. */
    [[nodiscard]] std::uint32_t GapeCount()const noexcept{return gapes_;}
    [[nodiscard]] std::size_t GapingCount()const noexcept;
    static constexpr float GapeSeconds=14.0f;
    static constexpr float GapeOpen=.50f;
    static constexpr float EntrySeconds=2.6f;
    /** @brief Blinks begun (every BlinkPeriodSeconds or so per crocodile, each BlinkSeconds)
     *  and the crocodiles mid-blink. */
    [[nodiscard]] std::uint32_t BlinkCount()const noexcept{return blinks_;}
    [[nodiscard]] std::size_t BlinkingCount()const noexcept;
    static constexpr float BlinkSeconds=0.3f,BlinkPeriodSeconds=8.0f;
    [[nodiscard]] std::span<const CrocodileHome> Homes()const noexcept{return homes_;}
    [[nodiscard]] int FullSimulationCount()const noexcept{return fullCount_;}
    [[nodiscard]] int ReducedSimulationCount()const noexcept{return reducedCount_;}
    [[nodiscard]] int DormantSimulationCount()const noexcept{return dormantCount_;}
    [[nodiscard]] bool IsValidHabitat(float x,float z,const CrocodileHome& home)const noexcept;

private:
    struct Agent {
        CrocodileState state;
        CrocodileHome home;
        float behaviorSeconds=0;
        float accumulatedSeconds=0;
        float phaseOffset=0;
        std::uint32_t transitions=0;
        bool wasAlert=false;
        float hissSeconds=0;
        float gapeDelay=0,gapeSeconds=0;   // the wait into a bask before the gape, and its remainder
        float blinkClock=0;bool blinking=false;
        bool gaping=false;
    };
    [[nodiscard]] std::pair<float,float> FindAnchor(float lateral,float targetShore,
                                                    bool requireDry)const;
    void InitializeAgents();
    void UpdateAgent(Agent&,float deltaSeconds,float observerX,float observerZ,
                     bool observerValid) noexcept;

    const IslandTerrain& terrain_;
    std::vector<Agent> agents_;
    std::vector<CrocodileState> states_;
    std::vector<CrocodileHome> homes_;
    std::vector<CircularObstacle> obstacles_;
    float elapsedSeconds_=0;
    std::uint64_t frameIndex_=0;
    Entry entry_;
    Hiss hiss_;
    std::uint32_t gapes_=0;
    std::uint32_t blinks_=0;
    int fullCount_=0,reducedCount_=0,dormantCount_=0;
};

}
