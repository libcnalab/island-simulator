#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include <memory>
#include <string>

namespace Microsoft::Xna::Framework::Graphics { class GraphicsDevice; class ShaderEffect; class Texture2D; }

namespace island
{
    class SunShadowMap;

    /**
     * A small material for the vertex-coloured structures that were drawn with the stock
     * BasicEffect (the jetty, the rope bridge, the lookout and the helicopter): the same lit
     * vertex colours (times one texture when `textured`) and linear land fog, plus the sun
     * shadow map and the cumulus shade with the palms' cool shadow tint, so a tree's shadow
     * falls across them as it does across the ground. The constructor throws when custom
     * effects are unavailable or the shader fails; the caller then keeps its BasicEffect.
     */
    class StructureShadowReceiver final
    {
    public:
        StructureShadowReceiver(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
                                const std::string& name, bool textured, float fogStart, float fogEnd);
        ~StructureShadowReceiver();
        StructureShadowReceiver(const StructureShadowReceiver&) = delete;
        StructureShadowReceiver& operator=(const StructureShadowReceiver&) = delete;

        /** Applies the material for one frame's draws; `texture` is bound to unit 0 when textured. */
        void Apply(const Microsoft::Xna::Framework::Matrix& viewProjection,
                   const Microsoft::Xna::Framework::Vector3& camera, const SunShadowMap* map,
                   Microsoft::Xna::Framework::Graphics::Texture2D* texture = nullptr);

    private:
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> effect_;
        bool textured_;
        float fogStart_, fogEnd_;
    };
}
