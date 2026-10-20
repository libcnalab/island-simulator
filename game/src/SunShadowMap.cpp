#include "IslandSimulator/WeatherSystem.hpp"
#include "IslandSimulator/SunShadowMap.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/CloudShadow.hpp"
#include "CNA/Graphics/ShaderCodeEXT.hpp"
#include "CNA/Graphics/ShaderPackageEXT.hpp"
#include "CNA/ShaderLanguageEXT.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include <cmath>
#include <stdexcept>
#include <string>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island {
    // The light matrix keeps the XNA [0,1] clip depth, so p.z is the stored depth directly.
    // EasyGL reports the sampled row order of a render target per unit through uRtFlipV;
    // the map lives on unit 3, hence the .w component.
    namespace {
        float cloudClock = 0.0f;
        bool qaDisabled = false;
    }
    void SunShadowMap::SetQaDisabled(const bool disabled) noexcept { qaDisabled = disabled; }
    bool SunShadowMap::QaDisabled() noexcept { return qaDisabled; }
    void SunShadowMap::SetCloudClock(const float seconds) noexcept { cloudClock = std::isfinite(seconds) ? seconds : 0.0f; }
    float SunShadowMap::CloudClock() noexcept { return cloudClock; }

    const std::string SunShadowMap::ReceiverShader = std::string(CloudShadow::Shader) + R"(
uniform mat4 uSunShadowMatrix;
uniform sampler2D uSunShadowMap;
uniform vec4 uSunShadowParams;
uniform vec4 uRtFlipV;
uniform vec4 uCloudShade;   // x: how much a cloud dims the sun, y: the cloud clock
uniform vec4 uSunShadowViewer; // xy: the viewer in the light's clip space, zw: the far fade's start and end
float sunShadowUnpack(vec4 c) {
    return dot(c, vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0));
}
float sunShadowVisibility(vec3 world) {
    float clouds = 1.0 - uCloudShade.x * cloudCover(world.xz, uCloudShade.y);
    if (uSunShadowParams.x <= 0.0) return clouds;   // x: the hour's shadow strength (none at night)
    vec4 p = uSunShadowMatrix * vec4(world, 1.0);
    vec2 uv = p.xy * 0.5 + 0.5;
    float depth = p.z - uSunShadowParams.y;
    float edge = min(min(uv.x, 1.0 - uv.x), min(uv.y, 1.0 - uv.y));
    uv.y = mix(uv.y, 1.0 - uv.y, uRtFlipV.w);
    // Receiver-plane depth bias: the surface's own depth slope across the map, from the
    // screen-space derivatives (taken here, in uniform control flow), extrapolates the
    // receiver's depth to every filter tap. A surface sloped to the sun (a crown, a frond,
    // a bank) then no longer shadows itself in bands that a constant bias cannot cover.
    vec2 duvdx = dFdx(uv), duvdy = dFdy(uv);
    float dzdx = dFdx(p.z), dzdy = dFdy(p.z);
    float det = duvdx.x * duvdy.y - duvdx.y * duvdy.x;
    vec2 dzduv = abs(det) > 1e-14
        ? vec2(duvdy.y * dzdx - duvdx.y * dzdy, duvdx.x * dzdy - duvdy.x * dzdx) / det
        : vec2(0.0);
    dzduv = clamp(dzduv, vec2(-6.0), vec2(6.0));
    if (edge <= 0.0 || depth <= 0.0 || depth >= 1.0) return clouds;
    float texel = uSunShadowParams.z;
    vec2 st = uv * uSunShadowParams.w - 0.5;
    vec2 f = fract(st);
    vec2 base = (floor(st) + 0.5) * texel;
    // 3x3 comparisons with bilinear corner weights: a 4x4 texel footprint whose edges
    // soften over about two texels instead of stepping, cheap enough for every receiver.
    float lit = 0.0;
    float weightSum = 0.0;
    for (int y = -1; y <= 2; ++y) {
        float wy = (y == -1) ? (1.0 - f.y) : (y == 2) ? f.y : 1.0;
        for (int x = -1; x <= 2; ++x) {
            float wx = (x == -1) ? (1.0 - f.x) : (x == 2) ? f.x : 1.0;
            float w = wx * wy;
            vec2 tap = base + vec2(float(x), float(y)) * texel;
            float tapDepth = depth + max(dot(dzduv, tap - uv), 0.0);
            lit += w * step(tapDepth, sunShadowUnpack(texture(uSunShadowMap, tap)));
            weightSum += w;
        }
    }
    lit /= max(weightSum, 0.0001);
    // The far fade: a rounded square in the sun's view around the viewer, which moves with the camera,
    // so the end of the shadows never jumps with the map's 8 m re-centring (SunShadowFade.hpp).
    vec2 fromViewer = (p.xy - uSunShadowViewer.xy) * (p.xy - uSunShadowViewer.xy);
    float farFade = smoothstep(uSunShadowViewer.z, uSunShadowViewer.w, sqrt(sqrt(dot(fromViewer, fromViewer))));
    return clouds * mix(1.0, lit, smoothstep(0.0, )" + std::to_string(sun_shadow_fade::EdgeBand) + R"(, edge) * (1.0 - farFade)
                        * uSunShadowParams.x);
}
)";

    const char* const SunShadowMap::CasterFragment = R"(
