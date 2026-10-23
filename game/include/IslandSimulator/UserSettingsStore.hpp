#pragma once

#include "IslandSimulator/UserSettings.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace island
{
    /** @brief Result of loading a primary settings file and its recovery copy. */
    struct UserSettingsLoadResult
    {
        UserSettings settings;
        bool loaded = false;
        bool usedBackup = false;
        std::string warning;
    };

    /** @brief CNA StorageDevice adapter for portable, recoverable user preferences. */
    class UserSettingsStore final
    {
    public:
        [[nodiscard]] static std::unique_ptr<UserSettingsStore> Open(std::string& error);

        ~UserSettingsStore();
        UserSettingsStore(const UserSettingsStore&) = delete;
        UserSettingsStore& operator=(const UserSettingsStore&) = delete;
        UserSettingsStore(UserSettingsStore&&) = delete;
        UserSettingsStore& operator=(UserSettingsStore&&) = delete;

        [[nodiscard]] UserSettingsLoadResult Load();
        [[nodiscard]] bool Save(const UserSettings& settings, std::string& error);
        [[nodiscard]] const std::string& Location() const noexcept { return location_; }

        /** @brief Reads another small record kept beside the settings (at most `maximumBytes`);
         *  empty when it does not exist or cannot be read, the reason then in `error`. */
        [[nodiscard]] std::optional<std::string> ReadRecord(std::string_view name,
                                                            std::size_t maximumBytes,
                                                            std::string& error);
        /** @brief Replaces such a record, keeping the one it replaces as `<name>.bak`. */
        [[nodiscard]] bool WriteRecord(std::string_view name, std::string_view text,
                                       std::string& error);

    private:
        class Impl;
        UserSettingsStore(std::unique_ptr<Impl> impl, std::string location);

        std::unique_ptr<Impl> impl_;
        std::string location_;
    };
}
