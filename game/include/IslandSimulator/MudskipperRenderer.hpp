#pragma once
#include "IslandSimulator/Mudskippers.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
#include <span>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics {
class BasicEffect;
class DynamicVertexBuffer;
class GraphicsDevice;
class VertexBuffer;
struct VertexPositionColor;
}

namespace island {
class IslandTerrain;
class RenderedTerrainSurface;

/** The mangroves' mud and its mudskippers: the wet patches laid over the ground under the trees
 *  (their edges blended into the sand), and the skippers in one batch (a tapered body, two
 *  bulging eyes on top of the blunt head, pectoral fins planted like elbows, a dorsal sail that
 *  rises in display, a tail fin), all from one template turned and moved per fish. */
class MudskipperRenderer final {
public:
    static constexpr std::size_t MaximumSkippers=96;
    MudskipperRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const MudskipperColony& colony,
                       const IslandTerrain& terrain,const RenderedTerrainSurface& ground);
    ~MudskipperRenderer();
    void DrawMud(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
                 const Microsoft::Xna::Framework::Vector3& camera);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera,std::span<const Mudskipper> skippers);
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
    struct TemplateVertex {float x,y,z;std::uint8_t r,g,b;std::uint8_t part;};   // part: 0 body, 1 dorsal sail, 2 tail
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> mud_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> staging_;
    std::vector<TemplateVertex> fish_;
    int mudTriangles_=0,visible_=0;
    Microsoft::Xna::Framework::Vector3 mudCentre_{};
};

}
