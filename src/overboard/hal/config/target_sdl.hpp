/**
 * @file target_sdl.hpp
 * @brief SDL Simulator target configuration
 *
 * Defines the keyboard layout and key mappings for the SDL-based
 * desktop simulator.
 */

#pragma once

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::hal::config {

/// @brief SDL simulator uses the Womier SK30 layout for visual consistency
inline core::Grid_Layout create_layout() {
    return core::Grid_Layout::womier_sk30();
}

/// Target identifier
inline constexpr const char* TARGET_NAME = "SDL Simulator";

} // namespace ovb::hal::config
