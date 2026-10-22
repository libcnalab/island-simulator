#pragma once

#include <array>

namespace island
{
    /**
     * The walking player's own sun shadow, the figure rather than a blot: the body as seventeen
     * capsules (each leg in thigh, shin and foot, the hips, a torso of two side-by-side capsules,
     * the two sloping shoulders, neck and head, each arm in two), posed by the gait, and laid
     * along the sun onto the plane of the feet. A sphere slid along a slanting ray lands as an
     * ellipse 1/sin(e) longer down the shadow; measured with that axis shortened by sin(e) it is a
     * circle again, so every capsule lands as a plain 2D capsule there and one signed distance per
     * fragment (a smooth union, filled out at the joints) gives the whole silhouette: soft at its
     * rim, never darker where the parts overlap, long in the morning and evening and short about
     * noon.
     */
    namespace walker_shadow
    {
        inline constexpr int CapsuleCount = 17;
        inline constexpr float JointBlend = 0.03f;      // metres of the smooth union at the joints
        inline constexpr float StrideMetres = 1.40f;    // one full gait cycle: a step with each foot
        inline constexpr float MinimumSunSine = 0.12f;  // a sun lower than some 7 degrees lays none

        /** A capsule in shadow space: (along the shadow x sin(e), across it), metres. */
        struct Capsule
        {
            float ax = 0.0f, ay = 0.0f, bx = 0.0f, by = 0.0f, radius = 0.0f;
        };

        struct Pose
        {
            float facingX = 0.0f, facingZ = 1.0f;   // the way the body faces, horizontal
            float gaitPhase = 0.0f;                 // radians, a full turn per stride
            float swing = 0.0f;                     // 0 standing .. 1 a brisk walk
        };

        struct Shape
        {
            std::array<Capsule, CapsuleCount> capsules{};
            float alongX = 0.0f, alongZ = 1.0f;   // unit horizontal direction the shadow falls in
            float sunSine = 1.0f;                 // sine of the light's elevation
            float length = 0.0f;                  // feet to the crown's shadow on level ground, metres
            bool cast = false;                    // false: the light is too low (or below the horizon)
        };

        /** The silhouette for a pose under the light toward (toSunX, toSunY, toSunZ). */
        [[nodiscard]] Shape Build(const Pose& pose, float toSunX, float toSunY, float toSunZ) noexcept;
        /** The silhouette's signed distance, metres in shadow space, at an offset from the feet on
         *  their plane (negative inside): the CPU mirror of the shader's, for the tests. */
        [[nodiscard]] float SignedDistance(const Shape& shape, float offsetX, float offsetZ) noexcept;
    }
}
