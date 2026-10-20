#pragma once
#include "IslandSimulator/CampfireSite.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <span>
#include <vector>
namespace island {
struct FirePoint {float x,y,z;};
enum class FireMaterial {Stone,CharredWood,Firewood,EndGrain,Ash,Ember,Coal};
struct FireVertex {FirePoint position,normal;float u,v;FireMaterial material;};
class CampfireLayout final {
public:
    explicit CampfireLayout(const IslandTerrain& terrain);
    [[nodiscard]] std::span<const FireVertex> Vertices() const noexcept {return vertices_;}
    [[nodiscard]] const std::array<CircularObstacle,2>& Obstacles() const noexcept {return obstacles_;}
    [[nodiscard]] FirePoint Centre() const noexcept {return centre_;}
    [[nodiscard]] FirePoint WoodCentre() const noexcept {return woodCentre_;}
    [[nodiscard]] static float GroundY();
    static constexpr int StoneCount=12,BurningLogCount=5,FirewoodCount=11,CoalPieceCount=10;
private:
    FirePoint centre_{},woodCentre_{};
    std::array<CircularObstacle,2> obstacles_{};
    std::vector<FireVertex> vertices_;
};
// The smoke plume's westward lean per unit altitude for a rise above the flames and a gust
// front strength 0..1: the trade-wind lean, plus the passing front laying it over further
// (and tearing it sideways by GustTear). The campfire vertex shader applies the same rule.
struct CampfirePlume {
    static constexpr float TradeLean=.34f,GustLean=.55f,GustTear=.30f;
    [[nodiscard]] static constexpr float Lean(float rise,float gust) noexcept {return rise*(TradeLean+gust*GustLean);}
};
}
