/**
 * @file    os_utils.hpp
 * @author  Marvin Smith
 * @date    2026-07-03
 *
 * @brief   OS-specific utility functions for SDL platform
 *
 * Provides platform-specific queries for system information that
 * varies between macOS and Linux (WiFi, Bluetooth, etc.).
 */
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace ovb::hal::sdl {

/**
 * @brief WiFi connection status
 */
struct Wifi_Status {
    bool connected = false;
    std::string ssid;
    int signal_strength = 0;  // 0-100 percentage
};

/**
 * @brief Check if WiFi is available on the system
 * @return true if WiFi hardware is present
 */
bool has_wifi();

/**
 * @brief Get current WiFi connection status
 * @return WiFi status if connected, nullopt otherwise
 */
std::optional<Wifi_Status> get_wifi_status();

/**
 * @brief Check if WiFi is currently enabled (not just present)
 * @return true if WiFi interface is powered on
 */
bool wifi_enabled();

/**
 * @brief Enable or disable WiFi
 * @param enable true to turn on, false to turn off
 */
void set_wifi_enabled(bool enable);

/**
 * @brief Scan for available WiFi networks
 * @return List of SSIDs visible to the adapter
 */
std::vector<std::string> scan_wifi_networks();

/**
 * @brief Check if Bluetooth is available on the system
 * @return true if Bluetooth hardware is present
 */
bool has_bluetooth();

/**
 * @brief Get current Bluetooth connection status
 * @return true if connected, false otherwise
 */
bool get_bluetooth_connected();

} // namespace ovb::hal::sdl
