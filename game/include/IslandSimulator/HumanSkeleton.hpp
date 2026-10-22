#pragma once
#include "IslandSimulator/SculptMeshBuilder.hpp"
#include <array>
#include <cstdint>

namespace island {
// A dead sailor's bones as they lie: the joints' places in the world, the way the chest and the
// face turn, and what is left of the clothes.
struct SkeletonPose {
    using Vector3=Microsoft::Xna::Framework::Vector3;
    Vector3 pelvis,chest,neck,head;
    Vector3 front;                 // the way the chest faces
    Vector3 face,headUp;           // the way the face looks, and the crown's direction
    std::array<Vector3,2> elbow,wrist,knee,ankle;   // [left, right]
    std::array<Vector3,2> fingers,toes;             // the hands' and the feet's directions
    float jaw=0.3f;                // how far the jaw has dropped (radians)
    bool trousers=true,boots=false,cap=false;
};

/** Poses: on the back or the face (`prone`) with the head along `heading`; sitting on the floor
 *  with the back to a wall, legs out along `intoRoom`; sitting on a seat slumped forward over a
 *  table along `towardTable`; curled on the right side, the head along `heading`. `floorY` is
 *  the floor's height (the seat's for the table). */
[[nodiscard]] SkeletonPose LyingSkeleton(const Microsoft::Xna::Framework::Vector3& pelvisOnFloor,
                                         const Microsoft::Xna::Framework::Vector3& heading,bool prone,float headTurn);
[[nodiscard]] SkeletonPose SittingSkeleton(const Microsoft::Xna::Framework::Vector3& seatAtWall,
                                           const Microsoft::Xna::Framework::Vector3& intoRoom);
[[nodiscard]] SkeletonPose SlumpedSkeleton(const Microsoft::Xna::Framework::Vector3& seat,
                                           const Microsoft::Xna::Framework::Vector3& towardTable,
                                           float floorY,float tableY);
[[nodiscard]] SkeletonPose CurledSkeleton(const Microsoft::Xna::Framework::Vector3& pelvisOnFloor,
                                          const Microsoft::Xna::Framework::Vector3& heading);

/** Builds the bones (skull and jaw with sockets and teeth, the spine's vertebrae, the ribs and
 *  the sternum, the collarbones and shoulder blades, the pelvis, the long bones with their
 *  knuckled ends, the hands' and the feet's small bones) and the clothes' remains, every colour
 *  times `shade` (a dim hold's gloom). Returns the triangles added. */
int BuildSkeleton(SculptMeshBuilder& builder,const SkeletonPose& pose,float shade,std::uint32_t seed);
}
