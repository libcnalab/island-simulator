#pragma once
#include <array>
#include <vector>

namespace island {
class IslandTerrain;

/** A hot-air balloon drifting over the island: a figure of eight that crosses the island's
 *  middle and swings out over the reef and the sea round each end, the whole figure turning
 *  slowly so that each crossing takes a new line over the island; some 4 m/s, 80 to 130 m over
 *  the highest ground near it (well over the volcano's crater), the basket rocking a little
 *  under the envelope, the burner fired in bursts of a few seconds. Deterministic in the clock. */
class BalloonFlight final {
public:
    static constexpr float Speed=4.0f;               // metres a second along its figure
    static constexpr float Reach=300.0f;             // the figure's half-length from the island's middle
    static constexpr float Width=150.0f;             // the loops' half-width
    static constexpr float Clearance=80.0f;          // its least height over the ground near it
    static constexpr float EnvelopeHeight=18.0f,EnvelopeRadius=7.6f;
    static constexpr float BasketBelow=4.2f;         // the basket's floor under the envelope's mouth
    struct State {
        float x=0,y=0,z=0;        // the envelope's mouth (the basket hangs under it)
        float heading=0;          // the way it drifts (radians, atan2(z, x))
        float spin=0;             // the envelope's slow turn about its axis (radians)
        float tiltX=0,tiltZ=0;    // the whole balloon's small rock (radians)
        float burner=0;           // 0..1 the burner's flame
    };
    explicit BalloonFlight(const IslandTerrain& terrain);
    [[nodiscard]] State At(float seconds) const noexcept;
    /** The ground under it, smoothed: the highest ground within some 60 m (a coarse grid). */
    [[nodiscard]] float Ceiling(float x,float z) const noexcept;
    /** The burns begun before `seconds` (for the burner's roar). */
    [[nodiscard]] static int BurnCount(float seconds) noexcept;
    [[nodiscard]] static bool Burning(float seconds) noexcept;

private:
    static constexpr int Cells=48;
    static constexpr float CellSize=20.0f;           // the grid spans +/- 480 m
    std::vector<float> ceiling_;
};

}
