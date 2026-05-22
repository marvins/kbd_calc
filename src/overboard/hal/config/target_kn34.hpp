/**
 * @file target_kn34.hpp
 * @brief KISNT KN34 hardware target configuration
 *
 * Defines the keyboard layout and key mappings for the KISNT KN34
 * 30-key macropad.
 */

#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::config {

/// @brief KN34 uses its native asymmetric layout
inline core::Grid_Layout create_layout() {
    return core::Grid_Layout::kn34();
}

/// Target identifier
inline constexpr const char* TARGET_NAME = "KISNT KN34";

} // namespace ovb::hal::config
