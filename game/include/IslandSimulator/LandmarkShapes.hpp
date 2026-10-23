#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace island {

/** The ground plan of the landmarks' drawn shapes where they stand within a height band over
 *  the ground (the quad's: its wheels, body and the rider's legs), as square cells: what a
 *  vehicle can not drive through, read from the triangles themselves rather than from circles
 *  fitted round them. Cells a smaller step does not reach (a stone, a ledge lower than the
 *  band) are left free to be driven over. */
class LandmarkShapes final {
public:
    static constexpr float Cell=0.2f;
    using Ground=std::function<float(float,float)>;

    /** One plan and the height band over the ground it keeps. */
    struct Band {LandmarkShapes& shapes;float low,high;};
    /** Adds a landmark's triangle list (three corners to a triangle, world positions) to each
     *  band's plan in one pass; returns the cells it newly takes in the first. */
    static std::size_t Add(std::string_view landmark,const std::vector<std::array<float,3>>& triangles,const Ground& ground,
                           std::initializer_list<Band> bands);
    std::size_t Add(std::string_view landmark,const std::vector<std::array<float,3>>& triangles,const Ground& ground,
                    float low,float high) {return Add(landmark,triangles,ground,{Band{*this,low,high}});}
    /** The landmark whose shape stands over the point, or empty. */
    [[nodiscard]] std::string_view At(float x,float z) const noexcept;
    [[nodiscard]] bool Occupied(float x,float z) const noexcept {return cells_.contains(Key(x,z));}
    [[nodiscard]] std::size_t CellCount() const noexcept {return cells_.size();}

private:
    [[nodiscard]] static std::uint64_t Key(float x,float z) noexcept;
    std::unordered_map<std::uint64_t,std::uint16_t> cells_;
    std::vector<std::string> names_;
};

namespace landmark_shapes {

/** The collector the landmark renderers hand their triangles to while the world is built (set
 *  by the game for that time only). */
using Sink=std::function<void(std::string_view landmark,const std::vector<std::array<float,3>>& triangles)>;

inline Sink& Current() {
    static Sink sink;
    return sink;
}

/** Hands a renderer's triangle list (vertices with a `Position`) to the collector, if one listens. */
template<class Mesh>
void Note(const std::string_view landmark,const Mesh& mesh) {
    if(!Current()) return;
    std::vector<std::array<float,3>> triangles;
    triangles.reserve(mesh.size());
    for(const auto& vertex:mesh) triangles.push_back({{vertex.Position.X,vertex.Position.Y,vertex.Position.Z}});
    Current()(landmark,triangles);
}

}
}
