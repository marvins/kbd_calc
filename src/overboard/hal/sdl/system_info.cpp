/**
 * @file    system_info.cpp
 * @author  Marvin Smith
 * @date    2026-06-10
 *
 * @brief   SDL platform system information implementation
 */
#include "system_info.hpp"
#include "os_utils.hpp"

// C++ Standard Libraries
#include <sys/statvfs.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <fstream>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

/****************************/
/*     Constructor          */
/****************************/
SDL_System_Info::SDL_System_Info() {
    LOG_DEBUG("SDL_System_Info initialized (macOS=", std::to_string(is_macos),
              ", Linux=", std::to_string(is_linux), ")");
}

/****************************/
/*     Get System Info      */
/****************************/
System_Info SDL_System_Info::get_info() {
    System_Info info;

    // CPU temperature
    info.cpu_temp_c = read_cpu_temp();

    // Battery - not available on typical dev machines
    // Could parse `pmset -g batt` on macOS, but it's slow and requires shelling out
    info.battery_voltage_v = std::nullopt;
    info.battery_percent   = std::nullopt;

    // Storage
    info.storage = read_storage();

    // USB - not meaningfully queryable in SDL context
    info.usb = std::nullopt;

    // Bluetooth - use OS utilities
    if (has_bluetooth()) {
        System_Info::BT_Status bt;
        bt.powered = true;
        bt.connected = get_bluetooth_connected();
        info.bluetooth = bt;
    } else {
        info.bluetooth = std::nullopt;
    }

    // WiFi - use OS utilities
    if (has_wifi()) {
        auto wifi_status = get_wifi_status();
        if (wifi_status) {
            System_Info::WiFi_Status wifi;
            wifi.connected = wifi_status->connected;
            wifi.ssid = wifi_status->ssid;
            wifi.signal_dbm = wifi_status->signal_strength;  // Convert percentage to dBm approximation
            info.wifi = wifi;
        } else {
            info.wifi = std::nullopt;
        }
    } else {
        info.wifi = std::nullopt;
    }

    return info;
}

/****************************/
/*     Capability Checks    */
/****************************/
bool SDL_System_Info::has_cpu_temp() const {
    // On Linux, check if thermal zone exists
    if (is_linux) {
        return access("/sys/class/thermal/thermal_zone0/temp", R_OK) == 0;
    }
    // On macOS, CPU temp is difficult to query reliably without SMC
    // We'll return false and rely on mock/powermetrics if needed
    return false;
}

/************************************************/
/*          Check if Battery is available       */
/************************************************/
bool SDL_System_Info::has_battery() const {
    // Most development machines don't expose battery in a standard way
    // macOS laptops have battery but querying requires IOKit
    return false;
}

/************************************************/
/*          Check if Storage is available       */
/************************************************/
bool SDL_System_Info::has_storage() const {
    return true;  // statvfs works everywhere
}

/************************************************/
/*          Check if USB is available           */
/************************************************/
bool SDL_System_Info::has_usb() const {
    return false;  // Not queryable in SDL context
}

/************************************************/
/*          Check if Bluetooth is available     */
/************************************************/
bool SDL_System_Info::has_bluetooth() const {
    return ovb::hal::sdl::has_bluetooth();
}

/************************************************/
/*          Check if Wifi is available          */
/************************************************/
bool SDL_System_Info::has_wifi() const {
    return ovb::hal::sdl::has_wifi();
}

/****************************/
/*     Read CPU Temp        */
/****************************/
std::optional<float> SDL_System_Info::read_cpu_temp() {
    if (is_linux) {
        // Try standard Linux thermal zone
        std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
        if (temp_file.is_open()) {
            int temp_millidegrees;
            temp_file >> temp_millidegrees;
            if (temp_file.good()) {
                return static_cast<float>(temp_millidegrees) / 1000.0f;  // Convert to Celsius
            }
        }

        // Try alternative location (some systems)
        temp_file.open("/sys/class/hwmon/hwmon0/temp1_input");
        if (temp_file.is_open()) {
            int temp_millidegrees;
            temp_file >> temp_millidegrees;
            if (temp_file.good()) {
                return static_cast<float>(temp_millidegrees) / 1000.0f;
            }
        }
    }

    #if defined(__APPLE__)
    // On macOS, we could shell out to `powermetrics` but it's slow and requires sudo
    // For now, return nullopt - the UI can show "N/A" or use a mock value
    #endif

    return std::nullopt;
}

/****************************/
/*     Read Storage         */
/****************************/
std::optional<System_Info::Storage> SDL_System_Info::read_storage() {
    struct statvfs stat;

    // Get current working directory filesystem stats
    if (statvfs(".", &stat) != 0) {
        LOG_WARN("SDL_System_Info: failed to get storage stats: ", std::strerror(errno));
        return std::nullopt;
    }

    System_Info::Storage storage;
    storage.total_bytes = stat.f_blocks * stat.f_frsize;
    storage.free_bytes  = stat.f_bfree * stat.f_frsize;
    storage.used_bytes  = storage.total_bytes - storage.free_bytes;

    return storage;
}

} // namespace ovb::hal::sdl
