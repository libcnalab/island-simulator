#include "IslandSimulator/NaturalPropLayout.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include "IslandSimulator/JungleLayout.hpp"
#include "IslandSimulator/RenderedTerrainSurface.hpp"

#include <algorithm>
#include <bit>
#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace
{
    void Require(bool value,const char* message)
    {
        if (!value) throw std::runtime_error(message);
    }
    auto Signature(const island::BeachDetailPlacement& p)
    {
        const auto bits=[](float value){return std::bit_cast<std::uint32_t>(value);};
        return std::tuple{bits(p.x),bits(p.y),bits(p.z),bits(p.yawRadians),bits(p.scale),
            bits(p.normalX),bits(p.normalY),bits(p.normalZ),p.variant,
            p.tidePoolLandmark,p.turtleTrace,p.turtleNest};
    }
}

int main()
try
{
    const island::IslandTerrain terrain;
    const island::VegetationLayout palms(terrain);
    const island::JungleLayout jungle(terrain);
    const island::RenderedTerrainSurface drawn(terrain);
    // These failures were reproduced against the original 36-ray search. Keep the
    // successful default/extreme controls alongside every failing seed found in the audit.
    constexpr std::array recoveredSeeds{2U,3U,11U,17U,23U,27U,28U,29U,30U,
        31U,32U,36U,37U,39U,43U,53U,93219U};
    std::vector<std::uint32_t> seeds{island::NaturalPropLayout::DefaultSeed,0U,1U,0xffffffffU};
    seeds.insert(seeds.end(),recoveredSeeds.begin(),recoveredSeeds.end());
    std::size_t worlds=0,refinedNests=0;
    for (const auto seed : seeds) {
        const island::NaturalPropLayout props(terrain,palms,jungle,seed);
        const bool formerlyFailed=std::ranges::find(recoveredSeeds,seed)!=recoveredSeeds.end();
        int segments=0,nests=0,refined=0;
        std::vector<std::array<float,2>> centres, trail;
        const auto requireClear = [&](float x,float z,float footprint) {
            for (const auto& rock : props.Rocks())
                Require(std::hypot(x-rock.x,z-rock.z)>=footprint+std::max(rock.scaleX,rock.scaleZ),
                        "Recovered turtle footprint overlaps a rock");
            for (const auto& tree : jungle.Canopy()) {
                const float root=(tree.variant==island::MangroveCanopyVariant?1.6f:.8f)*tree.scale;
                Require(std::hypot(x-tree.x,z-tree.z)>=footprint+root,
                        "Recovered turtle footprint overlaps exposed roots");
            }
            const auto clearWood=[&](const auto& wood) {
                const float c=std::cos(wood.yawRadians),s=std::sin(wood.yawRadians);
                const float dx=x-wood.x,dz=z-wood.z;
                const float along=std::clamp(dx*c+dz*s,-wood.length*.5f,wood.length*.5f);
                Require(std::hypot(dx-c*along,dz-s*along)>=footprint+wood.radius,
                        "Recovered turtle footprint crosses a fallen log or driftwood");
            };
            for (const auto& wood : props.Driftwood()) clearWood(wood);
            for (const auto& log : props.FallenLogs()) clearWood(log);
        };
        for (const auto& p : props.BeachDetails()) {
            if (!p.turtleTrace) continue;
            const auto sample=terrain.Sample(p.x,p.z);
            Require(sample.signedShoreDistance>=3 && sample.signedShoreDistance<=18
                && sample.height>=.22f && sample.slopeRadians<=.20943951f && p.x<-100,
                "Refined turtle sites left dry, low-slope western beach habitat");
            Require(!island::VegetationLayout::IsReservedRoute(p.x,p.z),
                    "Refined turtle sites obstruct an authored route");
            const float clearance=p.turtleNest?1.8f:1.4f;
            for (const auto& palm : palms.Palms())
                Require(std::hypot(p.x-palm.x,p.z-palm.z)>=clearance,
                        "Refined turtle site overlaps a palm");
            for (const auto& tree : jungle.Canopy())
                Require(std::hypot(p.x-tree.x,p.z-tree.z)>=clearance,
                        "Refined turtle site overlaps a canopy tree");
            for (const auto& other : props.BeachDetails()) {
                if (&p==&other) continue;
                Require(std::hypot(p.x-other.x,p.z-other.z)>=(p.turtleNest?1.5f:.8f)-1e-5f,
                        "Refined turtle site overlaps existing beach detail");
            }
            Require(std::abs(p.y-drawn.Height(p.x,p.z)-(p.turtleNest?.02f:.008f))<2e-6f,
                    "Refined turtle site lost its rendered sand support");
            if (formerlyFailed) requireClear(p.x,p.z,p.turtleNest?1.8f:.7f);
            if (p.turtleNest) {
                Require(p.variant==5 && segments==island::NaturalPropLayout::TurtleTrackSegmentsPerNest,
                        "A partial or duplicate turtle track was committed");
                if (formerlyFailed) {
                    // Independently oversample the rendered polyline, including its half-
                    // segment extensions; checking seven placement centres missed collisions.
                    const float dx=trail.back()[0]-trail.front()[0];
                    const float dz=trail.back()[1]-trail.front()[1];
                    for (int station=0;station<=128;++station) {
                        const float t=-1.0f/12.0f+static_cast<float>(station)/128.0f*7.0f/6.0f;
                        const float x=trail.front()[0]+t*dx,z=trail.front()[1]+t*dz;
                        requireClear(x,z,.66f);
                        Require(terrain.Height(x,z)>=.10f,
                                "Recovered crawl extends into submerged sand");
                    }
                }
                trail.clear();segments=0;++nests;
                for (const auto& centre : centres)
                    Require(std::hypot(p.x-centre[0],p.z-centre[1])>=18,
                            "Refined nests lost their separation");
                centres.push_back({p.x,p.z});
                float angle=std::atan2(p.z,p.x);
                if (angle<0) angle+=2*std::numbers::pi_v<float>;
                Require(angle>=2.78f-1e-5f && angle<=2.78f+35*.026f+1e-5f,
                        "Refinement expanded the authored nesting coast");
                const float coarse=(angle-2.78f)/.026f;
                refined+=std::abs(coarse-std::round(coarse))>.01f;
            } else {
                Require(p.variant==4,"Refined turtle track lost its paired-flipper form");
                trail.push_back({p.x,p.z});++segments;
            }
        }
        Require(nests==island::NaturalPropLayout::TurtleNestingTrackCount && segments==0,
                "Refinement must retain exactly two complete nesting traces");
        Require(formerlyFailed || refined==0,"Recovery changed a legacy valid seed");
        if (formerlyFailed) {
            const island::NaturalPropLayout repeated(terrain,palms,jungle,seed);
            Require(props.BeachDetails().size()==repeated.BeachDetails().size(),
                    "Refined beach population is nondeterministic");
            for (std::size_t i=0;i<props.BeachDetails().size();++i)
                Require(Signature(props.BeachDetails()[i])==Signature(repeated.BeachDetails()[i]),
                        "Refined beach placement is not bitwise deterministic");
        }
        refinedNests+=refined;++worlds;
    }
    std::cout<<worlds<<" seed layouts passed; "<<refinedNests<<" refined nests, all habitat/clearance rules retained\n";
}
catch (const std::exception& error)
{
    std::cerr<<error.what()<<'\n';
    return 1;
}
