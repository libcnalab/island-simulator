#include "IslandSimulator/WhalePod.hpp"
#include <algorithm>
#include <cmath>

namespace island
{
    float WhalePod::Unit(const std::uint32_t salt) const noexcept
    {
        std::uint32_t h = surfacings_ * 0x9e3779b9U ^ salt * 0x85ebca6bU;
        h ^= h >> 16; h *= 0x7feb352dU; h ^= h >> 15; h *= 0x846ca68bU; h ^= h >> 16;
        return static_cast<float>(h & 0xffffffU) / 16777215.0f;
    }

    void WhalePod::Update(const float seconds, const float windX, const float windZ) noexcept
    {
        const float dt = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.25f) : 0.0f;
        clock_ += dt;
        if (!surfacing_.active && clock_ >= nextSurfacing_) {
            // a new roll somewhere far out, heading along the island's coast
            const float bearing = Unit(1U) * 6.2831853f;
            const float distance = 330.0f + 290.0f * Unit(2U);
            surfacing_ = {std::cos(bearing) * distance, std::sin(bearing) * distance,
                          bearing + (Unit(3U) < 0.5f ? 1.5707963f : -1.5707963f), 0.0f, Unit(4U) < 0.34f, true};
            blowAge_ = 0.0f;
            blowX_ = surfacing_.x;
            blowZ_ = surfacing_.z;
            ++surfacings_;
        }
        if (surfacing_.active) {
            surfacing_.progress += dt / RollSeconds;
            surfacing_.x += std::cos(surfacing_.headingRadians) * 1.6f * dt;
            surfacing_.z += std::sin(surfacing_.headingRadians) * 1.6f * dt;
            if (surfacing_.progress >= 1.0f) {
                surfacing_.active = false;
                nextSurfacing_ = clock_ + MinimumGapSeconds + (MaximumGapSeconds - MinimumGapSeconds) * Unit(5U);
            }
        }
        // The blow: a column of mist that stands up five metres within a second, then spreads,
        // drifts downwind and thins away over about five.
        blowAge_ += dt;
        for (std::size_t i = 0; i < puffs_.size(); ++i) {
            MistPuff& puff = puffs_[i];
            const float t = static_cast<float>(i) / static_cast<float>(puffs_.size() - 1U);   // up the column
            const float age = blowAge_ - t * 0.35f;
            if (age < 0.0f || age > 5.5f) {
                puff.active = false;
                puff.density = 0.0f;
                continue;
            }
            const float rise = std::min(age / 0.9f, 1.0f);
            const float drift = std::max(age - 0.6f, 0.0f) * 1.4f;
            puff.x = blowX_ + windX * drift;
            puff.z = blowZ_ + windZ * drift;
            puff.y = 0.4f + (1.0f + 5.0f * t) * rise + 0.25f * std::max(age - 1.0f, 0.0f);
            puff.size = (1.1f + 1.6f * t) * (0.6f + 0.4f * rise) + 0.55f * std::max(age - 0.8f, 0.0f);
            puff.age = age;
            puff.density = std::clamp(age * 4.0f, 0.0f, 1.0f) * (1.0f - std::clamp((age - 1.2f) / 4.3f, 0.0f, 1.0f)) * (0.85f - 0.25f * t);
            puff.active = true;
        }
    }
}
