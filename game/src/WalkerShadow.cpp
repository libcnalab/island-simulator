#include "IslandSimulator/WalkerShadow.hpp"
#include "IslandSimulator/BearMotion.hpp"
#include "IslandSimulator/ElephantMotion.hpp"
#include "IslandSimulator/BeetlePopulation.hpp"
#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/ReefHeronGroup.hpp"
#include "IslandSimulator/BoarGroundPose.hpp"
#include "IslandSimulator/BoarHerd.hpp"
#include "IslandSimulator/BoarPose.hpp"
#include "IslandSimulator/CoconutCrabGroup.hpp"
#include "IslandSimulator/CrabColony.hpp"
#include "IslandSimulator/CrocodileGroup.hpp"
#include "IslandSimulator/FrogPopulation.hpp"
#include "IslandSimulator/HermitCrabGroup.hpp"
#include "IslandSimulator/RodentPopulation.hpp"
#include "IslandSimulator/SkinkPopulation.hpp"
#include "IslandSimulator/DeerHerd.hpp"
#include "IslandSimulator/GoatHerd.hpp"
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

    std::array<Limb, CapsuleCount> DeerLimbs(const DeerState& deer) noexcept
    {
        // The joints as DeerRenderer sets them (the body's centre, the neck and head by the
        // graze and the toss, the four legs by the gait, the bed and the stamp), the parts'
        // radii their mean girth: the barrel, then the legs in three, the neck, the head to the
        // nose, and the two antler beams of the stag or a doe's two long ears.
        struct V
        {
            float x, y, z;
            V operator+(const V& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
            V operator-(const V& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
            V operator*(const float k) const noexcept { return {x * k, y * k, z * k}; }
        };
        const auto lerp = [](const V& a, const V& b, const float t) { return a + (b - a) * t; };
        const float s = deer.scale;
        const V forward{std::cos(deer.headingRadians), 0.0f, std::sin(deer.headingRadians)};
        const V right{-forward.z, 0.0f, forward.x}, up{0.0f, 1.0f, 0.0f};
        const float bed = std::clamp(deer.bedAmount, 0.0f, 1.0f);
        const V body{deer.x, deer.y + (1.03f - 0.56f * bed) * s, deer.z};
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const V& a, const V& b, const float radius) {
            limbs[next++] = {a.x, a.y, a.z, b.x, b.y, b.z, radius};
        };
        add(body - forward * (0.42f * s), body + forward * (0.36f * s) + up * (0.05f * s), 0.32f * s);   // the barrel
        for (int leg = 0; leg < 4; ++leg) {
            const float fore = leg < 2 ? 0.48f : -0.47f;
            const float sideSign = leg % 2 == 0 ? -1.0f : 1.0f;
            const float phase = deer.gaitPhase + ((leg == 0 || leg == 3) ? 0.0f : std::numbers::pi_v<float>);
            const float swing = std::sin(phase) * std::min(1.0f, deer.speed / 0.8f);
            const bool hind = leg >= 2;
            const V hip = body + forward * (fore * s) + right * (sideSign * 0.255f * s) - up * (0.23f * s);
            const V standingKnee = hip + forward * (((hind ? 0.16f : -0.07f) + swing * 0.13f) * s) - up * (0.37f * s)
                + right * (sideSign * 0.018f * s);
            const V standingHock = standingKnee + forward * (((hind ? -0.22f : 0.08f) - swing * 0.15f) * s) - up * (0.34f * s);
            const V standingHoof = standingHock + forward * (((hind ? 0.18f : 0.04f) - swing * 0.08f) * s);
            const V foldedKnee = hip + forward * ((hind ? 0.22f : 0.10f) * s) - up * ((hind ? 0.12f : 0.14f) * s)
                + right * (sideSign * 0.05f * s);
            const V foldedHock = foldedKnee - forward * ((hind ? 0.28f : 0.30f) * s) - up * ((hind ? 0.05f : 0.03f) * s);
            const V foldedHoof = foldedHock + forward * ((hind ? 0.30f : -0.05f) * s);
            const float lift = (leg == 0 ? std::clamp(deer.stamp, 0.0f, 1.0f) : 0.0f) * 0.14f * s;
            const V knee = lerp(standingKnee, foldedKnee, bed) + up * (lift * 0.35f) + forward * (lift * 0.2f);
            const V hock = lerp(standingHock, foldedHock, bed) + up * (lift * 0.8f) - forward * (lift * 0.3f);
            V hoof = lerp(standingHoof, foldedHoof, bed) - forward * (lift * 0.2f);
            hoof.y = deer.y + 0.060f * s + lift;
            add(hip, knee, (hind ? 0.095f : 0.085f) * s);
            add(knee, hock, 0.035f * s);
            add(hock, hoof, 0.028f * s);
        }
        const float graze = deer.grazeAmount;
        const float toss = std::clamp(deer.headToss, 0.0f, 1.0f);
        const V neckBase = body + forward * (0.58f * s) + up * (0.24f * s);
        const V neckTop = neckBase + forward * (0.27f * s) + up * ((0.50f - 0.62f * graze) * s);
        const V head = neckTop + forward * ((0.22f + 0.035f * toss) * s) + up * ((0.045f - 0.105f * graze + 0.085f * toss) * s);
        const V nose = head + forward * (0.415f * s) - up * (0.065f * s);
        add(body + forward * (0.48f * s) + up * (0.17f * s), neckTop, 0.17f * s);
        add(head - forward * (0.10f * s), nose, 0.105f * s);
        for (const float sideSign : {-1.0f, 1.0f}) {
            if (deer.antlered) {
                const V base = head - forward * (0.12f * s) + right * (sideSign * 0.075f * s) + up * (0.18f * s);
                const V crown = base + forward * (0.107f * s) + right * (sideSign * 0.11f * s) + up * (0.58f * s);
                add(base, crown, 0.030f * s);
            } else {
                const V root = head - forward * (0.07f * s) + right * (sideSign * 0.105f * s) + up * (0.13f * s);
                add(root, root - forward * (0.075f * s) + right * (sideSign * 0.16f * s) + up * (0.23f * s), 0.040f * s);
            }
        }
        return limbs;
    }

    std::array<Limb, CapsuleCount> GoatLimbs(const GoatState& goat) noexcept
    {
        // GoatMesh's body frame: the barrel and neck ride the gait's bob, the head (with its
        // horns, ears and beard) pitches down by the graze about the neck, and the legs are the
        // planted-foot chain the pose carries (lying, folded under the lowered body).
        const GoatGroundPose& pose = goat.pose;
        const float bob = pose.bodyBob;
        const float graze = std::clamp(goat.grazingPose, 0.0f, 1.0f);
        const float pitch = graze * 0.85f, lower = graze * 0.18f;
        const float c = std::cos(pitch), s = std::sin(pitch);
        const auto head = [&](const GoatPoint p) {
            const float y = p.y - 0.72f, z = p.z - 0.25f;
            return GoatPoint{p.x, 0.72f + c * y - s * z - lower + bob, 0.25f + s * y + c * z};
        };
        const auto body = [&](const GoatPoint p) { return GoatPoint{p.x, p.y + bob, p.z}; };
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const GoatPoint a, const GoatPoint b, const float radius) {
            const GoatPoint wa = pose.ToWorld(a), wb = pose.ToWorld(b);
            limbs[next++] = {wa.x, wa.y, wa.z, wb.x, wb.y, wb.z, radius * pose.scale};
        };
        add(body({0.0f, 0.845f, -0.42f}), body({0.0f, 0.865f, 0.40f}), 0.26f);   // the barrel
        for (std::size_t leg = 0; leg < pose.legs.size(); ++leg) {
            const bool front = leg % 2 == 1;
            add(pose.legs[leg].root, pose.legs[leg].knee, front ? 0.085f : 0.10f);
            add(pose.legs[leg].knee, pose.legs[leg].hoof, 0.045f);
        }
        add(body({0.0f, 0.87f, 0.22f}), head({0.0f, 1.07f, 0.45f}), 0.12f);       // neck
        add(head({0.0f, 1.08f, 0.42f}), head({0.0f, 0.87f, 0.74f}), 0.095f);      // head to the nose
        for (const float side : {-1.0f, 1.0f}) {
            add(head({side * 0.062f, 1.18f, 0.42f}), head({side * 0.13f, 1.46f, 0.10f}), 0.025f);   // horn
            add(head({side * 0.10f, 1.095f, 0.44f}), head({side * 0.32f, 1.05f, 0.42f}), 0.030f);  // ear
        }
        add(head({0.0f, 0.83f, 0.68f}), head({0.0f, 0.70f, 0.66f}), 0.035f);      // beard
        add(body({0.0f, 0.88f, -0.52f}), body({0.0f, 0.94f, -0.61f}), 0.040f);    // tail
        return limbs;
    }

    std::array<Limb, CapsuleCount> BoarLimbs(const BoarState& boar, const BoarGroundPose& pose) noexcept
    {
        // BoarRenderer's frame: the rump and the high shoulder mantle, the cheek and the long
        // snout lowered by the rooting dip, the fitted legs, the ears and the tail.
        const float bob = pose.bodyBob;
        const float dip = std::clamp(boar.rootingPose, 0.0f, 1.0f) * 0.15f;
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const BoarJoint a, const BoarJoint b, const float radius) {
            const BoarJoint wa = pose.ToWorld(a), wb = pose.ToWorld(b);
            limbs[next++] = {wa.x, wa.y, wa.z, wb.x, wb.y, wb.z, radius * pose.scale};
        };
        add({0.0f, 0.66f + bob, -0.45f}, {0.0f, 0.70f + bob, -0.05f}, 0.28f);    // rump
        add({0.0f, 0.70f + bob, -0.05f}, {0.0f, 0.76f + bob, 0.30f}, 0.33f);     // shoulder mantle
        add({0.0f, 0.68f - 0.3f * dip + bob, 0.47f}, {0.0f, 0.55f - 0.8f * dip + bob, BoarMuzzleZ(0.78f)}, 0.24f);   // cheek
        add({0.0f, 0.55f - 0.8f * dip + bob, BoarMuzzleZ(0.78f)}, {0.0f, 0.435f - dip + bob, BoarMuzzleZ(1.04f)}, 0.11f);
        for (std::size_t leg = 0; leg < pose.legs.size(); ++leg) {
            const auto& chain = pose.legs[leg];
            add(chain.root, chain.knee, leg % 2 == 1 ? 0.11f : 0.115f);
            add(chain.knee, chain.hoof, 0.045f);
        }
        for (const float side : {-1.0f, 1.0f})
            add({side * 0.17f, 0.83f - 0.5f * dip + bob, 0.54f}, {side * 0.24f, 1.08f - 0.5f * dip + bob, 0.565f}, 0.045f);
        add({0.0f, 0.66f + bob, -0.70f}, {0.0f, 0.58f + bob, -0.80f}, 0.03f);   // tail
        for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
        return limbs;
    }

    std::array<Limb, CapsuleCount> BearLimbs(const BearState& bear) noexcept
    {
        // BearRenderer's frame: the trunk from the rump to the shoulder hump, the neck, the
        // head lowered and pushed forward by foraging and pitched nose-up by the sniff, the
        // muzzle, the ears, the stub tail, and the four fitted legs (folded when lying).
        const BearPose& pose = bear.pose;
        const float bob = pose.bodyBob;
        const float forage = std::clamp(bear.foragePose, 0.0f, 1.0f);
        const float sniff = std::clamp(bear.sniff, 0.0f, 1.0f) * 0.30f;
        const BearPoint head{0.0f, 1.30f - forage * 0.48f, 1.15f + forage * 0.18f};
        const float fy = std::sin(sniff), fz = std::cos(sniff);   // the head's forward, pitched up
        const auto onHead = [&](const float forward, const float up, const float side) {
            return BearPoint{side, head.y + forward * fy + up * fz, head.z + forward * fz - up * fy};
        };
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const BearPoint a, const BearPoint b, const float radius) {
            const BearPoint wa = pose.ToWorld(a), wb = pose.ToWorld(b);
            limbs[next++] = {wa.x, wa.y, wa.z, wb.x, wb.y, wb.z, radius};
        };
        add({0.0f, 1.04f + bob, -0.62f}, {0.0f, 1.07f + bob, -0.05f}, 0.53f);                 // rump and belly
        add({0.0f, 1.07f + bob, -0.05f}, {0.0f, 1.30f + bob, 0.42f}, 0.53f);                  // the shoulder hump
        add({0.0f, 1.28f + bob - forage * 0.10f, 0.55f}, head, 0.40f);                       // neck
        add(onHead(-0.05f, 0.0f, 0.0f), onHead(0.20f, -0.02f, 0.0f), 0.36f);                 // the skull
        add(onHead(0.30f, -0.05f, 0.0f), onHead(0.62f, -0.08f, 0.0f), 0.15f);                // muzzle to nose
        for (const float side : {-1.0f, 1.0f}) {
            const BearPoint ear = onHead(-0.045f, 0.285f, side * 0.282f);
            add(ear, ear, 0.11f);
        }
        add({0.0f, 1.10f + bob, -1.08f}, {0.0f, 1.10f + bob, -1.08f}, 0.13f);               // stub tail
        for (const auto& leg : pose.legs) {
            add(leg.root, leg.joint, 0.17f);
            add(leg.joint, BearPoint{leg.paw.x, leg.paw.y, leg.paw.z + 0.06f}, 0.12f);
        }
        for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
        return limbs;
    }

    std::array<Limb, CapsuleCount> ElephantLimbs(const ElephantMotion& elephant) noexcept
    {
        // ElephantRenderer's shapes: the deep barrel from the rump to the withers, the head,
        // the ears across it, the trunk in two along its curve, the tusks, the tail and the legs
        // from the solved joints.
        const ElephantState& s = elephant.State();
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const ElephantPoint& a, const ElephantPoint& b, const float radius) {
            limbs[next++] = {a.x, a.y, a.z, b.x, b.y, b.z, radius};
        };
        add(elephant.ToWorld(0.0f, -0.10f, -1.55f), elephant.ToWorld(0.0f, -0.08f, 0.0f), 0.82f);
        add(elephant.ToWorld(0.0f, -0.08f, 0.0f), elephant.ToWorld(0.0f, 0.0f, 1.55f), 0.80f);
        const ElephantMotion::HeadFrame h = elephant.Head();
        const auto head = [&](const float x, const float y, const float z) {
            return ElephantPoint{h.pivot.x + h.right.x * x + h.up.x * y + h.ahead.x * z,
                                 h.pivot.y + h.right.y * x + h.up.y * y + h.ahead.y * z,
                                 h.pivot.z + h.right.z * x + h.up.z * y + h.ahead.z * z};
        };
        add(head(0.0f, -0.04f, 0.05f), head(0.0f, -0.12f, 0.75f), 0.50f);
        add(head(-0.55f, -0.05f, 0.05f), head(0.55f, -0.05f, 0.05f), 0.32f);   // the ears
        const auto trunk = elephant.TrunkPoints();
        add(trunk[0], trunk[7], 0.19f);
        add(trunk[7], trunk[15], 0.11f);
        for (const float side : {-1.0f, 1.0f}) add(head(side * 0.20f, -0.50f, 0.90f), head(side * 0.30f, -0.82f, 1.80f), 0.06f);
        add(elephant.ToWorld(s.tailSwing * 0.3f, 0.30f, -2.30f), elephant.ToWorld(s.tailSwing * 0.8f, -0.80f, -2.40f), 0.05f);
        for (const auto& leg : s.legs) {
            add(leg.top, leg.middle, 0.30f);
            add(leg.middle, ElephantPoint{leg.foot.x, leg.foot.y + 0.12f, leg.foot.z}, 0.22f);
        }
        for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
        return limbs;
    }

    std::array<Limb, CapsuleCount> CrocodileLimbs(const CrocodileState& crocodile) noexcept
    {
        // CrocodileRenderer's frame: the low wide barrel as two capsules side by side, the long
        // skull and snout, the tail in three swinging with the body's wave, and the four
        // splayed legs (folded back along the flanks for the swim).
        const float s = crocodile.scale;
        const float fx = std::cos(crocodile.headingRadians), fz = std::sin(crocodile.headingRadians);
        const float rx = -fz, rz = fx;
        const float cy = crocodile.y - 0.04f * crocodile.submerged;
        const auto local = [&](const float x, const float y, const float z) {
            return std::array<float, 3>{crocodile.x + (rx * x + fx * z) * s, cy + y * s, crocodile.z + (rz * x + fz * z) * s};
        };
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const std::array<float, 3>& a, const std::array<float, 3>& b, const float radius) {
            limbs[next++] = {a[0], a[1], a[2], b[0], b[1], b[2], radius * s};
        };
        for (const float side : {-0.25f, 0.25f})
            add(local(side, 0.34f, -1.0f), local(side * 0.8f, 0.35f, 0.95f), 0.35f);           // the barrel
        add(local(0.0f, 0.37f, 0.95f), local(0.0f, 0.30f, 2.15f), 0.21f);                     // skull to the snout's tip
        for (const float side : {-0.20f, 0.20f})
            add(local(side, 0.36f, 1.0f), local(side * 0.6f, 0.33f, 1.55f), 0.16f);            // the broad jaw
        const auto tailAt = [&](const float t) {
            const float wave = std::sin(t * 4.2f + crocodile.gaitPhase) * crocodile.bodyWave * t * t * 0.55f;
            return local(wave, 0.22f - 0.17f * t, -1.04f - 2.66f * t);
        };
        add(local(0.0f, 0.27f, -1.12f), tailAt(0.33f), 0.24f);   // (round: as wide as the flat tail is high)
        add(tailAt(0.33f), tailAt(0.66f), 0.16f);
        add(tailAt(0.66f), tailAt(1.0f), 0.08f);
        const float fold = std::clamp(crocodile.legFold, 0.0f, 1.0f);
        for (int pair = 0; pair < 2; ++pair) {
            const float rootZ = pair == 0 ? 0.64f : -0.66f;
            for (int sideIndex = -1; sideIndex <= 1; sideIndex += 2) {
                const float side = static_cast<float>(sideIndex);
                const float stride = std::sin(crocodile.gaitPhase + (pair * 2 + (sideIndex > 0 ? 1 : 0)) * std::numbers::pi_v<float>)
                    * 0.13f * (1.0f - crocodile.submerged);
                const auto shoulder = local(side * 0.52f, 0.34f, rootZ);
                const auto elbow = local(side * std::lerp(0.83f, 0.62f, fold), std::lerp(0.18f, 0.22f, fold), rootZ + stride - 0.28f * fold);
                const auto foot = local(side * std::lerp(0.78f, 0.56f, fold), std::lerp(0.025f, 0.14f, fold) + 0.018f,
                                        rootZ + 0.24f + stride - 0.62f * fold + 0.20f);
                add(shoulder, elbow, 0.15f);
                add(elbow, foot, 0.10f);
            }
        }
        for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
        return limbs;
    }

    namespace
    {
        // A crab in its own frame (x right, y up, z forward, unscaled metres) laid out in the world.
        struct CrabFrame
        {
            float x, y, z, fx, fz, scale;
            std::array<Limb, CapsuleCount> limbs{};
            std::size_t next = 0;
            void Add(const float ax, const float ay, const float az, const float bx, const float by, const float bz,
                     const float radius) noexcept
            {
                const float rx = -fz, rz = fx;
                limbs[next++] = {x + (rx * ax + fx * az) * scale, y + ay * scale, z + (rz * ax + fz * az) * scale,
                                 x + (rx * bx + fx * bz) * scale, y + by * scale, z + (rz * bx + fz * bz) * scale,
                                 radius * scale};
            }
            // Four legs a side splayed out to the sand, stepping with the gait.
            void Legs(const float rootX, const float rootY, const float footX, const std::array<float, 4>& rows,
                      const float spread, const float radius, const float phase, const float stride) noexcept
            {
                for (const float side : {-1.0f, 1.0f})
                    for (std::size_t row = 0; row < rows.size(); ++row) {
                        const float step = stride * std::sin(phase + static_cast<float>(row) * 1.6f + (side > 0.0f ? 3.1f : 0.0f));
                        Add(side * rootX, rootY, rows[row], side * footX, 0.0f, rows[row] * spread + step, radius);
                    }
            }
            std::array<Limb, CapsuleCount> Finish() noexcept
            {
                for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
                return limbs;
            }
        };
    }

    std::array<Limb, CapsuleCount> GhostCrabLimbs(const CrabState& crab) noexcept
    {
        // CrabRenderer's beach crab: the near-square carapace (0.84 x 0.62 x 0.13 unscaled) as a
        // cross of two capsules, eight splayed legs and the two claws ahead.
        CrabFrame f{crab.x, crab.y, crab.z, std::cos(crab.bodyYawRadians), std::sin(crab.bodyYawRadians), crab.scale};
        f.Add(-0.27f, 0.08f, 0.0f, 0.27f, 0.08f, 0.0f, 0.13f);
        f.Add(0.0f, 0.08f, -0.14f, 0.0f, 0.08f, 0.12f, 0.14f);
        f.Legs(0.36f, 0.08f, 0.80f, {0.15f, 0.05f, -0.05f, -0.15f}, 1.7f, 0.035f, crab.legPhase, 0.05f * std::min(1.0f, crab.speed * 4.0f));
        for (const float side : {-1.0f, 1.0f}) f.Add(side * 0.22f, 0.07f, 0.26f, side * 0.34f, 0.05f, 0.42f, 0.06f);
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> HermitCrabLimbs(const HermitCrabState& crab) noexcept
    {
        // The coiled shell leaning back from the aperture, the walking legs and the claws ahead of
        // it (drawn back into the shell as the crab withdraws).
        const float withdraw = std::clamp(crab.withdrawAmount, 0.0f, 1.0f);
        const float carry = std::clamp(crab.shellCarry, 0.0f, 1.0f);
        CrabFrame f{crab.x, crab.y, crab.z, std::cos(crab.headingRadians), std::sin(crab.headingRadians), crab.scale};
        f.Add(0.0f, 0.10f + 0.03f * carry, -0.01f, 0.0f, 0.29f + 0.03f * carry, -0.14f, 0.12f);
        f.Add(0.0f, 0.11f + 0.03f * carry, 0.05f, 0.0f, 0.10f, -0.06f, 0.10f);
        const float reach = 1.0f - 0.7f * withdraw;
        f.Legs(0.05f, 0.06f, 0.05f + 0.15f * reach, {0.09f, 0.05f, 0.01f, -0.03f}, 1.3f, 0.022f, crab.legPhase,
               0.03f * std::min(1.0f, crab.speed * 6.0f));
        for (const float side : {-1.0f, 1.0f})
            f.Add(side * 0.035f, 0.07f, 0.12f * reach, side * 0.055f, 0.04f, 0.12f + 0.10f * reach, 0.035f);
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> CoconutCrabLimbs(const CoconutCrabState& crab) noexcept
    {
        // The long cephalothorax and the abdomen curled under its back, eight long legs and
        // the heavy claws.
        CrabFrame f{crab.x, crab.y, crab.z, std::cos(crab.headingRadians), std::sin(crab.headingRadians), crab.scale};
        for (const float side : {-0.12f, 0.12f}) f.Add(side, 0.245f, -0.28f, side, 0.245f, 0.32f, 0.17f);
        f.Add(0.0f, 0.20f, -0.30f, 0.0f, 0.12f, -0.55f, 0.14f);
        f.Legs(0.25f, 0.22f, 0.62f, {0.20f, 0.06f, -0.08f, -0.22f}, 1.3f, 0.045f, crab.legPhase,
               0.06f * std::min(1.0f, crab.speed * 3.0f));
        for (const float side : {-1.0f, 1.0f}) f.Add(side * 0.15f, 0.20f, 0.30f, side * 0.22f, 0.12f, 0.62f, 0.08f);
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> SkinkLimbs(const SkinkState& skink) noexcept
    {
        // The skink's low trunk (flattened and spread as it basks), the head, the long tapering
        // tail and the four splayed legs, in ButterflyRenderer's proportions.
        CrabFrame f{skink.x, skink.y, skink.z, std::cos(skink.headingRadians), std::sin(skink.headingRadians), skink.scale};
        const float flat = std::clamp(skink.flatten, 0.0f, 1.0f);
        const float lift = 0.06f * (1.0f - 0.17f * flat), girth = 0.065f * (1.0f + 0.1f * flat);
        f.Add(0.0f, lift, -0.28f, 0.0f, lift + 0.01f * std::clamp(skink.pushUp, 0.0f, 1.0f), 0.30f, girth);
        f.Add(0.0f, lift, 0.40f, 0.0f, lift, 0.46f, 0.045f);
        f.Add(0.0f, 0.045f, -0.30f, 0.0f, 0.02f, -0.62f, 0.035f);
        f.Add(0.0f, 0.02f, -0.62f, 0.0f, 0.01f, -0.95f, 0.015f);
        const float stride = 0.04f * std::min(1.0f, skink.speed * 6.0f);
        for (const float side : {-1.0f, 1.0f})
            for (const float row : {0.17f, -0.17f}) {
                const float step = stride * std::sin(skink.gaitPhase + (side * row > 0.0f ? 0.0f : 3.14159f));
                f.Add(side * 0.07f, 0.045f, row, side * 0.17f, 0.0f, row + 0.05f + step, 0.016f);
            }
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> FrogLimbs(const FrogState& frog) noexcept
    {
        // The squat body and broad head, the forelegs propped ahead and the long hind legs
        // folded at the haunches.
        CrabFrame f{frog.x, frog.y, frog.z, std::cos(frog.headingRadians), std::sin(frog.headingRadians), frog.scale};
        f.Add(0.0f, 0.095f, -0.11f, 0.0f, 0.105f, 0.07f, 0.085f);
        f.Add(0.0f, 0.10f, 0.13f, 0.0f, 0.095f, 0.17f, 0.065f);
        for (const float side : {-1.0f, 1.0f}) {
            f.Add(side * 0.06f, 0.06f, 0.08f, side * 0.09f, 0.0f, 0.15f, 0.02f);
            f.Add(side * 0.08f, 0.07f, -0.08f, side * 0.17f, 0.02f, -0.02f, 0.035f);
            f.Add(side * 0.17f, 0.02f, -0.02f, side * 0.13f, 0.0f, -0.16f, 0.022f);
        }
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> RodentLimbs(const RodentState& rodent) noexcept
    {
        // A rat (a mouse two-thirds its size): the body, the pointed head lifted as it sniffs
        // or grooms, the long thin tail on the ground, and the short legs.
        const bool rat = rodent.species == RodentSpecies::Rat;
        const float unit = (rat ? 1.0f : 0.67f) * std::clamp(rodent.visibility, 0.0f, 1.0f);
        CrabFrame f{rodent.x, rodent.y, rodent.z, std::cos(rodent.headingRadians), std::sin(rodent.headingRadians), unit};
        const float body = rat ? 0.095f : 0.072f, groom = std::clamp(rodent.groom, 0.0f, 1.0f);
        f.Add(0.0f, body, -0.12f, 0.0f, body + 0.01f, 0.12f, 0.062f);
        f.Add(0.0f, body + 0.02f + 0.075f * groom, 0.18f - 0.055f * groom, 0.0f, body + 0.01f + 0.075f * groom,
              0.29f - 0.055f * groom, 0.035f);
        f.Add(0.0f, 0.03f, -0.15f, 0.0f, 0.01f, -0.40f, 0.012f);
        for (const float side : {-1.0f, 1.0f})
            for (const float row : {0.09f, -0.09f}) f.Add(side * 0.04f, 0.05f, row, side * 0.05f, 0.0f, row + 0.02f, 0.013f);
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> BeetleLimbs(const BeetleState& beetle) noexcept
    {
        // ButterflyRenderer's beetle on level ground (in its own units, a twentieth of its scale;
        // the feet stand a tenth of a unit under the body's centre): the domed wing cases side
        // by side, the pronotum and the head ahead, and the six legs splayed out to the ground.
        const float unit = 0.05f * beetle.scale, lift = 0.105f;
        CrabFrame f{beetle.x, beetle.y - lift * unit, beetle.z, std::cos(beetle.headingRadians),
                    std::sin(beetle.headingRadians), unit};
        for (const float side : {-0.15f, 0.15f}) f.Add(side, 0.45f + lift, -0.80f, side, 0.45f + lift, 0.35f, 0.30f);
        f.Add(0.0f, 0.30f + lift, 0.63f, 0.0f, 0.30f + lift, 0.63f, 0.28f);
        f.Add(0.0f, 0.26f + lift, 1.10f, 0.0f, 0.24f + lift, 1.20f, 0.20f);
        const float gait = std::sin(beetle.gaitPhase);
        for (const float side : {-1.0f, 1.0f})
            for (int leg = 0; leg < 3; ++leg) {
                const float along = 0.48f - 0.52f * static_cast<float>(leg);
                const float swing = gait * (leg % 2 == 0 ? 1.0f : -1.0f) * 0.13f;
                f.Add(side * 0.38f, 0.18f + lift, along, side * 1.18f, 0.0f, along + 0.23f - 0.23f * static_cast<float>(leg) + swing,
                      0.05f);
            }
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> GroundBirdLimbs(const BirdState& bird) noexcept
    {
        // BirdRenderer's dove on the ground (the flock keeps its body 0.20 of its scale over the
        // ground, the stance lifts it 0.075 more): the plump body stood on its legs, breast
        // raised, the round head ahead with the slender bill, the tail behind, the legs to the feet.
        CrabFrame f{bird.x, bird.y - GroundBirdBodyHeight * bird.scale, bird.z, std::cos(bird.headingRadians),
                    std::sin(bird.headingRadians), bird.scale};
        const float bob = bird.headBob * 0.055f;
        f.Add(0.0f, 0.24f, -0.25f, 0.0f, 0.31f, 0.25f, 0.15f);                 // the body
        f.Add(0.0f, 0.36f, 0.40f + bob, 0.0f, 0.36f, 0.40f + bob, 0.11f);      // the head
        f.Add(0.0f, 0.35f, 0.48f + bob, 0.0f, 0.33f, 0.64f + bob, 0.02f);      // the bill
        f.Add(0.0f, 0.26f, -0.30f, 0.0f, 0.25f, -0.62f, 0.08f);               // the tail
        for (const float side : {-1.0f, 1.0f}) f.Add(side * 0.05f, 0.14f, 0.02f, side * 0.05f, 0.0f, 0.05f, 0.02f);
        return f.Finish();
    }

    std::array<Limb, CapsuleCount> HeronLimbs(const ReefHeronState& heron) noexcept
    {
        // ReefHeronRenderer's standing heron: the slim body tilted up at the breast, the long
        // neck curving up to the head (craned or drawn in, thrust out in the strike), the dagger
        // bill, the wingtips over the tail, and the two long legs wading in step.
        struct V
        {
            float x, y, z;
            V operator+(const V& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
            V operator-(const V& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
            V operator*(const float k) const noexcept { return {x * k, y * k, z * k}; }
        };
        const float s = heron.scale;
        const float rest = std::clamp(heron.rest, 0.0f, 1.0f), neck = std::clamp(heron.neckStretch, 0.0f, 1.0f);
        const float strike = std::clamp(heron.strike, 0.0f, 1.0f);
        const V forward{std::cos(heron.headingRadians), 0.0f, std::sin(heron.headingRadians)};
        const V right{-forward.z, 0.0f, forward.x}, up{0.0f, 1.0f, 0.0f};
        const V feet{heron.x, heron.y, heron.z};
        const float tilt = 0.38f - 0.22f * rest;
        const V axis = forward * std::cos(tilt) + up * std::sin(tilt);
        const V hip = feet + up * ((0.29f - 0.04f * rest) * s);
        const V body = hip + up * (0.07f * s);
        const V neckBase = body + axis * (0.13f * s) + up * (0.025f * s);
        const V head = body + axis * (0.16f * s) + up * (0.12f * s) + (forward * (0.16f * s) + up * (0.13f * s)) * neck
            + (forward * (0.22f * s) - up * (0.30f * s)) * strike;
        const V bend = (neckBase + head) * 0.5f + up * ((0.04f + 0.07f * neck) * s) - forward * (0.04f * neck * s);
        const float pitch = 0.30f + 0.28f * neck + 0.5f * strike;
        const V bill = forward * std::cos(pitch) - up * std::sin(pitch);
        std::array<Limb, CapsuleCount> limbs{};
        std::size_t next = 0;
        const auto add = [&](const V& a, const V& b, const float radius) {
            limbs[next++] = {a.x, a.y, a.z, b.x, b.y, b.z, radius * s};
        };
        add(body - axis * (0.13f * s), body + axis * (0.13f * s), 0.085f);
        add(neckBase, bend, 0.030f);
        add(bend, head, 0.020f);
        add(head, head, 0.035f);
        add(head + bill * (0.036f * s), head + bill * (0.141f * s), 0.006f);
        add(body - axis * (0.15f * s), body - axis * (0.36f * s), 0.030f);
        for (const float side : {-1.0f, 1.0f}) {
            const float phase = heron.stepPhase + (side > 0.0f ? 0.0f : std::numbers::pi_v<float>);
            const bool stalk = heron.behavior == ReefHeronBehavior::Stalk;
            V foot = feet + right * (side * 0.045f * s) + forward * (std::cos(phase) * (stalk ? 0.06f : 0.0f) * s)
                + up * (stalk ? std::max(0.0f, std::sin(phase)) * 0.05f * s : 0.0f);
            if (side < 0.0f) foot = foot + ((hip - up * (0.10f * s) + forward * (0.02f * s)) - foot) * rest;
            const V top = hip + right * (side * 0.035f * s);
            const V ankle = (top + foot) * 0.5f - forward * (0.035f * s);
            add(top, ankle, 0.011f);
            add(ankle, foot, 0.008f);
        }
        for (; next < limbs.size(); ++next) limbs[next].radius = UnusedRadius;
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
