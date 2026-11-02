#include "IslandSimulator/CastleWallSection.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
try {
    using namespace island::castle_wall;
    const auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
    std::size_t polygons=0;
    for(float a:{-.5f,.1f,.6f,1.2f,3.3f,6.2f,10.4f})
        for(float b:{-.5f,.1f,.6f,1.2f,3.3f,6.2f,10.4f})
            for(float floor:{-.5f,0.0f,2.4f,4.0f})
                for(float y=floor;y<11;y+=.6f) {
                    const auto p=Course(a,b,y,y+.6f);
                    require(p.count==0||(p.count>=3&&p.count<=5),"Invalid clipped wall polygon");
                    if(!p.count)continue;
                    double area=0;
                    for(std::size_t k=0;k<p.count;++k) {
                        const auto q=p.points[k],r=p.points[(k+1)%p.count];
                        require(q.along>=0&&q.along<=1&&q.height>=y-.00001f&&q.height<=y+.60001f,
                                "Wall course escapes its band");
                        require(q.height<=std::lerp(a,b,q.along)+.00001f,
                                "Wall course creates a fin above its broken top");
                        require(std::hypot(q.along-r.along,q.height-r.height)>1e-7f,
                                "Wall course repeats a corner");
                        area+=static_cast<double>(q.along)*r.height-static_cast<double>(r.along)*q.height;
                    }
                    require(area>1e-9,"Wall course is inverted or degenerate");++polygons;
                }
    // Reproduction: a course well above the low endpoint must begin inside the column.
    const auto wing=Course(1,5,3,3.6f);
    require(wing.count>0,"Missing rising broken-wall course");
    for(std::size_t k=0;k<wing.count;++k)
        require(wing.points[k].along>=.5f,"High course grows out of the low broken endpoint");
    require(Course(2,3,4,5).count==0,"Lintel emitted below its lower boundary");
    require(Course(std::numeric_limits<float>::quiet_NaN(),3,0,1).count==0,
            "Invalid broken-wall height accepted");
    std::cout<<"Clipped broken-wall courses="<<polygons<<"; no protruding fins\n";
}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
