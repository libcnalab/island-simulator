#pragma once
#include <algorithm>

namespace island {
inline constexpr float InspectionStepSeconds=.1f;
// Captures and named benchmark workloads must visit the same simulated states even
// when their wall-clock rendering speeds differ. Ordinary exploration keeps real time.
[[nodiscard]] inline float InspectionSimulationStep(bool fixed,float elapsedSeconds) noexcept {
    return fixed?InspectionStepSeconds:std::min(.1f,elapsedSeconds);
}
}
