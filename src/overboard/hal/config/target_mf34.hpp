/**
 * @file target_mf34.hpp
 * @brief KISNT MF34 hardware target configuration
 *
 * Defines the keyboard layout and key mappings for the KISNT MF34
 * 30-key macropad.
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/io/keyboard_config.hpp>

namespace ovb::hal::config {

/// @brief MF34 loads layout from keyboard.json
inline core::Grid_Layout create_layout(const std::filesystem::path& layout_path)
{
    io::Keyboard_Config keyboard_config = io::parse_keyboard_config(layout_path);
    return io::to_grid_layout(keyboard_config);
}

/// Target identifier
inline constexpr std::string_view TARGET_NAME = "KISNT MF34";

} // namespace ovb::hal::config
