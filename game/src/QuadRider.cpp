#include "IslandSimulator/QuadRider.hpp"
#include "IslandSimulator/DayNightCycle.hpp"
#include "IslandSimulator/QuadBikeRenderer.hpp"
#include "IslandSimulator/RuntimeDiagnostics.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <exception>
#include <iostream>
#include <numbers>
#include <vector>

#if defined(ISLAND_WITH_CNA_AVATAR)
#include "CNA/Internal/GamerServices/AvatarDescriptionCodec.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBone.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarExpression.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRenderer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarRendererState.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesDispatcher.hpp"
#endif

using namespace Microsoft::Xna::Framework;

namespace island
{
#if defined(ISLAND_WITH_CNA_AVATAR)
    namespace GS = Microsoft::Xna::Framework::GamerServices;
    struct QuadRider::Avatar
    {
        std::unique_ptr<GS::AvatarDescription> description;
        std::unique_ptr<GS::AvatarRenderer> renderer;
        std::vector<Matrix> bones;
        float hipHeight = 0.79f;
        bool readyReported = false;
    };

    namespace
    {
        constexpr float Degrees = std::numbers::pi_v<float> / 180.0f;
        // The rider: a man of 1.78 m in a red riding jersey, dark trousers and boots.
        std::vector<SharpRuntime::bytecs> RiderDescription()
        {
            namespace Avatars = CNA::Internal::GamerServices::Avatars;
            Avatars::AvatarDescriptor d;
            d.bodyType = 1;
            d.heightMillimeters = 1780;
            d.build = 128;
            d.catalogVersion = 3;
            d.colors = {{{205, 160, 125}, {52, 38, 28}, {70, 90, 60}, {196, 38, 34}, {46, 48, 54}, {28, 28, 30}, {30, 30, 30}}};
            d.items = {3, 22, 41, 62, 0, 0};
            return Avatars::encode(d);
        }

        // Astride the quad: thighs forward and a little apart, shins down to the footwells,
        // leaning into the bars, the arms forward to the grips with the elbows bent.
        // `steer` -1..1: turning the bars right brings the right grip back and sends the left
        // one forward, so the shoulders turn with them and the right elbow folds.
        void RidingPose(std::vector<Matrix>& bones, const float steer)
        {
            bones.assign(GS::AvatarRenderer::BoneCount, Matrix::getIdentityProperty());
            const auto set = [&](const int bone, const Matrix& m) { bones[static_cast<std::size_t>(bone)] = m; };
            set(1, Matrix::CreateRotationX(-14.0f * Degrees));                                           // BackLower
            set(5, Matrix::CreateRotationY(-20.0f * steer * Degrees) * Matrix::CreateRotationX(-10.0f * Degrees)); // BackUpper
            set(14, Matrix::CreateRotationX(6.0f * Degrees));                                            // Neck
            set(19, Matrix::CreateRotationX(10.0f * Degrees));                                           // Head
            set(2, Matrix::CreateRotationZ(-16.0f * Degrees) * Matrix::CreateRotationX(78.0f * Degrees)); // HipLeft
            set(3, Matrix::CreateRotationZ(16.0f * Degrees) * Matrix::CreateRotationX(78.0f * Degrees));  // HipRight
            set(6, Matrix::CreateRotationX(-92.0f * Degrees));                                           // KneeLeft
            set(8, Matrix::CreateRotationX(-92.0f * Degrees));                                           // KneeRight
            set(11, Matrix::CreateRotationX(18.0f * Degrees));                                           // AnkleLeft
            set(15, Matrix::CreateRotationX(18.0f * Degrees));                                           // AnkleRight
            set(20, Matrix::CreateRotationZ(-26.0f * Degrees) * Matrix::CreateRotationX(58.0f * Degrees)); // ShoulderLeft
            set(22, Matrix::CreateRotationZ(26.0f * Degrees) * Matrix::CreateRotationX(58.0f * Degrees));  // ShoulderRight
            set(25, Matrix::CreateRotationX(std::max(32.0f - 26.0f * steer, 6.0f) * Degrees));           // ElbowLeft
            set(28, Matrix::CreateRotationX(std::max(32.0f + 26.0f * steer, 6.0f) * Degrees));           // ElbowRight
            // The hands turned palm down on the grips (the arms hang palm in at rest), every
            // finger curled round the grip in its three joints, the thumbs under it.
            set(33, Matrix::CreateRotationY(-90.0f * Degrees));                                          // WristLeft
            set(36, Matrix::CreateRotationY(90.0f * Degrees));                                           // WristRight
            for (const auto& [joint, curl] : std::array<std::pair<int, float>, 3>{{{37, 50.0f}, {51, 62.0f}, {61, 45.0f}}}) {
                for (int finger = 0; finger < 4; ++finger) {
                    set(joint + finger, Matrix::CreateRotationZ(curl * Degrees));                       // left
                    set(joint + finger + (joint == 37 ? 7 : 5), Matrix::CreateRotationZ(-curl * Degrees)); // right
                }
            }
            set(43, Matrix::CreateRotationZ(30.0f * Degrees));                                           // ThumbLeft
            set(55, Matrix::CreateRotationZ(25.0f * Degrees));
            set(50, Matrix::CreateRotationZ(-30.0f * Degrees));                                          // ThumbRight
            set(60, Matrix::CreateRotationZ(-25.0f * Degrees));
        }
    }

