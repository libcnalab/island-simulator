#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/StreamDriftField.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** Leaves the crowns let go at a gust front: each spirals down through the shade, drifts
 *  downwind with the front, lands on the floor and lies there a while before it is gone.
 *  The leaves are DriftingLeaf records so the stream's leaf renderer draws them. */
class CanopyLeafFallField final {
public:
    static constexpr std::size_t Capacity=48;
    static constexpr float Range=60.0f;         // crowns within this of the observer shed
    static constexpr float LeafChance=0.4f;     // per crown, per gust front
    static constexpr float FallSpeed=0.55f;     // metres per second down
    static constexpr float DriftSpeed=0.9f;     // metres per second downwind (toward -x)
    static constexpr float SwaySpeed=0.5f;      // side-to-side as it rocks on the air
    static constexpr float SpinRate=2.4f;       // radians per second
    static constexpr float RestSeconds=40.0f;   // lying on the floor before it is gone
    explicit CanopyLeafFallField(const IslandTerrain& terrain) noexcept : terrain_(terrain) {}
    void SetCrowns(std::span<const CanopyPlacement> crowns) {
        crowns_=crowns; crownGust_.assign(crowns.size(),0.0f);
    }
    /** @param seconds the shared wind clock; @param observerX,Z the viewer, for the range. */
    void Update(float seconds,float deltaSeconds,float observerX,float observerZ) noexcept;
    [[nodiscard]] std::span<const DriftingLeaf> Leaves() const noexcept {return leaves_;}
    [[nodiscard]] std::size_t ActiveCount() const noexcept;
    [[nodiscard]] std::size_t FallingCount() const noexcept;
    [[nodiscard]] std::uint32_t ReleaseCount() const noexcept {return releases_;}
    /** @brief Something moving in a crown (a bird landing, a monkey passing) shakes a leaf
     *  loose just below the point. */
    void Shake(float x,float y,float z,std::uint32_t seed) noexcept;
    /** @brief A bird flushing lets a feather go: pale, falling at FeatherFallSpeed with a
     *  wider rock, drawn by the leaf renderer as its pale variant. */
    void Feather(float x,float y,float z,std::uint32_t seed) noexcept;
    [[nodiscard]] std::uint32_t FeatherCount() const noexcept {return feathers_;}
    static constexpr float FeatherFallSpeed=0.28f;
    static constexpr std::uint8_t FeatherVariant=3;
    [[nodiscard]] std::uint32_t ShakeCount() const noexcept {return shakes_;}
private:
    void Release(const CanopyPlacement& crown,std::uint32_t seed) noexcept;
    std::size_t Let(float x,float y,float z,float yaw,std::uint32_t seed) noexcept;
    const IslandTerrain& terrain_;
    std::span<const CanopyPlacement> crowns_;
    std::vector<float> crownGust_;
    std::array<DriftingLeaf,Capacity> leaves_{};
    std::array<float,Capacity> phase_{};
    std::array<float,Capacity> rest_{};   // seconds lying; negative while falling
    std::uint32_t releases_=0,shakes_=0,feathers_=0;
};
}
