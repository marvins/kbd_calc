/**
 * @file    app_registry.hpp
 * @author  Marvin Smith
 * @date    2026-06-01
 *
 * @brief   Central application registry
 *
 * Defines panel indices for all applications. Add new apps here
 * and update App_Manager to handle them.
 */
#pragma once

// C++ Standard Libraries
#include <string_view>

// Third-Party Libraries
#include <lvgl.h>

namespace ovb::gui {

// Panel indices for main applications (0, 1, 2 match menu order)
inline constexpr int PANEL_STATUS     { 0 };
inline constexpr int PANEL_CALCULATOR { 1 };
inline constexpr int PANEL_SETTINGS   { 2 };

// Number of main application panels
inline constexpr int MAIN_APP_COUNT   { 3 };

/**
 * @brief Panel information for menu display
 */
struct Panel_Info {
    std::string_view name;
    std::string_view symbol;
};

/**
 * @brief Get panel information by index
 */
constexpr Panel_Info get_panel_info(int idx) {
    switch (idx) {
        case PANEL_STATUS:     return {"Status", LV_SYMBOL_FILE};
        case PANEL_CALCULATOR: return {"Calculator", LV_SYMBOL_KEYBOARD};
        case PANEL_SETTINGS:   return {"Settings", LV_SYMBOL_SETTINGS};
        default:               return {"Unknown", LV_SYMBOL_CLOSE};
    }
}

/**
 * @brief Convert panel index to display name
 */
constexpr std::string_view panel_index_to_name(int idx) {
    return get_panel_info(idx).name;
}

} // namespace ovb::gui
