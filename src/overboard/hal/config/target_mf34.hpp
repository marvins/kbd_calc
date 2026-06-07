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
#include <overboard/io/via_layout.hpp>

namespace ovb::hal::config {

/// @brief MF34 loads layout from VIA JSON files
inline core::Grid_Layout create_layout(
    const std::filesystem::path& layout_path,
    const std::filesystem::path& mapping_path)
{
    io::Via_Layout via_layout = io::parse_via_layout(layout_path);

    // Apply optional mapping file if it exists
    if (std::filesystem::exists(mapping_path)) {
        io::Via_Mapping mapping = io::parse_via_mapping(mapping_path);
        io::apply_mapping(via_layout, mapping);
    }

    return io::to_grid_layout(via_layout);
}

/// Target identifier
inline constexpr std::string_view TARGET_NAME = "KISNT MF34";

} // namespace ovb::hal::config
