#include "IslandSimulator/SunShadeField.hpp"
#include "IslandSimulator/EnvironmentLighting.hpp"
#include "IslandSimulator/IslandTerrain.hpp"
#include <algorithm>
#include <cmath>

namespace island {
namespace {
constexpr float SlopeX=environment::SunDirectionX/environment::SunDirectionY;
constexpr float SlopeZ=environment::SunDirectionZ/environment::SunDirectionY;
[[nodiscard]] float CellCentre(const int index) noexcept {
    return -SunShadeField::HalfExtent+(static_cast<float>(index)+0.5f)*SunShadeField::CellSize;
}
}

SunShadeField::SunShadeField(const IslandTerrain& terrain,const JungleLayout& jungle)
    : terrain_(terrain),cells_(static_cast<std::size_t>(Cells)*Cells,0U) {
    // The same discs the sunlit-subject selectors count: each solid layer at its body's height.
    for(const auto& snag:jungle.StandingSnags())
        AddDisc(snag.x,snag.y+3.0f*snag.scale,snag.z,0.6f*snag.scale,SnagShade);
    for(const auto& plant:jungle.Understory())
        AddDisc(plant.x,plant.y+2.2f*plant.scale,plant.z,1.5f*plant.scale,UnderstoryShade);
    for(const auto& bush:jungle.Bushes())
        AddDisc(bush.x,bush.y+0.9f*bush.scale,bush.z,1.1f*bush.scale,BushShade);
}

void SunShadeField::AddDisc(const float x,const float centreY,const float z,const float radius,const float shade) noexcept {
    if(!std::isfinite(x)||!std::isfinite(centreY)||!std::isfinite(z)||!std::isfinite(radius)||!std::isfinite(shade)) return;
    if(radius<=0.0f||shade<=0.0f) return;
    // Where the disc's shadow lands: the ground point whose sun line passes the disc's centre,
    // found from the ground under the disc and refined once at the landing.
    float groundY=terrain_.Height(x,z);
    float landX=x-SlopeX*std::max(0.0f,centreY-groundY),landZ=z-SlopeZ*std::max(0.0f,centreY-groundY);
    groundY=terrain_.Height(landX,landZ);
    landX=x-SlopeX*std::max(0.0f,centreY-groundY);landZ=z-SlopeZ*std::max(0.0f,centreY-groundY);
    if(std::abs(landX)>HalfExtent+radius||std::abs(landZ)>HalfExtent+radius) return;
    // A cell is shaded when its centre's sun line, taken up to the disc's height, passes
    // within the radius; a half cell's slack keeps the thinnest trunk from falling between
    // cell centres.
    const float reach=radius+0.5f*CellSize+2.0f;
    const int c0=std::clamp(static_cast<int>(std::floor((landX-reach+HalfExtent)/CellSize)),0,Cells-1);
    const int c1=std::clamp(static_cast<int>(std::ceil((landX+reach+HalfExtent)/CellSize)),0,Cells-1);
    const int r0=std::clamp(static_cast<int>(std::floor((landZ-reach+HalfExtent)/CellSize)),0,Cells-1);
    const int r1=std::clamp(static_cast<int>(std::ceil((landZ+reach+HalfExtent)/CellSize)),0,Cells-1);
    const auto level=static_cast<std::uint8_t>(std::lround(std::clamp(shade,0.0f,1.0f)*255.0f));
    bool any=false;
    for(int row=r0;row<=r1;++row) {
        const float cz=CellCentre(row);
        for(int column=c0;column<=c1;++column) {
            const float cx=CellCentre(column);
            const float rise=centreY-terrain_.Height(cx,cz);
            if(rise<=0.0f) continue;
            const float hitX=cx+SlopeX*rise,hitZ=cz+SlopeZ*rise;
            if(std::hypot(hitX-x,hitZ-z)>=radius+0.5f*CellSize) continue;
            auto& cell=cells_[static_cast<std::size_t>(row)*Cells+column];
            cell=std::max(cell,level);
            any=true;
        }
    }
    if(any) ++casters_;
}

float SunShadeField::Cell(const int column,const int row) const noexcept {
    return static_cast<float>(cells_[static_cast<std::size_t>(row)*Cells+column])/255.0f;
}

float SunShadeField::Cover(const float x,const float z) const noexcept {
    if(!std::isfinite(x)||!std::isfinite(z)||std::abs(x)>HalfExtent||std::abs(z)>HalfExtent) return 0.0f;
    const float column=std::clamp((x+HalfExtent)/CellSize-0.5f,0.0f,static_cast<float>(Cells-1));
    const float row=std::clamp((z+HalfExtent)/CellSize-0.5f,0.0f,static_cast<float>(Cells-1));
    const int ix=static_cast<int>(column),iz=static_cast<int>(row);
    const int jx=std::min(ix+1,Cells-1),jz=std::min(iz+1,Cells-1);
    const float fx=column-static_cast<float>(ix),fz=row-static_cast<float>(iz);
    return std::lerp(std::lerp(Cell(ix,iz),Cell(jx,iz),fx),std::lerp(Cell(ix,jz),Cell(jx,jz),fx),fz);
}

float SunShadeField::CoverAt(const float x,const float y,const float z,const float groundHeight) const noexcept {
    if(!std::isfinite(y)||!std::isfinite(groundHeight)) return 0.0f;
    const float height=std::max(0.0f,y-groundHeight);
    return Cover(x-SlopeX*height,z-SlopeZ*height);
}
}
