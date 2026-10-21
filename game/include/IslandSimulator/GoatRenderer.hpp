#pragma once
#include "IslandSimulator/EyeshineRenderer.hpp"
#include <span>
#include "IslandSimulator/GoatHerd.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <array>
#include <memory>
#include <vector>
namespace Microsoft::Xna::Framework::Graphics {
class GraphicsDevice; class BasicEffect; class DynamicVertexBuffer; class Texture2D; struct VertexPositionColorTexture;
}
namespace island {
class CanopyLightField;
class GoatRenderer final {
public:
    static constexpr float DetailDistance=14.0f;   // the detailed mesh within this distance
    GoatRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
        const CanopyLightField& canopy,float maximumDistance);
    ~GoatRenderer();
    void Draw(const Microsoft::Xna::Framework::Matrix& view,
        const Microsoft::Xna::Framework::Matrix& projection,
        const Microsoft::Xna::Framework::Vector3& camera,std::span<const GoatState> goats);
    [[nodiscard]] int VisibleCount() const noexcept {return visible_;}
    /** The near goats' eyes as drawn this frame (for the eyeshine). */
    [[nodiscard]] std::span<const EyeGlint> Eyes() const noexcept {return eyes_;}
    [[nodiscard]] int DrawCallCount() const noexcept {return draws_;}
    /** The short straight-hair strand texture (the deer borrow it for their coats) and its
     *  mean colour (0..1), by which a tinted coat divides its colour to keep it. */
    [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D& CoatTexture() const noexcept {return *coat_;}
    [[nodiscard]] const std::array<float,3>& CoatMean() const noexcept {return coatMean_;}
private:
    Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
    const CanopyLightField& canopy_;
    float maximumDistance_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> effect_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> buffer_;
    std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> coat_;
    std::array<float,3> coatMean_{1.0f,1.0f,1.0f};
    std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColorTexture> vertices_,solidVertices_;
    int visible_=0,draws_=0;
    std::vector<EyeGlint> eyes_;
};
}
