#include "IslandSimulator/SunShadowFocus.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>

using namespace Microsoft::Xna::Framework;

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition) throw std::runtime_error(message);
    }

    Vector3 Focus(const Vector3& camera, const Vector3& forward, float clearance)
    {
        // Exercise the same row-vector view-column extraction used by IslandGame.
        const auto view = Matrix::CreateLookAt(camera, camera + forward, Vector3::Up);
        return island::SunShadowViewFocus(camera, {-view.M13, -view.M23, -view.M33}, clearance);
    }
}

int main()
try
{
    const Vector3 camera(0, 68, -365), forward(0, -0.16f, 0.987f);
    for (float clearance : {-2.0f, 1.7f, 12.0f}) {
        Require(Focus(camera, forward, clearance) == camera,
            "Walking and swimming must retain their exact shadow column");
    }
    const auto overview = Focus(camera, forward, 68);
    Require(overview.X == camera.X && overview.Y == camera.Y
        && overview.Z > -120 && overview.Z < -100,
        "Overview map must reach the visible near-side forest instead of empty sea");
    Require(island::sun_shadow_fade::Strength(overview.X, 12, overview.Z,
        overview.X, overview.Z, .5f, .8f, .3f) == 1,
        "The focused receiver must remain inside the existing map and fade");
    for (float sign : {-1.0f, 1.0f}) {
        const auto side = Focus({-365*sign,115,0}, {sign,-.4f,0}, 115);
        Require(side.X*sign > -130 && side.X*sign < -100 && std::abs(side.Z)<.001f,
            "Side views must follow the actual view direction");
    }
    Vector3 previous = camera;
    for (int height=12; height<=120; ++height) {
        const auto focus = Focus(camera, forward, static_cast<float>(height));
        const float shift=Vector3::Distance(camera,focus);
        Require(shift<=280.001f && focus.Z>=previous.Z
            && Vector3::Distance(focus,previous)<7,
            "Altitude transition must be bounded, continuous and monotone");
        previous=focus;
    }
    previous=Focus(camera,{0,.01f,1},100);
    for(int step=0;step<=300;++step) {
        const auto focus=Focus(camera,{0,-step*.001f,1},100);
        Require(Vector3::Distance(focus,previous)<4,
            "Tilting through the horizon must not jump the shadow map");
        previous=focus;
    }
    Require(Focus(camera,{0,.5f,1},100)==camera
        && island::SunShadowViewFocus(camera,{0,-1,0},100)==camera,
        "Upward and vertical views must not introduce a horizontal offset");
    Require(island::SunShadowViewFocus(camera,{0,0,0},100)==camera
        && island::SunShadowViewFocus(camera,forward,std::numeric_limits<float>::quiet_NaN())==camera,
        "Invalid direction/clearance must retain the current column");
    std::cout << "Sun shadow focus tests passed; no graphics context\n";
    return 0;
}
catch(const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
