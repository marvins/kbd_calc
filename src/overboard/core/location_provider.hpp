/**
 * @file    location_provider.hpp
 * @author  Marvin Smith
 * @date    2026-06-22
 *
 * @brief   Geographic location resolution for solar calculations
 *
 * Resolves Solar_Location from two sources in priority order:
 *  1. Settings file  (location.source = "settings")
 *  2. IP geolocation (location.source = "ip") via ip-api.com
 *
 * TOML settings keys:
 * @code
 * [location]
 * source    = "settings"    # "settings" or "ip"
 * latitude  = 39.7392
 * longitude = -104.9903
 * timezone  = -6.0          # UTC offset in hours
 * @endcode
 *
 * If source is "ip" or the file has no location block, an async HTTP
 * request is made to http://ip-api.com/json (no API key required).
 * The caller provides a callback that fires when the result is ready.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <optional>

// Project Libraries
#include <overboard/core/solar_calc.hpp>
#include <overboard/hal/settings_tree.hpp>

namespace ovb::core {

/// @brief Callback invoked when location is resolved (may be called from any thread)
using Location_Cb = std::function<void(Solar_Location)>;

/**
 * @brief Default fallback location (Denver, CO)
 */
inline constexpr Solar_Location DEFAULT_LOCATION {
    39.7392,
    -104.9903,
    -6.0
};

/**
 * @brief Try to resolve location from a Settings_Tree synchronously
 *
 * Reads `location.latitude`, `location.longitude`, and `location.timezone`.
 * Returns nullopt if any field is missing.
 *
 * @param tree Settings tree to read from
 * @return Populated Solar_Location or nullopt
 */
std::optional<Solar_Location> location_from_settings(const hal::Settings_Tree& tree);

/**
 * @brief Determine preferred source from settings ("settings" or "ip")
 *
 * Defaults to "settings" if the key is absent.
 */
std::string location_source( const hal::Settings_Tree& tree );

/**
 * @brief Resolve location asynchronously
 *
 * - If source is "settings" and all lat/lon/tz keys exist → calls cb immediately.
 * - If source is "ip" OR settings fields are missing → launches a background
 *   thread to call ip-api.com via curl; calls cb when done (or on failure falls
 *   back to DEFAULT_LOCATION).
 *
 * Thread safety: cb may be called from a background thread.  The caller is
 * responsible for dispatching back to the LVGL thread if needed (e.g. via an
 * lv_timer one-shot or an atomic flag checked in the update loop).
 *
 * @param tree  Settings tree (read-only)
 * @param cb    Callback to invoke with resolved location
 */
void resolve_location_async(const hal::Settings_Tree& tree, Location_Cb cb);

} // namespace ovb::core
