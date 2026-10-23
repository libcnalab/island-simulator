#pragma once
#include "IslandSimulator/SeabirdIslet.hpp"
#include "IslandSimulator/StructureShadowReceiver.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include <memory>

namespace island {
class SunShadowMap;

// The boobies' islet in one opaque vertex-coloured draw: its stacks of dark basalt out of the
// sea, a band of green weed and black wet rock at the waterline, their tops and ledges white
// with guano and its streaks running down the faces below them; receiving the sun shadows.
class SeabirdIsletRenderer final {
public:
    SeabirdIsletRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,const SeabirdIslet& islet);
    void Draw(const Microsoft::Xna::Framework::Matrix& view,const Microsoft::Xna::Framework::Matrix& projection,
              const Microsoft::Xna::Framework::Vector3& camera);
    void SetSunShadowMap(const SunShadowMap* map) noexcept {sunShadow_=map;}
    [[nodiscard]] int TriangleCount() const noexcept {return triangles_;}
    [[nodiscard]] int DrawCallCount() const noexcept {return drawCalls_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<StructureShadowReceiver> receiver_;
    const SunShadowMap* sunShadow_=nullptr;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> vertices_;
    Microsoft::Xna::Framework::BoundingBox bounds_;
    Microsoft::Xna::Framework::Vector3 centre_;
    int triangles_=0,drawCalls_=0;
};
}
