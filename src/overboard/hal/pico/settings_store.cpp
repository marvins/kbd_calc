/**
 * @file    settings_store.cpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   RP2350 (Pico) settings storage implementation (stub)
 */
#include "settings_store.hpp"

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pico {

// Module logger
static ovb::log::Stdout_Logger s_logger(ovb::log::Log_Level::Debug);

/****************************/
/*     Constructor          */
/****************************/
Pico_Settings_Store::Pico_Settings_Store() {
    s_logger.debug("Pico_Settings_Store created (uninitialized)");
}

/****************************/
/*     Initialize           */
/****************************/
bool Pico_Settings_Store::init() {
    // TODO: Initialize LittleFS on internal flash
    // 1. Determine flash size
    // 2. Calculate LittleFS offset (leave room for program)
    // 3. Mount filesystem

    s_logger.debug("Pico_Settings_Store::init() - stub, LittleFS not yet implemented");
    m_initialized = false;
    return false;
}

/****************************/
/*     Load                 */
/****************************/
bool Pico_Settings_Store::load(Settings_Tree& out_tree) {
    if (!m_initialized) {
        s_logger.warning("Load called before init()");
        out_tree = Settings_Tree();
        return true;  // Return empty tree, not an error
    }

    // TODO: Read settings.toml from LittleFS
    s_logger.debug("Pico load() - stub");
    out_tree = Settings_Tree();
    return true;
}

/****************************/
/*     Save                 */
/****************************/
bool Pico_Settings_Store::save(const Settings_Tree& tree) {
    if (!m_initialized) {
        s_logger.error("Save called before init()");
        return false;
    }

    // TODO: Write settings.toml to LittleFS
    s_logger.debug("Pico save() - stub");
    return false;
}

/****************************/
/*     Exists               */
*****************************
bool Pico_Settings_Store::exists() const {
    if (!m_initialized) {
        return false;
    }

    // TODO: Check if settings.toml exists in LittleFS
    return false;
}

/****************************/
/*     Location             */
/****************************/
std::string Pico_Settings_Store::location() const {
    return "flash:/settings.toml (LittleFS - not implemented)";
}

/****************************/
/*     Is Writable          */
/****************************/
bool Pico_Settings_Store::is_writable() const {
    return m_initialized;
}

} // namespace ovb::hal::pico
