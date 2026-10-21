#pragma once

#include "IslandSimulator/QuadBike.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

namespace Microsoft::Xna::Framework::Graphics
{
    class BasicEffect;
    class DepthStencilState;
    class DynamicVertexBuffer;
    class GraphicsDevice;
    class ShaderEffect;
    class VertexBuffer;
}

namespace island
{
    class SunShadowMap;

    /**
     * The quad bike: a utility quad built in its body frame from tubes, shells and lathed
     * parts (a steel frame with bumpers and racks, painted fenders and nose, a seat and tank,
     * the engine with its finned head, the exhaust and muffler, footwells, a skid plate,
     * headlights and a taillight), handlebars that turn with the steering, four wheels of
     * knobby tyres on spoked rims that spin and steer, and A-arms, coil-over shocks and a
     * swingarm that follow the suspension. One lit material (painted, plastic, rubber and metal
     * responses, the sun shadow map, fog); the lamps glow after the night grade.
     */
    class QuadBikeRenderer final
    {
    public:
        explicit QuadBikeRenderer(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);
        ~QuadBikeRenderer();
        void SetSunShadowMap(const SunShadowMap* map) noexcept { sunShadow_ = map; }
        /** @brief How dirty the quad is (0 clean .. 1 caked) and how wet that dirt is (0 dried
         *  dust .. 1 fresh mud). */
        void SetDirt(float amount, float wet) noexcept
        {
            dirt_ = std::isfinite(amount) ? std::clamp(amount, 0.0f, 1.0f) : 0.0f;
            dirtWet_ = std::isfinite(wet) ? std::clamp(wet, 0.0f, 1.0f) : 0.0f;
        }

        /** @param riderFallback draw a simple seated rider (when no CNA avatar is available). */
        void Draw(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                  const Microsoft::Xna::Framework::Vector3& camera, const QuadBike& bike, bool lightsOn,
                  bool riderFallback);
        /** @brief The quad's shadow on the ground under it: its silhouette (and the rider's, as
         *  the stand-in figure) cast along the sun onto the plane through @p groundPoint, with
         *  @p sunStrength 0..1 its darkness relative to the scene's own sun shadows, and a soft
         *  occlusion under the body and the tyres that stays by night and under cloud. */
        void DrawShadow(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                        const Microsoft::Xna::Framework::Vector3& camera, const QuadBike& bike, bool rider,
                        const BikeVec& groundPoint, const BikeVec& groundNormal, float sunStrength);
        /** @brief The brakes on (the rider pulling back while it rolls, or the handbrake): the
         *  brake light glows red behind, by day as well as by night. */
        void SetBraking(bool braking) noexcept { braking_ = braking; }
        /** @brief The lamps' glow, after the night grade (additive). */
        void DrawLights(const Microsoft::Xna::Framework::Matrix& view, const Microsoft::Xna::Framework::Matrix& projection,
                        const Microsoft::Xna::Framework::Vector3& camera, const QuadBike& bike, float nightness);
        [[nodiscard]] int TriangleCount() const noexcept { return triangles_; }
        [[nodiscard]] int DrawCallCount() const noexcept { return drawCalls_; }
        /** @brief Body-frame points the game needs: the exhaust's tip, the headlights, the seat. */
        static constexpr BikeVec ExhaustTip{0.22f, -0.10f, -1.05f};
        static constexpr BikeVec HeadlightLeft{-0.19f, -0.03f, 1.005f}, HeadlightRight{0.19f, -0.03f, 1.005f};
        static constexpr BikeVec Taillight{0.0f, 0.035f, -1.02f};
        static constexpr BikeVec Seat{0.0f, 0.255f, -0.18f};
        static constexpr BikeVec GripLeft{-0.40f, 0.455f, 0.31f}, GripRight{0.40f, 0.455f, 0.31f};
        static constexpr BikeVec FootLeft{-0.36f, -0.24f, 0.10f}, FootRight{0.36f, -0.24f, 0.10f};

    private:
        using Vertex = Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;
        struct Pose
        {
            Microsoft::Xna::Framework::Matrix body, bars;
            std::array<Microsoft::Xna::Framework::Matrix, 4> wheels;
        };
        [[nodiscard]] static Pose PoseOf(const QuadBike& bike);
        /** The suspension (within 30 m) and the stand-in rider in the body frame, uploaded to the
         *  rider's or the riderless buffer unless unchanged since the last time; its vertex count. */
        int BuildDynamic(const QuadBike& bike, bool rider, bool detailed);
        void DrawAll(Microsoft::Xna::Framework::Graphics::ShaderEffect& effect, const Pose& pose, int dynamicCount,
                     bool rider);
        void Upload(std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer>& buffer, int& count,
                    const std::vector<Vertex>& vertices);
        void DrawPart(Microsoft::Xna::Framework::Graphics::ShaderEffect& effect,
                      Microsoft::Xna::Framework::Graphics::VertexBuffer& buffer, int count,
                      const Microsoft::Xna::Framework::Matrix& world);
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::ShaderEffect> material_, shadow_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DepthStencilState> shadowDepth_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::BasicEffect> glowEffect_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer> body_, bars_, frontWheel_, rearWheel_;
        // [rider]: the shadow and the body ask for the stand-in rider apart (with the avatar riding)
        std::array<std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer>, 2> dynamic_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::DynamicVertexBuffer> glow_;
        struct DynamicKey
        {
            std::array<Microsoft::Xna::Framework::Vector3, 4> hubs{};
            float lean = 0.0f;
            bool detailed = false, valid = false;
            int count = 0;
        };
        std::array<DynamicKey, 2> dynamicKeys_{};
        std::vector<Vertex> dynamicStaging_;
        std::vector<Microsoft::Xna::Framework::Graphics::VertexPositionColor> glowStaging_;
        int bodyCount_ = 0, barsCount_ = 0, frontWheelCount_ = 0, rearWheelCount_ = 0;
        const SunShadowMap* sunShadow_ = nullptr;
        int triangles_ = 0, drawCalls_ = 0;
        float dirt_ = 0.0f, dirtWet_ = 0.0f;
        bool braking_ = false;
        float riderLean_ = 0.0f;   // the stand-in rider's lean into the corner, radians, + right
    };
}
