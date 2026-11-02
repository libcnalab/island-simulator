#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace island::castle_wall
{
    struct Point {float along=0,height=0;};
    struct Polygon {std::array<Point,6> points{};std::size_t count=0;};

    // Clip a horizontal masonry course under the actual sloping broken top. Raising the
    // low endpoint to each course's bottom instead produced unsupported triangular fins.
    inline Polygon Course(float topLeft,float topRight,float bottom,float top) noexcept
    {
        Polygon result;
        if(!std::isfinite(topLeft)||!std::isfinite(topRight)||!std::isfinite(bottom)
            ||!std::isfinite(top)||top<=bottom||std::max(topLeft,topRight)<=bottom) return result;
        const std::array<Point,4> rectangle{{{0,bottom},{1,bottom},{1,top},{0,top}}};
        const auto distance=[&](Point p) {
            return static_cast<double>(topLeft)+(static_cast<double>(topRight)-topLeft)*p.along-p.height;
        };
        const auto same=[](Point a,Point b) {
            return std::abs(a.along-b.along)<1e-6f&&std::abs(a.height-b.height)<1e-6f;
        };
        const auto append=[&](Point p) {
            if(result.count==0||!same(result.points[result.count-1],p))
                result.points[result.count++]=p;
        };
        Point previous=rectangle.back();double before=distance(previous);
        for(const Point current:rectangle) {
            const double after=distance(current);
            if((before>=0)!=(after>=0)) {
                const float t=static_cast<float>(before/(before-after));
                append({std::lerp(previous.along,current.along,t),
                        std::lerp(previous.height,current.height,t)});
            }
            if(after>=0) append(current);
            previous=current;before=after;
        }
        if(result.count>1&&same(result.points[0],result.points[result.count-1])) --result.count;
        if(result.count<3) result.count=0;
        return result;
    }
}
