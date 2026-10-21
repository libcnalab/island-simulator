#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace island
{
    /** @brief A small 3-vector for the vehicle model (the core links no graphics library). */
    struct BikeVec
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
    };

    /** @brief The ground under a point: height, unit normal, tyre grip (dirt 1, sand ~0.7,
     *  wet ~0.6) and the water's depth there (0 on land). */
    struct BikeGround
    {
        float height = 0.0f;
        BikeVec normal{0.0f, 1.0f, 0.0f};
        float grip = 1.0f;
        float waterDepth = 0.0f;
    };

    /** @brief The rider's commands, each -1..1 (throttle forward/reverse, steer left/right). */
    struct BikeControls
    {
        float throttle = 0.0f;   // + forward, - brake then reverse
        float steer = 0.0f;      // + right
        bool handbrake = false;
    };

    /**
     * A quad bike (ATV) as a rigid body on four suspended wheels: springs and dampers at the
     * wheel mounts, load-limited tyre grip by surface, a CVT drive to the rear wheels with a
     * real sport quad's top speed, speed-sensitive steering, drag, and a chassis whose corners
     * meet the ground when it tips, so it can roll over on a steep side slope or in a fast turn
     * and lie there until Right() sets it back on its wheels. Body frame: x to the rider's
     * right (forward x up), y up, z forward; in the right-handed world that frame is mirrored,
     * so a matrix built from its axes reflects. Units: metres, kilograms, seconds.
     */
    class QuadBike final
    {
    public:
        using GroundQuery = std::function<BikeGround(float x, float z)>;
        using ObstacleQuery = std::function<bool(float x, float z, float radius, float& pushX, float& pushZ)>;

        static constexpr float Mass = 260.0f;                // the quad and its rider
        static constexpr float WheelBase = 1.25f, Track = 1.18f, WheelRadius = 0.29f;
        /** Knobby tyres on the island's loose dirt (grip 1): they slide before the quad's
         *  rollover threshold on flat ground (half the track over the centre of mass, 0.86 g),
         *  so it rolls on a steep side slope or off a bump, not in every hard turn. */
        static constexpr float TyreFriction = 0.78f;
        static constexpr float WadingDrag = 300.0f;          // half water's density by a tyre's drag coefficient
        static constexpr float MountHeight = -0.10f;         // the wheel mounts below the centre of mass
        static constexpr float SuspensionTravel = 0.24f, SuspensionRest = 0.30f;
        static constexpr float SpringRate = 12500.0f, DamperRate = 1500.0f;
        static constexpr float TopSpeed = 80.0f / 3.6f;      // 80 km/h, a sport quad's
        static constexpr float MaxDriveForce = 2600.0f, MaxBrakeForce = 3600.0f;
        static constexpr float MaxSteerRadians = 0.55f;
        static constexpr float IdleRpm = 1400.0f, RedlineRpm = 8600.0f;
        static constexpr int Substeps = 8;

        explicit QuadBike(BikeVec position = {}, float yawRadians = 0.0f) noexcept;

        /** @brief Advances by real seconds (internally in fixed substeps). */
        void Step(float seconds, const BikeControls& controls, const GroundQuery& ground,
                  const ObstacleQuery& obstacles = {}) noexcept;
        /** @brief Sets it back on its wheels where it lies, facing its present heading. */
        void Right(const GroundQuery& ground) noexcept;
        void Place(BikeVec position, float yawRadians, const GroundQuery& ground) noexcept;

        [[nodiscard]] BikeVec Position() const noexcept { return position_; }
        [[nodiscard]] BikeVec Velocity() const noexcept { return velocity_; }
        [[nodiscard]] BikeVec AngularVelocity() const noexcept { return angular_; }
        /** @brief The body's axes in the world: right, up, forward. */
        [[nodiscard]] BikeVec Right() const noexcept { return axisX_; }
        [[nodiscard]] BikeVec Up() const noexcept { return axisY_; }
        [[nodiscard]] BikeVec Forward() const noexcept { return axisZ_; }
        [[nodiscard]] float Speed() const noexcept;                 // signed along the heading, m/s
        [[nodiscard]] float YawRadians() const noexcept;
        [[nodiscard]] bool Overturned() const noexcept { return axisY_.y < 0.25f; }
        [[nodiscard]] float SteerRadians() const noexcept { return steer_; }
        [[nodiscard]] float EngineRpm() const noexcept { return rpm_; }
        [[nodiscard]] float Throttle() const noexcept { return throttle_; }
        [[nodiscard]] std::uint32_t Rollovers() const noexcept { return rollovers_; }
        /** @brief The hardest knock in the last Step, m/s: speed into a tree or rock, of the
         *  chassis meeting the ground, or of a wheel's travel bottoming out on a landing. */
        [[nodiscard]] float LastImpact() const noexcept { return impact_; }
        /** @brief How fast the tyres slide sideways over the ground (the fastest touching one in
         *  the last Step), m/s: a skid, a power slide. */
        [[nodiscard]] float SlideSpeed() const noexcept { return slide_; }
        /** @brief Per wheel (front-left, front-right, rear-left, rear-right): compression 0..1,
         *  spin angle, contact, and the hub position in the world. */
        [[nodiscard]] float Compression(int wheel) const noexcept { return compression_[static_cast<std::size_t>(wheel)]; }
        [[nodiscard]] float WheelSpin(int wheel) const noexcept { return spin_[static_cast<std::size_t>(wheel)]; }
        [[nodiscard]] bool WheelContact(int wheel) const noexcept { return contact_[static_cast<std::size_t>(wheel)]; }
        [[nodiscard]] BikeVec WheelHub(int wheel) const noexcept { return hub_[static_cast<std::size_t>(wheel)]; }
        /** @brief Body-frame mount of a wheel (x right, y up, z forward). */
        [[nodiscard]] static BikeVec WheelMount(int wheel) noexcept;
        /** @brief A body-frame point in the world. */
        [[nodiscard]] BikeVec ToWorld(BikeVec local) const noexcept;

    private:
        void Substep(float dt, const BikeControls& controls, const GroundQuery& ground,
                     const ObstacleQuery& obstacles) noexcept;
        void Orthonormalise() noexcept;
        BikeVec position_{};
        BikeVec velocity_{};
        BikeVec angular_{};          // world angular velocity, rad/s
        BikeVec axisX_{1, 0, 0}, axisY_{0, 1, 0}, axisZ_{0, 0, 1};
        std::array<float, 4> compression_{}, spin_{}, previousLength_{};
        std::array<bool, 4> contact_{};
        std::array<BikeVec, 4> hub_{};
        float steer_ = 0.0f, rpm_ = IdleRpm, throttle_ = 0.0f, impact_ = 0.0f, slide_ = 0.0f;
        bool wasOverturned_ = false;
        std::uint32_t rollovers_ = 0;
    };
}
