/**
 * @file target_kn34.hpp
 * @brief KISNT KN34 hardware target configuration
 *
 * Defines the keyboard layout and key mappings for the KISNT KN34
 * 30-key macropad.
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/io/via_layout.hpp>

namespace ovb::hal::config {

/// @brief KN34 loads layout from VIA JSON files
inline core::Grid_Layout create_layout(
    const std::filesystem::path& layout_path,
    const std::filesystem::path& mapping_path)
{
    io::Via_Layout via_layout = io::parse_via_layout(layout_path);
    io::Via_Mapping mapping = io::parse_via_mapping(mapping_path);
    io::apply_mapping(via_layout, mapping);
    return io::to_grid_layout(via_layout);
}

/// Target identifier
inline constexpr std::string_view TARGET_NAME = "KISNT KN34";

} // namespace ovb::hal::config