    QuadRider::QuadRider(Game& game)
    {
        try {
            if (!GS::GamerServicesDispatcher::getIsInitializedProperty())
                GS::GamerServicesDispatcher::Initialize(game.getServicesProperty());
            avatar_ = std::make_unique<Avatar>();
            avatar_->description = std::make_unique<GS::AvatarDescription>(RiderDescription());
            avatar_->renderer = std::make_unique<GS::AvatarRenderer>(avatar_->description.get(), false);
            RidingPose(avatar_->bones, 0.0f);
            status_ = "a CNA avatar (loading)";
        } catch (const std::exception& error) {
            avatar_.reset();
            status_ = std::string("the stand-in rider (CNA avatar unavailable: ") + error.what() + ")";
        }
        const std::string message = "island-simulator: quad rider: " + status_;
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }

    QuadRider::~QuadRider() = default;

    bool QuadRider::Available() const noexcept
    {
        try {
            return avatar_ && avatar_->renderer->getStateProperty() == GS::AvatarRendererState::Ready;
        } catch (...) {
            return false;
        }
    }

    void QuadRider::Draw(const Matrix& view, const Matrix& projection, const QuadBike& bike)
    {
        if (!Available()) return;
        try {
            auto& renderer = *avatar_->renderer;
            if (!avatar_->readyReported) {
                const auto bind = renderer.getBindPoseProperty();
                avatar_->hipHeight = std::max(bind.getItem(2).getTranslationProperty().Y, 0.5f);
                avatar_->readyReported = true;
                status_ = "a CNA avatar";
                std::cout << "island-simulator: quad rider: the CNA avatar is ready, hips " << avatar_->hipHeight
                          << " m above its feet\n";
            }
            // The hips on the seat: the avatar's feet origin lies below the seat by its hip height.
            const BikeVec seat = bike.ToWorld(QuadBikeRenderer::Seat);
            const BikeVec up = bike.Up(), forward = bike.Forward();
            const Vector3 origin(seat.x - up.x * avatar_->hipHeight, seat.y - up.y * avatar_->hipHeight,
                                 seat.z - up.z * avatar_->hipHeight);
            renderer.setWorldProperty(Matrix::CreateWorld(origin, Vector3(forward.x, forward.y, forward.z), Vector3(up.x, up.y, up.z)));
            renderer.setViewProperty(view);
            renderer.setProjectionProperty(projection);
            const auto& light = sun_light::Current().lighting;
            const auto shading = sun_light::Current().shading;
            renderer.setLightDirectionProperty(Vector3(-shading.x, -shading.y, -shading.z));
            const float sun = 0.70f * std::max(light.sunVisibility, 0.25f);
            renderer.setLightColorProperty(Vector3(sun, sun * 0.97f, sun * 0.92f));
            renderer.setAmbientLightColorProperty(Vector3(0.40f, 0.42f, 0.46f));
            RidingPose(avatar_->bones, std::clamp(bike.SteerRadians() / QuadBike::MaxSteerRadians, -1.0f, 1.0f));
            renderer.Draw(avatar_->bones, GS::AvatarExpression());
        } catch (const std::exception& error) {
            std::cout << "island-simulator: quad rider draw failed: " << error.what() << '\n';
            avatar_.reset();
            status_ = "the stand-in rider (the CNA avatar failed to draw)";
        }
    }
#else
    struct QuadRider::Avatar {};
    QuadRider::QuadRider(Game&)
        : status_("the stand-in rider (this build has no CNA avatar)")
    {
        const std::string message = "island-simulator: quad rider: " + status_;
        std::cout << message << '\n';
        MirrorRuntimeDiagnosticToPlatformLog(message);
    }
    QuadRider::~QuadRider() = default;
    bool QuadRider::Available() const noexcept { return false; }
    void QuadRider::Draw(const Matrix&, const Matrix&, const QuadBike&) {}
#endif
}
