#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

#include "IslandSimulator/FrameSectionProfile.hpp"

namespace island
{
    /**
     * @brief Enables opt-in KHR_debug and render-pass attribution.
     *
     * Set ISLAND_GPU_DIAGNOSTICS=1 before launch. The normal game path remains unchanged when the
     * variable is absent. This diagnostic deliberately does not add sleeps, fences, or glFinish.
     */
    void InitializeGpuDiagnostics() noexcept;

    /** @brief Starts a frame in the opt-in GPU diagnostic stream. */
    // cpuBreadcrumbs records pass entry/exit without enabling any GL diagnostics.
    void BeginGpuDiagnosticFrame(std::uint64_t frame, bool cpuBreadcrumbs = false) noexcept;

    /** @brief Finishes a frame in the opt-in GPU diagnostic stream. */
    void EndGpuDiagnosticFrame() noexcept;

    /** @brief The CPU wall time of every render pass since the last reset (always recorded:
     *  two clock reads a pass), for the runtime frame report. */
    [[nodiscard]] FrameSectionProfile& RenderPassCpuProfile() noexcept;

    /** @brief RAII KHR_debug group and GL-error checkpoint for one high-level render pass. */
    class GpuDiagnosticScope final
    {
    public:
        explicit GpuDiagnosticScope(std::string_view name) noexcept;
        ~GpuDiagnosticScope() noexcept;

        GpuDiagnosticScope(const GpuDiagnosticScope&) = delete;
        GpuDiagnosticScope& operator=(const GpuDiagnosticScope&) = delete;

    private:
        std::string_view name_;
        std::chrono::steady_clock::time_point started_{};
        bool active_ = false;
        bool cpuBreadcrumbs_ = false;
        bool groupPushed_ = false;
    };
}
