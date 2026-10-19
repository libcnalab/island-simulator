#pragma once

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include <memory>
#include <string>

namespace island {
    /**
     * One orthographic depth map of the fixed sun around the camera. Every caster is static
     * (trees, palms, snags, bushes, the hut, the wreck and the helicopter), so the map is only
     * re-rendered when the camera leaves an AnchorStep cell; receivers sample it with a bilinear
     * four-tap comparison. Depth is packed into an RGBA8 colour target so WebGL 2, GLES 3 and
     * desktop GL all read it with an ordinary sampler2D and no depth-texture extension.
     */
    class SunShadowMap final
    {
    public:
        static constexpr float Radius = 72.0f;      // half-extent of the mapped square, metres
        static constexpr float AnchorStep = 8.0f;   // camera cell that triggers a re-render
        static constexpr float EyeDistance = 160.0f; // light eye offset along the sun ray
        static constexpr float DepthRange = 330.0f;  // far plane of the orthographic light
        static constexpr float CasterMargin = 26.0f; // casters beyond the box still cast into it
        static constexpr int TextureUnit = 3;        // sampler unit every receiver uses
        static constexpr float DepthBias = 0.0014f;  // normalised, about 0.45 m

        /** GLSL for receivers: the uniforms and sunShadowVisibility(vec3 world) in [0,1]. */
        static const char* const ReceiverShader;
        /** GLSL fragment for casters: packs the interpolated vSunDepth. */
        static const char* const CasterFragment;
        /** GLSL vertex for static, non-instanced casters whose position is attribute 0. */
        static const char* const StructureCasterVertex;

        SunShadowMap(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device, int resolution);
        ~SunShadowMap();

        /** Recomputes the light around the camera; true when the map must be rendered now. */
        [[nodiscard]] bool Prepare(const Microsoft::Xna::Framework::Vector3& camera);
        /** Binds the colour target, sets its viewport and clears it to "no caster". */
        void Begin();
        /** Restores the back buffer and the viewport captured by Begin(). */
        void End();

        [[nodiscard]] const Microsoft::Xna::Framework::Matrix& LightViewProjection() const noexcept
        {
            return lightViewProjection_;
        }
        void LightViewProjectionColumnMajor(float out[16]) const;
        /** True when a caster at (x, z) can reach the mapped box. */
        [[nodiscard]] bool CastsInto(float x, float z) const noexcept;
        /** Sets the receiver uniforms and the sampler on TextureUnit. */
        void Bind(Microsoft::Xna::Framework::Graphics::ShaderEffect& effect) const;
        /** Sets the receiver uniforms so sunShadowVisibility() returns fully lit. */
        static void BindDisabled(Microsoft::Xna::Framework::Graphics::ShaderEffect& effect);
        /** Builds a caster effect from a vertex body that writes gl_Position and vSunDepth. */
        [[nodiscard]] std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect>
            CreateCasterEffect(const char* vertexBody, const std::string& name) const;

        [[nodiscard]] int Resolution() const noexcept { return resolution_; }
        [[nodiscard]] int RenderCount() const noexcept { return renderCount_; }
        [[nodiscard]] int LastCasterCount() const noexcept { return lastCasterCount_; }
        void CountCasters(int count) noexcept { lastCasterCount_ += count; }
        [[nodiscard]] bool Rendered() const noexcept { return renderCount_ > 0; }

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        int resolution_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::RenderTarget2D> target_;
        Microsoft::Xna::Framework::Matrix lightViewProjection_;
        Microsoft::Xna::Framework::Vector3 anchor_;
        Microsoft::Xna::Framework::Graphics::Viewport savedViewport_;
        bool hasAnchor_ = false;
        int renderCount_ = 0;
        int lastCasterCount_ = 0;
    };
}
