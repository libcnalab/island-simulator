#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace island
{
    /**
     * @brief Accumulates wall time per named section of a frame, for the runtime frame report.
     *
     * Names are string literals (compared by content); a section is either timed between two
     * marks (Mark attributes the time since the previous mark to the previous name) or added
     * directly. Two clock reads per section: cheap enough to stay on in normal play.
     */
    class FrameSectionProfile final
    {
    public:
        using Clock = std::chrono::steady_clock;
        static constexpr std::size_t Capacity = 128U;

        void Begin(const char* name) noexcept
        {
            current_ = name;
            last_ = Clock::now();
        }

        void Mark(const char* name) noexcept
        {
            const auto now = Clock::now();
            if (current_ != nullptr) Add(current_, std::chrono::duration<double, std::milli>(now - last_).count());
            current_ = name;
            last_ = now;
        }

        void End() noexcept { Mark(nullptr); }

        void Add(std::string_view name, const double milliseconds) noexcept
        {
            for (std::size_t index = 0; index < count_; ++index) {
                if (names_[index] == name) {
                    milliseconds_[index] += milliseconds;
                    return;
                }
            }
            if (count_ == Capacity) return;
            names_[count_] = name;
            milliseconds_[count_] = milliseconds;
            ++count_;
        }

        void Reset() noexcept
        {
            count_ = 0U;
            current_ = nullptr;
        }

        [[nodiscard]] double Total() const noexcept
        {
            double total = 0.0;
            for (std::size_t index = 0; index < count_; ++index) total += milliseconds_[index];
            return total;
        }

        /** @brief The largest sections, each divided by `per` (frames or updates). */
        [[nodiscard]] std::string Top(const double per, const std::size_t limit) const
        {
            std::array<std::size_t, Capacity> order{};
            for (std::size_t index = 0; index < count_; ++index) order[index] = index;
            std::sort(order.begin(), order.begin() + static_cast<std::ptrdiff_t>(count_),
                      [this](const std::size_t a, const std::size_t b) {
                          return milliseconds_[a] > milliseconds_[b];
                      });
            std::ostringstream text;
            text << std::fixed << std::setprecision(2);
            const double divisor = per > 0.0 ? per : 1.0;
            for (std::size_t rank = 0; rank < std::min(limit, count_); ++rank) {
                const std::size_t index = order[rank];
                if (rank > 0U) text << ' ';
                text << names_[index] << '=' << milliseconds_[index] / divisor;
            }
            return text.str();
        }

    private:
        std::array<std::string_view, Capacity> names_{};
        std::array<double, Capacity> milliseconds_{};
        std::size_t count_ = 0U;
        const char* current_ = nullptr;
        Clock::time_point last_{};
    };
}
