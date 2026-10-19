#pragma once
#include "IslandSimulator/LocalTerrainPatch.hpp"
#include "IslandSimulator/StreamCourse.hpp"
#include <limits>

namespace island {
// Local replacement patch, not an overlay. Integration must remove the original
// covered terrain triangles and use SampleHeight for matching collision queries.
class StreamBedPatch final : public LocalTerrainPatch {
public:
    static constexpr int Subdivision=8;
    static constexpr float BankBlendWidth=1.0f;
    StreamBedPatch(const StreamCourse& course,const RenderedTerrainSurface& original)
        : LocalTerrainPatch(Range(course),Subdivision,original,
                            [&course](float x,float z,float base){return Height(course,x,z,base);}) {}
private:
    [[nodiscard]] static CoarseRange Range(const StreamCourse& course) {
        const auto& a=course.Station(0);const auto& b=course.Station(course.Count()-1);
        float routeMinimumZ=a.z,routeMaximumZ=a.z;
        for(std::size_t i=1;i<course.Count();++i) {
            routeMinimumZ=std::min(routeMinimumZ,course.Station(i).z);
            routeMaximumZ=std::max(routeMaximumZ,course.Station(i).z);
        }
        return {CoarseIndexBelow(b.x-7),CoarseIndexAbove(a.x+7),
                CoarseIndexBelow(routeMinimumZ-7),CoarseIndexAbove(routeMaximumZ+7)};
    }
    [[nodiscard]] static float Height(const StreamCourse& course,const float x,const float z,
                                      const float base) {
        const float length=course.Station(course.Count()-1).distance;
        const float rawStation=StreamRoute::Along(x);
        std::size_t i=0;
        float t=0;
        if(rawStation<0) {
            t=rawStation;
        } else if(rawStation>static_cast<float>(course.Count()-1)) {
            i=course.Count()-2;
            t=rawStation-static_cast<float>(i);
        } else {
            float nearestSquared=std::numeric_limits<float>::max();
            const int approximate=static_cast<int>(rawStation);
            const auto firstCandidate=static_cast<std::size_t>(std::max(0,approximate-6));
            const auto lastCandidate=static_cast<std::size_t>(std::min(
                static_cast<int>(course.Count()-2),approximate+6));
            for(std::size_t candidate=firstCandidate;candidate<=lastCandidate;++candidate) {
                const auto& candidateA=course.Station(candidate);
                const auto& candidateB=course.Station(candidate+1);
                const float segmentX=candidateB.x-candidateA.x;
                const float segmentZ=candidateB.z-candidateA.z;
                const float candidateT=std::clamp(((x-candidateA.x)*segmentX
                    +(z-candidateA.z)*segmentZ)/(segmentX*segmentX+segmentZ*segmentZ),
                    0.0f,1.0f);
                const float deltaX=x-std::lerp(candidateA.x,candidateB.x,candidateT);
                const float deltaZ=z-std::lerp(candidateA.z,candidateB.z,candidateT);
                const float squared=deltaX*deltaX+deltaZ*deltaZ;
                if(squared<nearestSquared) {
                    nearestSquared=squared;i=candidate;t=candidateT;
                }
            }
        }
        const auto& routeA=course.Station(i);const auto& routeB=course.Station(i+1);
        const float centreX=std::lerp(routeA.x,routeB.x,t);
        const float centreZ=std::lerp(routeA.z,routeB.z,t);
        float along=std::lerp(routeA.distance,routeB.distance,t);
        const float segmentX=routeB.x-routeA.x;
        const float segmentZ=routeB.z-routeA.z;
        const float segmentLength=std::hypot(segmentX,segmentZ);
        const float normalX=segmentZ/segmentLength;
        const float normalZ=-segmentX/segmentLength;
        const float signedAcross=(x-centreX)*normalX+(z-centreZ)*normalZ;
        const float across=std::abs(signedAcross);
        float height=base;
        if(along>0&&along<length+4) {
            const float shapeT=std::clamp(t,0.0f,1.0f);
            const float width=std::lerp(routeA.halfWidth,routeB.halfWidth,shapeT)
                * StreamRoute::BankWidthScale(along,signedAcross);
            const float water=std::lerp(routeA.water,routeB.water,shapeT);
            const float bed=std::lerp(routeA.bed,routeB.bed,shapeT);
            if(across<width+BankBlendWidth) {
                const auto smooth=[](float v){v=std::clamp(v,0.0f,1.0f);return v*v*(3-2*v);};
                // A compact incised cross-section keeps useful depth under the visible
                // ribbon, then climbs through a short exposed bank instead of reading as
                // a broad wet colour painted onto otherwise flat ground.
                const float cross=std::min(1.0f,across/width);
                const float bowl=std::lerp(bed,water,std::pow(cross,2.4f));
                const float influence=(1-smooth((across-width)/BankBlendWidth))*smooth(along/2)
                    *(1-smooth((along-length)/4));
                height=std::lerp(base,std::min(base,bowl),influence);
            }
        }
        return height;
    }
};
}
