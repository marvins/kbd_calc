/**
 * @file    settings_store_factory.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Factory for creating platform-appropriate settings store
 *
 * Automatically selects the correct I_Settings_Store implementation
 * based on compile-time target defines.
 */
#pragma once

// C++ Standard Libraries
#include <memory>
#include <string_view>

// Project Libraries
#include <overboard/hal/i_settings_store.hpp>

namespace ovb::hal {

/**
 * @brief Factory for creating platform-specific settings storage
 *
 * Compile-time selection ensures the correct backend is used:
 * - SDL targets: Filesystem-based storage
 * - Pi Zero: Linux filesystem storage
 * - Pico: LittleFS on internal flash (stub)
 * - PicoCalc: FAT32 on SD card (stub)
 */
class Settings_Store_Factory {

    public:

        /**
         * @brief Create appropriate settings store for current platform
         *
         * @return std::unique_ptr<I_Settings_Store> Platform-specific store
         */
        static std::unique_ptr<I_Settings_Store> create();

        /**
         * @brief Get human-readable description of active platform
         */
        static std::string_view platform_name();

        /**
         * @brief Check if this is a simulator build (SDL)
         */
        static bool is_simulator();

        /**
         * @brief Check if this is embedded hardware (RP2350, Pico, PicoCalc)
         */
        static bool is_embedded();
};

} // namespace ovb::hal
