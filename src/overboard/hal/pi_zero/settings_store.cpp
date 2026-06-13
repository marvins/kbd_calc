/**
 * @file    settings_store.cpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Pi Zero settings storage implementation
 */
#include "settings_store.hpp"

// C++ Standard Libraries
#include <cstdlib>
#include <fstream>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

// Module logger
static ovb::log::Stdout_Logger s_logger(ovb::log::Log_Level::Debug);

/****************************/
/*     Constructor          */
/****************************/
PiZero_Settings_Store::PiZero_Settings_Store(
    const std::string& file_name,
    bool use_system_path
) {
    m_config_dir = get_config_dir(use_system_path);
    m_settings_file = m_config_dir / file_name;

    s_logger.debug("PiZero_Settings_Store: config_dir={}, file={}",
                   m_config_dir.string(), m_settings_file.string());
}

/****************************/
/*     Load                 */
/****************************/
bool PiZero_Settings_Store::load(Settings_Tree& out_tree) {
    if (!exists()) {
        s_logger.debug("Settings file does not exist, returning empty tree");
        out_tree = Settings_Tree();
        return true;
    }

    try {
        toml::table table = toml::parse_file(m_settings_file.string());
        out_tree = Settings_Tree(std::move(table));
        s_logger.debug("Loaded settings from {}", m_settings_file.string());
        return true;
    } catch (const toml::parse_error& e) {
        s_logger.error("Failed to parse settings file: {} - {}",
                       m_settings_file.string(), std::string(e.what()));
        return false;
    }
}

/****************************/
/*     Save                 */
/****************************/
bool PiZero_Settings_Store::save(const Settings_Tree& tree) {
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
bool PiZero_Settings_Store::exists() const {
    return std::filesystem::exists(m_settings_file);
}

/****************************/
/*     Location             */
/****************************/
std::string PiZero_Settings_Store::location() const {
    return m_settings_file.string();
}

/****************************/
/*     Is Writable          */
/****************************/
bool PiZero_Settings_Store::is_writable() const {
    if (!std::filesystem::exists(m_config_dir)) {
        // Check parent
        std::filesystem::path parent = m_config_dir.parent_path();
        if (!std::filesystem::exists(parent)) {
            return false;
        }
        auto perms = std::filesystem::status(parent).permissions();
        return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
    }

    auto perms = std::filesystem::status(m_config_dir).permissions();
    return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none;
}

/****************************/
/*     Get Config Dir       */
/****************************/
std::filesystem::path PiZero_Settings_Store::get_config_dir(bool use_system_path) {
    if (use_system_path) {
        return "/etc/kbd_calc";
    }

    // Try XDG_CONFIG_HOME
    const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config) {
        return std::filesystem::path(xdg_config) / "kbd_calc";
    }

    // Try HOME
    const char* home = std::getenv("HOME");
    if (home) {
        return std::filesystem::path(home) / ".config" / "kbd_calc";
    }

    // Pi Zero specific fallback: assume pi user
    if (std::filesystem::exists("/home/pi")) {
        return "/home/pi/.config/kbd_calc";
    }

    // Final fallback
    return "/tmp/kbd_calc_config";
}

/****************************/
/*     Ensure Config Dir    */
/****************************/
bool PiZero_Settings_Store::ensure_config_dir() {
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

} // namespace ovb::hal::pi_zero
