#pragma once
#include "IslandSimulator/LocalTerrainPatch.hpp"
#include "IslandSimulator/ShipwreckLayout.hpp"

namespace island {
// Sand scoured and drifted against the grounded hull: a terrain replacement like the
// stream bed, so the wreck sits in a bar of its own making instead of on an untouched
// seabed. The lee (shoreward) side carries the long drift tongue that just breaks the
// surface; the seaward side keeps a short scour lip that stays submerged. Inside the
// hull outline the sand rises to the crest, which the hull planking hides.
class WreckScourPatch final : public LocalTerrainPatch {
public:
    static constexpr int Subdivision=4;
    static constexpr float Reach=16.0f;
    static constexpr float LeeCrest=0.62f,SeaCrest=0.30f;
    static constexpr float LeeDecay=5.2f,SeaDecay=2.4f;
    static constexpr float LeeCap=0.22f,SeaCap=-0.08f;
    WreckScourPatch(const ShipwreckLayout& wreck,const RenderedTerrainSurface& original)
        : LocalTerrainPatch(Range(wreck.Ship()),Subdivision,original,
                            [ship=wreck.Ship()](float x,float z,float base){return Height(ship,x,z,base);}) {}
    // Shared with tests: the bar height rule for one dense vertex.
    [[nodiscard]] static float Height(const ShipwreckPlacement& ship,const float x,const float z,
                                      const float base) noexcept {
        const float dx=x-ship.x,dz=z-ship.z;
        const float forwardX=std::sin(ship.yawRadians),forwardZ=std::cos(ship.yawRadians);
        const float along=dx*forwardX+dz*forwardZ;
        const float across=dx*forwardZ-dz*forwardX;
        const float halfLength=ship.length*0.5f,halfBeam=ship.beam*0.5f;
        // Distance outside the elliptical hull outline, in metres.
        const float outlineHalfWidth=halfBeam*std::sqrt(std::max(0.0f,
            1.0f-(along/halfLength)*(along/halfLength)));
        const float lateral=std::abs(across)-outlineHalfWidth;
        const float beyondEnd=std::max(0.0f,std::abs(along)-halfLength);
        const float outside=std::abs(along)<halfLength?std::max(0.0f,lateral)
            :std::hypot(beyondEnd,std::max(0.0f,std::abs(across)-0.4f));
        // Lee factor: +1 straight toward the beach, -1 straight out to sea.
        const float outwardX=std::cos(ShipwreckLayout::AuthoredCoastAngle);
        const float outwardZ=std::sin(ShipwreckLayout::AuthoredCoastAngle);
        const float radial=std::max(std::hypot(dx,dz),0.001f);
        const float lee=-(dx*outwardX+dz*outwardZ)/radial;
        const float leeAmount=std::clamp(0.5f+0.5f*lee,0.0f,1.0f);
        const float crest=std::lerp(SeaCrest,LeeCrest,leeAmount);
        const float decay=std::lerp(SeaDecay,LeeDecay,leeAmount);
        const float cap=std::lerp(SeaCap,LeeCap,leeAmount);
        // Irregular ripple of the bar edge from a cheap fixed-seed field.
        const float ripple=0.82f+0.18f*std::sin(along*0.9f+across*1.3f+1.7f)
            *std::cos(along*0.37f-across*0.61f);
        const float profile=std::exp(-(outside/decay)*(outside/decay))*ripple;
        // The patch boundary lies at least Reach from the ship on a world axis; the bar
        // must be flat there so the edge meets the untouched coarse terrain exactly.
        const float reach=std::max(std::abs(dx),std::abs(dz));
        const float fade=1.0f-SmoothStep((reach-(Reach-4.5f))/3.5f);
        const float mound=crest*profile*fade;
        if(mound<0.002f) return base;
        return std::max(base,std::min(base+mound,std::max(cap,base)));
    }
private:
    [[nodiscard]] static float SmoothStep(float v) noexcept {
        v=std::clamp(v,0.0f,1.0f);return v*v*(3-2*v);
    }
    [[nodiscard]] static CoarseRange Range(const ShipwreckPlacement& ship) {
        return {CoarseIndexBelow(ship.x-Reach),CoarseIndexAbove(ship.x+Reach),
                CoarseIndexBelow(ship.z-Reach),CoarseIndexAbove(ship.z+Reach)};
    }
};
}
