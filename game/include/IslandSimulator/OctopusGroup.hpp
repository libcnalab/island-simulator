#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#include "IslandSimulator/WildlifeSimulation.hpp"

namespace island
{
    class IslandTerrain;
    enum class OctopusBehavior : std::uint8_t { Den, Crawl, Forage, Swim };

    struct OctopusState
    {
        float x=0.0f,y=0.0f,z=0.0f,headingRadians=0.0f,speed=0.0f;
        float mantlePulse=0.0f,armWave=0.0f,hideAmount=0.0f,scale=1.0f;
        /** @brief 0..1 matched to the seabed: an octopus lying still in its den or at a
         *  forage takes the sand's tones over a few seconds and flashes back to its own
         *  colours the moment it moves or is startled. */
        float camouflageAmount=0.0f;
        /** @brief 0..1 the jet: setting out to swim, or startled for its den, the octopus
         *  draws its mantle in hard and straightens its arms for the first moment. */
        float jet=0.0f;
        OctopusBehavior behavior=OctopusBehavior::Den;
        WildlifeSimulationTier simulationTier=WildlifeSimulationTier::Dormant;
        std::uint8_t colorVariant=0U;
    };
    struct OctopusDen { float x=0.0f,z=0.0f,radius=0.0f; };

    /** @brief Four peaceful octopuses in bounded reef and rocky-shallow dens. */
    class OctopusGroup final
    {
    public:
        static constexpr std::size_t Count=4U;
        static constexpr float MinimumShoreDistance=-38.0f,MaximumShoreDistance=-7.0f;
        static constexpr float FullSimulationDistance=110.0f,ReducedSimulationDistance=280.0f;
        static constexpr float CamouflageSeconds=4.0f,FlashSeconds=0.4f,StillSpeed=0.03f;
        /** @brief Jets begun (one as each swim sets out, one as a startled octopus makes for
         *  its den) and the octopuses jetting this moment. */
        [[nodiscard]] std::uint32_t JetCount()const noexcept{return jets_;}
        [[nodiscard]] std::size_t JettingCount()const noexcept;
        static constexpr float JetSeconds=0.8f,EscapeJetSeconds=0.6f;
        [[nodiscard]] std::size_t CamouflagedCount() const noexcept;
        explicit OctopusGroup(const IslandTerrain& terrain);
        void Update(float deltaSeconds,float observerX,float observerY,float observerZ)noexcept;
        [[nodiscard]] std::span<const OctopusState> Octopuses()const noexcept{return states_;}
        [[nodiscard]] std::span<const OctopusDen> Dens()const noexcept{return dens_;}
    /** @brief The last octopus startled out of the open into its den by the swimmer: where it
     *  was, counted so an inking is noted once. */
    struct InkEvent { float x=0.0f,y=0.0f,z=0.0f,scale=1.0f; std::uint32_t count=0; };
    [[nodiscard]] const InkEvent& LastInk()const noexcept{return ink_;}
        [[nodiscard]] int FullSimulationCount()const noexcept{return fullCount_;}
        [[nodiscard]] int ReducedSimulationCount()const noexcept{return reducedCount_;}
        [[nodiscard]] int DormantSimulationCount()const noexcept{return dormantCount_;}
    private:
        struct Agent { OctopusState state;OctopusDen den;float routePhase=0,pulsePhase=0,armPhase=0,behaviorSeconds=0,accumulatedSeconds=0;std::uint32_t transitionCount=0;float jetSeconds=0;bool jetting=false; };
        void Initialize(std::size_t index);
        void UpdateAgent(Agent&,float,float,float,bool)noexcept;
        [[nodiscard]] bool ValidHabitat(float,float,const OctopusDen&)const noexcept;
        [[nodiscard]] float DesiredHeight(const Agent&)const noexcept;
        const IslandTerrain& terrain_;std::vector<Agent> agents_;std::vector<OctopusState> states_;std::vector<OctopusDen> dens_;
        float elapsedSeconds_=0;std::uint64_t frameIndex_=0;int fullCount_=0,reducedCount_=0,dormantCount_=0;
        InkEvent ink_;
        std::uint32_t jets_=0;
    };
}
