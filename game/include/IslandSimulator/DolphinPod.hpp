#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

enum class DolphinBehavior : std::uint8_t { Cruise, Surface, Dive, Leap, SpyHop };

struct DolphinState {
  float x = 0, z = 0, y = 0;
  float headingRadians = 0, pitchRadians = 0, bankRadians = 0;
  float speed = 0, tailBeat = 0, scale = 1;
  // 0..1 the tail slap: every other surfacing the flukes are raised clear of the water and
  // brought down on it, the splash where they strike.
  float tailSlap = 0;
  // 0..1 the blowhole open: as the dolphin's back breaks the surface it opens for the
  // breath and closes again as it goes under.
  float blowhole = 0;
  DolphinBehavior behavior = DolphinBehavior::Cruise;
  WildlifeSimulationTier simulationTier = WildlifeSimulationTier::Dormant;
  std::uint8_t colorVariant = 0;
};

struct DolphinHomeRange {
  float x = 0, z = 0, radius = 0;
};

/** Peaceful deterministic offshore pod with staggered breathing dives. */
class DolphinPod final {
public:
  static constexpr std::size_t DolphinCount = 4;
  static constexpr float MinimumShoreDistance = -105.0f;
  static constexpr float MaximumShoreDistance = -34.0f;
  static constexpr float FullSimulationDistance = 190.0f;
  // A leap: a ballistic arc from the surfacing depth, clear of the water at its crest.
  static constexpr float LeapSeconds = 1.5f;
  static constexpr float LeapHeight = 1.7f;
  static constexpr float ReducedSimulationDistance = 520.0f;

  explicit DolphinPod(const IslandTerrain &terrain);
  void Update(float deltaSeconds, float observerX, float observerY,
              float observerZ) noexcept;

  [[nodiscard]] std::span<const DolphinState> Dolphins() const noexcept {
    return states_;
  }
  [[nodiscard]] std::span<const DolphinHomeRange> Homes() const noexcept {
    return homes_;
  }
  [[nodiscard]] int FullSimulationCount() const noexcept { return fullCount_; }
  [[nodiscard]] std::uint32_t LeapCount() const noexcept { return leapCount_; }
  /** @brief Where the last leap began: the seabirds come to hunt where the pod is working. */
  struct Leap { float x = 0, z = 0; bool valid = false; };
  [[nodiscard]] const Leap& LastLeap() const noexcept { return lastLeap_; }
  /** @brief Where the last leap came down: the re-entry raises the sea's foam ring and a
   *  positional splash there; the ring's age runs for SplashSeconds, counted so the
   *  splash is noted once. */
  struct Splash { float x = 0, z = 0, ageSeconds = 1.0e9f; std::uint32_t count = 0; bool valid = false; bool tailSlap = false; };
  [[nodiscard]] const Splash& LastSplash() const noexcept { return splash_; }
  [[nodiscard]] std::uint32_t SplashCount() const noexcept { return splash_.count; }
  static constexpr float SplashSeconds = 2.6f;
  /** @brief Spy-hops begun: every fourth surfacing after the leap's, the dolphin slows,
   *  rises to SpyHopDepth and pitches head-up to SpyHopPitch, its head out of the water
   *  to look around for SpyHopSeconds, then cruises on. */
  [[nodiscard]] std::uint32_t SpyHopCount() const noexcept { return spyHops_; }
  [[nodiscard]] std::size_t SpyHoppingCount() const noexcept;
  static constexpr float SpyHopSeconds = 3.0f, SpyHopDepth = -0.22f, SpyHopPitch = 1.15f, SpyHopSpeed = 0.25f;
  /** @brief Tail slaps struck: every other surfacing (the count's second of four) raises the
   *  flukes over TailSlapSeconds and brings them down, the strike noted as a splash. */
  [[nodiscard]] std::uint32_t TailSlapCount() const noexcept { return tailSlaps_; }
  static constexpr float TailSlapSeconds = 1.0f;
  /** @brief Breaths taken (the blowhole opening as the back breaks the surface). */
  [[nodiscard]] std::uint32_t BreathCount() const noexcept { return breaths_; }
  static constexpr float BreathSeconds = 0.7f, BreathDepth = -0.30f;
  /** @brief The shark this frame (while present): a dolphin within BerthDistance of it
   *  turns off its route to keep the berth, a little faster, until it is clear. */
  void SetThreat(float x, float z, bool valid) noexcept;
  [[nodiscard]] std::uint32_t BerthCount() const noexcept { return berths_; }
  [[nodiscard]] std::size_t BerthingCount() const noexcept;
  static constexpr float BerthDistance = 28.0f;
  [[nodiscard]] int ReducedSimulationCount() const noexcept {
    return reducedCount_;
  }
  [[nodiscard]] int DormantSimulationCount() const noexcept {
    return dormantCount_;
  }

private:
  struct Agent {
    DolphinState state;
    DolphinHomeRange home;
    float routePhase = 0, depthPhase = 0, tailPhase = 0, behaviorSeconds = 0,
          accumulatedSeconds = 0;
    std::uint32_t transitionCount = 0;
    bool berthing = false;
    float slapSeconds = 0;    // remaining tail slap on this surfacing; 0 when none
    float breathSeconds = 0;  // remaining open blowhole at the breath
    bool belowBreath = true;  // the back was under the breath depth last update
    bool slapped = false;     // the flukes have struck the water this slap
  };

  void Initialize(std::size_t index);
  void UpdateAgent(Agent &, float deltaSeconds) noexcept;
  [[nodiscard]] bool ValidHabitat(float x, float z,
                                  const DolphinHomeRange &) const noexcept;
  [[nodiscard]] float DesiredDepth(const Agent &, float seabed) const noexcept;

  const IslandTerrain &terrain_;
  std::vector<Agent> agents_;
  std::vector<DolphinState> states_;
  std::vector<DolphinHomeRange> homes_;
  float elapsedSeconds_ = 0;
  std::uint64_t frameIndex_ = 0;
  int fullCount_ = 0, reducedCount_ = 0, dormantCount_ = 0;
  std::uint32_t leapCount_ = 0;
  std::uint32_t spyHops_ = 0;
  std::uint32_t tailSlaps_ = 0;
  std::uint32_t breaths_ = 0;
  Leap lastLeap_;
  Splash splash_;
  float threatX_ = 0, threatZ_ = 0;
  bool threatValid_ = false;
  std::uint32_t berths_ = 0;
};
} // namespace island
