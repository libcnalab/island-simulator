#pragma once
#include "IslandSimulator/FruitBatColony.hpp"
#include "IslandSimulator/MineLayout.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island
{
    class IslandTerrain;

    /** @brief The old mine's small insect-eating bats. By day they hang in three clusters from
     *  the chamber's dome; a walker coming under them sets them flying round under the roof until
     *  the chamber is quiet again. At dusk they leave one after another down the adit and out of
     *  the portal to hunt over the clearing in front of it, and at dawn they come back in the same
     *  way to their places. Drawn by a FruitBatRenderer: small, dark, and only as bright as the
     *  mine's lamps make them. */
    class MineBatColony final
    {
    public:
        static constexpr std::size_t Count = 12U;
        static constexpr float Scale = 0.22f;
        /** @brief A walker this close to a cluster (within the chamber) sets the bats flying. */
        static constexpr float FlushDistance = 4.5f;
        /** @brief The bats leave from this darkness and come home below `ReturnNightness`. */
        static constexpr float LeaveNightness = 0.55f, ReturnNightness = 0.35f;
        /** @brief Along the adit, metres a second, and the gap between one bat's leaving and the next. */
        static constexpr float AditSpeed = 5.0f, LeaveSpacingSeconds = 1.4f;

        MineBatColony(const IslandTerrain& terrain, const MineLayout& layout);

        void SetNightness(float nightness) noexcept;
        void Update(float deltaSeconds, float observerX, float observerY, float observerZ) noexcept;

        [[nodiscard]] std::span<const FruitBatState> Bats() const noexcept { return states_; }
        [[nodiscard]] std::size_t HangingCount() const noexcept;
        [[nodiscard]] std::size_t FlutteringCount() const noexcept;
        /** @brief Bats out of the portal, hunting over the clearing. */
        [[nodiscard]] std::size_t HuntingCount() const noexcept;
        /** @brief Bats on their way along the adit, out or home. */
        [[nodiscard]] std::size_t CommutingCount() const noexcept;
        [[nodiscard]] std::uint32_t FlushCount() const noexcept { return flushes_; }
        /** @brief Where a bat hangs: its feet under the dome's rock. */
        [[nodiscard]] MinePoint Roost(std::size_t index) const noexcept;
        /** @brief The dome's height over the chamber floor at (across, depth), zero outside it. */
        [[nodiscard]] static float DomeUp(float across, float depth) noexcept;

    private:
        enum class Phase : std::uint8_t { Hang, Flutter, Out, Hunt, Home, In };
        struct Agent
        {
            Phase phase = Phase::Hang;
            MinePoint roost{};
            std::array<MinePoint, 5> route{};   // the roost to the clearing outside the portal
            std::array<float, 5> along{};       // the route's length to each point
            float routeS = 0.0f;
            float flutterSeconds = 0.0f;
            float clock = 0.0f;
            float orbitPhase = 0.0f, orbitRadius = 10.0f, orbitSpeed = 1.0f;
            float flapPhase = 0.0f;
            float leaveAt = 0.0f;
            bool snap = false;   // placed straight onto its round (the island loads at night)
        };
        [[nodiscard]] MinePoint RoutePoint(const Agent& agent, float s) const noexcept;
        void Place(Agent& agent, std::size_t index, float deltaSeconds) noexcept;

        const IslandTerrain& terrain_;
        const MineLayout& layout_;
        MinePoint huntCentre_{};
        std::vector<Agent> agents_;
        std::vector<FruitBatState> states_;
        float nightness_ = 0.0f;
        float elapsed_ = 0.0f;
        float darkSince_ = -1.0f;
        bool started_ = false;
        std::uint32_t flushes_ = 0U;
    };
}
