/**
 * @file    settings_store.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Pi Zero settings storage implementation
 *
 * Uses standard Linux filesystem paths:
 * - /home/pi/.config/kbd_calc/settings.toml (user config)
 * - Or /etc/kbd_calc/settings.toml (system-wide fallback)
 *
 * Essentially the same as SDL implementation, but with Pi Zero specific paths.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <string>

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief Pi Zero settings store
 *
 * File-based TOML storage using standard Linux paths.
 * Suitable for Pi Zero and other Linux-based embedded targets.
 */
class PiZero_Settings_Store : public I_Settings_Store {
    public:
        /**
         * @brief Constructor
         * @param file_name Settings filename (default: "settings.toml")
         * @param use_system_path Use /etc instead of /home/pi/.config
         */
        explicit PiZero_Settings_Store(
            const std::string& file_name = "settings.toml",
            bool use_system_path = false
        );

        /**
         * @brief Load settings from file
         */
        bool load(Settings_Tree& out_tree) override;

        /**
         * @brief Save settings to file
         */
        bool save(const Settings_Tree& tree) override;

        /**
         * @brief Check if settings file exists
         */
        bool exists() const override;

        /**
         * @brief Get full path to settings file
         */
        std::string location() const override;

        /**
         * @brief Check if storage directory is writable
         */
        bool is_writable() const override;

        /**
         * @brief Get the config directory path
         */
        std::filesystem::path config_dir() const { return m_config_dir; }

    private:
        /**
         * @brief Determine config directory path
         */
        std::filesystem::path get_config_dir(bool use_system_path);

        /**
         * @brief Ensure config directory exists
         */
        bool ensure_config_dir();

        std::filesystem::path m_config_dir;
        std::filesystem::path m_settings_file;
};

} // namespace ovb::hal::pi_zero
