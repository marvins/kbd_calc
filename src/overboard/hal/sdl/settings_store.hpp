/**
 * @file    settings_store.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   SDL platform settings storage implementation
 *
 * Uses host filesystem for settings storage:
 * - Linux: $XDG_CONFIG_HOME/kbd_calc/settings.toml or ~/.config/kbd_calc/settings.toml
 * - macOS: ~/Library/Application Support/kbd_calc/settings.toml
 * - Windows: %APPDATA%/kbd_calc/settings.toml
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <string>

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL platform settings store
 *
 * File-based TOML storage using platform-appropriate paths.
 * Suitable for SDL, Pi Zero, and PICO-SDL targets.
 */
class SDL_Settings_Store : public I_Settings_Store {
    public:
        /**
         * @brief Constructor
         * @param app_name Application name for config directory (default: "kbd_calc")
         * @param file_name Settings filename (default: "settings.toml")
         */
        explicit SDL_Settings_Store(
            const std::string& app_name = "kbd_calc",
            const std::string& file_name = "settings.toml"
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
         * @brief Determine platform config directory
         */
        std::filesystem::path get_platform_config_dir(const std::string& app_name);

        /**
         * @brief Ensure config directory exists
         */
        bool ensure_config_dir();

        /// @brief Configuration directory path
        std::filesystem::path m_config_dir;

        /// @brief Settings file path
        std::filesystem::path m_settings_file;
};

} // namespace ovb::hal::sdl
