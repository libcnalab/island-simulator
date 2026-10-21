#pragma once
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WorldCollisionGrid.hpp"
#include <span>
#include <vector>
namespace island {
struct ResidentPoint {float x=0,y=0,z=0;bool operator==(const ResidentPoint&) const=default;};
// A closed, deterministic tour through existing accessible parts of the island.
// Construction refuses a disconnected tour rather than teleporting or clearing trees.
class ResidentRoute final {
public:
    static constexpr float Clearance=.50f,GridSpacing=3.0f,Extent=258.0f;
    // The campfire call ends this far from the fire's centre (the stones' ring is 1.25 m).
    static constexpr float TendSpotDistance=1.9f;
    // Landmarks are extra tour goals (the jetty foot) visited after the west clearing; a
    // landmark the search cannot reach safely is dropped rather than breaking the tour.
    // landmarkFacings, one unit direction per landmark (x,z), is where the resident turns to
    // look during the pause there (the jetty foot faces out along the deck); empty for none.
    ResidentRoute(const IslandTerrain& terrain,const RenderedTerrainSurface& ground,const WorldCollisionGrid& solids,
                  std::span<const ResidentPoint> landmarks={},std::span<const ResidentPoint> landmarkFacings={});
    [[nodiscard]] std::size_t LandmarkCount() const noexcept {return landmarks_;}
    [[nodiscard]] std::span<const ResidentPoint> Points() const noexcept {return points_;}
    [[nodiscard]] std::span<const std::size_t> PausePoints() const noexcept {return pauses_;}
    // Parallel to PausePoints(): the direction faced during that pause, or NaN x for none.
    [[nodiscard]] std::span<const ResidentPoint> PauseFacings() const noexcept {return pauseFacings_;}
    [[nodiscard]] float Length() const noexcept {return length_;}
    [[nodiscard]] static bool SafePoint(const IslandTerrain& terrain,const RenderedTerrainSurface& ground,
        const WorldCollisionGrid& solids,float x,float z);
    [[nodiscard]] static bool SafeSegment(const IslandTerrain& terrain,const RenderedTerrainSurface& ground,
        const WorldCollisionGrid& solids,ResidentPoint a,ResidentPoint b);
private:
    std::vector<ResidentPoint> points_;
    std::vector<std::size_t> pauses_;
    std::vector<ResidentPoint> pauseFacings_;
    float length_=0;
    std::size_t landmarks_=0;
    void Build(const IslandTerrain& terrain,const RenderedTerrainSurface& ground,const WorldCollisionGrid& solids,
               std::span<const ResidentPoint> landmarks,std::span<const ResidentPoint> landmarkFacings);
};
}
