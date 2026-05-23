/**
 * @file    via_layout.cpp
 * @author  Marvin Smith
 * @date    2026-05-22
 *
 * @brief   VIA keyboard layout JSON parser implementation
 */
#include <overboard/io/via_layout.hpp>

// C++ Standard Libraries
#include <fstream>
#include <iostream>
#include <map>

// Project Libraries
#include <overboard/core/keyboard_layout.hpp>

// Third-Party Libraries
#include <nlohmann/json.hpp>

namespace ovb::io {

/***********************************************/
/*          Parse the Via Layout JSON          */
/***********************************************/
Via_Layout parse_via_layout(const std::filesystem::path& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + json_path.string());
    }

    nlohmann::json j;
    file >> j;

    Via_Layout layout;

    // Parse basic info
    if (j.contains("name")) {
        layout.name = j["name"].get<std::string>();
    }

    if (j.contains("matrix")) {
        layout.matrix_rows = j["matrix"]["rows"].get<int>();
        layout.matrix_cols = j["matrix"]["cols"].get<int>();
    }

    // Parse keymap layout
    if (j.contains("layouts") && j["layouts"].contains("keymap")) {
        const auto& keymap = j["layouts"]["keymap"];
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        double pending_w = 1.0;  // Default width
        double pending_h = 1.0;  // Default height

        for (const auto& row : keymap) {
            cursor_x = 0.0; // Reset X at start of each row
            pending_w = 1.0;
            pending_h = 1.0;

            for (const auto& item : row) {
                if (item.is_object()) {
                    // This is a positioning object (x, y, w, h)
                    if (item.contains("x")) {
                        cursor_x += item["x"].get<double>();
                    }
                    if (item.contains("y")) {
                        cursor_y += item["y"].get<double>();
                    }
                    if (item.contains("w")) {
                        pending_w = item["w"].get<double>();
                    }
                    if (item.contains("h")) {
                        pending_h = item["h"].get<double>();
                    }
                } else if (item.is_string()) {
                    // This is a key definition "row,col"
                    std::string key_str = item.get<std::string>();
                    Via_Key key;

                    // Parse "row,col" format
                    size_t comma_pos = key_str.find(',');
                    if (comma_pos != std::string::npos) {
                        key.row = std::stoi(key_str.substr(0, comma_pos));
                        key.col = std::stoi(key_str.substr(comma_pos + 1));
                    }

                    key.x = cursor_x;
                    key.y = cursor_y;
                    key.w = pending_w;
                    key.h = pending_h;
                    key.label = key_str;

                    layout.keys.push_back(key);
                    cursor_x += key.w; // Advance cursor by key width

                    // Reset pending values for next key
                    pending_w = 1.0;
                    pending_h = 1.0;
                }
            }

            cursor_y += 1.0; // Move to next row
        }
    }

    return layout;
}

/***********************************************/
/*        Parse the Via Mapping JSON         */
/***********************************************/
Via_Mapping parse_via_mapping(const std::filesystem::path& json_path) {
    std::ifstream file(json_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + json_path.string());
    }

    nlohmann::json j;
    file >> j;

    Via_Mapping mapping;

    if (j.contains("name")) {
        mapping.name = j["name"].get<std::string>();
    }

    if (j.contains("layout_file")) {
        mapping.layout_file = j["layout_file"].get<std::string>();
    }

    if (j.contains("keys")) {
        for (const auto& key_json : j["keys"]) {
            Via_Key key;

            // Prefer separate row/col fields, fall back to matrix string
            if (key_json.contains("row") && key_json.contains("col")) {
                key.row = key_json["row"].get<int>();
                key.col = key_json["col"].get<int>();
            } else if (key_json.contains("matrix")) {
                std::string matrix = key_json["matrix"].get<std::string>();
                size_t comma_pos = matrix.find(',');
                if (comma_pos != std::string::npos) {
                    key.row = std::stoi(matrix.substr(0, comma_pos));
                    key.col = std::stoi(matrix.substr(comma_pos + 1));
                }
            }

            // Visual position (if present in mapping)
            if (key_json.contains("x")) {
                key.x = key_json["x"].get<double>();
            }
            if (key_json.contains("y")) {
                key.y = key_json["y"].get<double>();
            }
            if (key_json.contains("w")) {
                key.w = key_json["w"].get<double>();
            }
            if (key_json.contains("h")) {
                key.h = key_json["h"].get<double>();
            }

            if (key_json.contains("label")) {
                key.label = key_json["label"].get<std::string>();
            }

            if (key_json.contains("code")) {
                key.code = key_json["code"].get<std::string>();
            }

            mapping.keys.push_back(key);
        }
    }

    return mapping;
}

/***********************************************/
/*        Apply mapping to layout             */
/***********************************************/
void apply_mapping(Via_Layout& layout, const Via_Mapping& mapping) {
    // Create a map from matrix position to mapping key
    std::map<std::pair<int, int>, Via_Key> mapping_map;
    for (const auto& key : mapping.keys) {
        mapping_map[{key.row, key.col}] = key;
    }

    // Apply mapping to layout keys by matrix position
    for (auto& key : layout.keys) {
        auto it = mapping_map.find({key.row, key.col});
        if (it != mapping_map.end()) {
            key.label = it->second.label;
            key.code = it->second.code;
        }
    }
}

/*****************************************/
/*      Calculate the layout bounds      */
/*****************************************/
core::Size2d calculate_bounds(const Via_Layout& layout) {
    double max_x = 0.0;
    double max_y = 0.0;

    for (const auto& key : layout.keys) {
        double key_right = key.x + key.w;
        double key_bottom = key.y + key.h;
        max_x = std::max(max_x, key_right);
        max_y = std::max(max_y, key_bottom);
    }

    return core::Size2d{max_x, max_y};
}

/***********************************************/
/*   Convert Via_Layout to Grid_Layout        */
/***********************************************/
core::Grid_Layout to_grid_layout(const Via_Layout& via_layout) {
    std::vector<core::Key_Position> positions;
    positions.reserve(via_layout.keys.size());

    for (const auto& key : via_layout.keys) {
        core::Key_Position pos;
        pos.col = static_cast<int>(key.x);
        pos.row = static_cast<int>(key.y);
        pos.col_span = static_cast<int>(key.w);
        pos.row_span = static_cast<int>(key.h);
        positions.push_back(pos);
    }

    // Calculate grid dimensions from bounds
    auto bounds = calculate_bounds(via_layout);
    int grid_cols = static_cast<int>(bounds.x) + 1;
    int grid_rows = static_cast<int>(bounds.y) + 1;

    return core::Grid_Layout(grid_cols, grid_rows, std::move(positions));
}

} // namespace ovb::io
