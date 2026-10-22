#include "IslandSimulator/QuadWheelTraces.hpp"
#include "IslandSimulator/FootprintTrail.hpp"
#include <algorithm>
#include <cmath>

namespace island
{
    void TyreTrackField::Update(const float seconds, const std::span<const WheelOnGround> wheels) noexcept
    {
        const float dt = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.25f) : 0.0f;
        for (auto& track : tracks_) {
            if (!track.active) continue;
            track.age += dt * (1.0f + FootprintTrail::RainErosion * rain_);
            if (track.age >= track.life) {
                track.active = false;
                track.density = 0.0f;
                continue;
            }
            // the last fifth of its life the wind and the swash level it away
            const float left = 1.0f - track.age / track.life;
            track.density = track.strength * std::clamp(left * 5.0f, 0.0f, 1.0f);
        }
        for (std::size_t w = 0; w < trackers_.size(); ++w) {
            auto& tracker = trackers_[w];
            if (w >= wheels.size()) {
                tracker.valid = false;
                continue;
            }
            const WheelOnGround& wheel = wheels[w];
            const float wetTread = std::isfinite(wheel.wetTread) ? std::clamp(wheel.wetTread, 0.0f, 1.0f) : 0.0f;
            if (!wheel.down || (!wheel.soft && wetTread < 0.1f) || !std::isfinite(wheel.x) || !std::isfinite(wheel.z)) {
                tracker.valid = false;
                continue;
            }
            if (!tracker.valid) {
                tracker = {wheel.x, wheel.z, true};
                continue;
            }
            const float moved = std::hypot(wheel.x - tracker.x, wheel.z - tracker.z);
            if (moved > 3.0f) {   // a jump, a teleport: start afresh
                tracker = {wheel.x, wheel.z, true};
                continue;
            }
            if (moved < SegmentLength) continue;
            TyreTrack& track = tracks_[next_];
            next_ = (next_ + 1) % tracks_.size();
            // pressed into sand or soil (darker still, wet), or only wetting firm ground
            const float pressed = wheel.soft ? std::clamp(0.45f + 0.55f * wheel.load + 0.15f * wetTread, 0.0f, 1.0f)
                                             : 0.25f + 0.40f * wetTread;
            track = {tracker.x, tracker.z, wheel.x, wheel.z, wheel.width, pressed, 0.0f, 0.0f,
                     !wheel.soft ? DampPrintLifeSeconds : wheel.wet ? WetLifeSeconds : LifeSeconds, true};
            track.density = track.strength;
            tracker = {wheel.x, wheel.z, true};
            ++laid_;
        }
    }

    std::size_t TyreTrackField::ActiveCount() const noexcept
    {
        return static_cast<std::size_t>(std::count_if(tracks_.begin(), tracks_.end(), [](const TyreTrack& t) { return t.active; }));
    }

    float WheelThrowField::Jitter() noexcept
    {
        jitter_ ^= jitter_ << 13;
        jitter_ ^= jitter_ >> 17;
        jitter_ ^= jitter_ << 5;
        return static_cast<float>(jitter_ & 0xffffU) / 65535.0f;
    }

    void WheelThrowField::Update(const float seconds, const std::span<const WheelOnGround> wheels, const BikeVec velocity,
                                 const float windX, const float windZ) noexcept
    {
        const float dt = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.25f) : 0.0f;
        const bool dust = kind_ == Kind::Dust;
        const float life = dust ? DustLife : SprayLife;
        for (std::size_t i = 0; i < puffs_.size(); ++i) {
            DustPuff& puff = puffs_[i];
            if (!puff.active) continue;
            puff.age += dt;
            if (puff.age >= life) {
                puff.active = false;
                puff.density = 0.0f;
                continue;
            }
            BikeVec& v = velocity_[i];
            if (dust) {
                // dust slows in the air, settles slowly and is carried by the breeze
                const float drag = std::exp(-dt * 2.2f);
                v.x = windX * 0.7f + (v.x - windX * 0.7f) * drag;
                v.z = windZ * 0.7f + (v.z - windZ * 0.7f) * drag;
                v.y = v.y * drag - 0.35f * dt;
                puff.size += dt * 1.6f;
            } else {
                v.y -= 9.81f * dt;   // the droplets fall back
                puff.size += dt * 0.45f;
            }
            puff.x += v.x * dt;
            puff.y += v.y * dt;
            puff.z += v.z * dt;
            const float t = puff.age / life;
            puff.density = (dust ? 0.85f : 0.75f) * std::clamp(t * 20.0f, 0.0f, 1.0f) * (1.0f - t) * (1.0f - t * 0.3f);
        }
        const float speed = std::hypot(velocity.x, velocity.z);
        if (!(speed > 0.5f) || !std::isfinite(speed)) {
            pending_.fill(0.0f);
            return;
        }
        const float backX = -velocity.x / speed, backZ = -velocity.z / speed;
        for (std::size_t w = 0; w < pending_.size() && w < wheels.size(); ++w) {
            const WheelOnGround& wheel = wheels[w];
            const bool rear = w >= 2;
            float rate = 0.0f;   // puffs a second
            if (dust && wheel.down && wheel.dusty && wheel.waterDepth <= 0.0f)
                rate = std::min(14.0f, std::max(speed - 2.0f, 0.0f) * 1.3f) * (rear ? 1.0f : 0.45f);
            else if (!dust && wheel.waterDepth > 0.03f)
                rate = std::min(16.0f, speed * 2.2f) * (rear ? 1.0f : 0.8f);
            if (rate <= 0.0f) {
                pending_[w] = 0.0f;
                continue;
            }
            pending_[w] += rate * dt;
            while (pending_[w] >= 1.0f) {
                pending_[w] -= 1.0f;
                DustPuff& puff = puffs_[next_];
                BikeVec& v = velocity_[next_];
                next_ = (next_ + 1) % puffs_.size();
                const float side = Jitter() - 0.5f, lift = Jitter(), back = Jitter();
                const float surface = dust ? wheel.y + 0.22f : wheel.y + wheel.waterDepth;
                puff = {wheel.x + backX * 0.25f - backZ * side * wheel.width, surface, wheel.z + backZ * 0.25f + backX * side * wheel.width,
                        dust ? 0.45f + 0.20f * lift : 0.10f + 0.06f * lift, 0.0f, 0.0f, true};
                // dust hangs in the quad's wake (dragged a little after it); spray is flung back
                const float fling = dust ? -speed * (0.20f + 0.10f * back) : speed * (0.22f + 0.15f * back);
                v = {backX * fling - backZ * side * 1.2f + velocity.x * (dust ? 0.0f : 0.05f),
                     dust ? 0.6f + 0.7f * lift : 1.4f + 1.4f * lift + speed * 0.05f,
                     backZ * fling + backX * side * 1.2f + velocity.z * (dust ? 0.0f : 0.05f)};
                ++emitted_;
            }
        }
    }
}
