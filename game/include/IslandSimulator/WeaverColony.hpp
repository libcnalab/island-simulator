#pragma once
#include "IslandSimulator/BirdFlock.hpp"
#include "IslandSimulator/VegetationLayout.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

/** A frond's midrib point a nest can be woven onto: where it is at rest in the world, the
 *  frond's outward bearing, the palm (its index in the vegetation layout) and the frond. */
struct WeaverNestSite {
    float x=0,y=0,z=0,bearing=0;
    std::uint32_t palm=0;
    int frond=0;
};

/** One woven nest hanging from its frond: the strips of leaflet it is tied on by (`hang` metres
 *  long), the chamber's size, the way its entrance spout faces, its age (0 fresh green grass,
 *  1 dried to straw) and whether it is still a bare ring the male is weaving. */
struct WeaverNest {
    std::size_t site=0;
    float hang=0.18f,size=1.0f,yaw=0,age=0;
    bool building=false;
    std::uint32_t seed=0;
};

/** A colony of village weavers in a few palms near the shore: a cluster of kidney-shaped nests
 *  woven of grass hanging from each palm's fronds, the bright yellow, black-headed males
 *  hanging upside down under their nests and fluttering their wings to the females, flying down
 *  to the grass for a fresh strip and back to weave it in, the olive females going from nest
 *  to nest to look the work over. Deterministic in the clock: no state of its own. */
class WeaverColony final {
public:
    static constexpr std::size_t ColonyPalms=3,MaximumNests=30;
    static constexpr float NestAlong=0.56f;     // where on the frond the nests are woven (0 crown .. 1 tip)
    static constexpr float BirdScale=0.115f;
    /** The palms the colony nests in: living palms on the coastal flats some 12 to 60 m in from
     *  the shore, well apart from each other (their indices in `palms`). */
    [[nodiscard]] static std::vector<std::uint32_t> ChoosePalms(std::span<const PalmPlacement> palms,
                                                                 const IslandTerrain& terrain);

    /** Nests on most of the fronds offered (one a frond, a few left bare). */
    explicit WeaverColony(std::span<const WeaverNestSite> sites);
    [[nodiscard]] std::span<const WeaverNestSite> Sites() const noexcept {return sites_;}
    [[nodiscard]] std::span<const WeaverNest> Nests() const noexcept {return nests_;}

    /** The nest's swing on its strips at the clock's `seconds`: its tilt about the world's x and
     *  z axes (radians), a little more while a bird is clinging to it or leaving it. */
    [[nodiscard]] std::array<float,2> Swing(std::size_t nest,float seconds) const noexcept;
    /** A point of the nest in the world, given in its own frame (x across, y up from the knot it
     *  hangs by, z the way the spout faces), with the frond's point moved by `offset` from rest. */
    [[nodiscard]] std::array<float,3> NestPoint(std::size_t nest,float seconds,std::array<float,3> offset,
                                                float across,float up,float ahead) const noexcept;
    /** Whether the nest's male hangs under it displaying at `seconds`, and whether his wings
     *  are beating then. */
    [[nodiscard]] bool Displaying(std::size_t nest,float seconds) const noexcept;
    [[nodiscard]] bool Beating(std::size_t nest,float seconds) const noexcept;
    /** The spout's mouth in the nest's own frame (scaled by its size). */
    [[nodiscard]] std::array<float,3> EntranceLocal(std::size_t nest) const noexcept;

    /** The colony's birds at the clock's `seconds` (written into `out`, which is cleared); the
     *  frond points' offsets from rest (one per site, or none) keep the clinging birds on their
     *  swaying nests; `ground` is the drawn ground they fly down to for grass. */
    void At(float seconds,std::span<const std::array<float,3>> offsets,
            const std::function<float(float,float)>& ground,std::vector<BirdState>& out) const;
    /** Of the birds just written: how many hang displaying under a nest, how many are on the
     *  ground, how many are flying. */
    struct Census {int displaying=0,grounded=0,flying=0,perched=0;};
    [[nodiscard]] Census Count(float seconds) const;

private:
    enum class Phase : std::uint8_t {Display,Out,Ground,Back,Roof};
    struct MalePlan {Phase phase;float k;};
    [[nodiscard]] MalePlan MaleAt(std::size_t nest,float seconds) const noexcept;
    [[nodiscard]] std::array<float,2> GrassSpot(std::size_t nest,float seconds,
                                                const std::function<float(float,float)>& ground) const;
    std::vector<WeaverNestSite> sites_;
    std::vector<WeaverNest> nests_;
};

}
