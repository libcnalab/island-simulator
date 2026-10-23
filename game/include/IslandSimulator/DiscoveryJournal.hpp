#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace island {

enum class JournalKind : std::uint8_t {Place, Creature};

/** One thing the journal can record: its stable save key, the name the list shows and how near
 *  (metres) the walker must come to note it, looking at it. */
struct JournalEntry {
    std::string_view id;
    std::string_view title;
    JournalKind kind;
    float reach;
};

/** The castaway's journal of what they have found on the island: its places and the creatures
 *  living there. An entry is written once, the first time it is seen, and kept in the order the
 *  entries were found; it is saved as a short text record between runs. */
class DiscoveryJournal final {
public:
    static constexpr int CurrentSchema = 1;
    static constexpr std::size_t MaximumSerializedBytes = 8192U;

    DiscoveryJournal();
    [[nodiscard]] static std::span<const JournalEntry> Catalogue() noexcept;
    [[nodiscard]] static std::optional<std::size_t> IndexOf(std::string_view id) noexcept;
    [[nodiscard]] static int Count(JournalKind kind) noexcept;

    /** Writes the entry in; true only the first time. An index outside the catalogue is ignored. */
    bool Discover(std::size_t index);
    [[nodiscard]] bool Found(std::size_t index) const noexcept {return index<found_.size()&&found_[index];}
    [[nodiscard]] int FoundCount(JournalKind kind) const noexcept;
    [[nodiscard]] int FoundCount() const noexcept {return static_cast<int>(order_.size());}
    /** Catalogue indices in the order they were found. */
    [[nodiscard]] std::span<const std::size_t> Order() const noexcept {return order_;}
    void Clear();

    [[nodiscard]] std::string ToText() const;
    /** Reads a saved record; entries this build no longer knows are skipped, a malformed record
     *  is refused with the reason in `error`. */
    [[nodiscard]] static std::optional<DiscoveryJournal> Parse(std::string_view text,std::string& error);

    bool operator==(const DiscoveryJournal&) const=default;

private:
    std::vector<bool> found_;
    std::vector<std::size_t> order_;
};

/** Whether a thing of `radius` metres at `target` is in sight from `eye` looking along the unit
 *  `forward`: within `reach` of its edge and inside the middle of the view (a cone of
 *  `halfAngleRadians`), its own breadth widening that cone. */
[[nodiscard]] bool JournalSighted(const std::array<float,3>& eye,const std::array<float,3>& forward,
                                  const std::array<float,3>& target,float radius,float reach,
                                  float halfAngleRadians=0.5f) noexcept;

}
