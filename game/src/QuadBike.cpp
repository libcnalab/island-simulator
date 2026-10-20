#include "IslandSimulator/QuadBike.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island
{
    namespace
    {
        [[nodiscard]] BikeVec Add(const BikeVec& a, const BikeVec& b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
        [[nodiscard]] BikeVec Sub(const BikeVec& a, const BikeVec& b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
        [[nodiscard]] BikeVec Mul(const BikeVec& a, const float s) noexcept { return {a.x * s, a.y * s, a.z * s}; }
        [[nodiscard]] float Dot(const BikeVec& a, const BikeVec& b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }
        [[nodiscard]] BikeVec Cross(const BikeVec& a, const BikeVec& b) noexcept
        {
            return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
        }
        [[nodiscard]] float Length(const BikeVec& a) noexcept { return std::sqrt(Dot(a, a)); }
        [[nodiscard]] BikeVec Normalised(const BikeVec& a, const BikeVec& fallback = {0, 1, 0}) noexcept
        {
            const float length = Length(a);
            return length > 1e-6f ? Mul(a, 1.0f / length) : fallback;
        }
        [[nodiscard]] bool Finite(const BikeVec& a) noexcept { return std::isfinite(a.x) && std::isfinite(a.y) && std::isfinite(a.z); }

        constexpr float Gravity = 9.81f;
        constexpr float DrivePower = 18000.0f;          // watts at the rear wheels
        constexpr float LateralStiffness = 3500.0f;     // N per m/s of side slip, per tyre
        constexpr float RollingResistance = 0.03f;
        constexpr float EngineBraking = 110.0f;          // N per tyre off the throttle
        constexpr float ReverseSpeed = 4.0f;
        constexpr float DragCoefficient = 0.52f;         // 0.5 rho Cd A
        // Body-frame inertia of the quad and its rider (a 1.8 x 1.1 x 1.0 m box of 260 kg).
        constexpr float InertiaPitch = 92.0f, InertiaYaw = 96.0f, InertiaRoll = 48.0f;
        // The chassis and the rider's head: what meets the ground when the quad tips.
        constexpr std::array<BikeVec, 9> Hull{{
            {-0.56f, -0.12f, 0.92f}, {0.56f, -0.12f, 0.92f}, {-0.56f, -0.12f, -0.92f}, {0.56f, -0.12f, -0.92f},
            {-0.50f, 0.42f, 0.70f}, {0.50f, 0.42f, 0.70f}, {-0.48f, 0.40f, -0.80f}, {0.48f, 0.40f, -0.80f},
            {0.0f, 1.10f, -0.15f},
        }};
        constexpr float HullStiffness = 60000.0f, HullDamping = 3200.0f;
    }

    QuadBike::QuadBike(const BikeVec position, const float yawRadians) noexcept
        : position_(position)
    {
        axisZ_ = {std::sin(yawRadians), 0.0f, std::cos(yawRadians)};
        axisY_ = {0.0f, 1.0f, 0.0f};
        axisX_ = Cross(axisZ_, axisY_);
        previousLength_.fill(SuspensionRest);
    }

    BikeVec QuadBike::WheelMount(const int wheel) noexcept
    {
        const float x = (wheel % 2 == 0 ? -0.5f : 0.5f) * Track;
        const float z = (wheel < 2 ? 0.5f : -0.5f) * WheelBase;
        return {x, MountHeight, z};
    }

    BikeVec QuadBike::ToWorld(const BikeVec local) const noexcept
    {
        return Add(position_, Add(Add(Mul(axisX_, local.x), Mul(axisY_, local.y)), Mul(axisZ_, local.z)));
    }

    float QuadBike::Speed() const noexcept { return Dot(velocity_, axisZ_); }
    float QuadBike::YawRadians() const noexcept { return std::atan2(axisZ_.x, axisZ_.z); }

    void QuadBike::Orthonormalise() noexcept
    {
        axisY_ = Normalised(axisY_);
        axisZ_ = Normalised(Sub(axisZ_, Mul(axisY_, Dot(axisZ_, axisY_))), {0, 0, 1});
        axisX_ = Cross(axisZ_, axisY_);
    }

    void QuadBike::Place(const BikeVec position, const float yawRadians, const GroundQuery& ground) noexcept
    {
        position_ = position;
        axisZ_ = {std::sin(yawRadians), 0.0f, std::cos(yawRadians)};
        axisY_ = {0.0f, 1.0f, 0.0f};
        axisX_ = Cross(axisZ_, axisY_);
        Right(ground);
    }

    void QuadBike::Right(const GroundQuery& ground) noexcept
    {
        // The heading it lies along, upright again, lifted onto its wheels with nothing moving.
        BikeVec heading{axisZ_.x, 0.0f, axisZ_.z};
        if (Length(heading) < 0.2f) heading = {axisY_.x, 0.0f, axisY_.z};
        heading = Normalised(heading, {0, 0, 1});
        axisY_ = {0.0f, 1.0f, 0.0f};
        axisZ_ = heading;
        axisX_ = Cross(axisZ_, axisY_);
        if (!ground) {
            velocity_ = {};
            angular_ = {};
            return;
        }
        // Laid along the ground under its four wheels (a plane through their footprints), the
        // springs at their natural sag: set level on a slope, the uphill springs would fling it.
        std::array<float, 4> heights{};
        for (int wheel = 0; wheel < 4; ++wheel) {
            const BikeVec mount = ToWorld(WheelMount(wheel));
            heights[static_cast<std::size_t>(wheel)] = ground(mount.x, mount.z).height;
        }
        const float alongSlope = ((heights[0] + heights[1]) - (heights[2] + heights[3])) * 0.5f / WheelBase;
        const float acrossSlope = ((heights[1] + heights[3]) - (heights[0] + heights[2])) * 0.5f / Track;
        const BikeVec flatRight = axisX_, flatForward = axisZ_;
        axisY_ = Normalised(Sub(Sub(BikeVec{0.0f, 1.0f, 0.0f}, Mul(flatRight, acrossSlope)), Mul(flatForward, alongSlope)));
        axisZ_ = Normalised(Sub(flatForward, Mul(axisY_, Dot(flatForward, axisY_))), {0, 0, 1});
        axisX_ = Cross(axisZ_, axisY_);
        const float sag = Mass * 9.81f / 4.0f / SpringRate;
        const float average = (heights[0] + heights[1] + heights[2] + heights[3]) * 0.25f;
        const float lift = (WheelRadius + SuspensionRest - sag - MountHeight) / std::max(axisY_.y, 0.3f);
        position_.y = average + lift + 0.01f;
        velocity_ = {};
        angular_ = {};
        previousLength_.fill(SuspensionRest);
        wasOverturned_ = false;
    }

    void QuadBike::Step(const float seconds, const BikeControls& controls, const GroundQuery& ground,
                        const ObstacleQuery& obstacles) noexcept
    {
        if (!ground || !std::isfinite(seconds) || seconds <= 0.0f) return;
        const float total = std::min(seconds, 0.1f);
        const float dt = total / static_cast<float>(Substeps);
        for (int i = 0; i < Substeps; ++i) Substep(dt, controls, ground, obstacles);
        const bool over = Overturned();
        if (over && !wasOverturned_) ++rollovers_;
        wasOverturned_ = over;
    }

    void QuadBike::Substep(const float dt, const BikeControls& controls, const GroundQuery& ground,
                           const ObstacleQuery& obstacles) noexcept
    {
        const float throttleCommand = std::isfinite(controls.throttle) ? std::clamp(controls.throttle, -1.0f, 1.0f) : 0.0f;
        const float steerCommand = std::isfinite(controls.steer) ? std::clamp(controls.steer, -1.0f, 1.0f) : 0.0f;
        const float forwardSpeed = Speed();
        // Steering tightens at speed; the bars swing at a rider's pace.
        const float steerLimit = MaxSteerRadians * (1.0f - 0.68f * std::clamp(std::abs(forwardSpeed) / TopSpeed, 0.0f, 1.0f));
        const float steerTarget = steerCommand * steerLimit;
        steer_ += std::clamp(steerTarget - steer_, -3.0f * dt, 3.0f * dt);
        throttle_ += std::clamp(throttleCommand - throttle_, -6.0f * dt, 6.0f * dt);

        BikeVec force{0.0f, -Gravity * Mass, 0.0f};
        BikeVec torque{};
        const auto apply = [&](const BikeVec& f, const BikeVec& at) {
            force = Add(force, f);
            torque = Add(torque, Cross(Sub(at, position_), f));
        };
        const auto pointVelocity = [&](const BikeVec& at) { return Add(velocity_, Cross(angular_, Sub(at, position_))); };

        const float tilt = axisY_.y;
        for (int wheel = 0; wheel < 4; ++wheel) {
            const auto w = static_cast<std::size_t>(wheel);
            const BikeVec mount = ToWorld(WheelMount(wheel));
            const BikeGround g = ground(mount.x, mount.z);
            float length = SuspensionRest;
            contact_[w] = false;
            if (tilt > 0.2f) {
                const float down = (mount.y - g.height) / tilt;   // along the body's down axis to the ground
                length = std::clamp(down - WheelRadius, SuspensionRest - SuspensionTravel, SuspensionRest);
                if (down - WheelRadius < SuspensionRest) {
                    contact_[w] = true;
                    const float rawLength = down - WheelRadius;
                    const float compressionSpeed = (previousLength_[w] - rawLength) / dt;
                    float spring = SpringRate * (SuspensionRest - rawLength) + DamperRate * compressionSpeed;
                    // the bump stop: travel used up, the frame takes the blow
                    if (rawLength < SuspensionRest - SuspensionTravel)
                        spring += SpringRate * 8.0f * (SuspensionRest - SuspensionTravel - rawLength);
                    spring = std::max(spring, 0.0f);
                    const BikeVec hub = Sub(mount, Mul(axisY_, length));
                    const BikeVec contactPoint = Sub(hub, Mul(g.normal, WheelRadius));
                    apply(Mul(axisY_, spring), mount);

                    // The tyre: its heading on the ground plane; front wheels steered.
                    const bool front = wheel < 2;
                    const float s = front ? steer_ : 0.0f;
                    const BikeVec heading = Add(Mul(axisZ_, std::cos(s)), Mul(axisX_, std::sin(s)));
                    const BikeVec side = Sub(Mul(axisX_, std::cos(s)), Mul(axisZ_, std::sin(s)));
                    const BikeVec forwardOnGround = Normalised(Sub(heading, Mul(g.normal, Dot(g.normal, heading))), axisZ_);
                    const BikeVec sideOnGround = Normalised(Sub(side, Mul(g.normal, Dot(g.normal, side))), axisX_);
                    const BikeVec v = pointVelocity(contactPoint);
                    const float vLong = Dot(v, forwardOnGround), vSide = Dot(v, sideOnGround);
                    const float grip = TyreFriction * std::clamp(g.grip, 0.05f, 1.5f) * (g.waterDepth > 0.15f ? 0.6f : 1.0f);
                    const float limit = grip * spring;
                    float lateral = -vSide * LateralStiffness;
                    float longitudinal = 0.0f;
                    if (throttle_ > 0.02f && !front) {
                        // CVT drive to the rear: traction-limited at low speed, power-limited
                        // above, cut by the limiter at the top speed.
                        const float governor = 1.0f - std::clamp((forwardSpeed - TopSpeed * 0.97f) / (TopSpeed * 0.03f), 0.0f, 1.0f);
                        const float available = std::min(MaxDriveForce, DrivePower / std::max(std::abs(forwardSpeed), 1.0f));
                        longitudinal += throttle_ * available * governor * 0.5f;
                    } else if (throttle_ < -0.02f) {
                        if (forwardSpeed > 0.6f) {
                            longitudinal -= -throttle_ * MaxBrakeForce * 0.25f;   // brakes on all four
                        } else if (!front && forwardSpeed > -ReverseSpeed) {
                            longitudinal -= -throttle_ * MaxDriveForce * 0.35f * 0.5f;   // reverse
                        }
                    }
                    if (std::abs(throttle_) <= 0.02f || controls.handbrake) {
                        longitudinal -= std::clamp(vLong * 400.0f, -EngineBraking, EngineBraking);
                        if (controls.handbrake && !front) longitudinal -= std::clamp(vLong * 2000.0f, -MaxBrakeForce * 0.3f, MaxBrakeForce * 0.3f);
                    }
                    longitudinal -= std::clamp(vLong * 50.0f, -1.0f, 1.0f) * RollingResistance * spring;
                    // the friction circle: the tyre's grip is shared by drive, brake and turn
                    const float demand = std::hypot(lateral, longitudinal);
                    if (demand > limit && demand > 1e-3f) {
                        lateral *= limit / demand;
                        longitudinal *= limit / demand;
                    }
                    apply(Add(Mul(forwardOnGround, longitudinal), Mul(sideOnGround, lateral)), contactPoint);
                    spin_[w] += vLong / WheelRadius * dt;
                    previousLength_[w] = rawLength;
                }
            }
            if (!contact_[w]) previousLength_[w] = SuspensionRest;
            hub_[w] = Sub(mount, Mul(axisY_, length));
            compression_[w] = std::clamp((SuspensionRest - length) / SuspensionTravel, 0.0f, 1.0f);
        }

        // The chassis and the rider meet the ground when it tips or lands hard.
        for (const BikeVec& local : Hull) {
            const BikeVec at = ToWorld(local);
            const BikeGround g = ground(at.x, at.z);
            const float depth = g.height - at.y;
            if (depth <= 0.0f) continue;
            const BikeVec v = pointVelocity(at);
            const float vNormal = Dot(v, g.normal);
            const float normalForce = std::max(0.0f, HullStiffness * depth - HullDamping * vNormal);
            const BikeVec vTangent = Sub(v, Mul(g.normal, vNormal));
            BikeVec friction = Mul(vTangent, -1500.0f);
            const float frictionLimit = 0.7f * normalForce;
            const float frictionSize = Length(friction);
            if (frictionSize > frictionLimit && frictionSize > 1e-3f) friction = Mul(friction, frictionLimit / frictionSize);
            apply(Add(Mul(g.normal, normalForce), friction), at);
        }

        // Air, and water if it drives in.
        const float speed = Length(velocity_);
        force = Add(force, Mul(velocity_, -DragCoefficient * speed));
        const BikeGround here = ground(position_.x, position_.z);
        if (here.waterDepth > 0.0f) {
            const float surface = here.height + here.waterDepth;
            const float submerged = std::clamp(surface - (position_.y - 0.35f), 0.0f, 1.0f);
            force.y += Gravity * Mass * 0.85f * submerged;
            force = Add(force, Mul(velocity_, -420.0f * submerged));
            torque = Add(torque, Mul(angular_, -120.0f * submerged));
        }
        torque = Add(torque, Mul(angular_, -18.0f));   // the suspension's and the air's damping of spin

        velocity_ = Add(velocity_, Mul(force, dt / Mass));
        // world torque to the body frame, through the inertia, back to the world
        const BikeVec bodyTorque{Dot(torque, axisX_), Dot(torque, axisY_), Dot(torque, axisZ_)};
        const BikeVec bodyAccel{bodyTorque.x / InertiaPitch, bodyTorque.y / InertiaYaw, bodyTorque.z / InertiaRoll};
        angular_ = Add(angular_, Mul(Add(Add(Mul(axisX_, bodyAccel.x), Mul(axisY_, bodyAccel.y)), Mul(axisZ_, bodyAccel.z)), dt));
        if (Length(angular_) > 25.0f) angular_ = Mul(Normalised(angular_), 25.0f);
        position_ = Add(position_, Mul(velocity_, dt));
        axisX_ = Add(axisX_, Mul(Cross(angular_, axisX_), dt));
        axisY_ = Add(axisY_, Mul(Cross(angular_, axisY_), dt));
        axisZ_ = Add(axisZ_, Mul(Cross(angular_, axisZ_), dt));
        Orthonormalise();

        // Trees, rocks and walls: pushed out, the speed into them taken away.
        if (obstacles) {
            float pushX = 0.0f, pushZ = 0.0f;
            if (obstacles(position_.x, position_.z, 0.95f, pushX, pushZ)) {
                position_.x += pushX;
                position_.z += pushZ;
                const BikeVec n = Normalised({pushX, 0.0f, pushZ}, {0, 0, 0});
                const float into = Dot(velocity_, n);
                if (into < 0.0f) velocity_ = Sub(velocity_, Mul(n, into * 1.25f));
            }
        }
        if (!Finite(position_) || !Finite(velocity_) || !Finite(angular_) || !Finite(axisY_)) {
            velocity_ = {};
            angular_ = {};
            Right(ground);
        }

        // The engine: a CVT keeps the revs up under throttle and lifts them with the speed.
        const float pull = std::max(throttle_, 0.0f);
        const float moving = std::clamp(std::abs(forwardSpeed) / TopSpeed, 0.0f, 1.0f);
        const float target = std::clamp(IdleRpm + pull * 4300.0f + moving * (pull > 0.05f ? 2900.0f : 1300.0f),
                                        IdleRpm, RedlineRpm);
        rpm_ += std::clamp(target - rpm_, -3200.0f * dt, 7000.0f * dt);
    }
}
