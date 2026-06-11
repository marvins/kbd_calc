/**
 * @file    system_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   SDL platform system information implementation
 *
 * For SDL (simulator), we use host OS APIs:
 * - macOS: IOKit for CPU temp, host_statistics for memory
 * - Linux: /sys/class/thermal for temp, statfs for storage
 * Battery is typically unavailable/mocked on development machines.
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_system_info.hpp>

namespace ovb::hal::sdl {

/**
 * @brief SDL platform system info implementation
 *
 * Uses host OS APIs to gather system information.
 * Some sensors (battery) may be unavailable on typical dev machines.
 */
class SDL_System_Info : public I_System_Info {
    public:
        /**
         * @brief Constructor
         */
        SDL_System_Info();

        /**
         * @brief Get current system information
         */
        System_Info get_info() override;

        /**
         * @brief Check if CPU temperature is available
         */
        bool has_cpu_temp() const override;

        /**
         * @brief Check if battery info is available
         */
        bool has_battery() const override;

        /**
         * @brief Check if storage stats are available
         */
        bool has_storage() const override;

        /**
         * @brief Check if USB status is available
         */
        bool has_usb() const override;

        /**
         * @brief Check if Bluetooth status is available
         */
        bool has_bluetooth() const override;

    private:
        /**
         * @brief Read CPU temperature (platform-specific)
         */
        std::optional<float> read_cpu_temp();

        /**
         * @brief Read storage stats for the application directory
         */
        std::optional<System_Info::Storage> read_storage();

        /**
         * @brief Platform detection (set at compile time via defines)
         */
        #if defined(__APPLE__)
            static constexpr bool is_macos { true };
            static constexpr bool is_linux { false };
        #elif defined(__linux__)
            static constexpr bool is_macos { false };
            static constexpr bool is_linux { true };
        #else
            static constexpr bool is_macos { false };
            static constexpr bool is_linux { false };
        #endif
};

} // namespace ovb::hal::sdl
