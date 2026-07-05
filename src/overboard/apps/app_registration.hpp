/**
 * @file    app_registration.hpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Central application registration
 *
 * Single location to register all user-facing applications.
 * Modify this file when adding new apps to the system.
 */
#pragma once

// C++ Standard Libraries
#include <functional>
#include <string>

// Project Libraries
#include <overboard/apps/calculator/calculator_app.hpp>
#include <overboard/apps/settings/settings_page.hpp>
#include <overboard/apps/status/status_page.hpp>
#include <overboard/core/layer_manager.hpp>
#include <overboard/gui/app_registry.hpp>
#include <overboard/hal/i_system_info.hpp>
#include <overboard/math/calc_engine.hpp>

namespace ovb::apps {

// Panel indices (must match registration order in register_all_apps())
// These are used by App_View for panel management
inline constexpr int PANEL_STATUS     { 0 };  // First app registered
inline constexpr int PANEL_CALCULATOR   { 1 };  // Second app registered
inline constexpr int PANEL_SETTINGS     { 2 };  // Third app registered
inline constexpr int PANEL_MENU         { 3 };  // Not an app, created separately

/**
 * @brief Register all applications with the registry
 *
 * Central registration point for all user-facing apps.
 * Priority determines menu order (lower = first).
 *
 * @param registry      App registry to register with
 * @param engine        Calculator engine (for Calculator_App)
 * @param layers        Layer manager (for Calculator_App, Status_Page)
 * @param panels        Panel manager for navigation callbacks
 * @param settings      Application settings manager
 * @param system_info   Platform system info provider
 */
inline void register_all_apps(  gui::App_Registry&                     registry,
                                math::Calc_Engine&                     engine,
                                core::Layer_Manager&                   layers,
                                gui::Panel_Manager&                    panels,
                                std::shared_ptr<core::Settings_Manager> settings,
                                hal::I_System_Info&                    system_info)
{
    // Auto-incrementing priority - just add apps here in desired menu order
    int priority = 0;

    // Status - shown first in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Status", LV_SYMBOL_FILE, priority++, 's',
        [&layers, &panels, settings, &system_info]() mutable {
            return std::make_shared<gui::Status_Page>(layers, system_info, settings, [&panels]() {
                panels.pop();
            });
        });

    // Calculator - second in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Calculator", LV_SYMBOL_KEYBOARD, priority++, 'c',
        [&engine, &layers, &panels, settings, &system_info]() mutable {
            return std::make_shared<gui::Calculator_App>(engine, layers, system_info, [&panels]() {
                panels.pop();
            }, settings);
        });

    // Settings - third in menu
    // ESCAPE returns to menu (pop)
    registry.register_app(
        "Settings", LV_SYMBOL_SETTINGS, priority++, 'g',
        [&panels, settings, &system_info]() mutable {
            return std::make_shared<gui::Settings_Page>(system_info, settings, [&panels]() {
                panels.pop();
            });
        });
}

/**
 * @brief Resolve a default_app setting name to a panel index
 *
 * Maps the string value of "ui.default_app" in settings.toml to the
 * corresponding PANEL_* constant. Returns PANEL_MENU for unknown values.
 *
 * @param name  Value from settings (e.g. "status", "calculator", "settings", "menu")
 * @return      Corresponding PANEL_* index
 */
inline int resolve_default_panel(const std::string& name) {
    if (name == "status")     return PANEL_STATUS;
    if (name == "calculator") return PANEL_CALCULATOR;
    if (name == "settings")   return PANEL_SETTINGS;
    return PANEL_MENU;
}

} // namespace ovb::apps
