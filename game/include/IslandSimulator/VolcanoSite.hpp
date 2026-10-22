#pragma once

#include <algorithm>
#include <cmath>
#include <numbers>

namespace island {

/** Authored footprint of the small volcano on the eastern foothills over the sea: a cone of ash
 *  and old lava rising 25 m from a 55 m foot to a crater 11 m across the rim, a lava lake 9 m
 *  down in it.
 *  The cone's surface is analytic (its gullies and an old flow down its south-western side
 *  included), so the walker climbing it and the mesh drawn agree; the crater itself is not
 *  walked into. */
struct VolcanoSite final {
    static constexpr float X=174.0f;
    static constexpr float Z=0.0f;
    static constexpr float BaseY=2.0f;          // the foot's level, under the foothills, so the cone rises out of them
    static constexpr float BaseRadius=55.0f,Height=25.0f,RimRadius=11.0f,CraterDepth=9.0f,LavaRadius=6.0f;
    static constexpr float FlowAngle=0.25f;     // the old lava flow's bearing from the crater, down toward the sea
    // fumaroles steaming out of the upper slopes: their bearings and radii from the crater
    static constexpr float VentBearings[4]={1.05f,2.25f,4.0f,5.35f};
    static constexpr float VentRadii[4]={19.0f,23.0f,17.5f,21.0f};

    /** How far out the foot reaches on a bearing, as a share of BaseRadius (0.94 to 1). */
    [[nodiscard]] static float Spread(const float theta) noexcept {
        return 0.97f+0.02f*std::cos(theta-0.7f)+0.01f*std::cos(2.0f*theta+1.9f);
    }
    /** The cone's height above BaseY at radius `r` and bearing `theta` (zero beyond the foot). */
    [[nodiscard]] static float Profile(const float r,const float theta) noexcept {
        if(!std::isfinite(r)||r>=BaseRadius) return 0.0f;
        if(r<=LavaRadius) return Height-CraterDepth-0.6f;
        // the crater's wall rising to the rim and the outer slope falling from it, met in a
        // rounded crest (a smooth minimum), so a walker on the rim is not on a knife edge
        const float s=(r-LavaRadius)/(RimRadius-LavaRadius);
        const float inner=Height-CraterDepth+CraterDepth*std::pow(s,1.6f);
        // the foot lies further out on some bearings than on others, so the cone is not round
        const float reach=BaseRadius*Spread(theta);
        const float t=(reach-r)/(reach-RimRadius);
        if(t<=0.0f) return 0.0f;
        float h=Height*std::pow(t,1.15f);
        if(t>=1.0f) {
            constexpr float K=0.35f;
            return std::min(inner,h)-K*std::log1p(std::exp(-std::abs(inner-h)/K));
        }
        // gullies cut by the rain down the mid slopes
        const float mid=std::sin(std::numbers::pi_v<float>*t);
        const float g=0.5f+0.5f*std::cos(11.0f*theta+2.0f*std::sin(3.0f*theta)+r*0.07f);
        h-=1.0f*mid*g*g*g*g;
        // the old flow: a raised tongue of black lava down one flank
        float d=std::remainder(theta-FlowAngle,2.0f*std::numbers::pi_v<float>);
        d+=0.05f*std::sin(r*0.21f);
        const float width=0.09f+0.08f*(1.0f-t);
        // (welling out a little below the rim, so the rim itself stays one line)
        h+=1.5f*std::exp(-(d*d)/(width*width))*std::min(1.0f,t*3.0f)*std::min(1.0f,(1.0f-t)*6.0f);
        constexpr float K=0.35f;
        h=std::min(inner,h)-K*std::log1p(std::exp(-std::abs(inner-h)/K));
        return std::max(h,0.0f);
    }
    [[nodiscard]] static float Radius(const float x,const float z) noexcept {return std::hypot(x-X,z-Z);}
    [[nodiscard]] static float Bearing(const float x,const float z) noexcept {return std::atan2(z-Z,x-X);}
    [[nodiscard]] static float SurfaceY(const float x,const float z) noexcept {
        return BaseY+Profile(Radius(x,z),Bearing(x,z));
    }
    [[nodiscard]] static float LavaY() noexcept {return BaseY+Height-CraterDepth;}
    /** The ground under (x, z): the cone where it stands above the lowland. */
    [[nodiscard]] static float SupportHeight(const float x,const float z,const float terrainHeight) noexcept {
        if(Radius(x,z)>=BaseRadius) return terrainHeight;
        return std::max(terrainHeight,SurfaceY(x,z));
    }
    /** How loud the volcano is to a listener at (x, y, z): all of it at the rim, fading out
     *  over the lowland some 90 m from the crater. */
    [[nodiscard]] static float SoundPresence(const float x,const float y,const float z) noexcept {
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)) return 0.0f;
        const float distance=std::hypot(Radius(x,z),y-LavaY());
        const float near=std::clamp((90.0f-distance)/75.0f,0.0f,1.0f);
        return near*near*(3.0f-2.0f*near);
    }
    /** The forest and the grass keep off the cone. */
    [[nodiscard]] static bool Reserves(const float x,const float z,float margin=0.0f) noexcept {
        if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(margin)) return false;
        return Radius(x,z)<BaseRadius-4.0f+std::max(0.0f,margin);
    }
    /** The live lava spilling over the rim's lip and down the old flow's crest: from the rim
     *  this far down the slope, widening from FlowRootWidth to FlowFrontWidth. */
    static constexpr float FlowLength=22.0f,FlowRootWidth=0.8f,FlowFrontWidth=2.4f;
    /** The live flow's centreline bearing at radius r (along the old flow's crest). */
    [[nodiscard]] static float FlowCentre(const float r) noexcept {return FlowAngle-0.05f*std::sin(r*0.21f);}
    [[nodiscard]] static float FlowHalfWidth(const float r) noexcept {
        return 0.5f*(FlowRootWidth+(FlowFrontWidth-FlowRootWidth)*std::clamp((r-RimRadius)/FlowLength,0.0f,1.0f));
    }
    /** Whether (x, z) lies on the live flow, `margin` wider all round. */
    [[nodiscard]] static bool OnLavaFlow(const float x,const float z,const float margin=0.0f) noexcept {
        const float r=Radius(x,z);
        if(r<RimRadius-margin||r>RimRadius+FlowLength+margin) return false;
        const float across=std::remainder(Bearing(x,z)-FlowCentre(r),2.0f*std::numbers::pi_v<float>)*r;
        return std::abs(across)<FlowHalfWidth(r)+margin;
    }
    /** The walker stops on the rim (the crater's wall drops sheer to the lava) and at the live
     *  flow's edge. */
    [[nodiscard]] static bool BlocksWalker(const float x,const float z) noexcept {
        return Radius(x,z)<RimRadius+0.05f||OnLavaFlow(x,z,0.35f);
    }
    /** The quad and the animals keep off the steep cone. */
    [[nodiscard]] static bool BlocksVehicle(const float x,const float z,const float margin) noexcept {
        return Radius(x,z)<BaseRadius*0.72f+margin;
    }
};

}
