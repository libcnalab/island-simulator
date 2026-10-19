#pragma once
#include "IslandSimulator/RenderedTerrainSurface.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <vector>

namespace island {
struct StreamBedVertex {float x,y,z;};
// Local dense-grid replacement of the coarse terrain, not an overlay. Integration must
// remove the original covered terrain triangles and use SampleHeight for matching
// collision queries. Coarse cells that the height rule leaves untouched become fans in
// the original planes; cells it changes are subdivided. A vertex on the patch boundary
// must keep its original height so the edge meets the untouched terrain exactly.
class LocalTerrainPatch {
public:
    // Replacement height for a dense vertex, given the original rendered surface height.
    using HeightRule=std::function<float(float x,float z,float base)>;
    struct CoarseRange {int firstX,lastX,firstZ,lastZ;};
    static constexpr float CoarseSpacing=2*TerrainMeshLayout::Extent/TerrainMeshLayout::QuadsPerAxis;
    [[nodiscard]] static int CoarseIndexBelow(float p) noexcept {
        return static_cast<int>(std::floor((p+TerrainMeshLayout::Extent)/CoarseSpacing));
    }
    [[nodiscard]] static int CoarseIndexAbove(float p) noexcept {
        return static_cast<int>(std::ceil((p+TerrainMeshLayout::Extent)/CoarseSpacing));
    }
    LocalTerrainPatch(const CoarseRange range,const int subdivision,
                      const RenderedTerrainSurface& original,const HeightRule& rule) {
        minX_=TerrainMeshLayout::Coordinate(range.firstX);minZ_=TerrainMeshLayout::Coordinate(range.firstZ);
        columns_=(range.lastX-range.firstX)*subdivision+1;rows_=(range.lastZ-range.firstZ)*subdivision+1;
        step_=CoarseSpacing/subdivision;
        if(static_cast<std::size_t>(columns_)*rows_>=65536)
            throw std::runtime_error("Terrain patch exceeds portable index capacity");
        const int coarseColumns=range.lastX-range.firstX,coarseRows=range.lastZ-range.firstZ;
        std::vector<bool> refined(static_cast<std::size_t>(coarseColumns*coarseRows),false);
        vertices_.reserve(static_cast<std::size_t>(columns_)*rows_);
        for(int row=0;row<rows_;++row)for(int col=0;col<columns_;++col) {
            const float x=minX_+static_cast<float>(col)*step_;
            const float z=minZ_+static_cast<float>(row)*step_;
            const float base=original.Sample(x,z).height;
            const float height=rule(x,z,base);
            vertices_.push_back({x,height,z});
            heights_.push_back(height);
            if(height!=base) {
                // A changed coarse-edge vertex refines both adjacent cells.
                for(int r=std::max(0,(row-1)/subdivision);r<=std::min(coarseRows-1,row/subdivision);++r)
                    for(int c=std::max(0,(col-1)/subdivision);c<=std::min(coarseColumns-1,col/subdivision);++c)
                        refined[static_cast<std::size_t>(r*coarseColumns+c)]=true;
            }
        }
        const auto dense=[&](int row,int col){return row>=0&&col>=0&&row<coarseRows&&col<coarseColumns
            &&refined[static_cast<std::size_t>(row*coarseColumns+col)];};
        const auto index=[&](int row,int col){return static_cast<std::uint16_t>(row*columns_+col);};
        for(int r=0;r<coarseRows;++r)for(int c=0;c<coarseColumns;++c) {
            const int row=r*subdivision,col=c*subdivision;
            if(dense(r,c)) {
                for(int y=0;y<subdivision;++y)for(int x=0;x<subdivision;++x) {
                    const auto tl=index(row+y,col+x),tr=index(row+y,col+x+1);
                    const auto bl=index(row+y+1,col+x),br=index(row+y+1,col+x+1);
                    indices_.insert(indices_.end(),{tl,bl,tr,tr,bl,br});
                }
            } else {
                // Fan centre lies on the original TR-BL diagonal. Each fan sector
                // stays in one original plane; only edges next to fine cells split.
                const auto centre=index(row+subdivision/2,col+subdivision/2);
                const auto edge=[&](int y,int x,int dy,int dx2,bool split) {
                    const int stride=split?1:subdivision;
                    for(int k=0;k<subdivision;k+=stride)
                        indices_.insert(indices_.end(),{centre,index(row+y+dy*k,col+x+dx2*k),
                            index(row+y+dy*(k+stride),col+x+dx2*(k+stride))});
                };
                edge(0,0,1,0,dense(r,c-1));
                edge(subdivision,0,0,1,dense(r+1,c));
                edge(subdivision,subdivision,-1,0,dense(r,c+1));
                edge(0,subdivision,0,-1,dense(r-1,c));
            }
        }
        // Compact unused dense-grid vertices; keep only scalar heights for queries.
        std::vector<int> remap(vertices_.size(),-1);
        std::vector<StreamBedVertex> compact;
        for(auto& i:indices_) {
            if(remap[i]<0) {remap[i]=static_cast<int>(compact.size());compact.push_back(vertices_[i]);}
            i=static_cast<std::uint16_t>(remap[i]);
        }
        vertices_=std::move(compact);
    }
    virtual ~LocalTerrainPatch()=default;
    LocalTerrainPatch(const LocalTerrainPatch&)=delete;
    LocalTerrainPatch& operator=(const LocalTerrainPatch&)=delete;
    [[nodiscard]] const auto& Vertices() const noexcept{return vertices_;}
    [[nodiscard]] const auto& Indices() const noexcept{return indices_;}
    [[nodiscard]] int Columns() const noexcept{return columns_;}
    [[nodiscard]] int Rows() const noexcept{return rows_;}
    [[nodiscard]] bool Contains(float x,float z) const noexcept {
        return std::isfinite(x)&&std::isfinite(z)&&x>=minX_&&z>=minZ_
            &&x<=minX_+static_cast<float>(columns_-1)*step_
            &&z<=minZ_+static_cast<float>(rows_-1)*step_;
    }
    [[nodiscard]] bool IsBoundary(StreamBedVertex p) const noexcept {
        return p.x==minX_||p.z==minZ_||p.x==minX_+static_cast<float>(columns_-1)*step_
            ||p.z==minZ_+static_cast<float>(rows_-1)*step_;
    }
    [[nodiscard]] float SampleHeight(float x,float z) const {
        float gx=(x-minX_)/step_,gz=(z-minZ_)/step_;
        // Roundoff at an inclusive stored boundary must not reject its own vertex.
        if(!std::isfinite(gx)||!std::isfinite(gz)||gx<-.0001f||gz<-.0001f
            ||gx>columns_-1+.0001f||gz>rows_-1+.0001f)
            throw std::out_of_range("Outside terrain patch");
        gx=std::clamp(gx,0.0f,static_cast<float>(columns_-1));
        gz=std::clamp(gz,0.0f,static_cast<float>(rows_-1));
        const int col=std::min(static_cast<int>(gx),columns_-2),row=std::min(static_cast<int>(gz),rows_-2);
        const float u=gx-static_cast<float>(col),v=gz-static_cast<float>(row);
        const auto i=static_cast<std::size_t>(row*columns_+col);
        const float tl=heights_[i],tr=heights_[i+1],bl=heights_[i+columns_],br=heights_[i+columns_+1];
        return u+v<=1?tl+(tr-tl)*u+(bl-tl)*v:br+(bl-br)*(1-u)+(tr-br)*(1-v);
    }
private:
    float minX_=0,minZ_=0,step_=0;
    int columns_=0,rows_=0;
    std::vector<StreamBedVertex> vertices_;
    std::vector<float> heights_;
    std::vector<std::uint16_t> indices_;
};
}
