#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace island
{
    // The existing hut rain exclusion, including the full finite streak rather than
    // only its head. Six half-spaces bound the volume below a gabled roof.
    class GabledRainShelter final
    {
    public:
        void Set(float x, float z, float yaw, float halfSpan, float halfLength,
                 float eaveY, float ridgeY) noexcept
        {
            active_ = std::isfinite(x) && std::isfinite(z) && std::isfinite(yaw)
                && std::isfinite(halfSpan) && std::isfinite(halfLength)
                && std::isfinite(eaveY) && std::isfinite(ridgeY)
                && halfSpan > 0 && halfLength > 0 && ridgeY >= eaveY;
            if (!active_) return;
            x_=x; z_=z; cosine_=std::cos(yaw); sine_=std::sin(yaw);
            halfSpan_=halfSpan; halfLength_=halfLength; ridgeY_=ridgeY;
            pitch_=(ridgeY-eaveY)/halfSpan;
        }

        // Keep the uncovered prefix, stopping at its first entry into the shelter.
        // A start already inside has no visible prefix; an entirely outside segment
        // retains its exact length. Used by both ordinary and lamp-lit rain.
        [[nodiscard]] float VisibleFraction(const std::array<float,3>& start,
                                            const std::array<float,3>& end) const noexcept
        {
            for (int i=0;i<3;++i)
                if (!std::isfinite(start[i]) || !std::isfinite(end[i])) return 0;
            if (!active_) return 1;
            const auto local=[&](const auto& p) {
                const float dx=p[0]-x_,dz=p[2]-z_;
                return std::array{cosine_*dx-sine_*dz, p[1], sine_*dx+cosine_*dz};
            };
            const auto a=local(start),b=local(end);
            float entry=0,exit=1;
            const auto clip=[&](float da,float db) {
                if (da>0 && db>0) return false;
                if (da<=0 && db<=0) return true;
                const float crossing=da/(da-db);
                if (da>0) entry=std::max(entry,crossing);
                else exit=std::min(exit,crossing);
                return entry<=exit;
            };
            if (!clip(a[0]-halfSpan_,b[0]-halfSpan_)
                || !clip(-a[0]-halfSpan_,-b[0]-halfSpan_)
                || !clip(a[2]-halfLength_,b[2]-halfLength_)
                || !clip(-a[2]-halfLength_,-b[2]-halfLength_)
                || !clip(a[1]+pitch_*a[0]-ridgeY_,b[1]+pitch_*b[0]-ridgeY_)
                || !clip(a[1]-pitch_*a[0]-ridgeY_,b[1]-pitch_*b[0]-ridgeY_)) return 1;
            return std::clamp(entry,0.0f,1.0f);
        }

    private:
        bool active_=false;
        float x_=0,z_=0,cosine_=1,sine_=0,halfSpan_=0,halfLength_=0,ridgeY_=0,pitch_=0;
    };
}
