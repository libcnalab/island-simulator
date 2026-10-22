#pragma once
#include "IslandSimulator/MineLayout.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <array>
#include <memory>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;

// The old mine: the cutting into the slope with its log cribbing and the portal's timber frame,
// the adit's rock walls and gravel floor under timber sets every metre and a half with lagging
// over their caps, the narrow-gauge track on its sleepers, oil lanterns hanging from the posts,
// and at the end a domed chamber whose walls bristle with glowing crystals, an ore cart tipped
// over on the floor. Two draws: the rock and the timber in the daylight that comes in at the
// portal (graded with the hour like the rest of the scene), then, over the grade, the light the
// lanterns and the crystals throw added on, the lanterns' flames and the crystals themselves,
// and the lanterns' soft halos.
class MineRenderer final {
public:
    MineRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const MineLayout& layout,
                 const IslandTerrain& terrain,const RenderedTerrainSurface& ground);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    /** The lanterns' and the crystals' light, drawn after the hour's grade so the night keeps it. */
    void DrawLight(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                   const Microsoft::Xna::Framework::Vector3& camera,float seconds);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int TriangleCount() const noexcept {return baseTriangles_+lightTriangles_;}
    [[nodiscard]] int LanternCount() const noexcept {return lanterns_;}
    static constexpr float DrawDistance=140.0f;
    /** A puddle down the adit: its middle (across, depth in) and half extents, its rim pushed in
     *  and out round the oval. */
    struct Puddle {float across=0,depth=0,halfAcross=0.2f,halfAlong=0.5f;std::array<float,14> outline{};};
    [[nodiscard]] static std::vector<Puddle> Puddles(const MineLayout& layout);
    /** The lanterns hung down the adit: their flames' places. */
    [[nodiscard]] static std::vector<Microsoft::Xna::Framework::Vector3> HangingLanterns(const MineLayout& layout);
    /** QA: where to stand, eyes at `eyeHeight`, to see a lantern's flame in a puddle, and where to
     *  look; false if no puddle lies short of a lantern on its side. */
    [[nodiscard]] static bool ReflectionView(const MineLayout& layout,float eyeHeight,
                                             Microsoft::Xna::Framework::Vector3& eye,Microsoft::Xna::Framework::Vector3& look);
    /** The lanterns' glints drawn in the puddles last frame. */
    [[nodiscard]] int GlintCount() const noexcept {return glints_;}
private:
    [[nodiscard]] bool Visible(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                               const Microsoft::Xna::Framework::Vector3& camera) const;
    static constexpr std::size_t ReflectionCapacity=4800;
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const MineLayout& layout_;
    std::vector<Puddle> puddles_;
    std::vector<Microsoft::Xna::Framework::Vector3> lanternPositions_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> reflections_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> reflectionStaging_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> base_,light_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 entrance_;
    int baseTriangles_=0,lightTriangles_=0,drawCalls_=0,lanterns_=0,glints_=0;
};
}
