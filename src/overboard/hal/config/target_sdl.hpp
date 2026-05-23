/**
 * @file target_sdl.hpp
 * @brief SDL Simulator target configuration
 *
 * Defines the keyboard layout and key mappings for the SDL-based
 * desktop simulator.
 */

#pragma once

// C++ Standard Libraries
#include <filesystem>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>
#include <overboard/io/via_layout.hpp>

namespace ovb::hal::config {

/// @brief SDL simulator loads layout from VIA JSON files
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
inline constexpr std::string_view TARGET_NAME = "SDL Simulator";

} // namespace ovb::hal::config
