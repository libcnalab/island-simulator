#include "IslandSimulator/ExpeditionMarkerRenderer.hpp"

#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include "IslandSimulator/UnderwaterFog.hpp"

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

#include <cmath>
#include <array>
#include <iostream>
#include <string>
#include <vector>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;

namespace island
{
    ExpeditionMarkerRenderer::ExpeditionMarkerRenderer(
        GraphicsDevice& device, const std::span<const ExpeditionMarkerSite> sites)
        : device_(device), effect_(std::make_unique<BasicEffect>(device))
    {
        SculptMeshBuilder b;
        const Vector3 stone(92.0f, 91.0f, 84.0f);
        const Vector3 iron(83.0f, 69.0f, 58.0f);
        const Vector3 plate(73.0f, 86.0f, 82.0f);
        const Vector3 verdigris(66.0f, 143.0f, 130.0f);
        const Vector3 brass(204.0f, 167.0f, 82.0f);
        for (const ExpeditionMarkerSite& site : sites) {
            const Vector3 up = Vector3::Up;
            const Vector3 forward(std::sin(site.headingRadians), 0.0f,
                                  std::cos(site.headingRadians));
            const Vector3 right(forward.Z, 0.0f, -forward.X);
            const Vector3 ground(site.x, site.y, site.z);
            b.Box(ground + up * .075f, right, up, forward, .22f, .075f, .18f,
                  stone * .83f, stone);
            b.Box(ground + up * .43f, right, up, forward, .045f, .31f, .045f,
                  iron, iron * 1.04f);
            const Vector3 centre = ground + up * .75f;
            b.Box(centre, right, up, forward, .27f, .19f, .026f, plate * .82f,
                  plate);

            // Raised four-point compass and two wave bars: the same mark at every station.
            const Vector3 face = centre + forward * .030f;
            const Vector3 north = face + up * .135f;
            const Vector3 east = face + right * .135f;
            const Vector3 south = face - up * .105f;
            const Vector3 west = face - right * .135f;
            const Vector3 c = face + up * .025f;
            for (const auto& triangle : {
                     std::array<Vector3, 3>{c, north, c + right * .032f},
                     std::array<Vector3, 3>{c, c + right * .032f, east},
                     std::array<Vector3, 3>{c, south, c - right * .032f},
                     std::array<Vector3, 3>{c, west, c - right * .032f}}) {
                b.builder.Tri(triangle[0], triangle[1], triangle[2], brass);
                b.builder.Tri(triangle[2], triangle[1], triangle[0], brass * .88f);
            }
            b.Box(face - up * .095f, right, up, forward, .17f, .012f, .010f,
                  verdigris, verdigris * 1.06f);
            b.Box(face - up * .135f + right * .035f, right, up, forward,
                  .13f, .010f, .010f, verdigris * .88f, verdigris);

            // One to three studs make the plates sequential without written exposition.
            const int studs = 1 + static_cast<int>(site.chapter % 3U);
            for (int index = 0; index < studs; ++index) {
                const float offset = (static_cast<float>(index)
                    - .5f * static_cast<float>(studs - 1)) * .055f;
                b.Box(face + right * offset + up * .165f, right, up, forward,
                      .014f, .014f, .012f, brass * .78f, brass);
            }
            ++markerCount_;
        }

        vertexCount_ = static_cast<int>(b.builder.mesh.size());
        if (vertexCount_ > 0) {
            vertices_ = std::make_unique<VertexBuffer>(
                device_, VertexPositionColor::getVertexDeclarationStatic(), vertexCount_,
                BufferUsage::WriteOnly);
            vertices_->SetData(b.builder.mesh.data(), vertexCount_);
        }
        effect_->setVertexColorEnabledProperty(true);
        effect_->setLightingEnabledProperty(false);
        effect_->setFogEnabledProperty(true);
        effect_->setFogColorProperty(Vector3(
            environment::FogColorRed, environment::FogColorGreen, environment::FogColorBlue));
        effect_->setFogStartProperty(180.0f);
        effect_->setFogEndProperty(430.0f);
        const std::string message = "island-simulator: " + std::to_string(markerCount_)
            + " weathered expedition stakes repeat one compass-and-wave mark across the ruins";
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    ExpeditionMarkerRenderer::~ExpeditionMarkerRenderer() = default;

    void ExpeditionMarkerRenderer::Draw(const Matrix& view, const Matrix& projection,
                                         const Vector3&)
    {
        if (!vertices_ || vertexCount_ < 3) return;
        device_.setRasterizerStateProperty(RasterizerState::CullNone);
        device_.setBlendStateProperty(BlendState::Opaque);
        device_.setDepthStencilStateProperty(DepthStencilState::Default);
        device_.SetVertexBuffer(vertices_.get());
        device_.setIndicesProperty(nullptr);
        effect_->setWorldProperty(Matrix::getIdentityProperty());
        effect_->setViewProperty(view);
        effect_->setProjectionProperty(projection);
        underwater_fog::Apply(*effect_, 180.0f, 430.0f);
        effect_->Apply();
        device_.DrawPrimitives(PrimitiveType::TriangleList, 0, vertexCount_ / 3);
    }
}