in float vSunDepth;
out vec4 fragColor;
void main() {
    float depth = clamp(vSunDepth, 0.0, 0.9995);
    vec4 encoded = fract(vec4(1.0, 255.0, 65025.0, 16581375.0) * depth);
    encoded -= encoded.yzww * vec4(1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0, 0.0);
    fragColor = encoded;
}
)";

    const char* const SunShadowMap::StructureCasterVertex = R"(
layout(location = 0) in vec3 aPosition;
uniform mat4 uSunViewProjection;
out float vSunDepth;
void main() {
    vec4 clip = uSunViewProjection * vec4(aPosition, 1.0);
    vSunDepth = clip.z / clip.w;
    gl_Position = clip;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
}
)";

    SunShadowMap::SunShadowMap(GraphicsDevice& device, const int resolution)
        : device_(device), resolution_(resolution),
          lightViewProjection_(Matrix::getIdentityProperty()), savedViewport_(0, 0, 1, 1)
    {
        if (resolution < 256 || resolution > 4096)
            throw std::invalid_argument("Sun shadow map resolution out of range");
        target_ = std::make_unique<RenderTarget2D>(device, resolution, resolution, false,
                                                   SurfaceFormat::Color, DepthFormat::Depth24, 0,
                                                   RenderTargetUsage::PreserveContents);
    }

    SunShadowMap::~SunShadowMap() = default;

    bool SunShadowMap::Prepare(const Vector3& camera)
    {
        if (!std::isfinite(camera.X) || !std::isfinite(camera.Z)) return false;
        viewer_ = camera;
        const Vector3 anchor(sun_shadow_fade::AnchorOf(camera.X), 0.0f, sun_shadow_fade::AnchorOf(camera.Z));
        // The light follows the day: the map re-renders when the camera leaves its cell or when
        // the shading direction has turned a quarter of a degree since the last render.
        const auto shading = sun_light::Current().shading;
        Vector3 toSun(shading.x, shading.y, shading.z);
        toSun.Normalize();
        const bool sunMoved = !hasAnchor_ || Vector3::Dot(toSun, lightDirection_) < 0.99999048f;   // cos(0.25 deg)
        if (hasAnchor_ && anchor.X == anchor_.X && anchor.Z == anchor_.Z && !sunMoved) return false;
        anchor_ = anchor;
        hasAnchor_ = true;
        lightDirection_ = toSun;
        const Vector3 target(anchor.X, sun_shadow_fade::TargetHeight, anchor.Z);
        const Vector3 eye = target + toSun * EyeDistance;
        const Matrix view = Matrix::CreateLookAt(eye, target, Vector3::Up);
        const Matrix projection = Matrix::CreateOrthographic(2.0f * Radius, 2.0f * Radius,
                                                             1.0f, DepthRange);
        lightViewProjection_ = view * projection;
        // Texel snapping: the light's rotation never changes, so shifting the map by whole
        // texels keeps one world-anchored texel grid across every re-render; without it each
        // 8 m re-centre moved the grid by a fraction of a texel and the shadow edges (and any
        // self-shadow banding) crawled as the camera flew.
        const Vector3 origin = Vector3::Transform(Vector3::Zero, lightViewProjection_);
        const float half = 0.5f * static_cast<float>(resolution_);
        const float dx = (std::round(origin.X * half) - origin.X * half) / half;
        const float dy = (std::round(origin.Y * half) - origin.Y * half) / half;
        lightViewProjection_ = lightViewProjection_ * Matrix::CreateTranslation(dx, dy, 0.0f);
        return true;
    }

    void SunShadowMap::Begin()
    {
        savedViewport_ = device_.getViewportProperty();
        device_.SetRenderTarget(target_.get());
        device_.setViewportProperty(Viewport(0, 0, resolution_, resolution_));
        // White unpacks beyond every stored depth: an empty texel never shadows anything.
        device_.Clear(static_cast<ClearOptions>(static_cast<int>(ClearOptions::Target)
                                                | static_cast<int>(ClearOptions::DepthBuffer)),
                      Color(255, 255, 255, 255), 1.0f, 0);
        lastCasterCount_ = 0;
        ++renderCount_;
    }

    void SunShadowMap::End()
    {
        device_.SetRenderTarget(nullptr);
        device_.setViewportProperty(savedViewport_);
    }

    void SunShadowMap::LightViewProjectionColumnMajor(float out[16]) const
    {
        lightViewProjection_.ToColumnMajor(out);
    }

    bool SunShadowMap::CastsInto(const float x, const float z) const noexcept
    {
        if (!hasAnchor_) return false;
        return std::abs(x - anchor_.X) <= Radius + CasterMargin
            && std::abs(z - anchor_.Z) <= Radius + CasterMargin;
    }

    void SunShadowMap::Bind(ShaderEffect& effect) const
    {
        if (!Rendered()) {
            BindDisabled(effect);
            return;
        }
        float matrix[16];
        LightViewProjectionColumnMajor(matrix);
        effect.SetUniformMat4("uSunShadowMatrix", matrix);
        device_.getSamplerStatesProperty()[TextureUnit] = SamplerState::PointClamp;
        effect.SetTexture(TextureUnit, *target_);
        effect.SetUniformInt("uSunShadowMap", TextureUnit);
        effect.SetUniformVec4("uSunShadowParams", sun_light::Current().lighting.shadowStrength, DepthBias,
                              1.0f / static_cast<float>(resolution_),
                              static_cast<float>(resolution_));
        effect.SetUniformVec4("uCloudShade", CloudShadow::LandStrength * sun_light::Current().lighting.sunVisibility,
                              cloudClock, 0.0f, 0.0f);   // a cloud only dims sunlight
        effect.SetUniformFloat("uCloudCoverage", weather_now::Current().cloudCover);
        // The viewer's column at the light's target height, in the light's clip space: the far
        // fade is measured from it, so it follows the camera continuously between re-renders.
        const Vector3 viewer = Vector3::Transform(
            Vector3(viewer_.X, sun_shadow_fade::TargetHeight, viewer_.Z), lightViewProjection_);
        effect.SetUniformVec4("uSunShadowViewer", viewer.X, viewer.Y,
                              sun_shadow_fade::FarFadeStart / Radius, sun_shadow_fade::FarFadeEnd / Radius);
    }

    void SunShadowMap::BindDisabled(ShaderEffect& effect)
    {
        effect.SetUniformVec4("uSunShadowParams", 0.0f, 0.0f, 0.0f, 1.0f);
        effect.SetUniformVec4("uCloudShade", CloudShadow::LandStrength * sun_light::Current().lighting.sunVisibility,
                              cloudClock, 0.0f, 0.0f);   // a cloud only dims sunlight
        effect.SetUniformFloat("uCloudCoverage", weather_now::Current().cloudCover);
    }

    std::unique_ptr<ShaderEffect> SunShadowMap::CreateCasterEffect(const char* vertexBody,
                                                                   const std::string& name) const
    {
        using CNA::Graphics::ShaderCodeEXT;
        const auto es = [](const char* body) {
            return std::string("#version 300 es\nprecision highp float;\n") + body;
        };
        const auto desktop = [](const char* body) {
            return std::string("#version 330 core\n") + body;
        };
        const CNA::Graphics::ShaderPackageEXT package(
            {ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Vertex, "main",
                           name + "/sun-shadow.es.vert", es(vertexBody)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslEs, CNA::ShaderStageEXT::Fragment, "main",
                           name + "/sun-shadow.es.frag", es(CasterFragment)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Vertex,
                           "main", name + "/sun-shadow.gl.vert", desktop(vertexBody)),
             ShaderCodeEXT(CNA::ShaderLanguageEXT::GlslDesktop, CNA::ShaderStageEXT::Fragment,
                           "main", name + "/sun-shadow.gl.frag", desktop(CasterFragment))},
            {CNA::ShaderStageEXT::Vertex, CNA::ShaderStageEXT::Fragment});
        auto effect = std::make_unique<ShaderEffect>(device_, package);
        if (!effect->IsEffectValid())
            throw std::runtime_error("Sun shadow caster shader failed for " + name + ": "
                                     + effect->GetCompileErrorEXT());
        return effect;
    }
}
