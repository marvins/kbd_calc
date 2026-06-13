/**
 * @file    settings_store.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   PicoCalc settings storage implementation (stub)
 *
 * Planned implementation: FAT32 file on SD card at /settings.toml.
 * SD card is the primary storage for PicoCalc user data.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>

namespace ovb::hal::picocalc {

/**
 * @brief PicoCalc SD card settings store (stub)
 *
 * Future implementation using FAT32 filesystem on SD card.
 * Path: /settings.toml (root of SD card)
 *
 * For now, returns empty settings (no persistence until SD/FAT implemented).
 */
class PicoCalc_Settings_Store : public I_Settings_Store {

    public:

        /**
         * @brief Constructor
         * @param path Path on SD card (default: "/settings.toml")
         */
        explicit PicoCalc_Settings_Store(const std::filesystem::path& path = "/settings.toml");

        /**
         * @brief Load settings from SD card (stub)
         */
        bool load(Settings_Tree& out_tree) override;

        /**
         * @brief Save settings to SD card (stub)
         */
        bool save(const Settings_Tree& tree) override;

        /**
         * @brief Check if settings file exists
         */
        bool exists() const override;

        /**
         * @brief Get storage location description
         */
        std::string location() const override;

        /**
         * @brief Check if SD card is writable
         */
        bool is_writable() const override;

        /**
         * @brief Initialize SD card and FAT filesystem
         * @return true if SD card mounted and ready
         */
        bool init();

    private:

        /// @brief Path to settings file on SD card
        std::filesystem::path m_path;

        /// @brief Initialization state
        bool        m_initialized { false };

        /// @brief SD card mounted state
        bool        m_sd_mounted  { false };

        // TODO: Add SD card and FATFS instance
};

} // namespace ovb::hal::picocalc
