/**
 * @file    system_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Pi Zero platform system information implementation
 *
 * Pi Zero uses standard Linux interfaces:
 * - /sys/class/thermal/thermal_zone0/temp for CPU temp
 * - /sys/class/power_supply/ for battery (if LiPo HAT present)
 * - Standard statvfs for storage
 * - /sys/kernel/debug/usb/devices or similar for USB
 * - hciconfig or bluetoothctl for BT
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_system_info.hpp>

namespace ovb::hal::pi_zero {

/**
 * @brief Pi Zero system info implementation
 *
 * Uses standard Linux sysfs interfaces for hardware queries.
 * Some features depend on HATs or external hardware.
 */
class PiZero_System_Info : public I_System_Info {
    public:
        /**
         * @brief Constructor
         */
        PiZero_System_Info();

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

        /**
         * @brief Initialize - probe available sensors
         */
        bool init();

    private:
        /**
         * @brief Read BCM2835/BCM2836 temperature
         */
        std::optional<float> read_cpu_temp();

        /**
         * @brief Read from power_supply sysfs (if battery HAT attached)
         */
        struct Battery_Info {
            float voltage_v;
            int   percent;
        };
        std::optional<Battery_Info> read_battery();

        /**
         * @brief Read SD card or USB storage stats
         */
        std::optional<System_Info::Storage> read_storage();

        /**
         * @brief Read USB gadget mode status
         */
        std::optional<System_Info::USB_Status> read_usb();

        /**
         * @brief Read Bluetooth status via hciconfig or bluetoothctl
         */
        std::optional<System_Info::BT_Status> read_bluetooth();

        bool m_initialized { false };
        bool m_has_battery { false };  // Detected at init
        bool m_has_bt      { false };  // Detected at init
};

} // namespace ovb::hal::pi_zero
