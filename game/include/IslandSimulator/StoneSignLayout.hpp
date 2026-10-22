#pragma once
#include "IslandSimulator/WorldCollision.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** One stone of the castaway's sign. */
struct SignStone {
    float x=0,z=0;
    float radius=0.14f;      // across, metres
    float yaw=0;
    std::uint8_t tone=0;     // 0..3 dark volcanic stones, 4 a white lump of dead coral
    bool operator==(const SignStone&) const=default;
};

/** Where the castaway laid out HELP ME in stones on the beach: the letters' baseline along the
 *  upper beach, each letter's top toward the land, so it reads from the sea and from the air. */
struct StoneSignPlacement {
    static constexpr float LetterHeight=2.4f,LetterWidth=1.6f,LetterGap=0.6f,WordGap=1.6f;
    float x=0,z=0;            // the start of the baseline (the H's foot)
    float alongX=1,alongZ=0;  // unit direction the text runs (left to right as read from the sea)
    float upX=0,upZ=1;        // unit direction of the letters' tops (inland)
    [[nodiscard]] static float Length() noexcept;   // the whole line, H to E
    [[nodiscard]] std::array<float,2> At(float along,float up) const noexcept {
        return {{x+alongX*along+upX*up,z+alongZ*along+upZ*up}};
    }
    /** The stones along the letters' strokes, about every 0.3 m, a little jittered. */
    [[nodiscard]] std::vector<SignStone> Stones() const;
};

class StoneSignLayout final {
public:
    static constexpr float InlandFromShore=7.0f;
    /** The first stretch of open, level dry sand round the island (sunwise from the south) that
     *  holds the whole sign clear of the obstacles by 2 m and at least `keepAway` metres from
     *  each of the `avoid` points (the whale's bones, the jetty, the turtle nests). */
    [[nodiscard]] static std::optional<StoneSignPlacement> FindInWorld(
        const IslandTerrain& terrain,std::span<const CircularObstacle> obstacles,
        std::span<const std::array<float,2>> avoid,float keepAway=14.0f);
};
}
