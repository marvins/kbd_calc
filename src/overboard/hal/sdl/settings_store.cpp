/**
 * @file    settings_store.cpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   SDL platform settings storage implementation
 */
#include "settings_store.hpp"

// C++ Standard Libraries
#include <cstdlib>
#include <filesystem>
#include <fstream>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

// Module logger
static ovb::log::Stdout_Logger s_logger(ovb::log::Log_Level::Debug);

/****************************/
/*     Constructor          */
/****************************/
SDL_Settings_Store::SDL_Settings_Store(
    const std::string& app_name,
    const std::string& file_name
) {
    m_config_dir = get_platform_config_dir(app_name);
    m_settings_file = m_config_dir / file_name;

    s_logger.debug("SDL_Settings_Store: config_dir={}, file={}",
                   m_config_dir.string(), m_settings_file.string());
}

/****************************/
/*     Load                 */
/****************************/
bool SDL_Settings_Store::load(Settings_Tree& out_tree) {
    // Start with the defaults template so all keys are always present
    Settings_Tree merged;
    const std::filesystem::path defaults_path { "data/configs/settings.toml" };
    if (std::filesystem::exists(defaults_path)) {
        try {
            toml::table defaults = toml::parse_file(defaults_path.string());
            merged = Settings_Tree(std::move(defaults));
            s_logger.debug("Loaded defaults from {}", defaults_path.string());
        } catch (const toml::parse_error& e) {
            s_logger.warn("Failed to parse defaults file: {}", std::string(e.what()));
        }
    }

    // Overlay user settings on top of defaults
    if (exists()) {
        try {
            toml::table user_table = toml::parse_file(m_settings_file.string());
            Settings_Tree user_tree(std::move(user_table));
            merged.merge(user_tree);
            s_logger.debug("Overlaid user settings from {}", m_settings_file.string());
        } catch (const toml::parse_error& e) {
            s_logger.error("Failed to parse user settings file: {} - {}",
                           m_settings_file.string(), std::string(e.what()));
            // Continue with defaults only
        }
    } else {
        s_logger.debug("No user settings file found, using defaults only");
    }

    out_tree = std::move(merged);
    return true;
}

/****************************/
/*     Save                 */
/****************************/
bool SDL_Settings_Store::save(const Settings_Tree& tree) {
    if (!ensure_config_dir()) {
        s_logger.error("Failed to create config directory: {}", m_config_dir.string());
        return false;
    }

    std::ofstream file(m_settings_file);
    if (!file.is_open()) {
        s_logger.error("Failed to open settings file for writing: {}",
                       m_settings_file.string());
        return false;
    }

    // Serialize TOML
    file << tree.data();

    if (!file.good()) {
        s_logger.error("Failed to write settings file: {}", m_settings_file.string());
        return false;
    }

    s_logger.debug("Saved settings to {}", m_settings_file.string());
    return true;
}

/****************************/
/*     Exists               */
/****************************/
bool SDL_Settings_Store::exists() const {
    return std::filesystem::exists(m_settings_file);
}

/****************************/
/*     Location             */
/****************************/
std::string SDL_Settings_Store::location() const {
    return m_settings_file.string();
}

/****************************/
/*     Is Writable          */
/****************************/
bool SDL_Settings_Store::is_writable() const {
    // Check if parent directory exists and is writable
    std::filesystem::path parent = m_config_dir;

    // If config dir doesn't exist, check if we can create it
    if (!std::filesystem::exists(parent)) {
        std::filesystem::path grandparent = parent.parent_path();
        if (std::filesystem::exists(grandparent)) {
            auto perms = std::filesystem::status(grandparent).permissions();
            // Rough check - not comprehensive
            return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
        }
        return false;
    }

    auto perms = std::filesystem::status(parent).permissions();
    return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
}

/****************************/
/*     Get Platform Dir     */
/****************************/
std::filesystem::path SDL_Settings_Store::get_platform_config_dir(
    const std::string& app_name
) {
    #if defined(__linux__)
    {
        // XDG Base Directory spec
        const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
        if (xdg_config) {
            return std::filesystem::path(xdg_config) / app_name;
        }

        const char* home = std::getenv("HOME");
        if (home) {
            return std::filesystem::path(home) / ".config" / app_name;
        }

        // Fallback
        return std::filesystem::path("/tmp") / (app_name + "_config");
    }
    #elif defined(__APPLE__)
    {
        // macOS: ~/Library/Application Support/
        const char* home = std::getenv("HOME");
        if (home) {
            return std::filesystem::path(home) /
                   "Library" / "Application Support" / app_name;
        }

        return std::filesystem::path("/tmp") / (app_name + "_config");
    }
    #elif defined(_WIN32)
    {
        // Windows: %APPDATA%
        const char* appdata = std::getenv("APPDATA");
        if (appdata) {
            return std::filesystem::path(appdata) / app_name;
        }

        return std::filesystem::path("C:\\temp") / (app_name + "_config");
    }
    #else
    {
        // Unknown platform - use /tmp
        return std::filesystem::path("/tmp") / (app_name + "_config");
    }
    #endif
}

/****************************/
/*     Ensure Config Dir    */
/****************************/
bool SDL_Settings_Store::ensure_config_dir() {
    if (std::filesystem::exists(m_config_dir)) {
        return true;
    }

    try {
        std::filesystem::create_directories(m_config_dir);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        s_logger.error("Failed to create config directory: {}", std::string(e.what()));
        return false;
    }
}

} // namespace ovb::hal::sdl
