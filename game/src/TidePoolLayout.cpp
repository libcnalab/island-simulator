#include "IslandSimulator/TidePoolLayout.hpp"

#include "IslandSimulator/IslandTerrain.hpp"

#include <algorithm>
#include <cmath>

namespace island
{
    std::pair<float,float> TidePoolLayout::Centre(
        const IslandTerrain& terrain, const std::size_t index) noexcept
    {
        if (index>=Definitions.size()) return {};
        const auto& pool=Definitions[index];
        const float radius=terrain.ShoreRadius(pool.angleRadians)-pool.inlandDistance;
        return {std::cos(pool.angleRadians)*radius,std::sin(pool.angleRadians)*radius};
    }

    float TidePoolLayout::Depression(
        const IslandTerrain& terrain, const float x, const float z) noexcept
    {
        if (!std::isfinite(x) || !std::isfinite(z)) return 0.0f;
        float depression=0.0f;
        for (std::size_t index=0;index<Definitions.size();++index) {
            const auto [centreX,centreZ]=Centre(terrain,index);
            const auto& pool=Definitions[index];
            const float dx=x-centreX,dz=z-centreZ;
            const float normalized=std::hypot(dx,dz)/(pool.radius*OutlineScale(index,std::atan2(dz,dx)));
            if (normalized>=1.0f) continue;
            const float bowl=1.0f-normalized*normalized;
            depression=std::max(depression,pool.depth*bowl*bowl);
        }
        return depression;
    }

    float TidePoolLayout::WaterRadius(const std::size_t index) noexcept
    {
        return index<Definitions.size() ? Definitions[index].radius*.67f : 0.0f;
    }

    float TidePoolLayout::OutlineScale(const std::size_t index, const float angleRadians) noexcept
    {
        if (!std::isfinite(angleRadians)) return 1.0f;
        // a weathered rock basin is lobed, never round: three low harmonics, each pool its own
        const float seed=static_cast<float>(index)*2.399963f;
        return 1.0f+.11f*std::sin(2.0f*angleRadians+seed)
            +.07f*std::sin(3.0f*angleRadians+1.7f*seed+.6f)
            +.04f*std::sin(5.0f*angleRadians-seed+2.1f);
    }

    float TidePoolLayout::WaterLevel(
        const IslandTerrain& terrain, const std::size_t index) noexcept
    {
        if (index>=Definitions.size()) return 0.0f;
        const auto [x,z]=Centre(terrain,index);
        return terrain.Height(x,z)+Definitions[index].depth*.70f;
    }
}
