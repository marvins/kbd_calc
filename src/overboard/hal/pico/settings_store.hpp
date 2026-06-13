/**
 * @file    settings_store.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   RP2350 (Pico) settings storage implementation (stub)
 *
 * Planned implementation: LittleFS on internal flash memory.
 * Settings will be stored at end of flash, before the filesystem.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>

namespace ovb::hal::pico {

/**
 * @brief RP2350 internal flash settings store (stub)
 *
 * Future implementation using LittleFS on internal flash.
 * For now, returns empty settings (no persistence).
 */
class Pico_Settings_Store : public I_Settings_Store {
    public:
        /**
         * @brief Constructor
         */
        Pico_Settings_Store();

        /**
         * @brief Load settings (stub - returns empty tree)
         */
        bool load(Settings_Tree& out_tree) override;

        /**
         * @brief Save settings (stub - always fails)
         */
        bool save(const Settings_Tree& tree) override;

        /**
         * @brief Check if settings exist
         */
        bool exists() const override;

        /**
         * @brief Get storage location description
         */
        std::string location() const override;

        /**
         * @brief Check if writable (stub - returns false)
         */
        bool is_writable() const override;

        /**
         * @brief Initialize LittleFS (TODO)
         * @return true if flash/LittleFS ready
         */
        bool init();

    private:
        bool m_initialized { false };
        // TODO: Add LittleFS instance and flash offset configuration
};

} // namespace ovb::hal::pico
