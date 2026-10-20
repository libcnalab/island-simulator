#pragma once
#include "IslandSimulator/JungleLayout.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace island {
class IslandTerrain;

/** CPU mirror of the sun shadow map's solid casters for the animals' lighting: the standing
 *  snags, the understory, the bushes and the structures (the crowns and palm heads are the
 *  porous canopy light field's own). Each caster is a disc at its body's height whose shadow
 *  is rasterised onto a one-metre ground grid along the fixed sun direction; a receiver
 *  samples the shade under the point where its own sun line meets the ground. No GPU
 *  resources, no per-frame updates. */
class SunShadeField final {
public:
    static constexpr float HalfExtent=300.0f;
    static constexpr float CellSize=1.0f;
    static constexpr int Cells=static_cast<int>(2.0f*HalfExtent/CellSize);
    static constexpr float SnagShade=0.55f,UnderstoryShade=0.6f,BushShade=0.5f,StructureShade=0.85f;
    /** @brief The jungle's standing snags, understory and bushes as the shadow pass draws them. */
    SunShadeField(const IslandTerrain& terrain,const JungleLayout& jungle);
    /** @brief A solid caster: a disc of the given radius at centre height, shading the ground
     *  its shadow falls on by the given amount (max-combined with what is there). */
    void AddDisc(float x,float centreY,float z,float radius,float shade) noexcept;
    /** @brief 0..1 shade over the ground at a point (bilinear over the grid). */
    [[nodiscard]] float Cover(float x,float z) const noexcept;
    /** @brief Shade over a receiver at height y: the ground point its sun line rises from. */
    [[nodiscard]] float CoverAt(float x,float y,float z,float groundHeight) const noexcept;
    [[nodiscard]] std::size_t CasterCount() const noexcept {return casters_;}
private:
    [[nodiscard]] float Cell(int column,int row) const noexcept;
    const IslandTerrain& terrain_;
    std::vector<std::uint8_t> cells_;
    std::size_t casters_=0;
};
}
