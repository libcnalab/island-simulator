#include "IslandSimulator/ExhaustSmokeField.hpp"

#include <algorithm>
#include <cmath>

namespace island
{
    void ExhaustSmokeField::Update(const float seconds, const bool running, const float throttle, const BikeVec tip,
                                   const BikeVec direction, const BikeVec carrier, const float windX, const float windZ) noexcept
    {
        if (!std::isfinite(seconds) || seconds <= 0.0f) return;
        const float dt = std::min(seconds, 0.1f);
        const auto random = [this] {
            jitter_ ^= jitter_ << 13U; jitter_ ^= jitter_ >> 17U; jitter_ ^= jitter_ << 5U;
            return static_cast<float>(jitter_ >> 8U) * (1.0f / 16777216.0f) - 0.5f;
        };
        const float pull = std::clamp(throttle, 0.0f, 1.0f);
        if (running && std::isfinite(tip.x) && std::isfinite(tip.y) && std::isfinite(tip.z)) {
            pending_ += dt * (7.0f + 26.0f * pull);
            while (pending_ >= 1.0f) {
                pending_ -= 1.0f;
                DustPuff& puff = puffs_[next_];
                puff.active = true;
                puff.x = tip.x + random() * 0.03f;
                puff.y = tip.y + random() * 0.03f;
                puff.z = tip.z + random() * 0.03f;
                puff.age = 0.0f;
                puff.size = 0.06f;
                puff.density = 0.0f;
                velocity_[next_] = {carrier.x * 0.85f + direction.x * (1.6f + 2.2f * pull) + random() * 0.4f,
                                    carrier.y * 0.85f + direction.y * (1.6f + 2.2f * pull) + 0.25f,
                                    carrier.z * 0.85f + direction.z * (1.6f + 2.2f * pull) + random() * 0.4f};
                next_ = (next_ + 1U) % Capacity;
                ++emitted_;
            }
        }
        const float strength = 0.30f + 0.35f * pull;
        for (std::size_t i = 0; i < Capacity; ++i) {
            DustPuff& puff = puffs_[i];
            if (!puff.active) continue;
            puff.age += dt;
            if (puff.age >= Life) { puff.active = false; continue; }
            // the air takes the puff over: its own speed decays toward the breeze, it warms and rises
            BikeVec& v = velocity_[i];
            const float relax = std::exp(-dt * 2.6f);
            v.x = windX * 1.2f + (v.x - windX * 1.2f) * relax;
            v.z = windZ * 1.2f + (v.z - windZ * 1.2f) * relax;
            v.y = 0.32f + (v.y - 0.32f) * relax;
            puff.x += v.x * dt; puff.y += v.y * dt; puff.z += v.z * dt;
            const float t = puff.age / Life;
            puff.size = 0.06f + 0.55f * std::sqrt(t);
            puff.density = strength * std::min(1.0f, puff.age * 9.0f) * (1.0f - t) * (1.0f - t);
        }
    }
}
