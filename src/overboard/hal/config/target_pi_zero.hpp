/**
 * @file    target_pi_zero.hpp
 * @author  Marvin Smith
 * @date    2026-06-09
 *
 * @brief   Raspberry Pi Zero 2W target configuration
 *
 * Loads the keyboard layout from the unified keyboard.json format.
 * Input is handled by Linux_Input (evdev) in the Pi Zero HAL.
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/io/keyboard_config.hpp>

namespace ovb::hal::config {

/// @brief Pi Zero loads layout from unified keyboard.json (mapping_path unused)
inline core::Grid_Layout create_layout(
    const std::filesystem::path& layout_path,
    [[maybe_unused]] const std::filesystem::path& mapping_path)
{
    auto config = io::parse_keyboard_config(layout_path);
    return io::to_grid_layout(config);
}

/// Target identifier
inline constexpr std::string_view TARGET_NAME = "Raspberry Pi Zero 2W";

} // namespace ovb::hal::config
