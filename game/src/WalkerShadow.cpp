#include "IslandSimulator/WalkerShadow.hpp"
#include "IslandSimulator/ResidentMotion.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island::walker_shadow
{
    namespace
    {
        struct Point
        {
            float right = 0.0f, up = 0.0f, forward = 0.0f;   // body frame, metres above the feet
        };

        // A limb from a joint: `length` down its own angle from the vertical, forward positive.
        [[nodiscard]] Point Hang(const Point& joint, const float length, const float angle) noexcept
        {
            return {joint.right, joint.up - length * std::cos(angle), joint.forward + length * std::sin(angle)};
        }
    }

    Shape Lay(const std::array<Limb, CapsuleCount>& limbs, const float feetX, const float feetY, const float feetZ,
              const float toSunX, const float toSunY, const float toSunZ, const float stature) noexcept
    {
        Shape shape;
        const float length = std::sqrt(toSunX * toSunX + toSunY * toSunY + toSunZ * toSunZ);
        if (!(length > 1e-6f)) return shape;
        const float lx = toSunX / length, ly = toSunY / length, lz = toSunZ / length;
        if (ly < MinimumSunSine) return shape;
        // The shadow falls away from the sun; straight overhead it has no direction of its own.
        const float horizontal = std::hypot(lx, lz);
        shape.alongX = horizontal > 1e-4f ? -lx / horizontal : 0.0f;
        shape.alongZ = horizontal > 1e-4f ? -lz / horizontal : 1.0f;
        shape.sunSine = ly;
        shape.cast = true;
        const float acrossX = -shape.alongZ, acrossZ = shape.alongX;
        const auto project = [&](const float x, const float y, const float z, float& along, float& across) {
            // the body point slid down the light onto the feet's plane
            const float height = y - feetY;
            const float px = x - feetX - lx * (height / ly);
            const float pz = z - feetZ - lz * (height / ly);
            along = (px * shape.alongX + pz * shape.alongZ) * ly;
            across = px * acrossX + pz * acrossZ;
        };
        for (std::size_t i = 0; i < limbs.size(); ++i) {
            const Limb& limb = limbs[i];
            Capsule& capsule = shape.capsules[i];
            project(limb.ax, limb.ay, limb.az, capsule.ax, capsule.ay);
            project(limb.bx, limb.by, limb.bz, capsule.bx, capsule.by);
            capsule.radius = limb.radius;
        }
        shape.length = std::max(stature, 0.0f) * horizontal / ly;
        return shape;
    }

    Shape Build(const Pose& pose, const float toSunX, const float toSunY, const float toSunZ) noexcept
    {
        const float facingLength = std::hypot(pose.facingX, pose.facingZ);
        const float fx = facingLength > 1e-6f ? pose.facingX / facingLength : 0.0f;
        const float fz = facingLength > 1e-6f ? pose.facingZ / facingLength : 1.0f;
        const float rightX = -fz, rightZ = fx;

        // The gait: the thighs swing against each other, each knee folds while its leg swings
        // through, the arms swing against the legs; the hips sink as the legs spread, so the
        // planted foot stays on the ground.
        const float swing = std::clamp(pose.swing, 0.0f, 1.0f);
        const float reach = 0.42f * swing;
        const float hipDrop = 0.88f * (1.0f - std::cos(reach * std::abs(std::sin(pose.gaitPhase))));
        const float hipHeight = 0.94f - hipDrop;
        const float raise = -hipDrop;   // the trunk rides on the hips

        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const Point& a, const Point& b, const float radius) {
            limbs[next++] = {rightX * a.right + fx * a.forward, a.up, rightZ * a.right + fz * a.forward,
                             rightX * b.right + fx * b.forward, b.up, rightZ * b.right + fz * b.forward, radius};
        };
        for (const float side : {-1.0f, 1.0f}) {
            const float phase = pose.gaitPhase + (side > 0.0f ? std::numbers::pi_v<float> : 0.0f);
            const float thigh = reach * std::sin(phase);
            const float knee = swing * (0.08f + 0.60f * std::max(0.0f, std::cos(phase)));
            const Point hip{side * 0.095f, hipHeight, 0.0f};
            const Point kneePoint = Hang(hip, 0.44f, thigh);
            const Point ankle = Hang(kneePoint, 0.44f, thigh - knee);
            const Point toe{ankle.right, std::max(0.0f, ankle.up - 0.05f), ankle.forward + 0.19f};
            add(hip, kneePoint, 0.068f);
            add(kneePoint, ankle, 0.050f);
            add(ankle, toe, 0.040f);
            // the arm on this side swings with the other leg
            const float arm = -0.75f * reach * std::sin(phase);
            const float elbowBend = 0.18f + 0.22f * swing;
            // (hanging a hand's breadth clear of the waist, so the light shows between them)
            const Point shoulder{side * 0.215f, 1.39f + raise, 0.0f};
            const Point elbow = Hang(Point{side * 0.235f, shoulder.up, shoulder.forward}, 0.29f, arm);
            const Point hand = Hang(Point{side * 0.245f, elbow.up, elbow.forward}, 0.30f, arm + elbowBend);
            add(shoulder, elbow, 0.048f);
            add(elbow, hand, 0.040f);
        }
        add({-0.09f, 0.93f + raise, 0.0f}, {0.09f, 0.93f + raise, 0.0f}, 0.090f);     // hips
        add({-0.05f, 1.00f + raise, 0.0f}, {-0.07f, 1.33f + raise, 0.0f}, 0.110f);    // the torso, narrower
        add({0.05f, 1.00f + raise, 0.0f}, {0.07f, 1.33f + raise, 0.0f}, 0.110f);      // at the waist
        add({-0.04f, 1.43f + raise, 0.0f}, {-0.18f, 1.375f + raise, 0.0f}, 0.060f);   // the shoulders,
        add({0.04f, 1.43f + raise, 0.0f}, {0.18f, 1.375f + raise, 0.0f}, 0.060f);     // sloping
        add({0.0f, 1.44f + raise, 0.0f}, {0.0f, 1.54f + raise, 0.0f}, 0.050f);        // neck
        add({0.0f, 1.60f + raise, 0.01f}, {0.0f, 1.66f + raise, 0.01f}, 0.095f);      // head
        return Lay(limbs, 0.0f, 0.0f, 0.0f, toSunX, toSunY, toSunZ, 1.75f + raise);
    }

    std::array<Limb, CapsuleCount> ResidentLimbs(const ResidentState& resident) noexcept
    {
        // The resident's own skeleton (ResidentRenderer builds the body on it): the solved legs
        // and arms, the pelvis, and the neck and head above the pelvis as the renderer sets them.
        const ResidentPose& pose = resident.pose;
        const float rx = std::cos(pose.heading), rz = -std::sin(pose.heading);
        const float pelvis = pose.pelvisY;
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const ResidentPoint& a, const ResidentPoint& b, const float radius) {
            limbs[next++] = {a.x, a.y, a.z, b.x, b.y, b.z, radius};
        };
        const auto offset = [&](const float side, const float up) {
            return ResidentPoint{resident.x + rx * side, pelvis + up, resident.z + rz * side};
        };
        for (std::size_t s = 0; s < 2; ++s) {
            const ResidentLegPose& leg = pose.legs[s];
            const ResidentFootFrame& foot = leg.foot;
            // the sandal from heel to toe, as broad as the drawn foot
            const ResidentPoint heel{foot.centre.x - foot.forward.x * 0.09f, foot.centre.y + 0.03f,
                                     foot.centre.z - foot.forward.z * 0.09f};
            const ResidentPoint toe{foot.centre.x + foot.forward.x * 0.13f, foot.centre.y + 0.03f,
                                    foot.centre.z + foot.forward.z * 0.13f};
            add(leg.hip, leg.knee, 0.068f);
            add(leg.knee, leg.ankle, 0.050f);
            add(heel, toe, 0.055f);
            // the forearm carried on past the wrist to the fingertips
            const ResidentPoint& elbow = pose.elbows[s];
            const ResidentPoint& wrist = pose.wrists[s];
            const ResidentPoint fingers{wrist.x + (wrist.x - elbow.x) * 0.35f, wrist.y + (wrist.y - elbow.y) * 0.35f,
                                        wrist.z + (wrist.z - elbow.z) * 0.35f};
            add(pose.shoulders[s], elbow, 0.048f);
            add(elbow, fingers, 0.040f);
        }
        add(offset(-0.09f, 0.0f), offset(0.09f, 0.0f), 0.095f);                      // hips
        add(offset(-0.05f, 0.07f), offset(-0.07f, 0.40f), 0.110f);                   // the torso
        add(offset(0.05f, 0.07f), offset(0.07f, 0.40f), 0.110f);
        add(offset(-0.04f, 0.50f), pose.shoulders[0], 0.060f);                       // the shoulders
        add(offset(0.04f, 0.50f), pose.shoulders[1], 0.060f);
        add(offset(0.0f, 0.48f), offset(0.0f, 0.60f), 0.055f);                       // neck
        add(offset(0.0f, 0.67f), offset(0.0f, 0.73f), 0.100f);                       // head
        return limbs;
    }

    float SignedDistance(const Shape& shape, const float offsetX, const float offsetZ) noexcept
    {
        const float qx = (offsetX * shape.alongX + offsetZ * shape.alongZ) * shape.sunSine;
        const float qy = -offsetX * shape.alongZ + offsetZ * shape.alongX;
        float distance = 1000.0f;
        for (const Capsule& c : shape.capsules) {
            const float abx = c.bx - c.ax, aby = c.by - c.ay;
            const float t = std::clamp(((qx - c.ax) * abx + (qy - c.ay) * aby) / std::max(abx * abx + aby * aby, 1e-8f),
                                       0.0f, 1.0f);
            const float part = std::hypot(qx - c.ax - abx * t, qy - c.ay - aby * t) - c.radius;
            // a smooth union: the joints fill out like flesh instead of pinching where two parts meet
            const float blend = std::max(JointBlend - std::abs(distance - part), 0.0f) / JointBlend;
            distance = std::min(distance, part) - blend * blend * JointBlend * 0.25f;
        }
        return distance;
    }
}
