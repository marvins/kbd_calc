/**
 * @file    i_system_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Platform-agnostic system information interface
 *
 * Provides access to hardware sensors and system status across
 * all supported platforms (PicoCalc, SDL, Pi Zero).
 */
#pragma once

// C++ Standard Libraries
#include <optional>
#include <string>

namespace ovb::hal {

/**
 * @brief System information data structure
 */
struct System_Info {
    // CPU temperature in Celsius (nullopt if unavailable)
    std::optional<float> cpu_temp_c;

    // Battery voltage in volts (nullopt if unavailable)
    std::optional<float> battery_voltage_v;

    // Battery percentage 0-100 (nullopt if unavailable)
    std::optional<int> battery_percent;

    // Storage info
    struct Storage {
        uint64_t total_bytes;
        uint64_t free_bytes;
        uint64_t used_bytes;
    };
    std::optional<Storage> storage;

    // USB status
    struct USB_Status {
        bool connected;
        bool host_mode;  // true = host, false = device/gadget
    };
    std::optional<USB_Status> usb;

    // Bluetooth status
    struct BT_Status {
        bool powered;
        bool connected;
        std::string device_name;  // Empty if not connected
    };
    std::optional<BT_Status> bluetooth;
};

/**
 * @brief Platform system information interface
 *
 * Abstracts hardware sensors and system queries across targets.
 * Each platform implements its own data acquisition method.
 */
class I_System_Info {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~I_System_Info() = default;

        /**
         * @brief Get current system information
         *
         * Queries hardware sensors and returns a snapshot of
         * current system state. Individual fields may be nullopt
         * if not supported on this platform.
         *
         * @return System_Info struct with available data
         */
        virtual System_Info get_info() = 0;

        /**
         * @brief Check if CPU temperature is available
         */
        virtual bool has_cpu_temp() const = 0;

        /**
         * @brief Check if battery info is available
         */
        virtual bool has_battery() const = 0;

        /**
         * @brief Check if storage stats are available
         */
        virtual bool has_storage() const = 0;

        /**
         * @brief Check if USB status is available
         */
        virtual bool has_usb() const = 0;

        /**
         * @brief Check if Bluetooth status is available
         */
        virtual bool has_bluetooth() const = 0;
};

} // namespace ovb::hal
