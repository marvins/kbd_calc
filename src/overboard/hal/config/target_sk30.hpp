/**
 * @file target_sk30.hpp
 * @brief Womier SK30 hardware target configuration
 *
 * Defines the keyboard layout and key mappings for the Womier SK30
 * 30-key macropad.
 */

#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::config {

/// @brief SK30 uses its native asymmetric layout
inline core::Grid_Layout create_layout() {
    return core::Grid_Layout::womier_sk30();
}

/// Target identifier
inline constexpr const char* TARGET_NAME = "Womier SK30";

} // namespace ovb::hal::config
