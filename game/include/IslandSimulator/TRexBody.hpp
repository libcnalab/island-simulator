#pragma once
#include "IslandSimulator/TRexMotion.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {

/** One point of the tyrannosaur's skin as the renderer uploads it. */
struct TRexSkinVertex {
    float position[3]{};          // in the world, this frame
    float normal[3]{};            // smoothed over the surface round it, this frame
    std::uint8_t pigment[4]{};    // the hide's colour; alpha the light its folds and hollows keep
    float rest[4]{};              // where the point lies on the animal at rest (metres from the
                                  // hip joints: x to its left, y up, z ahead), w the size of its
                                  // scales there; an eye's is its own unit sphere instead
    std::uint8_t surface[4]{};    // the material (TRexMaterial), its gloss, how plainly the
                                  // scales show, unused
};
static_assert(sizeof(TRexSkinVertex)==48);

enum class TRexMaterial : std::uint8_t {Hide=0,Keratin=1,Eye=2,Mouth=3,Nostril=4};

/** The tyrannosaur's skin as one mesh of fixed topology, posed afresh each frame from its motion
 *  and smoothed over: the body lofted from the tail's tip through the hips and the deep chest to
 *  the thick S of the neck, which runs on into the skull; the deep box of the skull, broad at the
 *  cheeks and narrow over the snout, the eyes set forward under heavy brows, the hornlets before
 *  them, the bosses on the cheeks and the nostrils at the tip; the lower jaw hinged under it with
 *  the mouth's lining behind, and the teeth, hidden by the lips until the jaws part; the massive
 *  thighs flaring into the hips, the drumsticks of the shins, the metatarsi, the three padded
 *  toes with their claws and the dewclaw; the small two-fingered arms. Its pigment (a dark
 *  banded back, tawny flanks, a pale belly, dusty feet), its scales' size and its hollows are set
 *  once from the pose at rest. Renderer-independent and deterministic. */
class TRexBody final {
public:
    TRexBody();
    ~TRexBody();
    TRexBody(const TRexBody&)=delete;
    TRexBody& operator=(const TRexBody&)=delete;
    /** Poses the skin on the motion's present state, in the world. */
    void Pose(const TRexMotion& rex);
    /** Poses it on a state given in the body's own frame (the hip joints' point at the origin,
     *  x right, y up, z ahead; the leg joints too), as RestState() makes. */
    void PoseLocal(const TRexState& state);

    [[nodiscard]] std::span<const TRexSkinVertex> Vertices() const noexcept {return vertices_;}
    [[nodiscard]] std::span<const std::uint16_t> Indices() const noexcept {return indices_;}
    [[nodiscard]] std::size_t TriangleCount() const noexcept {return indices_.size()/3U;}
    /** Standing at rest (or lying, `lie` 1) in the body's own frame, the legs solved as the
     *  motion solves them with the feet on the ground below. */
    [[nodiscard]] static TRexState RestState(float lie=0.0f);

    struct Frame {TRexPoint origin,right,up,ahead;};
    /** The body's frame on the motion this moment: the hip joints' point, its right, up and ahead. */
    [[nodiscard]] static Frame FrameOf(const TRexMotion& rex);
    /** A point of the head in the world as the skin poses it: in the head's own frame (x right,
     *  y up, z ahead from the top of the neck) or, with `jaw`, the lower jaw's (from its hinge). */
    [[nodiscard]] static TRexPoint HeadPoint(const TRexState& state,const Frame& frame,float x,float y,float z,bool jaw=false);
private:
    struct Patch {
        std::uint32_t first=0;
        int rows=0,columns=0;
        bool wrap=true,startPole=false,endPole=false;
    };
    struct Recording;
    void Build(const TRexState& state,const Frame& frame);
    void Normals();
    std::vector<TRexSkinVertex> vertices_;
    std::vector<std::uint16_t> indices_;
    std::vector<Patch> patches_;
    std::vector<float> accumulated_;
    Recording* recording_=nullptr;
};

}
