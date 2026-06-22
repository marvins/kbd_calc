/**
 * @file    target_picocalc.hpp
 * @author  Marvin Smith
 * @date    2026-06-16
 *
 * @brief   ClockworkPi PicoCalc hardware target configuration
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/io/keyboard_config.hpp>

namespace ovb::hal::config {

/// @brief PicoCalc loads layout from keyboard.json
inline core::Grid_Layout create_layout(const std::filesystem::path& layout_path)
{
    io::Keyboard_Config keyboard_config = io::parse_keyboard_config(layout_path);
    return io::to_grid_layout(keyboard_config);
}

/// Target identifier
inline constexpr std::string_view TARGET_NAME = "ClockworkPi PicoCalc";

} // namespace ovb::hal::config
