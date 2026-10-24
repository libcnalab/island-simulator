#pragma once

#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {

struct BalloonShadowPoint {
    float x=0.0f,z=0.0f,receiverY=0.0f,height=0.0f;
    bool overLand=false,valid=false;
};

/** Projects the centre of the balloon's envelope down the current sun ray. Unlike the bird
 * shadow, this accepts the sea as a receiver and is intentionally unbounded in caster height;
 * the renderer applies its own camera, sun-height and opacity limits. */
struct BalloonShadowProjection {
    [[nodiscard]] static BalloonShadowPoint Project(const IslandTerrain& terrain,
        const float x,const float y,const float z,const float slopeX,const float slopeZ) noexcept {
        BalloonShadowPoint point;
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)
            ||!std::isfinite(slopeX)||!std::isfinite(slopeZ))return point;
        float sx=x,sz=z,receiver=std::max(0.0f,terrain.Height(x,z));
        float height=y-receiver;
        for(int pass=0;pass<3;++pass) {
            sx=x-slopeX*height;sz=z-slopeZ*height;
            receiver=std::max(0.0f,terrain.Height(sx,sz));
            height=y-receiver;
        }
        point.x=sx;point.z=sz;point.receiverY=receiver;point.height=height;
        point.overLand=terrain.Height(sx,sz)>0.0f;
        point.valid=height>1.0f&&height<420.0f&&std::abs(sx)<900.0f&&std::abs(sz)<900.0f;
        return point;
    }
};

}
