/**
 * @file    via_layout.hpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   VIA keyboard layout JSON parser
 *
 * Parses VIA JSON keyboard layout files to extract key position
 * and sizing information for visualization.
 */
#pragma once

// C++ Standard Libraries
#include <filesystem>
#include <map>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/core/point.hpp>
#include <overboard/core/keyboard_layout.hpp>

namespace ovb::io {

/**
 * @brief Key position and size from VIA layout
 */
struct Via_Key {
    int row;         ///< Matrix row index
    int col;         ///< Matrix column index
    double x;        ///< X offset in key units
    double y;        ///< Y offset in key units
    double w;        ///< Width in key units (default 1.0)
    double h;        ///< Height in key units (default 1.0)
    std::string label;    ///< Display label
    std::string code;     ///< Internal key code
    std::string scancode; ///< HAL scancode name (e.g. "SDL_SCANCODE_F13")
};

/**
 * @brief Parsed VIA keyboard layout
 */
struct Via_Layout {
    std::string name;
    int matrix_rows;
    int matrix_cols;
    std::vector<Via_Key> keys;
};

/**
 * @brief Key mapping with label and code
 */
struct Via_Mapping {
    std::string name;
    std::string layout_file;
    std::vector<Via_Key> keys;
};

/**
 * @brief Parse VIA JSON layout file
 * @param json_path Path to the VIA JSON file
 * @return Parsed layout, or throws on error
 */
Via_Layout parse_via_layout(const std::filesystem::path& json_path);

/**
 * @brief Parse VIA mapping JSON file
 * @param json_path Path to the mapping JSON file
 * @return Parsed mapping, or throws on error
 */
Via_Mapping parse_via_mapping(const std::filesystem::path& json_path);

/**
 * @brief Apply mapping to layout (merge label/code into layout keys)
 * @param layout Layout to modify
 * @param mapping Mapping to apply
 */
void apply_mapping(Via_Layout& layout, const Via_Mapping& mapping);

/**
 * @brief Calculate bounding box of all keys
 * @param layout Layout to measure
 * @return Size (width, height) in key units
 */
core::Size2d calculate_bounds(const Via_Layout& layout);

/**
 * @brief Convert Via_Layout to Grid_Layout
 * @param via_layout Parsed VIA layout
 * @return Grid_Layout for use in display/input systems
 */
core::Grid_Layout to_grid_layout( const Via_Layout& via_layout );

/**
 * @brief Build a map from SDL scancode name -> key index
 * @param layout Parsed VIA layout (must have scancode fields populated)
 * @return Map of scancode name string -> key index
 */
std::map<std::string, int> build_scancode_index_map( const Via_Layout& layout );

/**
 * @brief Load scancodes from a keymap JSON file and apply to an existing layout
 * @param layout Layout to update with scancode data
 * @param keymap_path Path to the keymap JSON file (contains top-level "scancodes" object)
 */
void apply_scancodes_from_json( Via_Layout&                  layout,
                                const std::filesystem::path& keymap_path );

} // namespace ovb::io
