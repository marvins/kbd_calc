/**
 * @file    i_settings_store.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Platform-agnostic settings storage interface
 *
 * Abstracts persistent storage for application settings across
 * all platforms (SDL, Pi Zero, Pico, PicoCalc).
 */
#pragma once

// Project Libraries
#include "settings_tree.hpp"

namespace ovb::hal {

/**
 * @brief Settings storage backend interface
 *
 * Each platform implements its own persistence mechanism:
 * - SDL: Host filesystem (XDG spec on Linux/macOS)
 * - Pi Zero: SD card or /home/pi/.config/
 * - Pico: LittleFS on internal flash
 * - PicoCalc: FAT32 on SD card
 */
class I_Settings_Store {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_Settings_Store() = default;

        /**
         * @brief Load settings from storage
         *
         * @param out_tree Settings_Tree to populate with loaded values
         * @return true if load succeeded (even if file didn't exist - returns empty tree)
         * @return false if I/O error occurred
         */
        virtual bool load(Settings_Tree& out_tree) = 0;

        /**
         * @brief Save settings to storage
         *
         * @param tree Settings_Tree to persist
         * @return true if save succeeded
         * @return false if I/O error occurred
         */
        virtual bool save(const Settings_Tree& tree) = 0;

        /**
         * @brief Check if settings file exists in storage
         */
        virtual bool exists() const = 0;

        /**
         * @brief Get storage location description (for debugging)
         */
        virtual std::string location() const = 0;

        /**
         * @brief Check if storage is writable
         */
        virtual bool is_writable() const = 0;
};

} // namespace ovb::hal
