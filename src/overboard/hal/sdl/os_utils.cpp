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
    #define IS_MACOS 1
    #define IS_LINUX 0
#elif defined(__linux__)
    #define IS_MACOS 0
    #define IS_LINUX 1
#else
    #define IS_MACOS 0
    #define IS_LINUX 0
#endif

/***************************/
/*        has_wifi         */
/***************************/
bool has_wifi() {
#if IS_MACOS
    // On macOS, check if Wi-Fi interface exists
    FILE* pipe = popen("networksetup -listallhardwareports 2>/dev/null", "r");
    if (!pipe) {
        LOG_DEBUG("has_wifi: popen failed");
        return false;
    }

    std::array<char, 256> buffer;
    bool found = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        if (line.find("Wi-Fi") != std::string::npos || line.find("AirPort") != std::string::npos) {
            found = true;
            break;
        }
    }
    pclose(pipe);
    return found;

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

    // macOS Sonoma+ redacts SSID from all user-space APIs without a signed entitlement.
    // Detect connectivity via scutil --nwi (interface reachable = connected).
    std::array<char, 256> buffer;
    Wifi_Status status;
    status.connected = false;
    status.signal_strength = 0;

    {
        std::string nwi_cmd = "scutil --nwi 2>/dev/null";
        FILE* nwi = popen(nwi_cmd.c_str(), "r");
        if (nwi) {
            while (fgets(buffer.data(), buffer.size(), nwi) != nullptr) {
                std::string line(buffer.data());
                // Line format: "     en0 : flags      : 0x5 (IPv4,DNS)"
                if (line.find(wifi_interface) != std::string::npos &&
                    line.find("Reachable") == std::string::npos &&
                    line.find("flags") != std::string::npos) {
                    if (line.find("IPv4") != std::string::npos) {
                        status.connected = true;
                    }
                }
            }
            pclose(nwi);
        }
    }

    if (status.connected) {
        // SSID is redacted by macOS; use the assigned IP address as display name instead
        std::string ip_cmd = "/usr/sbin/ipconfig getifaddr " + wifi_interface + " 2>/dev/null";
        FILE* ip_pipe = popen(ip_cmd.c_str(), "r");
        if (ip_pipe) {
            if (fgets(buffer.data(), buffer.size(), ip_pipe) != nullptr) {
                status.ssid = std::string(buffer.data());
                while (!status.ssid.empty() && (status.ssid.back() == '\n' || status.ssid.back() == '\r' || status.ssid.back() == ' ')) {
                    status.ssid.pop_back();
                }
            }
            pclose(ip_pipe);
        }
        if (status.ssid.empty()) {
            status.ssid = wifi_interface;
        }
    }

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
/*      wifi_enabled       */
/***************************/
bool wifi_enabled() {
#if IS_MACOS
    FILE* pipe = popen("networksetup -getairportpower en0 2>/dev/null", "r");
    if (!pipe) {
        return false;
    }
    std::array<char, 256> buffer;
    bool enabled = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string line(buffer.data());
        if (line.find("On") != std::string::npos) {
            enabled = true;
            break;
        }
    }
    pclose(pipe);
    return enabled;

#elif IS_LINUX
    FILE* pipe = popen("cat /sys/class/net/wlan0/operstate 2>/dev/null", "r");
    if (!pipe) {
        return false;
    }
    std::array<char, 64> buffer;
    bool enabled = false;
    if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string state(buffer.data());
        enabled = (state.find("up") != std::string::npos || state.find("dormant") != std::string::npos);
    }
    pclose(pipe);
    return enabled;

#else
    return false;
#endif
}

/****************************/
/*    set_wifi_enabled      */
/****************************/
void set_wifi_enabled([[maybe_unused]] bool enable) {
#if IS_MACOS
    // Determine the Wi-Fi interface name first
    std::string wifi_interface;
    FILE* pipe = popen("networksetup -listallhardwareports 2>/dev/null", "r");
    if (pipe) {
        std::array<char, 256> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            std::string line(buffer.data());
            if (line.find("Wi-Fi") != std::string::npos || line.find("AirPort") != std::string::npos) {
                if (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                    std::string device_line(buffer.data());
                    size_t start = device_line.find("Device: ");
                    if (start != std::string::npos) {
                        wifi_interface = device_line.substr(start + 8);
                        wifi_interface.erase(wifi_interface.find_last_not_of(" \n\r\t") + 1);
                        break;
                    }
                }
            }
        }
        pclose(pipe);
    }
    if (wifi_interface.empty()) {
        return;
    }
    const std::string cmd = "networksetup -setairportpower " + wifi_interface
                          + (enable ? " on" : " off") + " 2>/dev/null";
    FILE* p = popen(cmd.c_str(), "r");
    if (p) pclose(p);

#elif IS_LINUX
    const std::string cmd = std::string("ip link set wlan0 ")
                          + (enable ? "up" : "down") + " 2>/dev/null";
    FILE* p = popen(cmd.c_str(), "r");
    if (p) pclose(p);
#endif
}

/*****************************/
/*   scan_wifi_networks      */
/*****************************/
std::vector<std::string> scan_wifi_networks() {
    std::vector<std::string> networks;

#if IS_MACOS
    FILE* pipe = popen("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -s 2>/dev/null", "r");
    if (!pipe) {
        return networks;
    }
    std::array<char, 256> buffer;
    bool header_skipped = false;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        std::string line(buffer.data());
        // airport -s output: "     SSID  BSSID  RSSI  CHANNEL  HT  CC  SECURITY"
        // SSID is right-justified in the first 32 chars
        if (line.size() > 1) {
            // Trim leading spaces to get SSID (first token)
            size_t start = line.find_first_not_of(' ');
            if (start != std::string::npos) {
                size_t end = line.find(' ', start);
                std::string ssid = line.substr(start, end - start);
                if (!ssid.empty()) {
                    networks.push_back(ssid);
                }
            }
        }
    }
    pclose(pipe);

#elif IS_LINUX
    FILE* pipe = popen("iwlist wlan0 scan 2>/dev/null | grep 'ESSID' | sed 's/.*ESSID:\"\\(.*\\)\"/\\1/'", "r");
    if (!pipe) {
        return networks;
    }
    std::array<char, 256> buffer;
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        std::string ssid(buffer.data());
        // Trim trailing newline
        while (!ssid.empty() && (ssid.back() == '\n' || ssid.back() == '\r')) {
            ssid.pop_back();
        }
        if (!ssid.empty()) {
            networks.push_back(ssid);
        }
    }
    pclose(pipe);
#endif

    return networks;
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
