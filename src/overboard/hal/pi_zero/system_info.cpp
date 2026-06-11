/**
 * @file    system_info.cpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   Pi Zero platform system information implementation
 */
#include "system_info.hpp"

// C++ Standard Libraries
#include <sys/statvfs.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::pi_zero {

// Module logger
static ovb::log::Stdout_Logger s_logger(ovb::log::Log_Level::Debug);

/****************************/
/*     Constructor          */
/****************************/
PiZero_System_Info::PiZero_System_Info() {
    s_logger.debug("PiZero_System_Info created (uninitialized)");
}

/****************************/
/*     Initialize           */
/****************************/
bool PiZero_System_Info::init() {
    // Probe for optional hardware
    m_has_battery = (access("/sys/class/power_supply/BAT0", F_OK) == 0) ||
                    (access("/sys/class/power_supply/battery", F_OK) == 0);

    m_has_bt = (access("/sys/class/bluetooth", F_OK) == 0) ||
               (system("which hciconfig > /dev/null 2>&1") == 0);

    s_logger.debug("PiZero_System_Info::init() - battery={}, bt={}",
                   std::to_string(m_has_battery), std::to_string(m_has_bt));

    m_initialized = true;
    return true;
}

/****************************/
/*     Get System Info      */
/****************************/
System_Info PiZero_System_Info::get_info() {
    System_Info info;

    if (!m_initialized) {
        s_logger.warning("get_info() called before init()");
        return info;
    }

    info.cpu_temp_c = read_cpu_temp();

    auto batt = read_battery();
    if (batt) {
        info.battery_voltage_v = batt->voltage_v;
        info.battery_percent   = batt->percent;
    }

    info.storage    = read_storage();
    info.usb        = read_usb();
    info.bluetooth  = read_bluetooth();

    return info;
}

/****************************/
/*     Capability Checks    */
/****************************/
bool PiZero_System_Info::has_cpu_temp() const {
    // Pi Zero always has thermal zone
    return access("/sys/class/thermal/thermal_zone0/temp", R_OK) == 0;
}

/************************************/
/*     Check if Battery Present     */
/************************************/
bool PiZero_System_Info::has_battery() const {
    return m_has_battery;
}

/************************************/
/*     Check if Storage Present     */
/************************************/
bool PiZero_System_Info::has_storage() const {
    return true;  // Always has SD card
}

/************************************/
/*      Check if USB Present        */
/************************************/
bool PiZero_System_Info::has_usb() const {
    return true;  // Always has USB (gadget mode)
}

/************************************/
/*     Check if Bluetooth Present   */
/************************************/
bool PiZero_System_Info::has_bluetooth() const {
    return m_has_bt;
}

/****************************/
/*     Read CPU Temp        */
/****************************/
std::optional<float> PiZero_System_Info::read_cpu_temp() {
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    if (!temp_file.is_open()) {
        return std::nullopt;
    }

    int temp_millidegrees;
    temp_file >> temp_millidegrees;

    if (temp_file.good()) {
        return temp_millidegrees / 1000.0f;
    }

    return std::nullopt;
}

/****************************/
/*     Read Battery         */
/****************************/
std::optional<PiZero_System_Info::Battery_Info> PiZero_System_Info::read_battery() {
    if (!m_has_battery) {
        return std::nullopt;
    }

    // Try common battery paths
    const char* paths[] = {
        "/sys/class/power_supply/BAT0",
        "/sys/class/power_supply/battery"
    };

    for (const auto* path : paths) {
        if (access(path, F_OK) != 0) continue;

        Battery_Info info;

        // Read voltage (usually in microvolts)
        std::ifstream volt_file(std::string(path) + "/voltage_now");
        if (volt_file.is_open()) {
            int voltage_uv;
            volt_file >> voltage_uv;
            if (volt_file.good()) {
                info.voltage_v = voltage_uv / 1000000.0f;
            }
        }

        // Read capacity percentage
        std::ifstream cap_file(std::string(path) + "/capacity");
        if (cap_file.is_open()) {
            cap_file >> info.percent;
        }

        return info;
    }

    return std::nullopt;
}

/****************************/
/*     Read Storage         */
/****************************/
std::optional<System_Info::Storage> PiZero_System_Info::read_storage() {
    struct statvfs stat;

    // Get root filesystem stats
    if (statvfs("/", &stat) != 0) {
        s_logger.warning("Failed to get storage stats: {}", std::strerror(errno));
        return std::nullopt;
    }

    System_Info::Storage storage;
    storage.total_bytes = stat.f_blocks * stat.f_frsize;
    storage.free_bytes  = stat.f_bfree * stat.f_frsize;
    storage.used_bytes  = storage.total_bytes - storage.free_bytes;

    return storage;
}

/****************************/
/*     Read USB Status      */
/****************************/
std::optional<System_Info::USB_Status> PiZero_System_Info::read_usb() {
    // Pi Zero is typically in USB gadget mode
    // Could check /sys/kernel/debug/usb/devices or dwc2 driver status
    // For now, report connected=true, host_mode=false (gadget)
    System_Info::USB_Status status;
    status.connected = true;   // TODO: Detect actual USB connection
    status.host_mode = false;  // Pi Zero is typically gadget mode
    return status;
}

/****************************/
/*     Read Bluetooth       */
/****************************/
std::optional<System_Info::BT_Status> PiZero_System_Info::read_bluetooth() {
    if (!m_has_bt) {
        return std::nullopt;
    }

    // TODO: Use hciconfig or bluetoothctl to get actual status
    System_Info::BT_Status status;
    status.powered   = true;   // TODO: Detect actual power state
    status.connected = false;  // TODO: Check for active connections
    return status;
}

} // namespace ovb::hal::pi_zero
