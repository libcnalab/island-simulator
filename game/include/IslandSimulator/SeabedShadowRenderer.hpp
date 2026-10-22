#pragma once
#include "IslandSimulator/FishSchools.hpp"
#include "IslandSimulator/SeaTurtleGroup.hpp"
#include "IslandSimulator/SharkPatrol.hpp"
#include "IslandSimulator/StingrayGroup.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/DynamicVertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include <memory>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;
/** Soft shadows on the sandy seabed under the swimming animals — the fish, the stingrays,
 *  the sea turtles and the shark — one seabed-conforming ellipse each, projected along the
 *  fixed sun, darker and tighter close over the bottom, broad and faint for a high one,
 *  gone with the water's own reach; drawn in one alpha-blended pass before the sea. */
class SeabedShadowRenderer final {
public:
    static constexpr int Segments=12,VerticesPerShadow=Segments*3,MaximumShadows=72;
    static constexpr float MaximumDistance=34.0f;
    SeabedShadowRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const IslandTerrain& terrain,
        const RenderedTerrainSurface& ground);   // laid on the drawn ground where the mesh reaches
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
              const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,
              std::span<const FishState> fish,std::span<const StingrayState> rays,
              std::span<const SeaTurtleState> turtles,const SharkState* shark);
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
private:
    void AppendShadow(const Microsoft::Xna::Framework::Vector3& camera,float x,float y,float z,
                      float headingRadians,float forwardRadius,float sideRadius) noexcept;
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const IslandTerrain& terrain_;
    const RenderedTerrainSurface& ground_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> vertices_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    int drawCalls_=0,visible_=0;
    // this frame's light under the water: the refracted run per metre of depth, and its strength
    float slopeX_=0,slopeZ_=0,strength_=1;
};
}
