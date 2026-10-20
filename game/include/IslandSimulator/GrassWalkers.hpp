#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <span>

namespace island {
// One body moving through the grass besides the walker: where it is, how wide a press it
// makes (metres) and how hard (0..1, a standing animal still presses a little).
struct GrassWalker { float x=0,z=0,radius=0.5f,strength=0; };

// The grass shader takes a handful of these; the nearest to the camera are the ones that
// matter. Candidates further than reach, or with no strength, are left out.
struct GrassWalkers {
    static constexpr std::size_t Slots=6;
    static constexpr float Reach=35.0f;
    [[nodiscard]] static std::size_t Select(std::span<const GrassWalker> candidates,float cameraX,float cameraZ,
                                            std::span<GrassWalker> out) noexcept {
        std::array<std::pair<float,std::size_t>,64> order{};
        std::size_t count=0;
        for(std::size_t i=0;i<candidates.size()&&count<order.size();++i) {
            const GrassWalker& c=candidates[i];
            if(!std::isfinite(c.x)||!std::isfinite(c.z)||c.strength<=0.0f) continue;
            const float distance=std::hypot(c.x-cameraX,c.z-cameraZ);
            if(distance>Reach) continue;
            order[count++]={distance,i};
        }
        std::sort(order.begin(),order.begin()+static_cast<std::ptrdiff_t>(count));
        const std::size_t taken=std::min({count,out.size(),Slots});
        for(std::size_t i=0;i<taken;++i) out[i]=candidates[order[i].second];
        for(std::size_t i=taken;i<out.size();++i) out[i]=GrassWalker{};
        return taken;
    }
    // A moving body presses fully, a standing one lightly.
    [[nodiscard]] static float Strength(const float speed) noexcept {
        return std::isfinite(speed)?std::clamp(0.35f+speed*1.3f,0.35f,1.0f):0.0f;
    }
};
}
