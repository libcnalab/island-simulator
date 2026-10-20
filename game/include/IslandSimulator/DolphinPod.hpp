#pragma once

#include "IslandSimulator/WildlifeSimulation.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace island {
class IslandTerrain;

enum class DolphinBehavior : std::uint8_t { Cruise, Surface, Dive, Leap };

struct DolphinState {
  float x = 0, z = 0, y = 0;
  float headingRadians = 0, pitchRadians = 0, bankRadians = 0;
  float speed = 0, tailBeat = 0, scale = 1;
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
  float threatX_ = 0, threatZ_ = 0;
  bool threatValid_ = false;
  std::uint32_t berths_ = 0;
};
} // namespace island
