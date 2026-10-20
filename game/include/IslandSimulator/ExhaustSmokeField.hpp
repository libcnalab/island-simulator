#pragma once

#include "IslandSimulator/QuadBike.hpp"
#include "IslandSimulator/RootingDustField.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace island
{
    /** The quad bike's exhaust: puffs leave the tailpipe faster and thicker with the throttle,
     *  carried with the quad's speed, swelling, rising a little, drifting downwind and gone in
     *  a couple of seconds. Renderer-independent; drawn as dust puffs. */
    class ExhaustSmokeField final
    {
    public:
        static constexpr std::size_t Capacity = 36;
        static constexpr float Life = 2.4f;

        void Update(float seconds, bool running, float throttle, BikeVec tip, BikeVec direction,
                    BikeVec carrier, float windX, float windZ) noexcept;
        [[nodiscard]] std::span<const DustPuff> Puffs() const noexcept { return puffs_; }
        [[nodiscard]] std::uint32_t Emitted() const noexcept { return emitted_; }

    private:
        std::array<DustPuff, Capacity> puffs_{};
        std::array<BikeVec, Capacity> velocity_{};
        float pending_ = 0.0f;
        std::size_t next_ = 0;
        std::uint32_t emitted_ = 0;
        std::uint32_t jitter_ = 0x2545f491U;
    };
}
