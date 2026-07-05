/**
 * @file    os_utils.cpp
 * @author  Marvin Smith
 * @date    2026-07-03
 *
 * @brief   OS-specific utility functions for SDL platform
 */
#include "os_utils.hpp"

// C++ Standard Libraries
#include <array>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

// Project Libraries
#include <overboard/log/stdout_logger.hpp>

namespace ovb::hal::sdl {

// Platform detection
#if defined(__APPLE__)
    [[maybe_unused]] constexpr bool IS_MACOS = true;
    [[maybe_unused]] constexpr bool IS_LINUX = false;
#elif defined(__linux__)
    [[maybe_unused]] constexpr bool IS_MACOS = false;
    [[maybe_unused]] constexpr bool IS_LINUX = true;
#else
    [[maybe_unused]] constexpr bool IS_MACOS = false;
    [[maybe_unused]] constexpr bool IS_LINUX = false;
#endif

/***************************/
/*        has_wifi         */
/***************************/
bool has_wifi() {
#if IS_MACOS
    // On macOS, check if Wi-Fi interface exists
    FILE* pipe = popen("networksetup -listallhardwareports 2>/dev/null", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool has_wifi = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        if (line.find("Wi-Fi") != std::string::npos || line.find("AirPort") != std::string::npos) {
            has_wifi = true;
            break;
        }
    }
    pclose(pipe);
    return has_wifi;

#elif IS_LINUX
    // On Linux, check if wireless interface exists
    FILE* pipe = popen("ls /sys/class/net/ 2>/dev/null | grep -E 'wlan|wlp'", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool has_wifi = (fgets(buffer.data(), buffer.size(), pipe) != nullptr);
    pclose(pipe);
    return has_wifi;

#else
    return false;
#endif
}

/***************************/
/*     get_wifi_status     */
/***************************/
std::optional<Wifi_Status> get_wifi_status() {
#if IS_MACOS
    // First, find the WiFi interface name
    std::string wifi_interface;
    FILE* pipe = popen("networksetup -listallhardwareports 2>/dev/null", "r");
    if (pipe) {
        std::array<char, 256> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            std::string line(buffer.data());
            if (line.find("Wi-Fi") != std::string::npos || line.find("AirPort") != std::string::npos) {
                // Next line should have the device name
                if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                    std::string device_line(buffer.data());
                    size_t start = device_line.find("Device: ");
                    if (start != std::string::npos) {
                        wifi_interface = device_line.substr(start + 8);
                        // Trim whitespace
                        wifi_interface.erase(wifi_interface.find_last_not_of(" \n\r\t") + 1);
                        break;
                    }
                }
            }
        }
        pclose(pipe);
    }

    if (wifi_interface.empty()) {
        return std::nullopt;
    }

    // Get current Wi-Fi network on macOS using the detected interface
    std::string cmd = "networksetup -getairportnetwork " + wifi_interface + " 2>/dev/null";
    pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return std::nullopt;
    }

    std::array<char, 256> buffer;
    Wifi_Status status;
    status.connected = false;
    status.signal_strength = 0;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        // Output format: "Current Wi-Fi Network: <SSID>"
        if (line.find("Current Wi-Fi Network:") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                status.ssid = line.substr(pos + 1);
                // Trim whitespace
                while (!status.ssid.empty() && (status.ssid.back() == '\n' || status.ssid.back() == ' ')) {
                    status.ssid.pop_back();
                }
                while (!status.ssid.empty() && status.ssid.front() == ' ') {
                    status.ssid.erase(0, 1);
                }
                status.connected = !status.ssid.empty();
            }
        }
    }
    pclose(pipe);

    if (status.connected) {
        // Get signal strength using airport command
        pipe = popen("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I 2>/dev/null", "r");
        if (pipe) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                std::string line(buffer.data());
                // Look for "agrCtlRSSI" which is signal strength in dBm
                if (line.find("agrCtlRSSI") != std::string::npos) {
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        int rssi = std::stoi(line.substr(pos + 1));
                        // Convert RSSI (-100 to 0 dBm) to percentage (0-100)
                        status.signal_strength = std::max(0, std::min(100, (rssi + 100)));
                    }
                }
            }
            pclose(pipe);
        }
    }

    return status;

#elif IS_LINUX
    // On Linux, read from /proc/net/wireless
    std::ifstream wireless_file("/proc/net/wireless");
    if (!wireless_file.is_open()) {
        return std::nullopt;
    }

    std::string line;
    // Skip header lines
    std::getline(wireless_file, line);
    std::getline(wireless_file, line);

    if (std::getline(wireless_file, line)) {
        // Format: interface: status link_qual level noise nwid
        Wifi_Status status;
        status.connected = true;

        // Parse signal quality (typically 0-70 or 0-100)
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string rest = line.substr(pos + 1);
            int quality = 0;
            sscanf(rest.c_str(), "%*d %d", &quality);
            status.signal_strength = std::min(100, quality);
        }

        // Get SSID from iwconfig if available
        FILE* pipe = popen("iwconfig 2>/dev/null | grep ESSID", "r");
        if (pipe) {
            std::array<char, 256> iw_buffer;
            if (fgets(iw_buffer.data(), iw_buffer.size(), pipe) != nullptr) {
                std::string iw_line(iw_buffer.data());
                // Format: ESSID:"<ssid>"
                size_t essid_pos = iw_line.find("ESSID:");
                if (essid_pos != std::string::npos) {
                    size_t start = iw_line.find('"', essid_pos);
                    size_t end = iw_line.find('"', start + 1);
                    if (start != std::string::npos && end != std::string::npos) {
                        status.ssid = iw_line.substr(start + 1, end - start - 1);
                    }
                }
            }
            pclose(pipe);
        }

        return status;
    }

    return std::nullopt;

#else
    return std::nullopt;
#endif
}

/***************************/
/*      has_bluetooth      */
/***************************/
bool has_bluetooth() {
#if IS_MACOS
    // On macOS, check if Bluetooth is available
    FILE* pipe = popen("system_profiler SPBluetoothDataType 2>/dev/null | grep -i bluetooth", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool has_bt = (fgets(buffer.data(), buffer.size(), pipe) != nullptr);
    pclose(pipe);
    return has_bt;

#elif IS_LINUX
    // On Linux, check if Bluetooth controller exists
    FILE* pipe = popen("ls /sys/class/bluetooth/ 2>/dev/null", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool has_bt = (fgets(buffer.data(), buffer.size(), pipe) != nullptr);
    pclose(pipe);
    return has_bt;

#else
    return false;
#endif
}

/***************************/
/*  get_bluetooth_connected */
/***************************/
bool get_bluetooth_connected() {
#if IS_MACOS
    // On macOS, check Bluetooth connection status
    FILE* pipe = popen("system_profiler SPBluetoothDataType 2>/dev/null | grep -i connected", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool connected = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        if (line.find("Connected") != std::string::npos && line.find("Yes") != std::string::npos) {
            connected = true;
            break;
        }
    }
    pclose(pipe);
    return connected;

#elif IS_LINUX
    // On Linux, use bluetoothctl to check connection status
    FILE* pipe = popen("bluetoothctl info 2>/dev/null | grep -i connected", "r");
    if (!pipe) {
        return false;
    }

    std::array<char, 256> buffer;
    bool connected = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        if (line.find("Connected: yes") != std::string::npos) {
            connected = true;
            break;
        }
    }
    pclose(pipe);
    return connected;

#else
    return false;
#endif
}

} // namespace ovb::hal::sdl
