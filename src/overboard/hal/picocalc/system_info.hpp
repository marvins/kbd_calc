/**
 * @file    system_info.hpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   PicoCalc platform system information implementation
 *
 * PicoCalc hardware sensors:
 * - RP2350 internal temperature sensor (ADC)
 * - MAX17048 battery fuel gauge (I2C)
 * - SD card (SPI)
 * - USB host/device (RP2040/RP2350 built-in)
 * - Bluetooth (optional module)
 */
#pragma once

// Project Libraries
#include <overboard/hal/i_system_info.hpp>

namespace ovb::hal::picocalc {

/**
 * @brief PicoCalc system info implementation
 *
 * Interfaces with PicoCalc-specific hardware:
 * - MAX17048 battery monitor via I2C
 * - RP2350 temperature sensor via ADC
 * - SD card via SPI
 * - USB via TinyUSB
 */
class PicoCalc_System_Info : public I_System_Info {
    public:
        /**
         * @brief Constructor
         */
        PicoCalc_System_Info();

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
         * @brief Initialize I2C and sensors
         */
        bool init();

    private:
        /**
         * @brief Read from RP2350 temperature sensor ADC
         */
        std::optional<float> read_cpu_temp();

        /**
         * @brief Read from MAX17048 battery fuel gauge
         */
        struct Battery_Info {
            float voltage_v;
            int   percent;
        };
        std::optional<Battery_Info> read_battery();

        /**
         * @brief Read SD card FAT32 filesystem stats
         */
        std::optional<System_Info::Storage> read_storage();

        /**
         * @brief Read USB host/device status from TinyUSB
         */
        std::optional<System_Info::USB_Status> read_usb();

        bool m_initialized { false };
        // TODO: Add I2C instance for MAX17048
        // TODO: Add SD card mount point tracking
};

} // namespace ovb::hal::picocalc
